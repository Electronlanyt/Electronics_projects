#include <WiFi.h>
#include <WiFiServer.h>

// --- Configuration ---
const char* ssid = "Auto_Traffic_AP";
const char* password = "12345678Voltage"; 

const int trigPin = 5;
const int echoPins[4] = {18, 19, 21, 22};
const int ledPins[4]  = {13, 12, 14, 27}; // Green LEDs

const int thresholdDistance = 10; 

// System Tracking
int distances[4] = {0, 0, 0, 0};
bool laneHasVehicle[4] = {false, false, false, false};

// --- QUEUE & MODE SYSTEM ---
int queue[4] = {0, 0, 0, 0}; // 0 means empty slot. Holds Lane IDs (1-4)
int queueCount = 0;          // Number of lanes currently waiting in queue
int activeLane = 0;          // The lane currently showing GREEN (0 if none)
bool isAutoMode = true;      // Control Mode: true = Auto, false = Manual

// Timing
unsigned long lastSensorCheck = 0;
const unsigned long sensorInterval = 100;
int sensorCycleIndex = 0;

unsigned long laneClearTime = 0;
bool waitingForClearTimeout = false;
const unsigned long clearGracePeriod = 2000; // 2 seconds of green after car leaves

WiFiServer server(80);

// Helper function to check if a lane is already waiting in the queue
bool isLaneInQueue(int laneId) {
    if (activeLane == laneId) return true;
    for (int i = 0; i < 4; i++) {
        if (queue[i] == laneId) return true;
    }
    return false;
}

// Helper function to add a lane to the back of the queue
void enqueue(int laneId) {
    if (isAutoMode && !isLaneInQueue(laneId) && queueCount < 4) {
        queue[queueCount] = laneId;
        queueCount++;
        Serial.print("Lane ");
        Serial.print(laneId);
        Serial.println(" added to queue.");
    }
}

// Helper function to shift the queue forward after a lane finishes its green light
void dequeue() {
    if (queueCount > 0) {
        // Move the first waiting lane to active status
        activeLane = queue[0];
        
        // Shift remaining items in the array forward
        for (int i = 0; i < 3; i++) {
            queue[i] = queue[i + 1];
        }
        queue[3] = 0; // Clear the last slot
        queueCount--;
        
        waitingForClearTimeout = false;
        Serial.print("Traffic shifting. Lane ");
        Serial.print(activeLane);
        Serial.println(" is now GREEN.");
    } else {
        activeLane = 0; // No cars waiting anywhere, turn off all green lights
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(trigPin, OUTPUT);
    for (int i = 0; i < 4; i++) {
        pinMode(echoPins[i], INPUT);
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW); // Default: All OFF (Red)
    }

    WiFi.softAP(ssid, password);
    server.begin();
}

