#include <TimerOne.h>

/*
 * Capacitance Meter
 * 
 * Arduino nano
 * 
 * TODO: Add 7segment display
 * TODO: Add 100ohm resistor for discharge and higher capacitances
 * TODO: Add reference voltage to AIN1 (3.15V) or use VREF with 20K resistor to 5V
 * 
 * Principle:
 * 1. Discharge capacitor fully through the smallest resistor available
 * 2. Start charging cap through smallest of the resistors
 * 3. Internal comparator will trigger interupt when cap is charged to voltage higher than the ref
 * 4. Note the time needed to charge cap to 63% of the full voltage
 * 5. Using time and resistor value, calc capacitance
 * 6. If charge time too short try the next res for the new cycle
 * 
 * NOTES:
 * AIN0 = D6
 * AIN1 = D7
 * A0 = measure cap voltage during the discharge
 * D10,D11 = charging resistors
 */

#include "analogComp.h"

#define PINn    10
#define PINu    11
#define PINin   AIN0
#define resN    9890.0F
#define resU    993.0F

unsigned long startTime;
unsigned long elapsedTime;
float microFarads;                // floating point variable to preserve precision, make calculations
float nanoFarads;
volatile bool done, tout;
unsigned int cpin;
float cres;

void measure(){
  done = true;
}

void disable(){
  pinMode(10, INPUT);
  pinMode(11, INPUT);
}

void timeout(){
  tout = true;
}

void switchres(){
  switch (cpin) {
    case PINu: cpin = PINn; cres = resN; break;
    case PINn: cpin = PINu; cres = resU; break;
  }
}

void charge(unsigned int pin){
  disable();
  tout = false;
  pinMode(pin, OUTPUT);
  done = false;
  digitalWrite(pin, HIGH);
  startTime = micros();
  Serial.print("Charging cap via pin: ");
  Serial.println(pin);
  Timer1.initialize(5000000);
  Timer1.attachInterrupt(timeout);
  analogComparator.enableInterrupt(measure, FALLING);

}

void discharge(){
    disable();
    pinMode(PINu, OUTPUT);
    digitalWrite(PINu, LOW);
    Serial.println("Discharging cap");
    while(analogRead(A0) > 0){         // wait until capacitor is completely discharged
    }
}

void setup() {
  // put your setup code here, to run once:
  analogReference(EXTERNAL);
  analogComparator.setOn(INTERNAL_REFERENCE, AIN1);
  //analogComparator.setOn( AIN0, AIN1 );
  Serial.begin(9600); 
  Serial.println("Capacitance measurement");
  discharge();
  cpin = PINu;
  cres = resU;
  charge(cpin);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (tout) {
    analogComparator.disableInterrupt();
    Timer1.stop();
    Timer1.detachInterrupt();
    Serial.println("Failled! 5s timeout!");
    switchres();
    discharge();
    delay(500);
    charge(cpin);
  }
  if (done) {
    // why 4? don't have any idea... it happened when switched to external vref... to be investigated
    elapsedTime =  4*(micros() - startTime);
    done = false;
    Timer1.stop();
    Timer1.detachInterrupt();
    analogComparator.disableInterrupt();
    microFarads = ((float)elapsedTime / cres);
    nanoFarads = ((float)elapsedTime / cres) * 1000.0;
    Serial.print(elapsedTime);       // print the value to serial port
    Serial.print(" uS    "); 
    if (microFarads > 1){
      Serial.print(microFarads);       // print the value to serial port
      Serial.println(" uF");
    }else{
      Serial.print (nanoFarads);
      Serial.println(" nF");
    } 
    discharge();
    if (microFarads < 10.0) {
      cpin = PINn;
      cres = resN;
    } else {
      cpin = PINu;
      cres = resU;     
    }
    delay(500);
    charge(cpin);
  }
}
