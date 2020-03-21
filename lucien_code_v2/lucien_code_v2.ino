// If input 1 (button 1) is pressed, all LEDs are turned off for a seconds,
// then, red or yellow LED is turned on for b +/- x seconds, then off for c +/- y seconds,
// then, red or yellow LED is turned on for b +/- x seconds, then off for c +/- y seconds,
// if input 2 (button 2) is pressed during the lick window (d seconds)
// then green LED is turned on for e seconds.
// Enums and classes:
enum LoopState { // LoopState variable has 7 different values because we declare it as enum (enumeration)
    NOT_STARTED, // state when the button is not pressed
    PRE_LOOP_WAIT, // state when the button is pressed, but the loop hasn't started. Will start after a seconds.
    LOOP_START,   // state to indicate that the loop is allowed to start
    FIRST_LIGHT_ON, // state after a seconds delay is applied and the LED is now on for b seconds
    FIRST_WAIT, // state after the LED is on for b seconds and now c seconds delay is applied
    SECOND_LIGHT_ON, // state after c seconds delay is applied and the LED is now on for b seconds
    SECOND_WAIT // state after the LED is on for b seconds and now c seconds delay is applied
};

enum LightColor {
    NOTHING,
    YELLOW,
    RED  
};

// Constants that won't change:
const int LED1 = 6; // red LED, output 1
const int LED2 = 5; // yellow LED, output 2
const int LED3 = 4; // green LED, output 3
const int BUTTON_1 = 3; // input 1
const int BUTTON_2 = A5; // input 2
const int DELAY_AFTER_BUTTON_1_PRESSED = 6000; // the delay after button 1 is pressed is a seconds (6 seconds)
const int GREEN_LIGHT_ON_DURATION = 1000; // turn on the green light for e seconds (1 second)
const int WINDOW_DURATION = 4000; // window period for d seconds (4 seconds)
const int NUMBER_OF_REPEAT = 3; // the number of repeats in the loop
const int PRINT_RATE_MILLIS = 1000; // to print the values every 100 millis
const int RED_OR_YELLOW_LIGHT_ON_DURATION[] = { 2000, 3000, 4000 }; // turn on the red or yellow light for b seconds +/- x seconds
const int DELAY_BETWEEN_LIGHTS[] = { 6000, 7000, 8000 }; // give a delay between lights for c seconds +/- y seconds
const LightColor WINDOW_LIGHT_COLOR = RED; // choose the color (red or yellow) in which you can press the second button (window period).
const int ANALOG_BUTTON_PRESS_THRESHOLD = 512; // Analog button 2 press threshold, needs to be high

// Variable definitions:
int loopCount = -1; // to declare the iteration number of the loop: 0 is first iteration, (n-1)'th is n'th iteration
unsigned long currentLoopStartTime = 1; // to declare the current loop start time
unsigned long timeInCurrentLoop = 0; // to declare the time in current loop

LoopState loopState = NOT_STARTED; // to declare the loop state
unsigned long loopStateTransitionTimes[5] = { 0, 0, 0, 0, 0 }; // there are 5 loop transitions since the start of the loop

bool greenLightIsOn = false; // to declare the variable while green LED is on
int greenLightTurnedOnTime = 1; // to declare the timing while green LED is on.

bool button2WasPressed = false; // to declare the button 2 was pressed
unsigned long buttonOnePressTime = 0; // to declare the button 1 press time
unsigned long timeSinceProgramStart = 0; // to declare the time since program starts
unsigned long lastPrintTime = 0; // to declare the last print time 
unsigned long printCount = 0; // to declare the print count

int redLightTurnedOnCount = 0; // to declare the red light turned on count
int yellowLightTurnedOnCount = 0; // to declare the yellow light turned on count