void loop() {
    unsigned long currentMillis = millis();

    // 1. Scan Sensors Sequentially (Every 100ms)
    if (currentMillis - lastSensorCheck >= sensorInterval) {
        lastSensorCheck = currentMillis;
        int i = sensorCycleIndex;

        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        long duration = pulseIn(echoPins[i], HIGH, 20000);
        distances[i] = (duration == 0) ? -1 : (duration * 0.034 / 2);
        
        bool currentDetection = (distances[i] > 0 && distances[i] <= thresholdDistance);
        
        // If a vehicle is detected, attempt to add its lane (i + 1) into the FIFO sequence
        if (currentDetection) {
            enqueue(i + 1);
        }
        laneHasVehicle[i] = currentDetection;

        sensorCycleIndex = (sensorCycleIndex + 1) % 4;
    }

    // 2. Manage Automatic Queue Processing
    if (isAutoMode) {
        if (activeLane == 0) {
            // If no lane is currently green, check if anyone arrived in the queue
            if (queueCount > 0) {
                dequeue();
            }
        } else {
            // Lane remains green until the sensor reports it is clear
            if (!laneHasVehicle[activeLane - 1]) {
                if (!waitingForClearTimeout) {
                    waitingForClearTimeout = true;
                    laneClearTime = currentMillis; // Start the grace period countdown
                } else if (currentMillis - laneClearTime >= clearGracePeriod) {
                    digitalWrite(ledPins[activeLane - 1], LOW); // Turn old green light OFF
                    dequeue(); // Pull next lane from queue
                }
            } else {
                // If a car passes/remains before countdown finishes, cancel termination
                waitingForClearTimeout = false;
            }
        }
    }

    // Write hardware outputs based on active lane status
    for (int i = 0; i < 4; i++) {
        if ((i + 1) == activeLane) {
            digitalWrite(ledPins[i], HIGH); // Turn Green ON
        } else {
            digitalWrite(ledPins[i], LOW);  // Turn Green OFF (Red)
        }
    }

    // 3. Mobile UI Dashboard Server
    WiFiClient client = server.available();
    if (client) {
        String currentLine = "";
        String requestLine = "";

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        
                        // Output Live State JSON Data
                        if (requestLine.indexOf("GET /data") >= 0) {
                            client.println("HTTP/1.1 200 OK\nContent-Type: application/json\nConnection: close\n");
                            
                            String json = "{\"active\":" + String(activeLane) + ",\"mode\":" + String(isAutoMode ? 1 : 0) + ",\"queue\":[";
                            for(int i=0; i<4; i++) {
                                json += String(queue[i]);
                                if(i < 3) json += ",";
                            }
                            json += "],\"lanes\":[";
                            for(int i=0; i<4; i++) {
                                json += "{\"id\":" + String(i+1) + ",\"v\":" + String(laneHasVehicle[i] ? 1 : 0) + ",\"d\":" + String(distances[i]) + "}";
                                if(i < 3) json += ",";
                            }
                            json += "]}";
                            client.print(json);
                        } 
                        // API: Mode Selection Trigger
                        else if (requestLine.indexOf("GET /setmode?m=") >= 0) {
                            int pos = requestLine.indexOf("GET /setmode?m=");
                            isAutoMode = (requestLine.substring(pos + 15, pos + 16) == "1");
                            if (!isAutoMode) {
                                // Flush the queue immediately upon shifting to manual operation
                                for(int i=0; i<4; i++) queue[i] = 0;
                                queueCount = 0;
                            }
                            client.println("HTTP/1.1 200 OK\nContent-Type: text/plain\nConnection: close\n\nOK");
                        }
                        // API: Manual Overdrive Selection Trigger
                        else if (requestLine.indexOf("GET /trigger?l=") >= 0) {
                            int pos = requestLine.indexOf("GET /trigger?l=");
                            int selectedLane = requestLine.substring(pos + 15, pos + 16).toInt();
                            if (!isAutoMode && selectedLane >= 1 && selectedLane <= 4) {
                                activeLane = selectedLane;
                            }
                            client.println("HTTP/1.1 200 OK\nContent-Type: text/plain\nConnection: close\n\nOK");
                        }
                        // Output HTML Interface
                        else {
                            sendDashboard(client);
                        }
                        break;
                    } else {
                        if (currentLine.startsWith("GET")) requestLine = currentLine;
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        client.stop();
    }
}

