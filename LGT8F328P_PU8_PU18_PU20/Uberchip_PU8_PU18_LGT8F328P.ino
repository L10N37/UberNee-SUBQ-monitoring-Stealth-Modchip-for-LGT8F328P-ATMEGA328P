/*
  ╔╗░╔╦══╗╔═══╦═══╦═══╦╗░╔╦══╦═══╗░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╔╗║║╔══╣╔═╗║╔═╗║║░║╠╣╠╣╔═╗║░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╚╝╚╣╚══╣╚═╝║║░╚╣╚═╝║║║║╚═╝║░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╔═╗║╔══╣╔╗╔╣║░╔╣╔═╗║║║║╔══╝░░░░░░░░░░░░░░░░░░░░░░░░░
  ║╚═╝║╚═╝║╚══╣║║╚╣╚═╝║║░║╠╣╠╣║░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  ╚═══╩═══╩═══╩╝╚═╩═══╩╝░╚╩══╩╝░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  PU8 / PU18 / PU20 VERSION
  (c) VajskiDs Consoles 2022
  V2.00 [WIP]   Re-wrote to suit LG8F328P QFP32 Mini-EVB for PU8 / PU18/ PU20 - added easy region select for console magic key selection.
                Added a flag to adjust magic key injection quantity between cold boot, or boot with lid open / close lid / multi-disc games (extra injections in those cases)
                I have given up on this for now, when it's not broken i will remove the WIP and it will be a proper release.
                I don't have the motivation to get my logic analyser out and start probing points with meters and scopes.
                All the new ideas are here, these LGT8F328P's are a pain in the dick
                Even though there's more code and logic its down to 3% of program space
                Using PU20 (SCPH-7002) atm
  v2.1 [WIP]    Still broken, haven't touched the hardware ...just tied up the code for easy adjustments in defines. Combined all keys into a define selection
                which allows a single FOR loop.
  v2.2 [WIP]    Just some late night relaxation, changed the inject routine to a switch case. Maybe, eventually, it might even be looked at properly with                 a logic analyser and get it diagnosed / fixed, also added ALLREGIONS Injection.

Update: So i hooked this up to a logic analyzer and well ...nothing happens. It just sits there - doing *nothing*
I'm not even sure how the heck the other version for this MCU works (for PU22+) because that was blind coded with no signal analysis - I just worked out why it wasn't working with a switch. 

this MCU isn't liking having it's pins set up directly with the lower level bitwrite functions that directly manipulate the pin registers , but once again, how is the other version working?

When using 
    pinMode(7, OUTPUT);    
    digitalWrite(7, HIGH); 

As the first part of setup, I get some action, D7 actually goes high for 7.2 seconds.
Anyway, I'm still stumped on how the other version possibly works. I'm in no rush to fix this, I just wanted to know why it wasn't working.

  v2.3        Looks like i forgot to put the ' ' around the 1's and 0's. Case statement was searching for integers instead of characters.
              It's looking good on the logic analyzer. Screenshot attached. No reason it wouldn't work in the console. I may make slight
              changes to this and not update the version number (injection quantity with LoadAtBoot Flag falsed). You also can't use the 
              ALL regions injection without dividing the inject counter by 3 or you will get flagged on anti mod games.
*/



//***************** MAGIC KEYS *****************************
// EUROPE / AUSTRALIA / NEWZEALAND = SCEE
// USA = SCEA
// JP = SCEI
// ALL= ALLREGIONS
//**********************************************************

#define SELECT_MAGICKEY SCEE                                                //<---------------------------------------------REGION SELECT!! ENTER CONSOLE REGION


#define databit 7
#define lidbit 0
#define gatebit 2
#define LIDPORT PORTB
#define LIDIO DDRB
#define DATAPORT PORTD
#define DATAIO DDRD
#define LidOpenedProcess do{;}while (bitRead (LIDPORT, lidbit) == 1)
#define LidClosedProcess do{;}while (bitRead (LIDPORT, lidbit) == 0)
#define DriveLidOpen (bitRead (LIDPORT, lidbit) == 1)
#define LowBit  bitClear (DATAPORT, databit), delayMicroseconds (bitdelay)
#define HighBit bitWrite(DATAPORT, databit, 1), delayMicroseconds(bitdelay)
#define EndOfMagicKey injectcounter++, delay (stringdelay)
#define StealthModeNotActive (injectcounter < 100 && LoadAtBoot == true || injectcounter < 135 && LoadAtBoot == false );



boolean LoadAtBoot = true;

int injectcounter = 0x00;
int bitdelay (3794);    //delay between bits sent to drive controller (MS)
int stringdelay (65);  //delay between string injections


char ALLREGIONS[] = "S10011010100100111101001010111010010101110100S10011010100100111101001010111010010111110100S10011010100100111101001010111010010110110100";
char SCEE[] = "10011010100100111101001010111010010101110100S";
char SCEA[] = "10011010100100111101001010111010010111110100S";
char SCEI[] = "10011010100100111101001010111010010110110100S";


void setup() {

  bitWrite (DATAIO, databit, 1);    // Data as output
  bitClear (DATAPORT, databit);     // Tie Data Low
  bitClear (LIDIO, lidbit);         // Lid sensor as high-z input
  bitWrite (LIDIO, gatebit, 1);     // Gate as output
  bitClear (LIDPORT, gatebit);      // Gate tied LOW entire duration
  delay (7200);

}


void NewDisc() {

  LidOpenedProcess;       // Do 'nothing'while the lid is open between disc swaps
  Inject();               // Then jump to corresponding inject routine via switch case in Region()

}


void DriveLidStatus() {

  bitClear (DATAIO, databit);  // high-z data line
  injectcounter = 0;           // reset magic key inject counter
  LoadAtBoot = false;          // False the LoadAtBoot flag, the lids being opened. We aren't booting a disc from cold boot.
  LidClosedProcess;            // Do 'nothing'while the lid is closed
  NewDisc();                   // Jump to NewDisc() once the lid is opened

}


void Inject() {

  do
  {

    for (byte i = 0; i < sizeof(SELECT_MAGICKEY) - 1; i++)

      switch (SELECT_MAGICKEY[i]) {
        case '1':
          HighBit;
          break;
        case '0':
          LowBit;
          break;
        default:
          if (DriveLidOpen) {
            DriveLidStatus();
          }

          EndOfMagicKey;
      }

  }
  while StealthModeNotActive;
}



void loop()
{
  if (injectcounter >= 100) {
    DriveLidStatus();
  }

  
    Inject();
  }
