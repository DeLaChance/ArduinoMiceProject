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
// then output 3 (green LED) is turned on for 2 seconds.

// Enums and classes:
enum LoopState { // LoopState variable has 5 different values because we declare it as enum (enumeration)
    NOT_STARTED, // state when the button is not pressed
    PRE_LOOP_WAIT, // state when the button pressed, but the loop hasn't started. Will start after 6 seconds.
    LOOP_START,   // state indicating the loop is eligible to start
    FIRST_LIGHT_ON, // state after the 6 seconds delay is applied and the red LED is now on for 2 seconds
    FIRST_WAIT, // state after the button is pressed and the 6 seconds delay is applied
    SECOND_LIGHT_ON, // state after the 6 seconds delay is applied and the yellow LED is now on for 2 seconds
    SECOND_WAIT // state after the red LED is on and now the 6 seconds delay is applied
};

// Constants that won't change:
const int LED1 = 6; // red LED, output 1
const int LED2 = 5; // yellow LED, output 2
const int LED3 = 4; // green LED, output 3
const int BUTTON_1 = 3; // input 1
const int BUTTON_2 = 2; // input 2
const int DELAY_SHORT_MILLIS = 2000; // give a delay of 2 seconds
const int DELAY_MEDIUM_MILLIS = 4000; // give a delay of 4 seconds
const int DELAY_LONG_MILLIS = 6000; // give a delay of 6 seconds
const int MAX_LOOP_ITERATIONS = 2; // the maximum number of repeat in the loop is 5 times
const int COMPLETE_LOOP_MILLIS = 2 * DELAY_LONG_MILLIS + 2 * DELAY_SHORT_MILLIS;
const int PRINT_RATE_MILLIS = 2000;

// Variable definitions:
int loopCount = 0; // to declare the number of loop
unsigned long currentLoopStartTime = 1; // to declare the current loop start time
LoopState loopState = NOT_STARTED; // to declare the loop state

bool led3IsOn = false; // to declare the variable while LED 3 is on
int led3TurnedOnTime = 1; // to declare the timing while LED 3 is on.

unsigned long buttonOnePressTime = 0;
unsigned long timeSinceProgramStart = 0;
unsigned long lastPrintTime = 0;

int redLightTurnedOnCount = 0;
int yellowLightTurnedOnCount = 0;
bool redLightIsOn = false;
bool yellowLightIsOn = false;

// Program code:
void setup() {

    Serial.begin(9600); // to see values from Arduino and present it on the computer
                                 
    pinMode(LED1, OUTPUT); // set LED1 (red) as an output 1
    pinMode(LED2, OUTPUT); // set LED2 (yellow) as an output 2
    pinMode(LED3, OUTPUT); // set LED3 (green) as an output 3
    pinMode(BUTTON_1, INPUT); // set button 1 as an input 1
    pinMode(BUTTON_2, INPUT); // set button 2 as an input 2

    // if analog input pin 0 is unconnected, random analog
    // noise will cause the call to randomSeed() to generate
    // different seed numbers each time the sketch runs.
    // randomSeed() will then shuffle the random function.
    randomSeed(analogRead(0));

    resetLoop(); // reset all conditions
}

void loop() {

    timeSinceProgramStart = millis();
  
    if (buttonIsPressed(BUTTON_1) && loopState == NOT_STARTED) { 
        // if button 1 is pressed and the loop state is 'not started'
        printValuesToSerial();
        loopState = PRE_LOOP_WAIT;
        buttonOnePressTime = timeSinceProgramStart;
    }

    if (loopState == PRE_LOOP_WAIT && (timeSinceProgramStart - buttonOnePressTime) >= DELAY_LONG_MILLIS) {
        printValuesToSerial();
        loopCount = 0; // then the loop count is set to 0
        loopState = LOOP_START; // and the loop is eligible to start
        currentLoopStartTime = timeSinceProgramStart; // and the current loop start time begins and is set to miliseconds
    }
 
    if (loopHasStarted() && loopCount < MAX_LOOP_ITERATIONS) { 
        printValuesToSerial();
      
        // if the loop state 'starts' and the number of loop count is less than the max number of loop repeat
        int timeInCurrentLoop = (timeSinceProgramStart - currentLoopStartTime); // then the time in current loop is calculated from the number of current miliseconds substracted with the miliseconds of the starting loop

        if (loopState == LOOP_START) {
            turnRedOrYellowLedOn(); // red LED is on
            loopState = FIRST_LIGHT_ON;        
        }

        if (loopState == FIRST_LIGHT_ON && timeInCurrentLoop >= DELAY_SHORT_MILLIS) { 
            turnRedOrYellowLightOff();
            loopState = FIRST_WAIT; // and the loop state switches to 'first wait'. The first wait lasts for 6 seconds.
        }

        if (loopState == FIRST_WAIT && timeInCurrentLoop >= (DELAY_LONG_MILLIS + DELAY_SHORT_MILLIS)) { // when the loop state is 'first wait' and the time in current loop is bigger than 8 seconds
            turnRedOrYellowLedOn(); // yellow LED is on
            loopState = SECOND_LIGHT_ON; // and the loop state switches to 'yellow light on'. The yellow LED is on for 2 seconds
        }

        if (loopState == SECOND_LIGHT_ON && timeInCurrentLoop >= (DELAY_LONG_MILLIS + 2*DELAY_SHORT_MILLIS)) { // when the loop state is 'second light on' and the time in current loop is bigger than 10 seconds
            turnRedOrYellowLightOff();
            loopState = SECOND_WAIT;
        }

        if (loopState == SECOND_WAIT && timeInCurrentLoop >= COMPLETE_LOOP_MILLIS) { // loop is completed
            loopCount += 1; // loop count is incremented by +1 (another round of loop starts again).
            loopState = LOOP_START; // loop goes back to 'first wait' state
            currentLoopStartTime = millis(); // and the current loop start time begins again and is set to miliseconds
        }

        if (buttonIsPressed(BUTTON_2) && timeInCurrentLoop <= DELAY_MEDIUM_MILLIS) { // if button 2 is pressed and the time in current loop is below 4 seconds
            turnLedOn(LED3); // green LED is on
            led3IsOn = true; // green LED on is true
            led3TurnedOnTime = timeInCurrentLoop; // the time when green LED is on is now the time in current loop
        }

        if (led3IsOn == true && (timeInCurrentLoop - led3TurnedOnTime) >= DELAY_SHORT_MILLIS) { // when the green LED is on and the time in current loop substracted with the time when green LED is on is bigger than 2 seconds
            turnLedOff(LED3); // green LED is off
            led3IsOn = false; // green LED on is false
            led3TurnedOnTime =  1; // the time when green LED is on goes back to the original
        }         
    }

    if (loopCount == MAX_LOOP_ITERATIONS) { // if the number of loop reaches the maximum number of loop repeat
        resetLoop();
    }
}

