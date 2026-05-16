# Smart Traffic Management System

A 4-lane smart traffic controller built on **ESP32** with **ultrasonic vehicle detection**, **FIFO lane queuing**, and a **Wi‑Fi dashboard** for automatic and manual control.

---

## Overview

This project detects vehicles in up to 4 lanes, adds the lanes to a queue in the order they are detected, and turns the green signal on for only one lane at a time.  
In **automatic mode**, the next lane in the queue becomes green only after the current lane is clear.  
In **manual mode**, you can choose any lane directly from the web dashboard.

---

## Features

- 4-lane vehicle detection using ultrasonic sensors
- FIFO queue-based traffic control
- Automatic lane switching
- Manual override mode
- Minimum green behavior with a clear grace period
- Live Wi‑Fi dashboard hosted by the ESP32 itself
- Real-time lane distance and vehicle status display

---

## Hardware Required

| Item | Quantity | Notes |
|---|---:|---|
| ESP32 development board | 1 | Main controller |
| Ultrasonic sensor (HC-SR04 or similar) | 4 | One sensor per lane |
| Green LED or signal module | 4 | One per lane |
| 220Ω resistor | 4 | For LED current limiting |
| Breadboard | 1 | For prototype wiring |
| Jumper wires | As needed | Male–male / male–female |
| External 5V power supply | 1 | Recommended for stable sensor power |
| Voltage divider or level shifter | 4 | Needed for HC-SR04 echo to ESP32 |

---

## Important Electrical Note

The **HC-SR04 echo pin outputs 5V**, but ESP32 GPIO pins are **3.3V only**.

So, for each echo pin:
- use a **voltage divider** or a **logic level shifter**
- do **not** connect echo directly to the ESP32

A simple divider can be made using:
- **1kΩ resistor** from echo to ESP32 input
- **2kΩ resistor** from ESP32 input to GND

---

## Pin Connections

### ESP32 Pin Map

| Function | ESP32 Pin | Connected To | Notes |
|---|---:|---|---|
| Trigger | GPIO 5 | All ultrasonic sensor TRIG pins | Shared trigger line |
| Echo Lane 1 | GPIO 18 | Sensor 1 ECHO | Use level shifting |
| Echo Lane 2 | GPIO 19 | Sensor 2 ECHO | Use level shifting |
| Echo Lane 3 | GPIO 21 | Sensor 3 ECHO | Use level shifting |
| Echo Lane 4 | GPIO 22 | Sensor 4 ECHO | Use level shifting |
| Green Lane 1 | GPIO 13 | LED 1 | Use 220Ω resistor |
| Green Lane 2 | GPIO 12 | LED 2 | Use 220Ω resistor |
| Green Lane 3 | GPIO 14 | LED 3 | Use 220Ω resistor |
| Green Lane 4 | GPIO 27 | LED 4 | Use 220Ω resistor |

---

## Circuit Wiring Instructions

| Step | Instruction | Detail |
|---|---|---|
| 1 | Power the ESP32 | Connect the board to USB or stable 5V supply |
| 2 | Connect all sensor VCC pins | Use 5V for the ultrasonic sensors |
| 3 | Connect all sensor GND pins | Share common ground with ESP32 |
| 4 | Connect TRIG pins together | Join all 4 TRIG pins and connect to GPIO 5 |
| 5 | Connect each ECHO separately | Lane 1 → GPIO 18, Lane 2 → GPIO 19, Lane 3 → GPIO 21, Lane 4 → GPIO 22 |
| 6 | Add level shifting | Put a voltage divider on each ECHO line before ESP32 |
| 7 | Wire LEDs | Connect each LED anode to its GPIO through a 220Ω resistor |
| 8 | Connect LED cathodes | Connect all cathodes to GND |
| 9 | Ensure common ground | ESP32 GND, sensor GND, and LED GND must be shared |
| 10 | Upload code | Flash the sketch to the ESP32 |
| 11 | Open Serial Monitor | Set baud rate to 115200 |
| 12 | Connect to Wi‑Fi | Join the ESP32 hotspot named `Auto_Traffic_AP` |
| 13 | Open dashboard | Visit the ESP32 IP shown by your network or use the default AP gateway |

---

## How It Works

### Automatic Mode
1. The ESP32 scans each lane sequentially.
2. If a vehicle is detected within the threshold distance, that lane is added to the queue.
3. Only one lane gets green at a time.
4. The current lane stays green until it becomes clear.
5. After a **2-second grace period**, the system switches to the next waiting lane.

### Manual Mode
1. Automatic queueing stops.
2. The queue is cleared.
3. You can tap a lane on the dashboard to make it active manually.

---

## Vehicle Detection Logic

| Parameter | Value |
|---|---:|
| Detection threshold | 10 cm |
| Sensor scan interval | 100 ms |
| Clear grace period | 2 seconds |
| Queue type | FIFO |
| Maximum queued lanes | 4 |

A lane is considered occupied when the measured distance is **greater than 0 and less than or equal to 10 cm**.

---

## Dashboard

The built-in web dashboard shows:

- current mode: Automatic / Manual
- active lane
- queue order
- each lane’s distance
- vehicle present / empty status

### Dashboard Routes

| Route | Purpose |
|---|---|
| `/` | Main dashboard page |
| `/data` | Live JSON state |
| `/setmode?m=1` | Switch to automatic mode |
| `/setmode?m=0` | Switch to manual mode |
| `/trigger?l=1` to `/trigger?l=4` | Manually activate a lane |

---

## Code Behavior Summary

| Behavior | Description |
|---|---|
| Queue addition | A lane is added only once while waiting |
| No duplicate queue entries | Same lane cannot be added repeatedly |
| Active lane protection | The active lane is treated as already in queue |
| Automatic switching | Occurs only when the current lane is clear |
| Manual override | Works only when manual mode is selected |

---

## Upload Instructions

1. Open the `.ino` file in Arduino IDE.
2. Install ESP32 board support if not already installed.
3. Select your ESP32 board and COM port.
4. Upload the sketch.
5. Open Serial Monitor at **115200 baud**.
6. Connect to the Wi‑Fi network:
   - **SSID:** `Auto_Traffic_AP`
   - **Password:** `12345678Voltage`

---

## Recommended Improvements

| Idea | Benefit |
|---|---|
| Add red and yellow LEDs | More realistic traffic signal behavior |
| Use proper sensor mounts | Better detection accuracy |
| Add buzzer alerts | Extra visual/audio feedback |
| Add OLED display | Local lane and queue display |
| Add power regulation | More stable long-term operation |
| Enclose the circuit | Safer and cleaner installation |

---

## Troubleshooting

| Problem | Possible Cause | Fix |
|---|---|---|
| No Wi‑Fi network appears | ESP32 not powered or sketch not uploaded | Recheck upload and power |
| Wrong distance readings | Sensor wiring issue or noisy power | Verify TRIG/ECHO wiring and grounding |
| ESP32 restarts | Weak power supply | Use a stable supply |
| No lane turns green | LEDs wired incorrectly | Check GPIO pins and resistor placement |
| Random lane triggering | Echo line not level-shifted | Use a voltage divider for each ECHO pin |

---

## Notes

- The sketch uses **ESP32 Wi‑Fi SoftAP mode**, so the board creates its own hotspot.
- All grounds must be connected together.
- For best results, keep sensor wiring short.
- The project currently uses **green LEDs as the active signal** and turns the others off.

---

## File Name Suggestion

If you want a cleaner project structure, rename:

`Trafic_Management_system.ino` → `Traffic_Management_System.ino`

---

## License

You may add your preferred license here, such as MIT or Apache 2.0.
