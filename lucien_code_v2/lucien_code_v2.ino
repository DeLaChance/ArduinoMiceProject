
// Description:
// input 1 is a scanner trigger
// input 2 is a lick detector
// output 1 is an odor 1 (red LED)
// output 2 is an odor 2 (yellow LED)
// output 3 is a reward (green LED)

// If input 1 (button 1) is pressed, all LEDs are turned off for 6 seconds,
// then, output 1 (red LED) is turned on for 2 seconds, then off for 6 seconds,
// then, output 2 (yellow LED) is turned on for 2 seconds, then off for 6 seconds,
// if input 2 (button 2) is pressed while red LED is on (+ 2 seconds extra),
// then output 3 (green LED) is turned on for x seconds.

// Enums and classes:
enum LoopState {
  NOT_STARTED,
  FIRST_WAIT,
  RED_LIGHT_ON,
  SECOND_WAIT,
  YELLOW_LIGHT_ON    
};

// Constants:
const int LED1 = 6; //red LED, output 1
const int LED2 = 5; //yellow LED, output 2
const int LED3 = 4; //green LED, output 3
const int BUTTON_1 = 3; //input 1
const int BUTTON_2 = 2; //input 2
const int DELAY_SHORT_MILLIS = 2000;
const int DELAY_MEDIUM_MILLIS = 4000;
const int DELAY_LONG_MILLIS = 6000;
const int COMPLETE_LOOP_MILLIS = 3 * DELAY_MEDIUM_MILLIS + 2 * DELAY_SHORT_MILLIS;
const int MAX_LOOP_ITERATIONS = 4;

// Variable definitions:
int loopCount = 0;
int currentLoopStartTime = -1;
LoopState loopState = NOT_STARTED;

bool led3IsOn = false;
int led3TurnedOnTime = -1;

// Program code:
void setup() {
    pinMode(LED1, OUTPUT); //set LED1 (red) as an output 1
    pinMode(LED2, OUTPUT); //set LED2 (yellow) as an output 2
    pinMode(LED3, OUTPUT); //set LED3 (green) as an output 3
    pinMode(BUTTON_1, INPUT); //set button 1 as an input 1
    pinMode(BUTTON_2, INPUT); //set button 2 as an input 2

    reset();
}

void loop() {

    if (buttonIsPressed(BUTTON_1) && loopState == NOT_STARTED) {
        loopCount = 0;
        loopState = FIRST_WAIT;
        currentLoopStartTime = millis();
    }
 
    if (loopState != NOT_STARTED && loopCount < MAX_LOOP_ITERATIONS) {
      
        int timeInCurrentLoop = (millis() - currentLoopStartTime);

        if (loopState == FIRST_WAIT && timeInCurrentLoop >= DELAY_LONG_MILLIS) {
            turnLedOn(LED1);
            loopState = RED_LIGHT_ON;        
        }

        if (loopState == RED_LIGHT_ON && timeInCurrentLoop >= (DELAY_LONG_MILLIS+DELAY_SHORT_MILLIS)) {
            turnLedOff(LED1);
            loopState = SECOND_WAIT;
        }

        if (loopState == SECOND_WAIT && timeInCurrentLoop >= (2*DELAY_LONG_MILLIS + DELAY_SHORT_MILLIS)) {
            turnLedOn(LED2);
            loopState = YELLOW_LIGHT_ON;
        }

        if (loopState == YELLOW_LIGHT_ON && timeInCurrentLoop >= (2*DELAY_LONG_MILLIS + 2*DELAY_SHORT_MILLIS)) {
            turnLedOff(LED2);

            loopCount += 1;
            loopState = FIRST_WAIT;
            currentLoopStartTime = millis();
        }

        if (buttonIsPressed(BUTTON_2) && timeInCurrentLoop >= DELAY_LONG_MILLIS && timeInCurrentLoop <= (DELAY_LONG_MILLIS + DELAY_MEDIUM_MILLIS)) {
            turnLedOn(LED3);
            led3IsOn = true;
            led3TurnedOnTime = timeInCurrentLoop;
        }

        if (led3IsOn == true && (timeInCurrentLoop - led3TurnedOnTime) >= DELAY_SHORT_MILLIS) {
            turnLedOff(LED3);
            led3IsOn = false; 
            led3TurnedOnTime = -1;
        }         
    }

    if (loopCount == MAX_LOOP_ITERATIONS) {
        loopState = NOT_STARTED;
    }
}

bool buttonIsPressed(int buttonNumber) {
    int buttonState = digitalRead(buttonNumber); 
    return buttonState == HIGH;
}

void turnLedOn(int ledNumber) {
    digitalWrite(ledNumber, HIGH);
}

void turnLedOff(int ledNumber) {
    digitalWrite(ledNumber, LOW); 
}

void reset() {
  
    turnLedOff(LED1);
    turnLedOff(LED2);
    turnLedOff(LED3);  
}
