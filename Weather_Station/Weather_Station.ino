#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define RAIN_SIG_PIN 15

// ===== HOTSPOT SETTINGS =====
const char* ap_ssid     = "WeatherStation";
const char* ap_password = "12345678";      // Min 8 characters, leave "" for open
// ============================

DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

int rainFrame = 0;
float currentTemp = 0;
float currentHumi = 0;
bool currentRain = false;

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="5">
  <title>Weather Station</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: Arial, sans-serif;
      background: linear-gradient(135deg, #1a1a2e, #16213e);
      color: white;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    h1 { font-size: 28px; margin-bottom: 30px; letter-spacing: 2px; opacity: 0.9; }
    .cards { display: flex; flex-wrap: wrap; gap: 20px; justify-content: center; }
    .card {
      background: rgba(255,255,255,0.08);
      border: 1px solid rgba(255,255,255,0.15);
      border-radius: 16px;
      padding: 30px 40px;
      text-align: center;
      min-width: 160px;
    }
    .icon { font-size: 48px; margin-bottom: 10px; }
    .label { font-size: 13px; opacity: 0.6; letter-spacing: 1px; text-transform: uppercase; margin-bottom: 8px; }
    .value { font-size: 36px; font-weight: bold; }
    .unit  { font-size: 18px; opacity: 0.7; }
    .rain-yes { color: #74b9ff; }
    .rain-no  { color: #fdcb6e; }
    .footer { margin-top: 30px; font-size: 12px; opacity: 0.4; }
  </style>
</head>
<body>
  <h1>&#127783; Weather Station</h1>
  <div class="cards">
    <div class="card">
      <div class="icon">&#127777;</div>
      <div class="label">Temperature</div>
      <div class="value">)rawliteral";
  html += String(currentTemp, 1);
  html += R"rawliteral(<span class="unit"> °C</span></div>
    </div>
    <div class="card">
      <div class="icon">&#128167;</div>
      <div class="label">Humidity</div>
      <div class="value">)rawliteral";
  html += String(currentHumi, 0);
  html += R"rawliteral(<span class="unit"> %</span></div>
    </div>
    <div class="card">
      <div class="icon">)rawliteral";
  html += currentRain ? "&#127783;" : "&#9728;";
  html += R"rawliteral(</div>
      <div class="label">Rain</div>
      <div class="value )rawliteral";
  html += currentRain ? "rain-yes\">Raining" : "rain-no\">Clear";
  html += R"rawliteral(</div>
    </div>
  </div>
  <div class="footer">Auto refreshes every 5 seconds</div>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void drawSun(int x, int y) {
  display.drawCircle(x, y, 8, WHITE);
  display.fillCircle(x, y, 5, WHITE);
  display.drawLine(x,      y - 12, x,      y - 9,  WHITE);
  display.drawLine(x,      y + 12, x,      y + 9,  WHITE);
  display.drawLine(x - 12, y,      x - 9,  y,      WHITE);
  display.drawLine(x + 12, y,      x + 9,  y,      WHITE);
  display.drawLine(x - 9,  y - 9,  x - 7,  y - 7,  WHITE);
  display.drawLine(x + 9,  y - 9,  x + 7,  y - 7,  WHITE);
  display.drawLine(x - 9,  y + 9,  x - 7,  y + 7,  WHITE);
  display.drawLine(x + 9,  y + 9,  x + 7,  y + 7,  WHITE);
}

void drawCloud(int x, int y) {
  display.fillCircle(x,      y,      7, WHITE);
  display.fillCircle(x + 8,  y - 3,  5, WHITE);
  display.fillCircle(x - 7,  y - 2,  5, WHITE);
  display.fillRect(x - 12,   y,     28,  8, WHITE);
}

void drawRain(int x, int y, int frame) {
  drawCloud(x, y);
  for (int i = 0; i < 4; i++) {
    int dropX = x - 9 + (i * 7);
    int dropY = y + 10 + ((frame + i * 2) % 12);
    display.drawLine(dropX, dropY, dropX - 1, dropY + 3, WHITE);
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RAIN_SIG_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true);
  }

  // Startup screen
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(20, 10);
  display.println("Weather Station");
  display.setCursor(20, 25);
  display.println("Starting AP...");
  display.display();

  // Start Access Point
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress IP = WiFi.softAPIP();

  Serial.println("Access Point Started!");
  Serial.print("SSID: ");     Serial.println(ap_ssid);
  Serial.print("Password: "); Serial.println(ap_password);
  Serial.print("IP: ");       Serial.println(IP);

  // Show connection info on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connect to WiFi:");
  display.setCursor(0, 14);
  display.setTextSize(1);
  display.println(ap_ssid);
  display.setCursor(0, 28);
  display.println("Password:");
  display.setCursor(0, 40);
  display.println(ap_password);
  display.setCursor(0, 54);
  display.println("192.168.4.1");
  display.display();
  delay(5000);

  // Start server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();

  currentTemp = dht.readTemperature();
  currentHumi = dht.readHumidity();
  currentRain = (digitalRead(RAIN_SIG_PIN) == HIGH);

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("-- WEATHER --");

  display.setTextSize(2);
  display.setCursor(0, 16);
  if (!isnan(currentTemp)) {
    display.print("T:");
    display.print(currentTemp, 1);
    display.print("C");
  } else {
    display.print("T: Err");
  }

  display.setTextSize(2);
  display.setCursor(0, 38);
  if (!isnan(currentHumi)) {
    display.print("H:");
    display.print(currentHumi, 0);
    display.print("%");
  } else {
    display.print("H: Err");
  }

  if (currentRain) {
    drawRain(104, 20, rainFrame);
    display.setTextSize(1);
    display.setCursor(90, 54);
    display.print("Rainy");
    rainFrame = (rainFrame + 2) % 12;
  } else {
    drawSun(104, 28);
    display.setTextSize(1);
    display.setCursor(92, 54);
    display.print("Clear");
  }

  display.display();

  delay(200);
}
