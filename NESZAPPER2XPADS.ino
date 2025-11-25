#include "Joystick2.h"
#include <Keyboard.h>

// Joystick Definitions 
#define NES
//Connector (Connect also GND and 5V):  CUP, OUT0, D1
const uint8_t inputPinsPort1[] = { 7, 8, 9 };
const uint8_t inputPinsPort2[] = { 2, 3, 4 };

// NES 
#if defined(NES) 
#define CLOCK1 inputPinsPort1[0]
#define LATCH1 inputPinsPort1[1]
#define DATA1  inputPinsPort1[2]

#define CLOCK2 inputPinsPort2[0]
#define LATCH2 inputPinsPort2[1]
#define DATA2  inputPinsPort2[2]

#ifdef NES
#define BITS 8
#endif

#define EVENTS_TOTAL BITS

uint8_t lastStatusPort1[BITS];
uint8_t newStatusPort1[BITS];
uint8_t lastStatusPort2[BITS];
uint8_t newStatusPort2[BITS];


// Direct port bitmasks for NES lines 
uint8_t latch1_bit;
uint8_t clock1_bit;
uint8_t data1_bit;

uint8_t latch2_bit;
uint8_t clock2_bit;
uint8_t data2_bit;

// Port registers for much faster I/O 
volatile uint8_t *latch1_port;
volatile uint8_t *clock1_port;
volatile uint8_t *data1_pin;

volatile uint8_t *latch2_port;
volatile uint8_t *clock2_port;
volatile uint8_t *data2_pin;


void setupJoysticks() {
  pinMode(LATCH1, OUTPUT);
  pinMode(CLOCK1, OUTPUT);
  pinMode(DATA1, INPUT_PULLUP);
  pinMode(LATCH2, OUTPUT);
  pinMode(CLOCK2, OUTPUT);
  pinMode(DATA2, INPUT_PULLUP);

  // Cache direct port registers and masks for faster speed 
  latch1_port = portOutputRegister(digitalPinToPort(LATCH1));
  clock1_port = portOutputRegister(digitalPinToPort(CLOCK1));
  data1_pin   = portInputRegister(digitalPinToPort(DATA1));

  latch1_bit  = digitalPinToBitMask(LATCH1);
  clock1_bit  = digitalPinToBitMask(CLOCK1);
  data1_bit   = digitalPinToBitMask(DATA1);

  latch2_port = portOutputRegister(digitalPinToPort(LATCH2));
  clock2_port = portOutputRegister(digitalPinToPort(CLOCK2));
  data2_pin   = portInputRegister(digitalPinToPort(DATA2));

  latch2_bit  = digitalPinToBitMask(LATCH2);
  clock2_bit  = digitalPinToBitMask(CLOCK2);
  data2_bit   = digitalPinToBitMask(DATA2);
}


// Optimized latch/clock macros using direct port writes 
#define latchlow   (*latch1_port &= ~latch1_bit, *latch2_port &= ~latch2_bit)
#define latchhigh  (*latch1_port |=  latch1_bit, *latch2_port |=  latch2_bit)
#define clocklow   (*clock1_port &= ~clock1_bit, *clock2_port &= ~clock2_bit)
#define clockhigh  (*clock1_port |=  clock1_bit, *clock2_port |=  clock2_bit)

// Faster delay
#define wait delayMicroseconds(2)


void readJoysticks() {
  latchlow;
  clocklow;
  latchhigh;
  wait;
  latchlow;

  // Optimized NES shift-register read loop 
  for (int i = 0; i < BITS; i++) {
    newStatusPort1[i] = ((*data1_pin & data1_bit) ? 1 : 0);
    newStatusPort2[i] = ((*data2_pin & data2_bit) ? 1 : 0);

    clockhigh;
    wait;
    clocklow;
    wait;
  }
}


void interpretJoystickState(uint8_t j, uint8_t *status) {
  Joystick[j].setYAxis(0);
  Joystick[j].setXAxis(0);
  if (!status[4]) Joystick[j].setYAxis(-127); // UP
  if (!status[5]) Joystick[j].setYAxis(127); // DOWN
  if (!status[6]) Joystick[j].setXAxis(-127); // LEFT
  if (!status[7]) Joystick[j].setXAxis(127); // RIGHT
  #ifdef NES
  Joystick[j].setButton(0, !status[3]); // BUTTON1 (Start)
  Joystick[j].setButton(1, !status[2]); // BUTTON2 (Select)
  Joystick[j].setButton(2, !status[0]); // BUTTON3 (A)
  Joystick[j].setButton(3, !status[1]); // BUTTON4 (B)
  #endif
}
#endif



