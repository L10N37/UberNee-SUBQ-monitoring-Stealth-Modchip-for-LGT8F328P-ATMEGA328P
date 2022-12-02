// Based off PSNEE V7 by Rama
//  WIP, not tuned, not anything
// Purely WIP and will be deleted
// TO DO: WFCK PWM output, connect on PU22+, else tie gate to ground on PS1 mainboard on (PS1 < PU22)
// TO DO: fine Tune hysteresis, get injections working - have had them working on many revisions prior but something wrong here.
// Increasing injection quantity doesn't help so it's safe to say the actual injections are somehow broken, (CHECKED with scope, they are good, maybe i killed this PU20)
// FOR LGT8F328P (big POS)

#define injectpin 4
#define yes true;
#define no false;
#define badread (sqb[0] == 0x00 && sqb[1] == 0x00 && sqb[2] == 0x00 && sqb[3] == 0x00)
#define sqdatastate (bitRead(PINB, sqdatapin) == 1)
#define clk 1
#define sqdatapin 0
#define NOP __asm__ __volatile__("nop\n\t")
#define LowBit bitWrite(DDRD, 4, 1), delayMicroseconds(bitdelay)
#define HighBit bitClear(DDRD, 4), delayMicroseconds(bitdelay)
#define EndOfMagicKey injectcounter++, delay(stringdelay)


int bitdelay(3970);
int stringdelay(67);  //delay between string injections
int injectcounter = 0x00;

const char SCEE[] PROGMEM= "10011010100100111101001010111010010101110100";
const char SCEA[] PROGMEM= "10011010100100111101001010111010010111110100";
const char SCEI[] PROGMEM= "10011010100100111101001010111010010110110100";

uint8_t sqb[12] = { 0xFF };
uint8_t sqbp = 0;
uint8_t hysteresis = 0x00;


//***************** MAGIC KEYS *****************************
// EUROPE / AUSTRALIA / NEWZEALAND = SCEE
// USA = SCEA
// JP = SCEI

//**********************************************************

//***************** Debug Mode *****************************
//      yes / no ? 
bool DEBUG_MODE = yes         //<---------------------------------------------Debug Mode / Fine Tuning 

//**********************************************************

#define SELECT_MAGICKEY SCEE  //<---------------------------------------------REGION SELECT!! ENTER CONSOLE REGION


void setup() {

  DDRB = 0x00;                // Direction register for port B all high-z inputs
  Serial.begin(115200);       // enable serial monitor as a type of debug console
  bitClear(DDRD, injectpin);  // ensure datapin (injectpin) is high-z

    if (DEBUG_MODE) {
      Serial.print("UberNEe is awake!");  // Say hello, confirms reboot
        Serial.print("\n");
          Serial.flush();  // Finish saying hi before moving on :P
            }
}





void print() {  // subq byte capture debug serial printing, also resets the array (required or MCU crashes and bootloops)

  for (int debugprint = 0; debugprint < 12; debugprint++) {
    if (sqb[debugprint] == 0x00 || sqb[debugprint] < 0x10) Serial.print("0");
    Serial.print(sqb[debugprint], HEX);
    Serial.print("  ");
    Serial.flush();
    if (debugprint == 11) Serial.print("\n");
  }
}



void inject() {


    bitWrite(DDRD, injectpin, 1);  // data pin for injections as output
    bitClear(PORTD, injectpin);    // likely not necessary but want to ensure it defaults to low on toggle (it's meant to)

  do {

    for (byte i = 0; i < sizeof(SELECT_MAGICKEY); i++)

      switch (SELECT_MAGICKEY[i]) {
        case '1':
          HighBit;
          break;
        case '0':
          LowBit;
          break;
      }

    EndOfMagicKey;

  } while (injectcounter < 12);
}



void capturepackets() {


  while (sqbp < 12) {

    //noInterrupts();  ////start critical timing section////
    uint8_t TOC = 0x00;

    for (int TOCpos = 0; TOCpos < 8; TOCpos++) {  //capture 8 bits, bits valid as subq clock is high when captured




      /////////////////// Clock Sync /////////////////// 
      interrupts();
      static unsigned long clkstart = millis();
      while (bitRead(PINB, clk) == 1)
              ;
      static unsigned long clkend = millis();
      if (clkend - clkstart > 0) capturepackets();
      /////////////////// Clock Sync ///////////////////
      noInterrupts();

                                                  while (bitRead(PINB, clk) == LOW)
                                                        ;  // just pause here, wait for clock to go high again and grab our bit

            if (sqdatastate) bitWrite(TOC, TOCpos,1);  // write out the captured bit
                else bitClear (TOC, TOCpos);
    }

    sqb[sqbp] = TOC;  //store the captured byte from TOC in full 12 byte SQ capture array
    sqbp++;           // Subq storage byte position in the array, increment one byte per 8 bits captured above until 12 bytes are captured.
  }

  sqbp = 0x00;    // reset the captured byte array index for the next run
        if (badread) {  // reset array index counter and restart capture on a bad read
              Serial.print("   bad read");
                  Serial.flush();
                      capturepackets();
                                        }
}

void _hysteresis() {

  /*
wobble (sqb[0] == 0x01 || datasector) && (sqb[1] == 0x00 && sqb[6] == 0x00))
datasector (sqb[0] == 0x41 || sqb[0] == 0x61)
indicator (sqb[2] == 0xA0 || sqb[2] == 0xA1 || sqb[2] == 0xA2)
indicator_ (sqb[0] == 0x41 && sqb[2] == 0x01) && (sqb[3] >= 0x98) || sqb[3] <= 0x02))
*/

  interrupts();
  bool hysteresisflag = false;


  if (
      (sqb[0] == 0x41 || sqb[0] == 0x61) && (sqb[2] == 0xA0 || sqb[2] == 0xA1 || sqb[2] == 0xA2) || ((sqb[0] == 0x41 && sqb[2] == 0x01) && (sqb[3] >= 0x98) || sqb[3] <= 0x02))

          hysteresisflag = true;

              else if (hysteresis < 0 && (sqb[0] == 0x41 || sqb[0] == 0x61 || sqb[0] == 0x01) && (sqb[1] == 0x00 && sqb[6] == 0x00) && hysteresis > 0x00)
    
                  hysteresisflag = true;



  if (hysteresisflag) hysteresis++;
  else if (!hysteresisflag && hysteresis > 0x00) hysteresis--;
  else hysteresis = 0x00;

  if (DEBUG_MODE) {
      Serial.print("\n");
        Serial.print("Hysteresis value: ");
          Serial.print(hysteresis);
            Serial.print("\n");
              Serial.flush();
}


  if (hysteresis >= 50) {
    interrupts();

      if (DEBUG_MODE){
        Serial.print("INJECT!");
            Serial.flush();
            }

    inject();
    bitClear(DDRD, injectpin);  // keys end in '0' so will return as output, ensure we high-z it again
    hysteresis = hysteresis/3;
    injectcounter = 0x00;
  }
}


void loop() {

  capturepackets();
  if (DEBUG_MODE) print();  // debug print
  _hysteresis();
  
}
