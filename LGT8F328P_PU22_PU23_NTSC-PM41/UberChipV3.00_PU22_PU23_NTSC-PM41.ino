/*
  ╔╗░╔╦══╗╔═══╦═══╦═══╦╗░╔╦══╦═══╗░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╔╗║║╔══╣╔═╗║╔═╗║║░║╠╣╠╣╔═╗║░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╚╝╚╣╚══╣╚═╝║║░╚╣╚═╝║║║║╚═╝║░░░░░░░░░░░░░░░░░░░░░░░░░
  ║║░║║╔═╗║╔══╣╔╗╔╣║░╔╣╔═╗║║║║╔══╝░░░░░░░░░░░░░░░░░░░░░░░░░
  ║╚═╝║╚═╝║╚══╣║║╚╣╚═╝║║░║╠╣╠╣║░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  ╚═══╩═══╩═══╩╝╚═╩═══╩╝░╚╩══╩╝░░░░░░░░░░░░░░░░░░░░░░░░░░░░
   PU22/ PU23 / PM41 (PSOne) NTSC version (for PAL PM41 with addional region lock, use Uber D-D)
   (c) VajskiDs Consoles 2022
   
   Revision History:
   
   2021
   Refer to Github for install diagrams @ https://github.com/L10N37/tehUberChip_Another_PSX_Modchip
   V1.1, Installed one today to ship off in this model, why wouldn't I? Spare arduinos on hand, doesn't interfere with the laser after boot and it takes 5 minutes to install.
   A tidy Mayumi installation can take an hour if you're a trying to be *super neat*.
   Gave it a quick test and it seems there was a slight oversight, like the PM41 version I have added genuine disc mode (start with lid open) as it was only working
   with burnt games
   and displaying Sony Computer Entertainment America with the SCEE string on the black Screen for NTSC US games, and the Sony Europe with SCEE string for copied PAL games
   and finally, Sony Computer Entertainment Inc. for Japanese games :)
   V2.00 Used some lower level type code for reading and changing pin states and some general optimisation, added link wire needed for genuine imports
   at this stage, haven't been able to use software to produce this signal from an arduino.
   V2.10 Added support for Genuine and Genuine imports without a genuine mode. The ability to disable the chip still remains.
   The program used to make the calculations spits out a lot of broken code, used a pocket oscilliscope and uploaded a bunch of different settings and desired clock speeds until
   I had something on the scope that was almost a 1:1 match of the WFCK signal. This signal gets turned off once injections are complete.
   V2.1J:  Small timing change, fixes: Legend of Mana (JP), was getting antimod flag 50% of the time.
  2022
  V2.1Jx    
            Re-Wrote to suit LGT8F328P QFP32 Mini-EVB - may suit other boards with the same IC, DATA is now D7, not D12 (needless, but CBF changing it back)
            Uses: 'https://github.com/dbuezas/lgt8fx/discussions/207'
            I haven't bothered adjusting this for the simple region changes, but it's still easy to change regions. Just change the SCEE to your region in all the references in inject(). Managed to shave off 1%, 
            so this uses 5% of storage space :P
            Devved on a PU22 with full wire installation (Reset, Data, DriveLid, WFCK, GND, 3.3v). Removed chip disable mode, there's no reason to ever disable the chip.
            
  V3.00     As per the notes for the version to suit mainboards prior to PU22, multidisc support has being added. The source code for that version is now used as it was neater so
            have just made the adjustments to suit PU22+. Grabbed a smaller batch of LGT8F328P's that were pin compatible with the USB->Serial adapter I had on hand. These are tiny and fit on the mainboard. Reset is 
            no longer optional (the chip would reset along with the console) as it's now tied to D5 with a link wire (D5 to reset on the MCU), don't tie reset to the PS1 mainboard or you won't have multidisc support. 
            The chip locks into a reset loop when the drive lid is open, this was for multi-disc support.
*/





//***************** MAGIC KEYS *****************************
// EUROPE / AUSTRALIA / NEWZEALAND = SCEE
// USA = SCEA
// JP = SCEI
// ALL= ALLREGIONS
//**********************************************************


#define SELECT_MAGICKEY SCEE  //<---------------------------------------------REGION SELECT!! ENTER CONSOLE REGION




#define multiBIT 5
#define databit 7
#define lidbit 0
#define wfckbit 1
#define LIDPORT PORTB
#define LIDIO DDRB
#define DATAPORT PORTD
#define DATAIO DDRD
#define DriveLidClosed (bitRead(LIDPORT, lidbit) == LOW)
#define LowBit bitWrite(DATAIO, databit, 1), delayMicroseconds(bitdelay)
#define HighBit bitClear(DATAIO, databit), delayMicroseconds(bitdelay)
#define EndOfMagicKey injectcounter++, delay(stringdelay)
#define StealthModeNotActive (injectcounter < 75 && DriveLidClosed)
#define RESET pinMode(5, OUTPUT); digitalWrite(5, LOW);

int injectcounter = 0x00;
int bitdelay(3970);   //delay between bits sent to drive controller (MS)
int stringdelay(160);  //delay between string injections


char ALLREGIONS[] = "S10011010100100111101001010111010010101110100S10011010100100111101001010111010010111110100S10011010100100111101001010111010010110110100";
char SCEE[] = "10011010100100111101001010111010010101110100S";
char SCEA[] = "10011010100100111101001010111010010111110100S";
char SCEI[] = "10011010100100111101001010111010010110110100S";


void setup() {

  bitClear (DATAPORT, multiBIT);              // High-Z MultiDisc self reset on lid open 
  bitClear(LIDIO, lidbit);                    // Lid sensor as high-z input
  bitWrite(LIDPORT, wfckbit, 1);                // Gate as output, required for WFCK freq. output, high-z after injections

}


void StealthMode() {

  TCCR1B = 0x18;               // 0001 1000, Disable Timer Clock
  bitClear (DATAIO, databit);  // high-z data line
  bitClear (LIDPORT, wfckbit);   // high-z wfck line
  
  if (digitalRead(8) == LOW) {StealthMode();}       // Lock into a StealthMode loop
  }


void Inject() {


    //Gate Wire output for WFCK frequency, 7.342Khz out of D2 for duration of magic key injections
  TCCR1B = 0x18; // 0001 1000, Disable Timer
  TCCR1A = 0xA2; // 1010 0010
  ICR1 = 4334 - 1;
  OCR1A = (int) (ICR1 * 0.25);
  OCR1B = (int) (ICR1 * 0.50);
  TCNT1 = 0x0;
  TCCR1B |= 1; // Prescale=1, Enable Timer
    //


  bitWrite(DATAIO, databit, 1);  // Data as output
  bitClear(DATAPORT, databit);   // Tie Data Low
  delay(4500);



  do {

    for (byte i = 0; i < sizeof(SELECT_MAGICKEY) - 1; i++)

      switch (SELECT_MAGICKEY[i]) {
        case '1':
          HighBit;
          break;
        case '0':
          LowBit;
          break;
      }

    EndOfMagicKey;
  } while StealthModeNotActive;

  
}



void loop() {

if (digitalRead(8) == HIGH) {RESET}
else if StealthModeNotActive Inject();
else StealthMode();  //Injections finished, disappear into StealthMode

  
}
