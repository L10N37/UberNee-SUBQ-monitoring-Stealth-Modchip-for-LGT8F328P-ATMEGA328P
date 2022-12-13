// Based off PSNEE V7 by Rama
// Boots the hardest game I know of with the most brutal anti-mod -    Legend Of Mana JP. 100% (anti-mod)
//                                                                     Um Jammer Lammy 100% (anti-mod)
//                                                                     Resident Evil 3 100% (Multiple queries, Long load time! final query after you hear the laser skim over a large portion of the disc)
//                                                                     PopoRogue JP    100% (anti-mod, told on twitter by someone who devved a modchip years back. Active regardless of mecha region)
//                                                                     Spyro -Year of the Dragon (can't confirm, was told this hard locks at boot if a chip is flagged, cranked up quantity of injections, no flag, possibly no check on PAL mecha)
//                                                                     Lots of random games, all good :P
//                                                                     Staring at a serial monitor and making sure there are no false flags to inject, all good :P
//                                                                      
//
// IN TESTING - Devved on PU20, haven't checked PU22+, though it was basically copy paste the WFCK stuff from UberChip.
// Went back to the original library and remembered how bad the timing was in dbuezas library (inject bitwidth needs to be ~5150 for example)
// Other libraries won't work at all and just crash out
// FOR LGT8F328P



/*
Based off PSNEE V7 by Rama
 __    __  __                            __    __                     
|  \  |  \|  \                          |  \  |  \                    
| $$  | $$| $$____    ______    ______  | $$\ | $$  ______    ______  
| $$  | $$| $$    \  /      \  /      \ | $$$\| $$ /      \  /      \ 
| $$  | $$| $$$$$$$\|  $$$$$$\|  $$$$$$\| $$$$\ $$|  $$$$$$\|  $$$$$$\
| $$  | $$| $$  | $$| $$    $$| $$   \$$| $$\$$ $$| $$    $$| $$    $$
| $$__/ $$| $$__/ $$| $$$$$$$$| $$      | $$ \$$$$| $$$$$$$$| $$$$$$$$
 \$$    $$| $$    $$ \$$     \| $$      | $$  \$$$ \$$     \ \$$     \
  \$$$$$$  \$$$$$$$   \$$$$$$$ \$$       \$$   \$$  \$$$$$$$  \$$$$$$$
  VajskiDs Consoles                                                                    
                                                                      
 PU22+ -              DATA / SCEx output = DIGITAL PIN 4
                      SUBQ DATA          = DIGITAL PIN 8
                      SUBQ CLOCK         = DIGITAL PIN 9 
                      WFCK / GATE        = DIGITAL PIN 10

                      
 Mainboard <= PU20    DATA / SCEx output = DIGITAL PIN 4
                      SUBQ DATA          = DIGITAL PIN 8
                      SUBQ CLOCK         = DIGITAL PIN 9 
                      WFCK / GATE        = DIGITAL PIN 3 (Best to just tie to ground on the PS1 mainboard, but you can use this pin on the MCU)
*/  



#define wfckpin 2
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


const char SCEE[] = "10011010100100111101001010111010010101110100";
const char SCEA[] = "10011010100100111101001010111010010111110100";
const char SCEI[] = "10011010100100111101001010111010010110110100";

uint8_t sqb[12] = { 0xFF };
uint8_t hysteresis = 0x00;


//***************** MAGIC KEYS *****************************
// EUROPE / AUSTRALIA / NEWZEALAND/ UK = SCEE
// USA = SCEA
// JP = SCEI
//**********************************************************

//***************** Debug Mode *****************************
//      yes / no ? 
const bool DEBUG_MODE = yes         //<---------------------------------------------Debug Mode / Fine Tuning      
//**********************************************************

#define SELECT_MAGICKEY SCEE        //<---------------------------------------------REGION SELECT!! ENTER CONSOLE REGION