void sendDashboard(WiFiClient& client) {
    client.println("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n");
    client.print(R"rawliteral(
    <!DOCTYPE html><html><head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Auto Traffic Dashboard</title>
    <style>
        body { font-family: -apple-system, sans-serif; background: #f5f5f7; margin: 0; padding: 20px; display: flex; flex-direction: column; align-items: center; user-select: none; }
        .wrapper { width: 100%; max-width: 400px; }
        h2 { font-weight: 600; color: #1d1d1f; margin-bottom: 20px; text-align: center; }
        .mode-container { background: white; border-radius: 14px; padding: 4px; display: flex; margin-bottom: 20px; box-shadow: 0 2px 8px rgba(0,0,0,0.04); }
        .mode-btn { flex: 1; text-align: center; padding: 10px; border-radius: 10px; font-weight: 600; font-size: 0.9rem; cursor: pointer; transition: all 0.2s; color: #86868b; }
        .mode-btn.active { background: #0071e3; color: white; }
        .lane-card { background: white; border-radius: 14px; padding: 16px; margin-bottom: 12px; display: flex; align-items: center; justify-content: space-between; box-shadow: 0 2px 8px rgba(0,0,0,0.04); transition: transform 0.1s, border 0.2s; border: 2px solid transparent; }
        body.manual-mode .lane-card { cursor: pointer; }
        body.manual-mode .lane-card:active { transform: scale(0.98); }
        .lane-card.active-green { border-color: #34c759; }
        .lane-info { display: flex; align-items: center; gap: 12px; }
        .light-indicator { width: 14px; height: 14px; border-radius: 50%; background: #ff3b30; transition: background 0.3s; }
        .light-indicator.green { background: #34c759; box-shadow: 0 0 8px #34c759; }
        .lane-title { font-weight: 600; color: #1d1d1f; }
        .lane-meta { font-size: 0.85rem; color: #86868b; }
        .vehicle-badge { font-size: 0.75rem; padding: 4px 8px; border-radius: 20px; background: #e5e5ea; color: #1d1d1f; font-weight: 500; }
        .vehicle-badge.present { background: #ffe6e6; color: #ff3b30; }
        .queue-box { background: white; border-radius: 14px; padding: 16px; box-shadow: 0 2px 8px rgba(0,0,0,0.04); margin-top: 20px; text-align: center;}
        .queue-title { font-weight: 600; margin-bottom: 8px; font-size: 0.9rem; color: #1d1d1f; text-transform: uppercase; letter-spacing: 0.5px;}
        #queue-route { font-size: 1.4rem; font-weight: 700; color: #0071e3; letter-spacing: 1px; }
    </style>
    </head><body>
    <div class="wrapper">
        <h2>Smart Traffic</h2>
        <div class="mode-container">
            <div id="btn-auto" class="mode-btn" onclick="setMode(1)">Automatic</div>
            <div id="btn-manual" class="mode-btn" onclick="setMode(0)">Manual</div>
        </div>
        <div id="lanes-container"></div>
        <div id="queue-container" class="queue-box">
            <div class="queue-title">Live Waiting Line Queue</div>
            <div id="queue-route">Empty</div>
        </div>
    </div>
    <script>
        let currentMode = 1;
        function setMode(m) { fetch('/setmode?m=' + m); }
        function triggerLane(id) { if(currentMode === 0) fetch('/trigger?l=' + id); }
        
        function updateDashboard() {
            fetch('/data').then(res => res.json()).then(data => {
                currentMode = data.mode;
                document.getElementById('btn-auto').className = 'mode-btn' + (data.mode ? ' active' : '');
                document.getElementById('btn-manual').className = 'mode-btn' + (!data.mode ? ' active' : '');
                
                if(data.mode) {
                    document.body.classList.remove('manual-mode');
                    document.getElementById('queue-container').style.display = 'block';
                } else {
                    document.body.classList.add('manual-mode');
                    document.getElementById('queue-container').style.display = 'none';
                }

                const container = document.getElementById('lanes-container');
                container.innerHTML = '';
                
                let cleanQueue = data.queue.filter(x => x !== 0);
                document.getElementById('queue-route').innerText = cleanQueue.length > 0 ? cleanQueue.join(' -> ') : "No active queue";

                data.lanes.forEach(lane => {
                    const isGreen = data.active === lane.id;
                    const card = document.createElement('div');
                    card.className = 'lane-card' + (isGreen ? ' active-green' : '');
                    card.onclick = () => triggerLane(lane.id);
                    card.innerHTML = `
                        <div class="lane-info">
                            <div class="light-indicator ${isGreen ? 'green' : ''}"></div>
                            <div>
                                <div class="lane-title">Lane ${lane.id}</div>
                                <div class="lane-meta">${lane.d < 0 ? 'Clear' : lane.d + ' cm'}</div>
                            </div>
                        </div>
                        <div class="vehicle-badge ${lane.v ? 'present' : ''}">${lane.v ? 'Vehicle Arrived' : 'Empty'}</div>
                    `;
                    container.appendChild(card);
                });
            });
        }
        setInterval(updateDashboard, 400);
        updateDashboard();
    </script>
    </body></html>
    )rawliteral");
}