LightColor currentLightColor = NOTHING; // to declare the current light light (red or yellow) that is now shining bright like a diamond.
LightColor lastLightColor = NOTHING; // to declare the last light (red or yellow) that was turned on.

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
    timeSinceProgramStart = millis(); // the time since program starts is defined in milliseconds
  
    if (buttonIsPressed(BUTTON_1) && loopState == NOT_STARTED) { // if button 1 is pressed and the loop state is  'not started'
        loopState = PRE_LOOP_WAIT; // the loop state enters 'pre loop wait'
        buttonOnePressTime = timeSinceProgramStart; // the button one press time is the time since program starts
    }
    if (loopState == PRE_LOOP_WAIT && (timeSinceProgramStart - buttonOnePressTime) >= DELAY_AFTER_BUTTON_1_PRESSED) { // if the loop state is in 'pre loop wait' and difference between the time since program starts and the button one press time is bigger than a seconds
        startLoopIteration(); // start the loop iteration        
    }
 
    if (loopHasStarted() && loopCount < NUMBER_OF_REPEAT) { // if the loop has started and the loop count is less than the number of loop repeat
        timeInCurrentLoop = (timeSinceProgramStart - currentLoopStartTime); // then the time in current loop is the time since program starts substracted with the current loop start time
        
        if (loopState == LOOP_START) { // if the loop state enters LOOP_START
            turnRedOrYellowLedOn(); // red or yellow LED is on
            loopState = FIRST_LIGHT_ON; // and the loop state switches to FIRST_LIGHT_ON. The light is on for b seconds       
        }
        if (loopState == FIRST_LIGHT_ON && timeInCurrentLoop >= loopStateTransitionTimes[1]) { // if the loop state enters FIRST_LIGHT_ON and the time in current loop is bigger than state 1
            turnRedOrYellowLightOff(); // red or yellow LED is off
            loopState = FIRST_WAIT; // and the loop state switches to FIRST_WAIT. The first wait lasts for c seconds.
        }
        if (loopState == FIRST_WAIT && timeInCurrentLoop >= loopStateTransitionTimes[2]) { // if the loop state enters FIRST_WAIT and the time in current loop is bigger than state 2
            turnRedOrYellowLedOn(); // red or yellow LED is on
            loopState = SECOND_LIGHT_ON; // and the loop state switches to SECOND_LIGHT_ON. The light is on for b seconds
        }
        if (loopState == SECOND_LIGHT_ON && timeInCurrentLoop >= loopStateTransitionTimes[3]) { // if the loop state enters SECOND_LIGHT_ON and the time in current loop is bigger than state 3
            turnRedOrYellowLightOff(); // red or yellow LED is off
            loopState = SECOND_WAIT; // and the loop state switches to SECOND_WAIT. The second wait lasts for c seconds
        }
        if (loopState == SECOND_WAIT && timeInCurrentLoop >= loopStateTransitionTimes[4]) { // if the loop state enters SECOND_WAIT and the time in current loop is bigger than state 4
            startLoopIteration();// start a new loop iteration
        }
        if (buttonIsPressed(BUTTON_2)) { // if button 2 is pressed 
            button2WasPressed = true; // button 2 was pressed is true
            
            if (lastLightColor == WINDOW_LIGHT_COLOR && greenLightIsOn == false) { // Last time a red or yellow light could be turned on, a red light was chosen.
              
                if (isInFirstWindow() || isInSecondWindow()) { // if it is in the first or second window
                    turnLedOn(LED3); // green LED is on
                    greenLightIsOn = true; // green LED is on is true
                    greenLightTurnedOnTime = timeInCurrentLoop; // the time when green LED is on is now the time in current loop
                }
            }
        }
        if (greenLightIsOn == true && (timeInCurrentLoop - greenLightTurnedOnTime) >= GREEN_LIGHT_ON_DURATION) { // when the green LED is on and the time in current loop substracted with the time when green LED is on is bigger than e seconds
            turnLedOff(LED3); // green LED is off
            greenLightIsOn = false; // green LED is on is false
            greenLightTurnedOnTime =  1; // the green light turned on time is set to 1
        }
    }
    
    printValuesToSerial(); // print the values to serial monitor
    if (loopCount == NUMBER_OF_REPEAT) { // if the number of loop reaches the number of loop repeat
        resetLoop(); // reset the loop
    }
}

