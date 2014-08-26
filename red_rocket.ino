// Set up the shift registers
const int registers = 3;

const int clockPins[registers] = {2, 5, 8};
const int latchPins[registers] = {3, 6, 9};
const int dataPins[registers] = {4, 7, 10};

// Store the LED Arrays as byte data
byte leds[registers] = {0, 0, 0};

// Potentiometer for controlling speed
const int potentiometer = A0;

// A general time delay for all animations
int timeDelay = 200;

// Keep track of which method we are we running
int directive = 0;

// Resuable timer, gets reset when calling the
// next directive method
unsigned long startTime;
int timeElapsed;

// Keep track of the pins, each method manipulates
// these vars individually
int currentRailPin = 0;
int currentBasePin = 0;


void setup()
{
  startTime = millis();

  for(int i=0; i<registers; i++){
    pinMode(clockPins[i], OUTPUT);
    pinMode(latchPins[i], OUTPUT);
    pinMode(dataPins[i], OUTPUT);

    updateShiftRegister(i);
  }

  pinMode(potentiometer, INPUT);
}



void loop()
{
  timeElapsed = millis() - startTime;
  
  // get pot value and enforce a minimum timer
  int potValue = analogRead(potentiometer);
  timeDelay = map(potValue, 230, 1024, 0, 1000);
  if(timeDelay < 1) timeDelay = 1;

  switch(directive){
    case 0:
      buildBase();
      break;
    case 1:
      buildRails();
      break;
    case 2:
      killRails();
      break;
    case 3:
      killBase();
      break;
    case 4:
      turnAllOff();
      break;
    case 5:
      turnAllOn();
      break;
    case 6:
      turnAllOff();
      break;
    case 7:
      turnAllOn();
      break;
    case 8:
      turnAllOff();
      break;
  }
}



// Called at the end of each animation method
// resets the timer and selects the next
// directive
void nextDirective()
{
  startTime = millis();

  if(directive < 8)
    directive++;
  else
    directive = 0;
}



// Build the base from the inside out
void buildBase()
{
  if(currentBasePin == 0){
    bitSet(leds[0], 0);
    bitSet(leds[0], 3);
    updateShiftRegister(0);
    currentBasePin = 3;
  } else{
    if(timeElapsed > timeDelay){
      bitSet(leds[0], 1);
      bitSet(leds[0], 2);
      updateShiftRegister(0);
    }
    if(timeElapsed > timeDelay*2){
      nextDirective();
    }
  }
}



// Turns on the rails, one pin at a time
void buildRails()
{
  if(timeElapsed > currentRailPin*timeDelay){
    int railChunk = 1;
    int pinIndex = currentRailPin;

    if(currentRailPin > 7){
      railChunk = 2;
      pinIndex = currentRailPin - 8;
    }

    bitSet(leds[railChunk], pinIndex);
    updateShiftRegister(railChunk);

    currentRailPin++;

    if(currentRailPin == 16)
      nextDirective();
  }
}



// Turn off the rails, one pin at a time
void killRails()
{
  if(timeElapsed > (16-currentRailPin)*timeDelay){
    int railChunk = 1;
    int pinIndex = currentRailPin;

    if(currentRailPin > 7){
      railChunk = 2;
      pinIndex = currentRailPin - 8;
    }

    bitClear(leds[railChunk], pinIndex);
    updateShiftRegister(railChunk);

    currentRailPin--;

    if(currentRailPin < 0){
      currentRailPin = 0;
      nextDirective();
    }
  }
}



// Kill the base from the outside in
void killBase()
{
  if(currentBasePin == 3){
    bitClear(leds[0], 0);
    bitClear(leds[0], 3);
    updateShiftRegister(0);
    currentBasePin = 0;
  } else{
    if(timeElapsed > timeDelay){
      bitClear(leds[0], 1);
      bitClear(leds[0], 2);
      updateShiftRegister(0);
    }

    if(timeElapsed > timeDelay*5)
      nextDirective();
  }
}



// Turn all the pins on at the same time
void turnAllOn()
{
  for(int i=0; i<sizeof(leds); i++){
    for(int j=0; j<8; j++){
      bitSet(leds[i], j);
    }
    
    updateShiftRegister(i);
  }

  if(timeElapsed > timeDelay*5)
    nextDirective();
}



// Turn all the pins off at the same time
void turnAllOff()
{
  for(int i=0; i<sizeof(leds); i++){
    for(int j=0; j<8; j++){
      bitClear(leds[i], j);
    }

    updateShiftRegister(i);
  }  
  
  if(timeElapsed > timeDelay*5)
    nextDirective();
}



// Utiltiy to update a shift register
void updateShiftRegister(int index)
{
   digitalWrite(latchPins[index], LOW);
   shiftOut(dataPins[index], clockPins[index], MSBFIRST, leds[index]);
   digitalWrite(latchPins[index], HIGH);
}

