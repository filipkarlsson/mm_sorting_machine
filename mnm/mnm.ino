/* M&M-sorter
    Use 8 bit-Timer2 for interupt every ms (freq 1kHz)
    for controlling steppers.

    Timer0 is used by Delay() micros() etc
    Timer1 is used by Servo.h

*/
#include "Servo.h" 

// Wheel stepper_pins
#define WHEEL_IN1  8
#define WHEEL_IN2  9
#define WHEEL_IN3  10
#define WHEEL_IN4  11

#define ARM_IN1 4
#define ARM_IN2 5
#define ARM_IN3 6
#define ARM_IN4 7

#define SERVO_PIN 3


unsigned int wheelStepperSteps = 0;
volatile int wheel_dir;

unsigned int armStepperSteps = 0;
volatile int arm_dir;

byte wheelStepperStates = B0001;
byte armStepperStates = B0001;
const byte MASK0 = B0110;
const byte MASK1 = B1100;
const byte MASK2 = B1001;
const byte MASK3 = B0011;

Servo servo;

int wheelSteps;
int armPos;
int armNewPos;
int armSteps;

int color[2];

void setup() {
  wheelSteps = 0;
  armPos = 0;
  armNewPos = 0;
  armSteps = 0;

  Serial.begin(9600);

  pinMode(WHEEL_IN1, OUTPUT);
  pinMode(WHEEL_IN2, OUTPUT);
  pinMode(WHEEL_IN3, OUTPUT);
  pinMode(WHEEL_IN4, OUTPUT);

  pinMode(ARM_IN1, OUTPUT); 
  pinMode(ARM_IN2, OUTPUT); 
  pinMode(ARM_IN3, OUTPUT); 
  pinMode(ARM_IN4, OUTPUT); 

  servo.attach(SERVO_PIN);
  servo.write(93);

  

  // Init interrupts
  cli(); //stop interrupts

  //set timer2 interrupt at 1kHz
  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2  = 0; //initialize counter value to 0

  // set compare match register for 8khz increments
  OCR2A = 255;// = (16*10^6) / (8000*32) - 1 (must be <256) 249

  // turn on CTC mode
  TCCR2A |= (1 << WGM21);

  // Set CS22 bit for 64 prescaler
  TCCR2B |= (1 << CS22);   // TCCR2B = TCCR2B | (1 << CS22)

  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  sei(); //allow interrupts

}//end setup

//timer2 interrupt 1kHz
ISR(TIMER2_COMPA_vect) {

  /*if (armSteps != 0) {
    byte bit0 = (MASK0 & wheelStepperStates) == 0;
    byte bit1 = (MASK1 & wheelStepperStates) == 0;
    byte bit2 = (MASK2 & wheelStepperStates) == 0;
    byte bit3 = (MASK3 & wheelStepperStates) == 0;

    if (armSteps > 0) {
      wheelStepperStates = (bit1) | (bit2 << 1) | (bit3 << 2) | (bit0 << 3);
      armSteps--;
    } else if (armSteps < 0) {
      wheelStepperStates = (bit0) | (bit1 << 1) | (bit2 << 2) | (bit3 << 3);
      armSteps++;
    }

    PORTB = (PORTB & B11110000) | wheelStepperStates;
    wheelStepperSteps += wheel_dir;
    
  }*/
  /*
 * Wheel controll
*/
  if(wheelSteps != 0){
    byte bit0 = (MASK0 & wheelStepperStates) == 0;
    byte bit1 = (MASK1 & wheelStepperStates) == 0;
    byte bit2 = (MASK2 & wheelStepperStates) == 0;
    byte bit3 = (MASK3 & wheelStepperStates) == 0;
    
    if (wheelSteps > 0){
      wheelStepperStates = (bit1) | (bit2 << 1) | (bit3 << 2) | (bit0 << 3);
      wheelSteps--;
    } else if (wheelSteps < 0) {
      wheelStepperStates = (bit0) | (bit1 << 1) | (bit2 << 2) | (bit3 << 3);
      wheelSteps++;
    }
  
    PORTB = (PORTB & B11110000) | wheelStepperStates; // Set bit0-bit3 according to wheelStepperStates (= Pin 8-11 on Arduino)
    wheelStepperSteps += wheel_dir;
  
  }

  
 /*
  * Arm controll
  */

  if(armSteps != 0){
    byte bit0 = (MASK0 & armStepperStates) == 0;
    byte bit1 = (MASK1 & armStepperStates) == 0;
    byte bit2 = (MASK2 & armStepperStates) == 0;
    byte bit3 = (MASK3 & armStepperStates) == 0;
    
    if (armSteps > 0){
      armStepperStates = (bit1) | (bit2 << 1) | (bit3 << 2) | (bit0 << 3);
      armSteps--;
    } else if (armSteps < 0) {
      armStepperStates = (bit0) | (bit1 << 1) | (bit2 << 2) | (bit3 << 3);
      armSteps++;
    }
  
    PORTD = (PORTD & B00001111) | armStepperStates << 4;  // Set bit 7-4 in PORTD according to armStepperStates (Pin 4-7 on Arduino)
    wheelStepperSteps += wheel_dir;
  
  }


}


void loop() {

  if ( ((wheelSteps == 0) && armPos == armNewPos)) {//
    //Get candy color
    if (false) { //check candy color, if white continue to next candy, if wheel havn´t rotated do a wiggle,
      ;
    }

    else {
      int c = (color[0]+1); //the new color
      

      color[1] = color[0];  //updating color
      color[0] = c;

      armNewPos = color[1] * 360 / 8; //if there are 8 bowls

      armSteps =  StepArmCalc();//How many steps the motor have to take

      wheelSteps += 4096 / (4 * 8);
    }
  }

  else if (armSteps == 0 && armPos != armNewPos) {
    armPos = armNewPos;
    wheelSteps += 4096 / (4 * 8);
  }
}

//Calculates how many steps the motor have to take the shortest way to its target
int StepArmCalc() {
  long steps;
  if ((armNewPos - armPos + 360) % 360 < 180)
    steps = ((long)(armNewPos - armPos) * 4096) / 360;
  else
    steps = ((long)(armNewPos - 360 - armPos) * 4096) / 360;
  return (int)steps;
}