// Keyboard Definitions 
// Define the button and light pin
const int buttonPin = 5; // Pin 5 
const int lightPin  = 6; // Pin 6 

// Variables to hold the state and timing of button 2
bool button2WasPressed = false;
unsigned long button2PressTime = 0;
const unsigned long button2HoldDuration = 12; // milliseconds 

bool button1HasBeenPressed = false; // Tracks if button 1 has been pressed

void setup() {
  // Initialize joystick status arrays (1=OFF, 0=ON)
  for (uint8_t i = 0; i < EVENTS_TOTAL; i++) {
    lastStatusPort1[i] = 1;
    newStatusPort1[i] = 1;
    lastStatusPort2[i] = 1;
    newStatusPort2[i] = 1;
  }

  // Setup joysticks
  setupJoysticks();
  Joystick[0].begin(false);
  Joystick[1].begin(false);

  // Initialize button and light pin for keyboard
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(lightPin, INPUT_PULLUP);

  // Start the Keyboard library
  Keyboard.begin();
}

uint8_t flag1 = 0;
uint8_t flag2 = 0;

void loop() {
  // Read joystick inputs
  readJoysticks();

  // Check for changes in joystick states
  for (uint8_t i = 0; i < EVENTS_TOTAL; i++) {
    if (newStatusPort1[i] != lastStatusPort1[i]) {
      lastStatusPort1[i] = newStatusPort1[i];
      flag1 = 1;
    }
    if (newStatusPort2[i] != lastStatusPort2[i]) {
      lastStatusPort2[i] = newStatusPort2[i];
      flag2 = 1;
    }
  }

  // Interpret and send joystick states if there were changes
  if (flag1) interpretJoystickState(0, newStatusPort1);
  if (flag2) interpretJoystickState(1, newStatusPort2);
  if (flag1) Joystick[0].sendState();
  if (flag2) Joystick[1].sendState();
  flag1 = 0;
  flag2 = 0;

  // Read button states for keyboard
  bool button1State = digitalRead(buttonPin) == LOW;
  bool button2State = digitalRead(lightPin) == HIGH;

  // 5 ms debounce for Button 1
  static bool lastRawButton1 = false;
  static unsigned long lastDebounceTime1 = 0;

  bool rawButton1 = digitalRead(buttonPin) == LOW;

  if (rawButton1 != lastRawButton1) {
      lastDebounceTime1 = millis(); // debounce timer start
  }

  if (millis() - lastDebounceTime1 >= 5) { // debounce duration
      button1State = rawButton1; // stable state becomes actual state
  }

  lastRawButton1 = rawButton1;

  // Track if button 1 has been pressed at least once
  if (button1State) {
    button1HasBeenPressed = true;
  }

  // Handle 'R' key button 1 with 16ms hold time
  static unsigned long button1PressStartTime = 0;
  static bool button1Held = false;

  if (button1State && !button1Held) {
    button1PressStartTime = millis();
    button1Held = true;
    Keyboard.press('r');
  } else if (button1Held && (millis() - button1PressStartTime >= 16)) {
    Keyboard.release('r');
    button1Held = false;
  } else if (!button1State) {
    Keyboard.release('r');
    button1Held = false; // Reset if button is released before ms amount set in button1PressStartTime
  }

  // Handle 'T' key for button 2 only if button 1 has been pressed at least once
  if (button1HasBeenPressed) {
    if (button2State && !button2WasPressed) {
      // Button 2 was just pressed (light pin interrupted)
      Keyboard.press('t');
      button2PressTime = millis(); // Record the time the button was pressed
      button2WasPressed = true;
    }

    // Check if the button 2 hold duration has passed
    if (button2WasPressed && (millis() - button2PressTime >= button2HoldDuration)) {
      Keyboard.release('t');
      button2WasPressed = false;
    }
  }
}
