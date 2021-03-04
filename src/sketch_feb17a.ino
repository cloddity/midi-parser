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
unsigned long currentTime, prevTime = 0;
int count = 0;
bool on = false;

int arrSize = 194;
int notes[] = {35, 59, 97, 188, 126, 66, 35, 149, 80, 46, 126, 66, 35, 66, 24, 66, 126, 66, 35, 149, 80, 46, 126, 66, 35, 66, 24, 66, 126, 66, 35, 149, 80, 46, 97, 46, 19, 97, 97, 46, 115, 59, 29, 115, 59, 29, 126, 66, 35, 126, 66, 126, 149, 80, 46, 149, 80, 46, 174, 97, 59, 46, 59, 97, 59, 66, 80, 35, 10, 1, 10, 35, 80, 10, 97, 115, 59, 29, 19, 29, 59, 29, 115, 59, 126, 66, 35, 149, 80, 46, 126, 66, 35, 66, 24, 66, 126, 66, 35, 149, 80, 46, 126, 66, 35, 66, 24, 66, 59, 19, 97, 59, 19, 97, 66, 24, 105, 66, 24, 66, 80, 35, 126, 80, 35, 80, 66, 24, 105, 66, 24, 66, 59, 19, 97, 59, 19, 97, 66, 24, 105, 66, 24, 66, 80, 35, 10, 1, 10, 35, 80, 10, 97, 115, 59, 29, 19, 29, 59, 115, 115, 59, 29, 115, 59, 115, 126, 66, 35, 149, 80, 46, 126, 66, 35, 66, 24, 66, 126, 66, 35, 149, 80, 46, 126, 66, 35, 66, 24, 66, 126, 66, 35, 149};
int start[] = {0, 750, 1500, 1875, 2250, 3375, 4125, 4500, 5250, 6000, 6750, 7875, 8625, 9000, 9750, 10500, 11250, 12375, 13125, 13500, 14250, 15000, 15750, 16875, 17625, 18000, 18750, 19500, 20250, 21375, 22125, 22500, 23250, 24000, 24750, 25875, 26625, 27000, 27750, 28500, 29250, 30375, 31125, 31500, 32250, 33000, 33750, 34875, 35625, 36000, 36750, 37500, 38250, 39375, 40125, 40500, 41250, 42000, 42750, 43875, 44625, 45000, 45375, 45750, 46125, 46500, 47250, 48375, 49125, 49500, 49875, 50250, 50625, 51000, 51375, 51750, 52875, 53625, 54000, 54375, 54750, 55125, 55500, 55875, 56250, 57375, 58125, 58500, 59250, 60000, 60750, 61875, 62625, 63000, 63750, 64500, 65250, 66375, 67125, 67500, 68250, 69000, 69750, 70875, 71625, 72000, 72750, 73500, 74250, 75375, 76125, 76500, 77250, 78000, 78750, 79875, 80625, 81000, 81750, 82500, 83250, 84375, 85125, 85500, 86250, 87000, 87750, 88875, 89625, 90000, 90750, 91500, 92250, 93375, 94125, 94500, 95250, 96000, 96750, 97875, 98625, 99000, 99750, 100500, 101250, 102375, 103125, 103500, 103875, 104250, 104625, 105000, 105375, 105750, 106875, 107625, 108000, 108750, 109500, 109875, 110250, 111375, 112125, 112500, 113250, 114000, 114750, 115875, 116625, 117000, 117750, 118500, 119250, 120375, 121125, 121500, 122250, 123000, 123750, 124875, 125625, 126000, 126750, 127500, 128250, 129375, 130125, 130500, 131250, 132000, 132750, 133875, 134625, 135000};
int len[] = {625, 1375, 1867, 2242, 3179, 4000, 4304, 5125, 5875, 6625, 7679, 8500, 8804, 9625, 10375, 11125, 12179, 13000, 13304, 14125, 14875, 15625, 16679, 17500, 17804, 18625, 19375, 20125, 21179, 22000, 22304, 23125, 23875, 24625, 25679, 26500, 26804, 27625, 28375, 29125, 30179, 31000, 31304, 32125, 32875, 33625, 34679, 35500, 35804, 36625, 37375, 38125, 39179, 40000, 40304, 41125, 41875, 42625, 43679, 44500, 44992, 45367, 45742, 46117, 46492, 47125, 48179, 49000, 49492, 49867, 50242, 50617, 50992, 51367, 51742, 52679, 53500, 53992, 54367, 54742, 55117, 55492, 55867, 56242, 57179, 58000, 58304, 59125, 59875, 60625, 61679, 62500, 62804, 63625, 64117, 65125, 66179, 67000, 67304, 68125, 68875, 69625, 70679, 71500, 71804, 72625, 73117, 74125, 75179, 76000, 76304, 77125, 77875, 78625, 79679, 80500, 80804, 81625, 82375, 83125, 84179, 85000, 85304, 86125, 86875, 87625, 88679, 89500, 89804, 90625, 91375, 92125, 93179, 94000, 94304, 95125, 95875, 96625, 97679, 98500, 98804, 99625, 100375, 101125, 102179, 103000, 103492, 103867, 104242, 104617, 104992, 105367, 105742, 106679, 107500, 107992, 108625, 109375, 109867, 110054, 111179, 112000, 112492, 113125, 113875, 114625, 115679, 116500, 116804, 117625, 118375, 119125, 120179, 121000, 121304, 122125, 122875, 123625, 124679, 125500, 125804, 126625, 127375, 128125, 129179, 130000, 130304, 131125, 131875, 132625, 133679, 134500, 134804, 135625};
int vib[] = {313, 1063, 1867, 2242, 2715, 3688, 4304, 4813, 5563, 6313, 7215, 8188, 8804, 9313, 10063, 10813, 11715, 12688, 13304, 13813, 14563, 15313, 16215, 17188, 17804, 18313, 19063, 19813, 20715, 21688, 22304, 22813, 23563, 24313, 25215, 26188, 26804, 27313, 28063, 28813, 29715, 30688, 31304, 31813, 32563, 33313, 34215, 35188, 35804, 36313, 37063, 37813, 38715, 39688, 40304, 40813, 41563, 42313, 43215, 44188, 44992, 45367, 45742, 46117, 46492, 46813, 47715, 48688, 49492, 49867, 50242, 50617, 50992, 51367, 51742, 52215, 53188, 53992, 54367, 54742, 55117, 55492, 55867, 56242, 56715, 57688, 58304, 58813, 59563, 60313, 61215, 62188, 62804, 63313, 64117, 64813, 65715, 66688, 67304, 67813, 68563, 69313, 70215, 71188, 71804, 72313, 73117, 73813, 74715, 75688, 76304, 76813, 77563, 78313, 79215, 80188, 80804, 81313, 82063, 82813, 83715, 84688, 85304, 85813, 86563, 87313, 88215, 89188, 89804, 90313, 91063, 91813, 92715, 93688, 94304, 94813, 95563, 96313, 97215, 98188, 98804, 99313, 100063, 100813, 101715, 102688, 103492, 103867, 104242, 104617, 104992, 105367, 105742, 106215, 107188, 107992, 108313, 109063, 109867, 110054, 110715, 111688, 112492, 112813, 113563, 114313, 115215, 116188, 116804, 117313, 118063, 118813, 119715, 120688, 121304, 121813, 122563, 123313, 124215, 125188, 125804, 126313, 127063, 127813, 128715, 129688, 130304, 130813, 131563, 132313, 133215, 134188, 134804, 135313};


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
  
  
  
  if(!on && count < arrSize && currentTime >= start[count]) {  // play note
    
    play(notes[count]);
    on = true;
    //Serial.print("on");
  }
  else if (on && count < arrSize && currentTime >= len[count]) {
    silence();
    on = false;
    count++; 
    //Serial.print("off");
  }
  //else if (on && count < arrSize && currentTime >= vib[count]){
    //vibrato();
  //}

Serial.println(currentTime);
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
  while (currentTime < len[count]) {
    currentTime = millis();
    if (currentTime - prevTime > 24) {
        if (i % 8 < 2 || i % 8 > 5)
              pot.trimPot(1,X9C_DOWN,false);
        else
              pot.trimPot(1,X9C_UP,false);
        i++;
        if (i > 7)
            i = 0;
        prevTime = currentTime;
    }
  }
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
