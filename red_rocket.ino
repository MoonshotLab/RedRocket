const int registers = 3;

const int clockPins[registers] = {2, 5, 8};
const int latchPins[registers] = {3, 6, 9};
const int dataPins[registers] = {4, 7, 10};

byte leds[registers] = {0, 0, 0};

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
  Serial.begin(9600);
  
  startTime = millis();

  for(int i=0; i<registers; i++){
    pinMode(clockPins[i], OUTPUT);
    pinMode(latchPins[i], OUTPUT);
    pinMode(dataPins[i], OUTPUT);
    
    updateShiftRegister(i);
  }
}



void loop() 
{
  timeElapsed = millis() - startTime;
  
  switch(directive){
    case 0:
      buildBase();
      break;
    case 1:
      buildRails();
      break;
  }
}



// Called at the end of each animation method
// resets the timer and selects the next
// directive
void nextDirective()
{
  startTime = millis();
  Serial.println(startTime);

  if(directive < 2)
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
    currentBasePin++;
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
    int railIndex = 1;
    int pinIndex = currentRailPin;
  
    if(currentRailPin > 7){
      railIndex = 2;
      pinIndex = currentRailPin - 8;
    }
  
    bitSet(leds[railIndex], pinIndex);
    updateShiftRegister(railIndex);
    
    currentRailPin++;
    
    if(currentRailPin == 16)
      nextDirective();
  }
}



// Utiltiy to update a shift register
void updateShiftRegister(int index)
{
   digitalWrite(latchPins[index], LOW);
   shiftOut(dataPins[index], clockPins[index], MSBFIRST, leds[index]);
   digitalWrite(latchPins[index], HIGH);
}
