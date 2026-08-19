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
float temp, hum, press, roll, pitch, alti;
char state[32];
String latestSTM32Msg = "Waiting for STM32...";
String latestCommandStatus = "No Command Sent";
String loggingStatus = "UNKNOWN";
String pingStatus = "Waiting";

bool telemetryValid = false;
uint32_t latestTelemetryMs = 0;

//TIMING
uint32_t lastPingMs = 0;
constexpr uint32_t PING_INTERVAL_MS = 5000;

/* =========================================================
 * HTML page
 * ========================================================= */
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">

    <meta
        name="viewport"
        content="width=device-width, initial-scale=1.0">

    <title>STM32 Telemetry Dashboard</title>

    <style>
        * {
            box-sizing: border-box;
        }

        body {
            margin: 0;
            padding: 24px;
            font-family: Arial, Helvetica, sans-serif;
            background: #f1f5f9;
            color: #0f172a;
        }

        .container {
            max-width: 1100px;
            margin: auto;
        }

        h1 {
            margin-top: 0;
            margin-bottom: 6px;
        }

        .subtitle {
            color: #64748b;
            margin-bottom: 24px;
        }

        .grid {
            display: grid;

            grid-template-columns:
                repeat(auto-fit, minmax(210px, 1fr));

            gap: 16px;
        }

        .card {
            background: white;
            border-radius: 14px;
            padding: 20px;

            box-shadow:
                0 4px 15px rgba(15, 23, 42, 0.08);
        }

        .label {
            color: #64748b;
            font-size: 14px;
            margin-bottom: 8px;
        }

        .value {
            font-size: 34px;
            font-weight: bold;
        }

        .unit {
            font-size: 16px;
            color: #64748b;
        }

        .state-value {
            font-size: 28px;
            font-weight: bold;
        }

        .control-card {
            margin-top: 16px;
        }

        button {
            border: none;
            border-radius: 8px;
            padding: 12px 20px;
            margin-right: 8px;
            margin-bottom: 8px;
            font-size: 15px;
            font-weight: bold;
            cursor: pointer;
        }

        .start {
            background: #16a34a;
            color: white;
        }

        .stop {
            background: #dc2626;
            color: white;
        }

        .ping {
            background: #2563eb;
            color: white;
        }

        button:hover {
            opacity: 0.88;
        }

        button:disabled {
            opacity: 0.45;
            cursor: not-allowed;
        }

        .status-row {
            margin-top: 12px;
            padding: 10px;
            background: #f8fafc;
            border-radius: 7px;
            overflow-wrap: anywhere;
        }

        .connected {
            color: #16a34a;
            font-weight: bold;
        }

        .disconnected {
            color: #dc2626;
            font-weight: bold;
        }

        .state-stable {
            color: #16a34a;
        }

        .state-moving {
            color: #2563eb;
        }

        .state-tilted {
            color: #d97706;
        }

        .state-shaking {
            color: #dc2626;
        }

        .state-unknown {
            color: #64748b;
        }
    </style>
</head>

<body>