bool buttonIsPressed(int buttonNumber) { // when button x is pressed
    if (buttonNumber == BUTTON_1) { 
        return isDigitalButtonPressed(buttonNumber); // button 1 is digital pin
    } else {
        return isAnalogButtonPressed(buttonNumber); // button2 is analog pin
    }
}

bool isDigitalButtonPressed(int buttonNumber) {
    int buttonState = digitalRead(buttonNumber); // then the state of button x is high
    return buttonState == HIGH;  
}

bool isAnalogButtonPressed(int buttonNumber) {
    int rawValue = analogRead(buttonNumber); // raw value is 10-bit value, hence value between 0 and 2^10-1 = 1023
    // To calculate the voltage -> int voltage = rawValue * (5.00 / 1023.00);
    return rawValue >= ANALOG_BUTTON_PRESS_THRESHOLD;  
}
void turnRedOrYellowLedOn() { // when red or yellow light is on
    if (redLightTurnedOnCount == NUMBER_OF_REPEAT) { // if the red light turned on count reaches the number of loop repeat
        turnYellowLedOn(); // turn on yellow LED, as red LED has been turned on max number of times.
    } else if (yellowLightTurnedOnCount == NUMBER_OF_REPEAT) { // else if the yellow light turned on count reaches the number of loop repeat
        turnRedLedOn(); // // turn on red LED, as yellow LED has been turned on max number of times.
    } else {
        long randNumber = random(100); // random number is selected between 0-99
        if (randNumber < 50) { // and if the selected number is less than 50
            turnYellowLedOn(); // yellow light is on        
        } else { // else the red light is on
            turnRedLedOn();
        }
    }  
}

void turnRedLedOn() {
    turnLedOn(LED1); // if LED 1 (red) is turned on
    redLightTurnedOnCount += 1; // increase the count of red
    currentLightColor = RED; // red light is on is true
    lastLightColor = RED; // last light was red is true
}

void turnYellowLedOn() {
    turnLedOn(LED2); // if LED 2 (yellow) is turned on
    yellowLightTurnedOnCount += 1; // increase the count of yellow
    currentLightColor = YELLOW; // yellow light is on is true
    lastLightColor = YELLOW; // last light was red is false
}

void turnLedOn(int ledNumber) { // when LED x is on
    digitalWrite(ledNumber, HIGH); // then the state of LED x is high
}

void turnRedOrYellowLightOff() { // turn the red or yellow light off
  
    if (currentLightColor == RED) { // if the red light is on is true, then turn off the red light
        turnLedOff(LED1);  
    } else if (currentLightColor == YELLOW) { // if the yellow light is on is true, then turn off the yellow light
        turnLedOff(LED2);  
    }

    currentLightColor = NOTHING; // red light is on is false
}

void turnLedOff(int ledNumber) { // when LED x is off
    digitalWrite(ledNumber, LOW);  // then the state of LED x is low
}

bool loopHasStarted() { // when the loop has started
   return loopState != NOT_STARTED && loopState != PRE_LOOP_WAIT;  // return the loop state
}

bool isInFirstWindow() { // first window is when the time in current loop is below d seconds
    return (timeInCurrentLoop <= WINDOW_DURATION); 
}

bool isInSecondWindow() { // second window is when the time in current loop is between state 2 and state 2+d seconds
    return timeInCurrentLoop >= loopStateTransitionTimes[2] && timeInCurrentLoop <= (loopStateTransitionTimes[2] + WINDOW_DURATION);
}