bool buttonIsPressed(int buttonNumber) { // when button x is pressed
    int buttonState = digitalRead(buttonNumber); // then the state of button x is high
    return buttonState == HIGH;
}

void turnRedOrYellowLedOn() {

    if (redLightTurnedOnCount == MAX_LOOP_ITERATIONS) {
        turnYellowLedOn(); // turn on yellow led, as red led has been turned on max number of times.
    } else if (yellowLightTurnedOnCount == MAX_LOOP_ITERATIONS) {
        turnRedLedOn(); // // turn on red led, as red led has been turned on max number of times.
    } else {
        long randNumber = random(100);
        if (randNumber < 50) {
            turnYellowLedOn();         
        } else {
            turnRedLedOn();
        }
    }
    
}

void turnRedLedOn() {
    turnLedOn(LED1); 
    redLightTurnedOnCount += 1; // increase the count of red
    redLightIsOn = true;
}

void turnYellowLedOn() {
    turnLedOn(LED2); 
    yellowLightTurnedOnCount += 1; // increase the count of yellow
    yellowLightIsOn = true;
}

void turnLedOn(int ledNumber) { // when LED x is on
    digitalWrite(ledNumber, HIGH); // then the state of LED x is high
}

void turnRedOrYellowLightOff() {
    if (redLightIsOn == true) {
        turnLedOff(LED1);  
        redLightIsOn = false;
    }

    if (yellowLightIsOn == true) {
        turnLedOff(LED2);  
        yellowLightIsOn = false;
    }    
}

void turnLedOff(int ledNumber) { // when LED x is off
    digitalWrite(ledNumber, LOW);  // then the state of LED x is low
}

bool loopHasStarted() {
   return loopState != NOT_STARTED && loopState != PRE_LOOP_WAIT;  
}

void printValuesToSerial() {

    if (loopState == NOT_STARTED) {
        Serial.println("Button press"); // Then these words are printed 
    } else if (loopState == PRE_LOOP_WAIT) {
        Serial.println("Loop will start");
        Serial.println("Time in millisecs,Red LED value,Yellow LED value,Green LED value,Button 2 value"); // Then these words are printed 
    } else { 
        int redLedValue = digitalRead(LED1); // to read the red LED value from pin 6
        int yellowLedValue = digitalRead(LED2); // to read the yellow LED value from pin 5
        int greenLedValue = digitalRead(LED3); // to read the green LED value from pin 4
        int button2Value = digitalRead(BUTTON_2); // to read the button 2 value from pin 2
    
        if(lastPrintTime != timeSinceProgramStart && (timeSinceProgramStart % PRINT_RATE_MILLIS) == 0) {
            Serial.print("\b"); // print \b as it is in python configuration
            Serial.print(timeSinceProgramStart); // print the time while the loop is running in milliseconds
            Serial.print(","); // give a comma
            Serial.print(redLedValue); // print the red LED value
            Serial.print(","); // give a comma
            Serial.print(yellowLedValue); // print the yellow LED value
            Serial.print(","); // give a comma
            Serial.print(greenLedValue); // print the green LED value
            Serial.print(","); // give a comma
            Serial.println(button2Value); // print the button 2 value

            lastPrintTime = timeSinceProgramStart;
        }
    }
}

void resetLoop() { // when the system is reset, all LEDs are off 
    turnLedOff(LED1);
    turnLedOff(LED2);
    turnLedOff(LED3);  

    loopState = NOT_STARTED;
    redLightTurnedOnCount = 0;
    yellowLightTurnedOnCount = 0;    
    
    redLightIsOn = false;
    yellowLightIsOn = false;    
    loopCount = 0;
}
