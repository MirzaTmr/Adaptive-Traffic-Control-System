//works with the arduino as the variables are functional then
//added gates at the end for improved functionality


//use any arduino + LED + 2200Ohm Resistor and breadboard
//Pin definition
int redPins[4] = {13, 10, 5, 2};
int yellowPins[4] = {12, 9, 6, 3};
int greenPins[4] = {11, 8, 7, 4};

//Time for lights, in ms
int redTime = 5000;    
int yellowTime = 2000; 
int greenTime = 5000;  

void setup() {

    for (int i = 0; i < 4; i++) {
        pinMode(redPins[i], OUTPUT);
        pinMode(yellowPins[i], OUTPUT);
        pinMode(greenPins[i], OUTPUT);
    }
}

void loop() {
    for (int lane = 0; lane < 4; lane++) {
        setTrafficLight(lane);
    }
}

    //Function for lanes
void setTrafficLight(int lane) {
    // Turn off all lanes
    for (int i = 0; i < 4; i++) {
        digitalWrite(greenPins[i], LOW);
        digitalWrite(yellowPins[i], LOW);
        digitalWrite(redPins[i], HIGH);
    }

    //Turn green ON
    digitalWrite(greenPins[lane], HIGH);
    digitalWrite(redPins[lane], LOW);
    
    //Green light timer
    delay(greenTime);
    
    //green OFF
    digitalWrite(greenPins[lane], LOW);
    
    //Turn Yellow ON
    digitalWrite(yellowPins[lane], HIGH);
    delay(yellowTime);
    
    //Yellow OFF
    digitalWrite(yellowPins[lane], LOW);
}