<div class="container">

    <h1>STM32 Telemetry Dashboard</h1>

    <div class="subtitle">
        ESP32 Wi-Fi gateway and STM32 sensor controller
    </div>

    <div class="grid">

        <div class="card">
            <div class="label">Temperature</div>

            <div class="value">
                <span id="temperature">--</span>
                <span class="unit">°C</span>
            </div>
        </div>

        <div class="card">
            <div class="label">Pressure</div>

            <div class="value">
                <span id="pressure">--</span>
                <span class="unit">hPa</span>
            </div>
        </div>

        <div class="card">
            <div class="label">Humidity</div>

            <div class="value">
                <span id="humidity">--</span>
                <span class="unit">%</span>
            </div>
        </div>

        <div class="card">
            <div class="label">Altitude</div>

            <div class="value">
                <span id="altitude">--</span>
                <span class="unit">m</span>
            </div>
        </div>

        <div class="card">
            <div class="label">Roll</div>

            <div class="value">
                <span id="roll">--</span>
                <span class="unit">°</span>
            </div>
        </div>

        <div class="card">
            <div class="label">Pitch</div>

            <div class="value">
                <span id="pitch">--</span>
                <span class="unit">°</span>
            </div>
        </div>

        <div class="card">
            <div class="label">Motion State</div>

            <div
                id="sensorState"
                class="state-value state-unknown">
                UNKNOWN
            </div>
        </div>

        <div class="card">
            <div class="label">STM32 Link</div>

            <div
                id="connection"
                class="disconnected">
                Waiting
            </div>

            <div class="status-row">
                Data age:
                <span id="age">--</span> ms
            </div>
        </div>

    </div>

    <div class="card control-card">

        <div class="label">
            SD Logging Control
        </div>

        <button
            id="startButton"
            class="start"
            onclick="sendCommand('start')">
            START
        </button>

        <button
            id="stopButton"
            class="stop"
            onclick="sendCommand('stop')">
            STOP
        </button>

        <button
            class="ping"
            onclick="sendCommand('ping')">
            PING
        </button>

        <div class="status-row">
            Logging:
            <strong id="logging">
                UNKNOWN
            </strong>
        </div>

        <div class="status-row">
            Ping:
            <strong id="ping">
                Waiting
            </strong>
        </div>

        <div class="status-row">
            Command:
            <span id="commandStatus">
                No command sent
            </span>
        </div>

        <div class="status-row">
            STM32 message:
            <span id="stm32Message">
                Waiting for STM32...
            </span>
        </div>

    </div>

</div>

<script>

function updateStateStyle(state) {
    const stateElement =
        document.getElementById("sensorState");

    stateElement.className =
        "state-value state-unknown";

    const normalizedState =
        state.toLowerCase();

    if (normalizedState === "stable") {
        stateElement.className =
            "state-value state-stable";
    }
    else if (normalizedState === "moving") {
        stateElement.className =
            "state-value state-moving";
    }
    else if (normalizedState === "tilted") {
        stateElement.className =
            "state-value state-tilted";
    }
    else if (normalizedState === "shaking") {
        stateElement.className =
            "state-value state-shaking";
    }
}

function updateLoggingButtons(loggingStatus) {
    const startButton =
        document.getElementById("startButton");

    const stopButton =
        document.getElementById("stopButton");

    if (loggingStatus === "RUNNING") {
        startButton.disabled = true;
        stopButton.disabled = false;
    }
    else if (loggingStatus === "STOPPED") {
        startButton.disabled = false;
        stopButton.disabled = true;
    }
    else if (
        loggingStatus === "STARTING" ||
        loggingStatus === "STOPPING"
    ) {
        startButton.disabled = true;
        stopButton.disabled = true;
    }
    else {
        startButton.disabled = false;
        stopButton.disabled = false;
    }
}

async function updateTelemetry() {
    try {
        const response = await fetch(
            "/api/telemetry",
            {
                cache: "no-store"
            }
        );

        if (!response.ok) {
            throw new Error(
                "HTTP error " + response.status
            );
        }

        const data = await response.json();

        if (data.valid) {
            document
                .getElementById("temperature")
                .textContent =
                data.temperature.toFixed(1);

            /*
             * STM32 normally sends pressure in Pa.
             * Convert Pa to hPa for display.
             */
            document
                .getElementById("pressure")
                .textContent =
                (data.pressure / 100.0).toFixed(1);

            document
                .getElementById("humidity")
                .textContent =
                data.humidity.toFixed(1);

            document
                .getElementById("altitude")
                .textContent =
                data.altitude.toFixed(1);

            document
                .getElementById("roll")
                .textContent =
                data.roll.toFixed(1);

            document
                .getElementById("pitch")
                .textContent =
                data.pitch.toFixed(1);

            document
                .getElementById("sensorState")
                .textContent =
                data.sensorState;

            updateStateStyle(
                data.sensorState
            );
        }

        document
            .getElementById("age")
            .textContent =
            data.age;

        document
            .getElementById("logging")
            .textContent =
            data.logging;

        document
            .getElementById("ping")
            .textContent =
            data.ping;

        document
            .getElementById("commandStatus")
            .textContent =
            data.commandStatus;

        document
            .getElementById("stm32Message")
            .textContent =
            data.lastMessage;

        updateLoggingButtons(
            data.logging
        );

        const connection =
            document.getElementById("connection");

        if (data.connected) {
            connection.textContent =
                "CONNECTED";

            connection.className =
                "connected";
        }
        else {
            connection.textContent =
                "DISCONNECTED";

            connection.className =
                "disconnected";
        }
    }
    catch (error) {
        console.log(error);

        const connection =
            document.getElementById("connection");

        connection.textContent =
            "ESP32 REQUEST ERROR";

        connection.className =
            "disconnected";
    }
}

