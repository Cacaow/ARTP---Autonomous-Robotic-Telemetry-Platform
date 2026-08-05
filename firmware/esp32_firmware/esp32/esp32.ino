#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>


const char *ssid = "huangaihui74";
const char *password = "Cimtek628!";

// http://ARTP.local
const char *MDNS_NAME = "ARTP";

//WEBSERVER
WebServer server(80);

//UART CONFIGURATION
HardwareSerial STM32Serial(2);

constexpr int STM32_UART_RX_PIN = 16;
constexpr int STM32_UART_TX_PIN = 17;
constexpr uint32_t STM32_UART_BAUD = 115200;

int count = 0;
char buffer[256];
double temp, hum, press, roll, pitch, alti;
char state[32];
String latestSTM32Msg = "Waiting for STM32...";
String latestCommandStatus = "No Command Sent";
String loggingStatus = "UNKNOWN";
String pingStatus = "Waiting";

bool telemetryValid = false;

//TIMING
uint32_t lastPingMs = 0;
constexpr uint32_t PING_INTERVAL_MS = 5000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Connect to STM32
  STM32Serial.begin(
    STM32_UART_BAUD,
    SERIAL_8N1,
    STM32_UART_RX_PIN,
    STM32_UART_TX_PIN
  );
  Serial.println("STM32 UART Start.");

  // Connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  temp = 0.0;
  hum = 0.0;
  press = 0.0;
  roll = 0.0;
  pitch = 0.0;
  alti = 0.0;
  sprintf(state, "UNKNOWN");
  
  // Connect to Webserver
  Serial.println("Webserver launched");
  server.on("/", makePage);
  server.begin();
  Serial.println("Webserver Started");

  if (MDNS.begin(MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);

    Serial.print("mDNS address: http://");
    Serial.print(MDNS_NAME);
    Serial.println(".local");
  } else {
    Serial.println("mDNS startup failed");
  }

  Serial.println("Set-Up Complete");
}

void loop() {
  // put your main code here, to run repeatedly:
  sprintf(buffer, "Running Cycle %d", count);
  Serial.println(buffer);
  count = count + 1;
  temp+=1;

  server.handleClient();

  readFromSTM32();

  delay(1000);
}  // put your setup code here, to run once:

// WEBDASHBOARD
String makePage()
{
  String html;

  html += "<html>";
  html += "<head>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "</head>";

  html += "<body>";

  html += "<h1>STM32 Telemetry</h1>";

  html += "Temperature: ";
  html += String(temp);
  html += "<br>";

  html += "Humidity: ";
  html += String(hum);
  html += "<br>";

  html += "Pressure: ";
  html += String(press);
  html += "<br>";

  html += "Roll: ";
  html += String(roll);
  html += "<br>";

  html += "Pitch: ";
  html += String(pitch);
  html += "<br>";

  html += "State: ";
  html += state;
  html += "<br>";

  html += "</body></html>";

  server.send(200, "text/html", html);

  return html;
}

// STM32 -> UART
void readFromSTM32() {
  while (STM32Serial.available() > 0) {
    latestSTM32Msg = STM32Serial.readStringUntil('\n');
    latestSTM32Msg.trim();

    Serial.print("STM32 -> ESP32: ");
    Serial.println(latestSTM32Msg);

    char tempBuffer[32]; 
    latestSTM32Msg.toCharArray(tempBuffer, sizeof(tempBuffer));
    const char* line = tempBuffer; 

    if (strncmp(line, "TEL,", 4) == 0)
    {
        double tmpTemp;
        double tmpPres;
        double tmpHumid;
        double tmpRoll;
        double tmpPitch;
        char tmpState[32];

        int matched = sscanf(
            line,
            "TEL,temp=%f,pres=%f,hum=%f,roll=%f,pitch=%f,state=%s",
            &tmpTemp,
            &tmpPres,
            &tmpHumid,
            &tmpRoll,
            &tmpPitch,
            &tmpState
        );

        if (matched != 6)
        {
            Serial.println("Invalid telemetry format");
            return;
        }

        temp = tmpTemp;
        press = tmpPres;
        hum = tmpHumid;
        roll = tmpRoll;
        pitch = tmpPitch;
        strcpy(state, tmpState);

        telemetryValid = true;

        return;
    }

    if (strcmp(line, "PONG") == 0)
    {
        pingStatus = "PONG received";
        Serial.println("STM32 link test passed");
        return;
    }

    if (strcmp(line, "ACK,START") == 0)
    {
        loggingStatus = "RUNNING";
        latestCommandStatus = "STM32 confirmed START";
        return;
    }

    if (strcmp(line, "ACK,STOP") == 0)
    {
        loggingStatus = "STOPPED";
        latestCommandStatus = "STM32 confirmed STOP";
        return;
    }

    if (strcmp(line, "ERR,SD_START") == 0)
    {
        loggingStatus = "ERROR";
        latestCommandStatus = "STM32 could not start SD logging";
        return;
    }
  }
}

void sendToSTM32(const char *command)
{
    STM32Serial.print(command);
    STM32Serial.print('\n');

    Serial.print("ESP32 -> STM32: ");
    Serial.println(command);
}