int TWEAK_DRIVE = 5;                //<---------------------------------------------Likely won't need adjustment, but tweakable to the level of wear on your disc drive. Default 5.





void setup() {

  DDRB = 0x00;                    // Direction register for port B all high-z inputs
  bitClear(DDRD, injectpin);      // ensure datapin (injectpin) is high-z


  // If using mainboard < PU22 and INSIST on using the MCU to pull gate to ground //
  bitWrite(DDRD, 3, 1);           
  bitClear(PORTD, 3);         
  //                                                                              //


    if (DEBUG_MODE) {
      Serial.begin(115200);                 // enable serial monitor as a type of debug console
        Serial.print("UberNEe is awake!");  // Say hello, confirms reboot
          Serial.print("\n");
            Serial.flush();                 // Finish saying hi before moving on :P
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

    static const int bitdelay(3970);     //  Specific to this library, 4ms bitwidth on scope
    static const int stringdelay(160);   //  delay between string injections
    uint8_t injectcounter = 0x00;
    bitWrite(DDRB, wfckpin, 1);    // Gate as output, required for WFCK freq. output, high-z after injections (WFCK is required for Genuine discs- ONLY USED ON PU22+)
    bitWrite(DDRD, injectpin, 1);  // data pin for injections as output
    bitClear(PORTD, injectpin);    // likely not necessary but want to ensure it defaults to low on toggle (it's meant to)

    
    //Gate Wire output for WFCK frequency, 7.342Khz out of D10 for duration of magic key injections
    TCCR1B = 0x18; // 0001 1000, Disable Timer
    TCCR1A = 0xA2; // 1010 0010
    ICR1 = 4334 - 1;
    OCR1A = (int) (ICR1 * 0.25);
    OCR1B = (int) (ICR1 * 0.50);
    TCNT1 = 0x0;
    TCCR1B |= 1; // Prescale=1, Enable Timer
    //



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

  } while (injectcounter !=4);
}



void capturepackets() {
  uint8_t sqbp = 0;

  while (sqbp < 12) {

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

          if (badread) {  // reset array index counter and restart capture on a bad read
             if (DEBUG_MODE) { 
                  Serial.print("   bad read");
                      Serial.flush();
                            }
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
   bool hysteresisflag_ = false;

  if (
          (sqb[0] == 0x41 || sqb[0] == 0x61) && (sqb[2] == 0xA0 || sqb[2] == 0xA1 || sqb[2] == 0xA2)) 

                    hysteresisflag = true;

                            else if (sqb[0] == 0x41 && sqb[2] == 0x01 && (sqb[3] >= 0x98 || sqb[3] <= 0x02))

                                hysteresisflag_ = true;

                                    else if ((sqb[0] == 0x41 || sqb[0] == 0x61 || sqb[2] == 0x01) && (sqb[1] == 0x00 && sqb[6] == 0x00))
    
                                        hysteresisflag_ = true;



  if (hysteresisflag) hysteresis++;
  else if (hysteresisflag_ && hysteresis > 0x00) hysteresis--;
  else hysteresis =0x00;
  

  if (DEBUG_MODE) {
      Serial.print("\n");
        Serial.print("Hysteresis value: ");
          Serial.print(hysteresis);
            Serial.print("\n");
              Serial.flush();
}


  if (hysteresis >= TWEAK_DRIVE) {
    
          if (DEBUG_MODE){
             Serial.print("\n");
                Serial.print("!!!!!!!!!!!!! INJECT !!!!!!!!!!!!!");
                   Serial.flush();
                         }

    inject();
    bitClear(DDRD, injectpin);  // keys end in '0' so will return as output, ensure we high-z it again
    TCCR1B = 0x18;              // 0001 1000, Disable Timer (disable WFCK output)
    bitClear(DDRB, wfckpin);    // also high-z WFCK freq. output pin
    hysteresis = 0x00;
   }
}


void loop() {

  capturepackets();
  if (DEBUG_MODE) print();  // debug print
  _hysteresis();
  
}
