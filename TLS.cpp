/*
 * Adaptive Traffic Light System (ATLS)
 * - Sensor input simulation
 * - Adaptive timing based on traffic density
 * - Pedestrian crossing integration
 * - Emergency vehicle detection
 */

// Pin definitions
const int redPins[4] = {13, 10, 5, 2};
const int yellowPins[4] = {12, 9, 6, 3};
const int greenPins[4] = {11, 8, 7, 4};

// Sensor pins (simulated with buttons for prototype)
const int vehicleSensors[4] = {A0, A1, A2, A3};  // Analog pins for vehicle detection
const int pedestrianButtons[4] = {A4, A5, A6, A7}; // Pedestrian crossing requests
const int emergencySensor = A8;                   // Emergency vehicle detection

// Base timing parameters (in ms)
const int BASE_GREEN_TIME = 5000;
const int MIN_GREEN_TIME = 3000;
const int MAX_GREEN_TIME = 10000;
const int YELLOW_TIME = 2000;
const int ALL_RED_TIME = 1000;  // Safety buffer between phases
const int PEDESTRIAN_TIME = 5000;

// System state variables
int laneWeights[4] = {0};  // Traffic density weighting
bool pedestrianRequest[4] = {false};
bool emergencyMode = false;
unsigned long lastEmergencyTime = 0;
const unsigned long EMERGENCY_COOLDOWN = 30000;  // 30 seconds

void setup() {
  // Initialize all traffic light pins as outputs
  for (int i = 0; i < 4; i++) {
    pinMode(redPins[i], OUTPUT);
    pinMode(yellowPins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    
    // Initialize sensor pins
    pinMode(vehicleSensors[i], INPUT);
    pinMode(pedestrianButtons[i], INPUT_PULLUP);
  }
  
  pinMode(emergencySensor, INPUT_PULLUP);
  
  Serial.begin(9600);  // For debugging and monitoring
}

void loop() {
  // Check for emergency vehicles first (highest priority)
  checkEmergency();
  
  if (emergencyMode) {
    handleEmergency();
    return;  // Skip normal operation during emergency
  }
  
  // Read all sensors and update system state
  updateTrafficDensity();
  checkPedestrianRequests();
  
  // Adaptive traffic light cycle
  for (int lane = 0; lane < 4; lane++) {
    if (pedestrianRequest[lane]) {
      handlePedestrianCrossing(lane);
    } else {
      runAdaptiveLightCycle(lane);
    }
  }
}

void checkEmergency() {
  bool emergencyDetected = digitalRead(emergencySensor) == LOW;
  
  if (emergencyDetected && !emergencyMode) {
    emergencyMode = true;
    lastEmergencyTime = millis();
    Serial.println("Emergency vehicle detected - activating priority mode");
  } else if (emergencyMode && (millis() - lastEmergencyTime > EMERGENCY_COOLDOWN)) {
    emergencyMode = false;
    Serial.println("Emergency mode deactivated");
  }
}

void handleEmergency() {
  // Turn all lights red except for one path (simulating emergency route)
  for (int i = 0; i < 4; i++) {
    digitalWrite(greenPins[i], LOW);
    digitalWrite(yellowPins[i], LOW);
    digitalWrite(redPins[i], HIGH);
  }
  
  // Give green to one lane (emergency route)
  digitalWrite(redPins[0], LOW);
  digitalWrite(greenPins[0], HIGH);
  
  delay(10000);  // Extended green for emergency vehicle passage
}

void updateTrafficDensity() {
  for (int i = 0; i < 4; i++) {
    // Read analog sensor (0-1023), higher values indicate more vehicles
    int sensorValue = analogRead(vehicleSensors[i]);
    laneWeights[i] = map(sensorValue, 0, 1023, 0, 100);
    Serial.print("Lane ");
    Serial.print(i);
    Serial.print(" density: ");
    Serial.println(laneWeights[i]);
  }
}

void checkPedestrianRequests() {
  for (int i = 0; i < 4; i++) {
    pedestrianRequest[i] = digitalRead(pedestrianButtons[i]) == LOW;
    if (pedestrianRequest[i]) {
      Serial.print("Pedestrian request detected at lane ");
      Serial.println(i);
    }
  }
}

void handlePedestrianCrossing(int lane) {
  Serial.print("Handling pedestrian crossing at lane ");
  Serial.println(lane);
  
  // Transition to yellow first if currently green
  if (digitalRead(greenPins[lane]) == HIGH) {
    digitalWrite(greenPins[lane], LOW);
    digitalWrite(yellowPins[lane], HIGH);
    delay(YELLOW_TIME);
    digitalWrite(yellowPins[lane], LOW);
  }
  
  // Turn all lanes red for safety
  setAllLightsRed();
  
  // Indicate pedestrian crossing (could add pedestrian lights here)
  // For now, we'll blink the red light
  for (int i = 0; i < 5; i++) {
    digitalWrite(redPins[lane], LOW);
    delay(500);
    digitalWrite(redPins[lane], HIGH);
    delay(500);
  }
  
  pedestrianRequest[lane] = false;
  Serial.println("Pedestrian crossing complete");
}

void runAdaptiveLightCycle(int lane) {
  // Calculate adaptive green time based on traffic density
  int adaptiveGreenTime = calculateAdaptiveGreenTime(lane);
  
  Serial.print("Running adaptive cycle for lane ");
  Serial.print(lane);
  Serial.print(" with green time: ");
  Serial.println(adaptiveGreenTime);
  
  // Transition from red to green
  digitalWrite(redPins[lane], LOW);
  digitalWrite(greenPins[lane], HIGH);
  delay(adaptiveGreenTime);
  
  // Transition to yellow
  digitalWrite(greenPins[lane], LOW);
  digitalWrite(yellowPins[lane], HIGH);
  delay(YELLOW_TIME);
  
  // Transition to red
  digitalWrite(yellowPins[lane], LOW);
  digitalWrite(redPins[lane], HIGH);
  
  // Short all-red phase for safety
  delay(ALL_RED_TIME);
}

int calculateAdaptiveGreenTime(int lane) {
  // Base time plus weighted additional time based on traffic density
  int additionalTime = map(laneWeights[lane], 0, 100, 0, MAX_GREEN_TIME - MIN_GREEN_TIME);
  int totalTime = MIN_GREEN_TIME + additionalTime;
  
  // Constrain to safe limits
  return constrain(totalTime, MIN_GREEN_TIME, MAX_GREEN_TIME);
}

void setAllLightsRed() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(greenPins[i], LOW);
    digitalWrite(yellowPins[i], LOW);
    digitalWrite(redPins[i], HIGH);
  }
}
