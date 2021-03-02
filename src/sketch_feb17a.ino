

/*******************************************************************************
       This is an example for using the X9Cxxx digital potentiometers,
       allowing control over the wiper position from min to max, and
       optionally saving the wiper position so it can be recalled
       automatically upon next power up.
       Uses the Bounce2 library (installed through Arduino library manager)
       Requires the X9C potentiometer control library by Phil Bowles
       https://github.com/philbowles/Arduino-X9C
       
       Note:  Changes required to the original X9C.cpp file from the above GitHub
       library (as of Last commit Jun 8 2017)
       The following 2 functions should be replaced with this code:
       
       void X9C::_deselectAndSave(){
        digitalWrite(_inc,LOW);           //***********GADGET REBOOT CHANGED******************
        delayMicroseconds(1);             //***********GADGET REBOOT CHANGED******************
        digitalWrite(_inc,HIGH);          //***********GADGET REBOOT CHANGED******************
        delayMicroseconds(1);             //***********GADGET REBOOT CHANGED******************
        digitalWrite(_cs,HIGH);             // unselect chip and write current value to NVRAM
       }
      void X9C::_stepPot(uint8_t amt,uint8_t dir){
        uint8_t cnt=(amt > X9C_MAX) ? X9C_MAX:amt-1;  //***********GADGET REBOOT CHANGED******************
        digitalWrite(_ud,dir);              // set direction
        digitalWrite(_cs,LOW);              // select chip
        delayMicroseconds(1);
        while(cnt--){
          digitalWrite(_inc,LOW);           // falling pulse triggers wiper change (xN = cnt)
          delayMicroseconds(1);       
          digitalWrite(_inc,HIGH);
          delayMicroseconds(1);
          }
          delayMicroseconds(100);             // let new value settle; (datasheet P7 tIW)
        }
       This code is released to the public domain.
       Gadget Reboot
*******************************************************************************/

#include <X9C.h>                // X9C pot library

#define UD              8      // pot up/down mode pin
#define INC             9      // pot increment pin
#define CS              10      // pot chip select pin
#define CS2             11

// X9C wiring:  pin 3[High Terminal] -- R1 -- pin 5[Wiper] -- R2 -- pin 6[Low Terminal]

// The "X9C_UP" direction refers to the amount of resistance being created between the wiper and the "High" terminal 
// rather than the position of the wiper itself moving up toward the high terminal 
// (which would reduce resistance from wiper to High).
// i.e. setPot(70, false) will set the resistance between the X9C device pins 5 and 3 to 70% of maximum resistance
// where R1 = 70% of max, R2 = 30% of max

//const int debounceInterval = 10;    // debounce time (ms) for button readings

X9C pot, pot2;                            // instantiate a pot controller

void setup() {
  Serial.begin(9600);
  pinMode(12, OUTPUT);
  
  pot.begin(CS2, INC, UD);
  pot2.begin(CS, INC, UD);
  pot.setPot(0, false);  // set pot wiper to about half way (between 0 and 99)
  pot2.setPot(0, false);
}

void loop() {
  /*for (int i = 0; i < 100; i++){
     pot.setPot(i, false);
     delay(200);
  }
  for (int j = 99; j >= 0; j--){
     pot.setPot(j, false);
     delay(200);
  }*/

  /*for (int i = 25; i < 100; i++){
     pot.trimPot(1,X9C_UP,false);
     delay(1000);
  }*/

  /*pot.setPot(0, false); 
  delay(5000);
  for (int j = 0; j <= 99; j++){
     play(j,600);
     if (j % 10 == 0)
        delay(1000);
  }*/
  
  /*
  play(38,500);
  play(99,750);
  play(0,500);
  play(21,250);
  play(38,500);
  play(99,1500); 
  */

  /*pot.setPot(59, true);

  delay(1000);
  pot.setPot(68, true);
  
  delay(2000);*/

  // update the Bounce instances

  // change potentiometer setting if required based on button presses,
  // storing the setting in the chip if "true" is passed to the pot controller
}

void setNote(int note){
  if (note > 99) {
    pot.setPot(99, false);
    pot2.setPot(note - 99, false);
  }
  else {
    pot.setPot(note, false);
    pot2.setPot(0, false);
  }
}

void play(int note, int len, int wait) {
  play(note, len);
  digitalWrite(12, LOW);
  delay(wait - len);
}

void play(int note, int wait) {
  digitalWrite(12, HIGH);
  setNote(note);
  delay(wait);
}

void playV(int note, int len, int wait, int vibr, int mode){
  playV(note, len, vibr, mode);
  digitalWrite(12, LOW);
  delay(wait - len);
}

// wait = note length, vibr = vibrato start
void playV(int note, int len, int vibr, int mode){
  digitalWrite(12, HIGH);
  setNote(note);
  delay(vibr);
  if (mode == 2){
    for (int i = 0; i < (len - vibr)/24; i++) {
       if (i % 8 < 2 || i % 8 > 5)
          pot.trimPot(1,X9C_DOWN,false);
       else
          pot.trimPot(1,X9C_UP,false);
       delay(24);
    }
  }
  else {
    for (int i = 0; i < (len - vibr)/48; i++) {
       if (i % 4 == 0 || i % 4 == 3)
          pot.trimPot(1,X9C_DOWN,false);
       else
          pot.trimPot(1,X9C_UP,false);
       delay(48);
     
    }
  }
}

void playS(int note, int wait, int next, int dur) {
  digitalWrite(12, HIGH);
  pot.setPot(note, false);
  int interval = wait - dur * (note - next);
  Serial.print(interval);
  delay(interval);
  for (int i = 1; i <= note - next; i++) {
    pot.trimPot(1,X9C_DOWN,false);
    delay(dur);
    Serial.print(dur * i);
    
  }
}

void silence(int wait) {
  digitalWrite(12, LOW);
  delay(wait);
}
