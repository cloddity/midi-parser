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
#define TRS             12

/*
// X9C wiring:  pin 3[High Terminal] -- R1 -- pin 5[Wiper] -- R2 -- pin 6[Low Terminal]

// The "X9C_UP" direction refers to the amount of resistance being created between the wiper and the "High" terminal 
// rather than the position of the wiper itself moving up toward the high terminal 
// (which would reduce resistance from wiper to High).
// i.e. setPot(70, false) will set the resistance between the X9C device pins 5 and 3 to 70% of maximum resistance
// where R1 = 70% of max, R2 = 30% of max

//const int debounceInterval = 10;    // debounce time (ms) for button readings

  // update the Bounce instances

  // change potentiometer setting if required based on button presses,
  // storing the setting in the chip if "true" is passed to the pot controller
  */

X9C pot, pot2;                            // instantiate a pot controller
unsigned long prevTime = 0;
unsigned long currentTime = 0;
unsigned long prevVib = 0;
int count = 0;
bool on = false;

int arrSize = 69;
byte notes[] = {126, 66, 80, 73, 66, 126, 66, 80, 66, 126, 66, 80, 105, 97, 126, 66, 80, 73, 66, 126, 66, 80, 59, 46, 59, 66, 24, 19, 24, 46, 66, 97, 105, 149, 126, 66, 80, 73, 66, 46, 66, 80, 126, 105, 97, 66, 126, 66, 80, 73, 66, 126, 66, 80, 97, 137, 105, 149, 137, 126, 115, 105, 80, 66, 59, 46, 59, 80, 115};
int start[] = {0, 187, 187, 187, 54, 507, 187, 187, 187, 562, 187, 187, 187, 562, 1125, 187, 187, 187, 54, 507, 187, 187, 187, 54, 507, 562, 375, 54, 132, 187, 187, 187, 187, 187, 187, 187, 187, 187, 54, 507, 562, 562, 562, 54, 507, 562, 562, 187, 187, 187, 54, 507, 187, 187, 187, 562, 562, 750, 222, 152, 222, 152, 222, 152, 222, 152, 222, 152, 222};
byte finish[] = {35, 37, 37, 13, 96, 41, 37, 37, 108, 41, 38, 37, 108, 219, 46, 38, 37, 13, 96, 40, 38, 37, 13, 96, 111, 63, 29, 24, 37, 37, 38, 37, 37, 37, 37, 38, 37, 13, 96, 111, 113, 112, 17, 96, 111, 113, 41, 37, 37, 13, 97, 40, 37, 37, 108, 113, 159, 39, 29, 45, 30, 45, 29, 45, 29, 46, 29, 45, 29};
byte factor = 5;

unsigned long startVal = start[0];
unsigned long endVal = finish[0] * factor;

bool vib = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRS, OUTPUT);
  
  pot.begin(CS2, INC, UD);
  pot2.begin(CS, INC, UD);
  pot.setPot(0, false);  // set pot wiper to about half way (between 0 and 99)
  pot2.setPot(0, false);
}

void loop() {
  currentTime = millis() - prevTime;

  if(!on && count < arrSize && currentTime >= startVal) {  // play note
    startVal += start[count + 1];
    play(notes[count]);
    on = true;
    //Serial.print("on");
  }
  else if (on && count < arrSize && currentTime >= endVal) {  // off note
    endVal += finish[count + 1] * factor;
    silence();
    on = false;
    count++; 
    //Serial.print("off");
    vib = false;
  }
  
  if (!vib && on && count < arrSize && finish[count] * 5 > 400){  // initiate vibrato
    vib = true;
    prevVib = currentTime;
    Serial.print("on");
  }

  if (vib && on && count < arrSize && (currentTime - prevVib) >= finish[count] * 5 / 2) {
    int i = 0;
    unsigned long localPrevTime = currentTime;
    while (currentTime < prevVib + finish[count] * 5 - 10) {  // activate vibrato
      currentTime = millis();
      if (currentTime - localPrevTime > 24) {
          if (i % 8 < 2 || i % 8 > 5)
                pot.trimPot(1,X9C_DOWN,false);
          else
                pot.trimPot(1,X9C_UP,false);
          i++;
          if (i > 7)
              i = 0;
          localPrevTime = currentTime;
      }
    }
  }
  


  //Serial.print(count);
  if (count >= arrSize - 1) {
    count = 0;
    prevTime = currentTime + 100;
    on = false;
    Serial.print("done");
  }

  { // misc funcs
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
  }

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

/* play(note, len, wait)
void play(int note, int len, int wait) {
  play(note, len);
  digitalWrite(TRS, LOW);
  delay(wait - len);
}*/

void play(int note) {
  digitalWrite(TRS, HIGH);
  setNote(note);
}

void vibrato() {
  int i = 0;
  int prevTime = currentTime;
  
}

void playV(int note, int len, int wait, int vibr, int mode){
  playV(note, len, vibr, mode);
  digitalWrite(TRS, LOW);
  delay(wait - len);
}

// wait = note length, vibr = vibrato start
void playV(int note, int len, int vibr, int mode){
  digitalWrite(TRS, HIGH);
  setNote(note);
  
  //delay(vibr);
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
  digitalWrite(TRS, HIGH);
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

void silence() {
  digitalWrite(TRS, LOW);
}
