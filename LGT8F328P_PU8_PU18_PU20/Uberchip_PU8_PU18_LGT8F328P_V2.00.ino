/*
  ╔╗░╔╦══╗╔═══╦═══╦═══╦╗░╔╦══╦═══╗░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╔╗║║╔══╣╔═╗║╔═╗║║░║╠╣╠╣╔═╗║░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╚╝╚╣╚══╣╚═╝║║░╚╣╚═╝║║║║╚═╝║░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╔═╗║╔══╣╔╗╔╣║░╔╣╔═╗║║║║╔══╝░░░░░░░░░░░░░░░░░░░░░░░░░
  ║╚═╝║╚═╝║╚══╣║║╚╣╚═╝║║░║╠╣╠╣║░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  ╚═══╩═══╩═══╩╝╚═╩═══╩╝░╚╩══╩╝░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  PU8 / PU18 / PU20 VERSION
  (c) VajskiDs Consoles 2022

  Revision History - Versions prior to V2.00 are for Arduino Nano, INO's exist in the GitHub repository and full history is on the main ReadMe.
  
  V2.00         Re-wrote to suit LG8F328P QFP32 Mini-EVB for PU8 / PU18/ PU20
                Developed Using PU20 (SCPH-7002)  - former dev console was PU18 / SCPH-5502.
                Added easy region select for console magic key selection. 
                Added ALLREGION string option (inject all 3 keys).
                
                This version is a complete 'back to drawing board'. A lot of timing changes were made, in particular the delay between bits was around 300ms below the 4ms requirement,
                though this didn't seem to affect the later models PS1's at all for the LG8F328P version written for those. This delay was adjusted until I hit exactly 4ms between magic- 
                key bits in DSVIEW (slight upgrade on the analyser used originally). I had no luck with getting to the black screen (key acception) at all, until this adjustment was made.
                
                Added a flag to adjust magic key injection quantity between cold boot, or boot with lid open then closing lid (boot from BIOS menu / multi-disc games - extra injections-
                in those cases). The console can now reliably boot from a stand still (from BIOS screen), tested with anti-mod games, and games that take a long time to load (like RE3).
               
                The original initial 7.2s delay (original timing used in arduino nano versions) ends up being around 10seconds.
                The new initial start up delay used of 4.5 seconds (used on this version) ends up closer to 7s.
                
                Uses: 'https://github.com/dbuezas/lgt8fx/discussions/207' (at this time, i don't think a near perfect library exists for this board ...delay timing wise, nor the arduino nano for that case).
                
                The last version of PU18 for Nano's used bit counting instead of complete iterations of magic key. This goes back to counting iterations, with a slight adjustment in quantity used.

                General tidy up - moved some code segments into defines. Adds a lot of readability and makes adjustments a little nicer. You can see how the chip flows, without even understanding code.

                This version doesn't drive the data line high, it reverts to high-z (like used in all later model versions) for the 1's. This means the consoles internal pullup creates 1's and Uberchip 
                only pulls the data line down for the zero's.

                Identified a line of useless code for the low bits, when going between high-z and output, output defaults to LOW, so now, just switching between high-z and output (not telling it to pull down).

                No longer uses if/else statements for the injection. Injection still uses a for loop but now contains a switch statement.
                                
 */



//***************** MAGIC KEYS *****************************
// EUROPE / AUSTRALIA / NEWZEALAND = SCEE
// USA = SCEA
// JP = SCEI
// ALL= ALLREGIONS (this breaks warm boot, but cold boots reliably)
//**********************************************************

#define SELECT_MAGICKEY SCEE                                              //<---------------------------------------------REGION SELECT!! ENTER CONSOLE REGION


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
#define LowBit bitWrite (DATAIO, databit, 1), delayMicroseconds (bitdelay);
#define HighBit bitClear (DATAIO, databit), delayMicroseconds(bitdelay)
#define EndOfMagicKey injectcounter++, delay (stringdelay)
#define StealthModeNotActive (injectcounter < 110 && LoadAtBoot == true || injectcounter < 145 && LoadAtBoot == false );



boolean LoadAtBoot = true;

int injectcounter = 0x00;
int bitdelay (3970);    //delay between bits sent to drive controller (MS)
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
  delay (4500);

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
