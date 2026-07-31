#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>


const char *ssid = "huangaihui74";
const char *password = "Cimtek628!";

WebServer server(80);

int count = 0;
char buffer[256];
double temp, hum, press, roll, pitch, alti;
char status[32];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // We start by connecting to a WiFi network

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
  sprintf(status, "UNKNOWN");
  
  Serial.println("Webserver launched");
  server.on("/", makePage);
  server.begin();
  Serial.println("Webserver Started");

  Serial.println("Set-Up Complete");
}

void loop() {
  // put your main code here, to run repeatedly:
  sprintf(buffer, "Running Cycle %d", count);
  Serial.println(buffer);
  count = count + 1;
  temp+=1;

  server.handleClient();
  delay(1000);
}  // put your setup code here, to run once:

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

  html += "Status: ";
  html += status;
  html += "<br>";

  html += "</body></html>";

  server.send(200, "text/html", html);

  return html;
}