async function sendCommand(command) {
    const status =
        document.getElementById(
            "commandStatus"
        );

    status.textContent =
        "Sending " +
        command.toUpperCase() +
        "...";

    try {
        const response = await fetch(
            "/api/" + command,
            {
                method: "POST",
                cache: "no-store"
            }
        );

        const result =
            await response.text();

        status.textContent =
            result;
    }
    catch (error) {
        status.textContent =
            "Request failed: " +
            error;
    }
}

setInterval(
    updateTelemetry,
    500
);

updateTelemetry();

</script>

</body>
</html>
)rawliteral";

/* =========================================================
 * Function declarations
 * ========================================================= */



/* =========================================================
 * Setup
 * ========================================================= */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);

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
  setupWebServer();
  Serial.println("Webserver Started");

  if (MDNS.begin(MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);

    Serial.print("mDNS address: http://");
    Serial.print(MDNS_NAME);
    Serial.println(".local");
  } else {
    Serial.println("mDNS startup failed");
  }

  lastPingMs = millis();
  Serial.println("Set-Up Complete");
}

/* =========================================================
 * Loop
 * ========================================================= */

void loop() {
  // put your main code here, to run repeatedly:
  //sprintf(buffer, "Running Cycle %d", count);
  //Serial.println(buffer);
  count = count + 1;

  server.handleClient();

  readFromSTM32();

  if (millis() - lastPingMs >= PING_INTERVAL_MS)
    {
        lastPingMs += PING_INTERVAL_MS;
        pingStatus = "PING sent, waiting for PONG";
        sendToSTM32("PING");
    }

  delay(1000);
}  // put your setup code here, to run once:

/* =========================================================
 * Web server
 * ========================================================= */

