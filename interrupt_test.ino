/* M&M-sorter
 *  Use 8 bit-Timer2 for interupt every ms (freq 1kHz)
 *  for controlling steppers. 
 *  
 *  Timer0 is used by Delay() micros() etc
 *  Timer1 is used by Servo.h
 *  
 */

// Wheel stepper_pins
#define WHEEL_IN1  8
#define WHEEL_IN2  9
#define WHEEL_IN3  10
#define WHEEL_IN4  11

#define FORWARD 1
#define BACKWARD -1
#define STOP 0

unsigned int wheelStepperSteps = 0;
volatile int wheel_dir;

byte wheelStepperStates = B1001;
const byte MASK0 = B0110;
const byte MASK1 = B1100;
const byte MASK2 = B1001;
const byte MASK3 = B0011;  

void setup(){
  Serial.begin(9600);
  
  pinMode(WHEEL_IN1, OUTPUT); 
  pinMode(WHEEL_IN2, OUTPUT); 
  pinMode(WHEEL_IN3, OUTPUT); 
  pinMode(WHEEL_IN4, OUTPUT); 



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
ISR(TIMER2_COMPA_vect){

  if(wheel_dir != 0){
    byte bit0 = (MASK0 & wheelStepperStates) == 0;
    byte bit1 = (MASK1 & wheelStepperStates) == 0;
    byte bit2 = (MASK2 & wheelStepperStates) == 0;
    byte bit3 = (MASK3 & wheelStepperStates) == 0;
    
    if (wheel_dir == FORWARD){
      wheelStepperStates = (bit1) | (bit2 << 1) | (bit3 << 2) | (bit0 << 3);
    } else if (wheel_dir == BACKWARD) {
      wheelStepperStates = (bit0) | (bit1 << 1) | (bit2 << 2) | (bit3 << 3);
    }
  
    PORTB = (PORTB & B11110000) | wheelStepperStates;
    wheelStepperSteps += wheel_dir;
  
  }
 
    
}


void loop(){
  wheel_dir = FORWARD;
  delay(1000);
  wheel_dir = STOP;
  delay(1000);
  wheel_dir = BACKWARD;
  delay(1000);
  
}
