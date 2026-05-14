# 🌤️ Weather Station - IoT Project

A smart weather monitoring system with real-time temperature, humidity, and rain detection. Features a beautiful OLED display and a web dashboard accessible via WiFi — no internet required.

---

## 📸 Project Photo

![Weather Station](PHOTO.jpg)

> OLED display mounted on a custom enclosure showing live temperature (34.7°C), humidity (38%), and weather status (Clear ☀️).

---

## 🎥 Demo Video

[![Watch the demo](https://img.youtube.com/vi/wccDj2mXb9s/0.jpg)](https://youtube.com/shorts/wccDj2mXb9s)

> Click the thumbnail above to watch the project in action.

---

## ✨ Features

- 🌡️ **Real-time Temperature Monitoring** — Accurate readings from DHT11 sensor (-40 to 50°C)
- 💧 **Humidity Tracking** — Precise humidity levels (20–90%)
- 🌧️ **Rain Detection** — Animated rain indicator with cloud effects on OLED
- 📱 **Web Dashboard** — Beautiful, responsive interface accessible from any device
- 📊 **OLED Display** — Live data with animated weather icons
- 📡 **WiFi Hotspot** — Built-in access point; no external network needed
- ⚡ **Low Power Consumption** — Efficient sensor readings and updates
- 🎨 **Modern UI** — Dark-themed card-based design with gradient background

---

## 🛠️ Hardware Requirements

| Component | Model | Purpose |
|-----------|-------|---------|
| Microcontroller | ESP32 / ESP8266 | Main processor |
| Temperature/Humidity Sensor | DHT11 | Environmental monitoring |
| OLED Display | 0.96" SSD1306 (I2C) | Real-time display |
| Rain Sensor | Capacitive/Resistive | Rain detection |
| Enclosure | Custom cardboard/plastic box | Housing all components |
| USB Cable | Micro USB | Power & programming |

---

## 📌 Pin Configuration

```
DHT11 Sensor:
  Data Pin → GPIO 4
  Power    → 5V
  GND      → GND

Rain Sensor:
  Signal Pin → GPIO 15
  Power      → 5V
  GND        → GND

OLED Display (I2C):
  SDA   → GPIO 21 (ESP32) / GPIO 4 (ESP8266)
  SCL   → GPIO 22 (ESP32) / GPIO 5 (ESP8266)
  Power → 3.3V
  GND   → GND
```

---

## 🚀 Quick Start

### 1. Install Arduino IDE
Download from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Install Board Support
1. Go to **Tools → Board Manager**
2. Search for `esp32` (or `esp8266`)
3. Install the latest version

### 3. Install Required Libraries
Go to **Sketch → Include Library → Manage Libraries** and install:
- `Adafruit GFX Library` by Adafruit
- `Adafruit SSD1306` by Adafruit
- `DHT sensor library` by Adafruit
- `Adafruit Unified Sensor` by Adafruit

### 4. Wire the Hardware
Connect components according to the pin configuration above.

### 5. Configure & Upload
1. Open `Weather_Station.ino`
2. Optionally edit WiFi hotspot name/password (lines 17–18)
3. Select board: **Tools → Board → ESP32 / ESP8266**
4. Select port: **Tools → Port → [Your Device]**
5. Click **Upload ⬆️**

### 6. Access the Dashboard
1. Device boots and shows WiFi info on OLED
2. Connect your phone/PC to the `WeatherStation` hotspot (password: `12345678`)
3. Open a browser → `http://192.168.4.1`
4. View live weather data! 🎉

---

## ⚙️ Configuration

Edit these lines in the code to customize:

```cpp
// WiFi Hotspot Settings (lines 17–18)
const char* ap_ssid     = "WeatherStation";
const char* ap_password = "12345678";

// Pin Assignments (lines 12–14)
#define DHT_PIN      4
#define RAIN_SIG_PIN 15
```

---

## 📊 API Endpoints

### `GET /`
Returns the HTML dashboard with current sensor readings.

### `GET /data` *(JSON)*
```json
{
  "temperature": 34.2,
  "humidity": 38,
  "raining": false,
  "status": "Clear"
}
```

---

## 🔧 Troubleshooting

**OLED Not Displaying**
- Check I2C address (default: `0x3C`)
- Verify wiring: SDA, SCL, VCC, GND

**Sensor Readings Not Updating**
- Confirm DHT11 is on GPIO 4
- Verify library is installed
- Add a 10kΩ pull-up resistor on the data pin

**WiFi Hotspot Not Appearing**
- Ensure board is ESP32/ESP8266 (not a regular Arduino)
- Check board selection in Arduino IDE
- Verify power supply is adequate (5V, 1A minimum)

**Rain Sensor Not Working**
- Test with `digitalRead()` and Serial Monitor
- Confirm GPIO 15 is free on your board
- Adjust calibration threshold in code

---

## 📁 Project Structure

```
Electronics_projects/
└── Weather_Station/
    ├── Weather_Station.ino   # Main Arduino sketch
    └── README.md             # This file
```

---

## 🙏 Acknowledgments

- [Adafruit](https://adafruit.com) for excellent sensor and display libraries
- Arduino community for support and documentation
- DHT11 sensor manufacturer for reliable hardware

---

## 🤝 Contributing

Contributions are welcome!

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/AmazingFeature`
3. Commit your changes: `git commit -m 'Add AmazingFeature'`
4. Push to branch: `git push origin feature/AmazingFeature`
5. Open a Pull Request

---

## ⭐ Support

If you found this project useful or interesting, please give it a star! ⭐

---

**Made with ❤️ by an IoT enthusiast**
