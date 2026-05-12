# 🌤️ Weather Station - IoT Project

A smart weather monitoring system with real-time temperature, humidity, and rain detection. Features a beautiful OLED display and web dashboard accessible via WiFi.

## ✨ Features

- 🌡️ **Real-time Temperature Monitoring** - Accurate readings from DHT11 sensor (-40 to 50°C)
- 💧 **Humidity Tracking** - Precise humidity levels (20-90%)
- 🌧️ **Rain Detection** - Animated rain indicator with cloud effects
- 📱 **Web Dashboard** - Beautiful, responsive interface accessible from any device
- 📊 **OLED Display** - Live data display with animated weather icons
- 📡 **WiFi Hotspot** - Built-in access point, no external network needed
- ⚡ **Low Power Consumption** - Efficient sensor readings and updates
- 🎨 **Modern UI** - Dark-themed card-based design with gradient background

## 📋 Hardware Requirements

| Component | Model | Purpose |
|-----------|-------|---------|
| Microcontroller | ESP32 / ESP8266 | Main processor |
| Temperature/Humidity Sensor | DHT11 | Environmental monitoring |
| OLED Display | 0.96" SSD1306 (I2C) | Real-time display |
| Rain Sensor | Capacitive/Resistive | Rain detection |
| USB Cable | Micro USB | Power & programming |

### Pin Configuration

```
DHT11 Sensor:
  - Data Pin → GPIO 4
  - Power → 5V
  - GND → GND

Rain Sensor:
  - Signal Pin → GPIO 15
  - Power → 5V
  - GND → GND

OLED Display (I2C):
  - SDA → GPIO 21 (ESP32) / GPIO 4 (ESP8266)
  - SCL → GPIO 22 (ESP32) / GPIO 5 (ESP8266)
  - Power → 3.3V
  - GND → GND
```

## 🚀 Quick Start

### 1. Install Arduino IDE
Download from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Install Board Support
1. Go to Tools → Board Manager
2. Search for "esp32" (or "esp8266")
3. Install the latest version

### 3. Install Required Libraries
Sketch → Include Library → Manage Libraries, then search for:
- `Adafruit GFX Library` by Adafruit
- `Adafruit SSD1306` by Adafruit
- `DHT sensor library` by Adafruit
- `Adafruit Unified Sensor` by Adafruit

### 4. Hardware Setup
Wire components according to the pin configuration above

### 5. Configure & Upload
1. Open `Weather_Station.ino`
2. Modify WiFi hotspot name/password (lines 17-18) if desired
3. Select your board: Tools → Board → ESP32 / ESP8266
4. Select COM port: Tools → Port → [Your Device]
5. Click Upload ⬆️

### 6. Access Dashboard
1. Device boots and shows WiFi info on OLED
2. Connect to WiFi hotspot "WeatherStation" (password: 12345678)
3. Open browser → `http://192.168.4.1`
4. View live weather data! 🎉

## 📱 Web Dashboard

The web interface displays:
- **Temperature** in Celsius with sun/cloud icon
- **Humidity** as percentage
- **Weather Status** - "Rainy" or "Clear" with dynamic icons
- **Auto-refresh** every 5 seconds

## 🔧 Configuration

Edit these lines in the code to customize:

```cpp
// Line 17-18: WiFi Hotspot Settings
const char* ap_ssid     = "WeatherStation";
const char* ap_password = "12345678";

// Line 12-14: Pin Assignments
#define DHT_PIN 4
#define RAIN_SIG_PIN 15
```

## 📊 API Endpoints

### GET `/`
Returns HTML dashboard with current readings:
```json
{
  "temperature": 34.2,
  "humidity": 38,
  "raining": false,
  "status": "Clear"
}
```

## 🛠️ Troubleshooting

### OLED Not Displaying
- Check I2C address (default: 0x3C)
- Verify wiring: SDA, SCL, VCC, GND

### Sensor Readings Not Updating
- Verify DHT11 is on GPIO 4
- Check if library is installed correctly
- Add 5V pullup resistor on data pin

### WiFi Hotspot Not Appearing
- Ensure board is ESP32/ESP8266 (not regular Arduino)
- Check board selection in Arduino IDE
- Verify power supply is adequate

### Rain Sensor Not Working
- Test with `digitalRead()` on serial monitor
- Verify GPIO 15 is available on your board
- Check sensor calibration threshold

## 🙏 Acknowledgments

- Adafruit for excellent sensor and display libraries
- Arduino community for support
- DHT11 sensor manufacturer for reliable hardware

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## ⭐ Support

If you find this project helpful, please give it a star! ⭐

---

**Made with ❤️ by an IoT enthusiast**