void setupWebServer()
{
    server.on(
        "/",
        HTTP_GET,
        []()
        {
            server.send_P(
                200,
                "text/html",
                INDEX_HTML
            );
        }
    );

    server.on(
        "/api/start",
        HTTP_POST,
        []()
        {
            sendToSTM32("START");

            latestCommandStatus =
                "START sent to STM32";

            server.send(
                200,
                "text/plain",
                latestCommandStatus
            );
        }
    );

    server.on(
        "/api/stop",
        HTTP_POST,
        []()
        {
            sendToSTM32("STOP");

            latestCommandStatus =
                "STOP sent to STM32";

            server.send(
                200,
                "text/plain",
                latestCommandStatus
            );
        }
    );

    server.on(
        "/api/ping",
        HTTP_POST,
        []()
        {
            sendToSTM32("PING");

            latestCommandStatus =
                "PING sent to STM32";

            pingStatus =
                "PING sent, waiting for PONG";

            server.send(
                200,
                "text/plain",
                latestCommandStatus
            );
        }
    );

    server.on(
        "/api/telemetry",
        HTTP_GET,
        []()
        {
            uint32_t age;

            if (telemetryValid)
            {
                age = millis() - latestTelemetryMs;
            }
            else
            {
                age = 0;
            }

            /*
             * STM32 is considered connected if telemetry was
             * received during the last three seconds.
             */
            bool connected =
                telemetryValid && age < 3000;

            String json;

            json.reserve(400);

            json += "{";

            json += "\"valid\":";
            json += telemetryValid ? "true" : "false";

            json += ",\"connected\":";
            json += connected ? "true" : "false";

            json += ",\"temperature\":";
            json += String(temp, 1);

            json += ",\"pressure\":";
            json += String(press, 1);

            json += ",\"humidity\":";
            json += String(hum, 1);

            json += ",\"altitude\":";
            json += String(alti, 1);

            json += ",\"roll\":";
            json += String(roll, 1);

            json += ",\"pitch\":";
            json += String(pitch, 1);

            json += ",\"sensorState\":\"";
            json += jsonEscape(state);
            json += "\"";

            json += ",\"age\":";
            json += String(age);

            json += ",\"logging\":\"";
            json += jsonEscape(loggingStatus);
            json += "\"";

            json += ",\"ping\":\"";
            json += jsonEscape(pingStatus);
            json += "\"";

            json += ",\"commandStatus\":\"";
            json += jsonEscape(latestCommandStatus);
            json += "\"";

            json += ",\"lastMessage\":\"";
            json += jsonEscape(latestSTM32Msg);
            json += "\"";

            json += "}";

            server.send(
                200,
                "application/json",
                json
            );
        }
    );

    server.onNotFound(
        []()
        {
            server.send(
                404,
                "text/plain",
                "Not found"
            );
        }
    );

    server.begin();

    Serial.println("Web server started");
}

/* =========================================================
 * STM32 UART processing
 * ========================================================= */

// STM32 -> UART
void readFromSTM32() {
  while (STM32Serial.available() > 0) {
    latestSTM32Msg = STM32Serial.readStringUntil('\n');
    latestSTM32Msg.trim();

    Serial.print("STM32 -> ESP32: ");
    Serial.println(latestSTM32Msg);

    char tempBuffer[256]; 
    latestSTM32Msg.toCharArray(tempBuffer, sizeof(tempBuffer));
    const char* line = tempBuffer; 

    if (strncmp(line, "TEL,", 4) == 0)
    {
        char tmpTemp[32];
        char tmpPres[32];
        char tmpHumid[32];
        char tmpRoll[32];
        char tmpPitch[32];
        char tmpState[32];
        char tmpAlti[32];

        int matched = sscanf(
            line,
            "TEL,temp=%31[^,],pres=%31[^,],hum=%31[^,],alti=%31[^,],roll=%31[^,],pitch=%31[^,],state=%31s",
            tmpTemp,
            tmpPres,
            tmpHumid,
            tmpAlti,
            tmpRoll,
            tmpPitch,
            tmpState
        );
        Serial.println(matched);
        Serial.println(tmpTemp);
        Serial.println(tmpPres);
        Serial.println(tmpHumid);
        Serial.println(tmpAlti);
        Serial.println(tmpRoll);
        Serial.println(tmpPitch);
        Serial.println(tmpState);

        if (matched != 7)
        {
            Serial.println("Invalid telemetry format");
            return;
        }
        
        temp = atof(tmpTemp);
        press = atof(tmpPres);
        hum = atof(tmpHumid);
        alti = atof(tmpAlti);
        roll = atof(tmpRoll);
        pitch = atof(tmpPitch);
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

/* =========================================================
 * JSON helper
 * ========================================================= */

String jsonEscape(const String &input)
{
    String output;

    output.reserve(input.length() + 16);

    for (size_t i = 0; i < input.length(); i++)
    {
        char character = input[i];

        switch (character)
        {
            case '"':
                output += "\\\"";
                break;

            case '\\':
                output += "\\\\";
                break;

            case '\n':
                output += "\\n";
                break;

            case '\r':
                output += "\\r";
                break;

            case '\t':
                output += "\\t";
                break;

            default:
                output += character;
                break;
        }
    }

    return output;
}