void printValuesToSerial() {
  
    if (loopState != NOT_STARTED) { // if loop state is started
        if (lastPrintTime == 0) { // and if the last print time is 0
            Serial.println("Time in millisecs,Red LED value,Yellow LED value,Green LED value,Button 2 value"); // Then these words are printed 
        }
        int redLedValue = digitalRead(LED1); // to read the red LED value from pin 6
        int yellowLedValue = digitalRead(LED2); // to read the yellow LED value from pin 5
        int greenLedValue = digitalRead(LED3); // to read the green LED value from pin 4
        int button2Value; // to read the button 2 value
        if (button2WasPressed == true) {
            button2Value = 1;  
        } else {
            button2Value = 0;
        }
       unsigned long delta = timeSinceProgramStart - lastPrintTime; // delta is the time since program starts minus the last print time 
        if (delta >= PRINT_RATE_MILLIS) { // if delta is higher than print rate millis (100 ms)
            Serial.print("\b"); // print \b as it is in python configuration
            Serial.print(printCount * PRINT_RATE_MILLIS); // print the time while the loop is running in milliseconds
            Serial.print(","); // give a comma
            Serial.print(redLedValue); // print the red LED value
            Serial.print(","); // give a comma
            Serial.print(yellowLedValue); // print the yellow LED value
            Serial.print(","); // give a comma
            Serial.print(greenLedValue); // print the green LED value
            Serial.print(","); // give a comma
            Serial.println(button2Value); // print the button 2 value
            lastPrintTime = timeSinceProgramStart; // the last print time is the time since program starts
            printCount += 1; // increment the print count by one  
                     
            button2WasPressed = false; // button 2 was pressed is false
        }
    }
}

void resetLoop() { // when the system is reset, all LEDs are off 
    turnLedOff(LED1);
    turnLedOff(LED2);
    turnLedOff(LED3);  
    loopState = NOT_STARTED; // the loop state goes back to not started
    redLightTurnedOnCount = 0; // red light turned on count is 0
    yellowLightTurnedOnCount = 0; // yellow light turned on count is 0 
    lastLightColor = NOTHING;
    currentLightColor = NOTHING;
    
    loopCount = -1; // loop count is -1
    lastPrintTime = 0; // last print time is 0
    printCount = 0; // print count is 0
}

void startLoopIteration() {
    
    loopState = LOOP_START; // when the loop is eligible to start
    loopCount += 1; // loop count is incremented by +1 (another round of loop starts again).
    currentLoopStartTime = timeSinceProgramStart; // and the current loop start time is the time since program start
    
    // Calculate randomly generated transtion times between loop startes relative to the loop start. Loop start is thus 0.
    unsigned long firstWaitStartTime = generateRandomLightDuration(); // 'first wait' starts at this time and  the random light duration is generated
    unsigned long secondLightOnStartTime = firstWaitStartTime + generateRandomDelayBetweenLights(); // 'second light on' starts at this time 
    unsigned long secondWaitStartTime = secondLightOnStartTime + generateRandomLightDuration(); // 'second wait' starts at this time
    unsigned long loopEndTime = secondWaitStartTime + generateRandomDelayBetweenLights(); // loop iteration ends at this time
    loopStateTransitionTimes[1] = firstWaitStartTime;
    loopStateTransitionTimes[2] = secondLightOnStartTime;
    loopStateTransitionTimes[3] = secondWaitStartTime;
    loopStateTransitionTimes[4] = loopEndTime;
}

int generateRandomLightDuration() {
    int arraySize = sizeof(RED_OR_YELLOW_LIGHT_ON_DURATION) / sizeof(RED_OR_YELLOW_LIGHT_ON_DURATION[0]);
    int randomIndex = random(0, arraySize); // Value equals 0, 1 or 2. This is because RED_OR_YELLOW_LIGHT_ON_DURATION has 3 values.
    return RED_OR_YELLOW_LIGHT_ON_DURATION[randomIndex]; // index 0 maps to 8000, 1 to 9000 and 2 to 10000.  
}

int generateRandomDelayBetweenLights() {
    int arraySize = sizeof(DELAY_BETWEEN_LIGHTS) / sizeof(DELAY_BETWEEN_LIGHTS[0]);
    int randomIndex = random(0, arraySize); // Value equals 0, 1 or 2. This is because DELAY_BETWEEN_LIGHTS has 3 values.
    return DELAY_BETWEEN_LIGHTS[randomIndex]; //  index 0 maps to 10000, 1 to 11000 and 2 to 12000. 
}
