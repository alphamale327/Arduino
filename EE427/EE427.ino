// ---------------------------------------------------------------------------
// This example code was used to successfully communicate with 15 ultrasonic sensors. You can adjust
// the number of sensors in your project by changing SONAR_NUM and the number of NewPing objects in the
// "sonar" array. You also need to change the pins for each sensor for the NewPing objects. Each sensor
// is pinged at 33ms intervals. So, one cycle of all sensors takes 495ms (33 * 15 = 495ms). The results
// are sent to the "oneSensorCycle" function which currently just displays the distance data. Your project
// would normally process the sensor results in this function (for example, decide if a robot needs to
// turn and call the turn function). Keep in mind this example is event-driven. Your complete sketch needs
// to be written so there's no "delay" commands and the loop() cycles at faster than a 33ms rate. If other
// processes take longer than 33ms, you'll need to increase PING_INTERVAL so it doesn't get behind.
// ---------------------------------------------------------------------------
#include <NewPing.h>

#define leftSig 9
#define rightSig 8
#define soundSig 6

#define SONAR_NUM     1 // Number or sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

int sig;
int debounce;

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(13, 12, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  /*
  NewPing(43, 44, MAX_DISTANCE),
  NewPing(45, 20, MAX_DISTANCE),
  NewPing(21, 22, MAX_DISTANCE),
  NewPing(23, 24, MAX_DISTANCE),
  NewPing(25, 26, MAX_DISTANCE),
  NewPing(27, 28, MAX_DISTANCE),
  NewPing(29, 30, MAX_DISTANCE),
  NewPing(31, 32, MAX_DISTANCE),
  NewPing(34, 33, MAX_DISTANCE),
  NewPing(35, 36, MAX_DISTANCE),
  NewPing(37, 38, MAX_DISTANCE),
  NewPing(39, 40, MAX_DISTANCE),
  NewPing(50, 51, MAX_DISTANCE),
  NewPing(52, 53, MAX_DISTANCE)
  */
};

void setup() {
  Serial.begin(115200);
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
    
  pinMode(leftSig, OUTPUT);
  pinMode(rightSig, OUTPUT); 
  pinMode(soundSig, OUTPUT); 
  Serial.println("EE427 Creative Project");
}

void loop() {
 
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  // The rest of your code would go here.
  if(debounce > 0){
    switch(sig){
      case 1:
        digitalWrite(leftSig, HIGH);
        break;
      case 2:
        digitalWrite(rightSig, HIGH);
        break;
      case 3:
        digitalWrite(soundSig, HIGH);
        break;
      default:
        break;
    }
    debounce--;
  }
  
  if (debounce == 0){
    digitalWrite(leftSig, LOW);
    digitalWrite(rightSig, LOW);
    digitalWrite(soundSig, LOW);      
  }
}


void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    
      if(cm[i] > 0 && cm[i] <=5){
      Serial.print("Left: ");
      Serial.print(cm[i]);
      Serial.println(" cm");
      debounce = 3000;
      sig = 1;
    }else if(cm[i] > 15 && cm[i] <= 20){
      Serial.print("Right: ");
      Serial.print(cm[i]);
      Serial.println(" cm");
      debounce = 3000;
      sig = 2;
    }
  }

  int sensorValue = analogRead(0);
  if(sensorValue > 50){
    Serial.println(sensorValue, DEC);
    debounce = 3000;
    sig = 3;
  }
}
