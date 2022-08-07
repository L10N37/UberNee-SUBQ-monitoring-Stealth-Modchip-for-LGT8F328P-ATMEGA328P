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
  V2.1Jx    Re-Wrote to suit LGT8F328P QFP32 Mini-EVB - may suit other boards with the same IC. Uses: https://github.com/dbuezas/lgt8fx/discussions/207, DATA is now D7, not D12 (needless, but CBF changing it back)
            I haven't bothered adjusting this for the simple region changes, but it's still easy to change regions. Just change the SCEE to your region in all the references in inject(). Managed to shave off 1%, 
            so this uses 5% of storage space :P
            Devved on a PU22 with full wire installation (Reset, Data, DriveLid, WFCK, GND, 3.3v). Removed chip disable mode, there's no reason to ever disable the chip.

*/

#define databit 7
#define lidbit 0
#define wfckbit 2
#define LIDPORT PORTB
#define LIDIO DDRB
#define DATAPORT PORTD
#define DATAIO DDRD


int injectcounter = 0x00;
int bitdelay (3794);    //delay between bits sent to drive controller (MS)
int stringdelay (160);  //delay between string injections
char SCEE[] = "10011010100100111101001010111010010101110100S";
char SCEA[] = "10011010100100111101001010111010010111110100S";
char SCEI[] = "10011010100100111101001010111010010110110100S";
//DEFAULT 'SCEE' INJECTION, CHANGE IN INJECT() TO SUIT YOUR REGION (x 3 references to character string)

void setup() {

  //    Set up the used pins

  delay (4700);                    //start-up delay (these need a few seconds to wake up for some reason, i've just moved this from main loop to here, it broke compatiblity.
  bitClear(LIDIO, lidbit);         // Lid sensor as high-z input
  bitWrite (DATAIO, databit, 1);   // Data starts as output
  bitClear (DATAPORT, databit);    // Data starts tied low (very temporary)
  bitClear (LIDIO, wfckbit);       // WFCK starts as high-z input, Equivalant of 'bitWrite (LIDIO, wfckbit, 0)'

}


void NewDisc() {                                // Do 'nothing' while the lids open between disc swap, once closed again, jump into inject routine.
  do
  {
    ;
  }
  while (bitRead (LIDPORT, lidbit) == 1);

  inject();
}

void DriveLidStatus() {
  TCCR1B = 0x18;               // 0001 1000, Disable Timer Clock
  bitClear (DATAIO, databit);  // high-z data line
  bitClear (LIDIO, wfckbit);   // high-z wfck line
  injectcounter = 0;

 // if (bitRead (LIDPORT, lidbit) == 0) { // don't really need to keep checking if the lids still closed, only check if it gets opened.
 //   ;                                   // so commented out
 //}
   
   if (bitRead (LIDPORT, lidbit) == 1) {  
    NewDisc();
  }
   
  DriveLidStatus();                        //Keep checking if the lids being opened (multi-disc games), if not, loop this function infinitely.
}

void inject() {


  // ****************GATE WIRE SETUP***********************
  // RTM_TimerCalc 1.31,  RuntimeMicro.com (this is written for 16mhz MCU's, but you can still work with it, this MCU approximately doubles the calculations)
  // = 7.342Khz
  TCCR1B = 0x18; // 0001 1000, Disable Timer
  TCCR1A = 0xA2; // 1010 0010

  ICR1 = 4334 - 1;
  OCR1A = (int) (ICR1 * 0.25);
  OCR1B = (int) (ICR1 * 0.50);
  TCNT1 = 0x0;
  bitWrite (LIDIO, wfckbit, 1);      //Unlike the 328P clones I was using, you have to set this pin as an output on this MCU, we will high-z this after injections.
  TCCR1B |= 1; // Prescale=1, Enable Timer

  //********************************************************


  do
  {

    for (byte i = 0; i < sizeof(SCEE) - 1; i++)

      if (SCEE[i] == '0')
      {
        bitWrite (DATAIO, databit, 1);  // Set DataPin as Output
        bitClear (DATAPORT, databit);   // Pull Data line Low for zero's
        delayMicroseconds (bitdelay);
      }
      else if (SCEE[i] == '1')
      {
        bitClear (DATAIO, databit);     // Set Datapin as input, releases data line for 1's
        delayMicroseconds(bitdelay);
      }
      else if (bitRead (LIDPORT, lidbit) == 1) // Magic Key injection broken by drive lid being opened?
      {
        DriveLidStatus();
      }
      else if (SCEE[i] == 'S')
      {
        injectcounter++;
        delay (stringdelay);
      }
  }
  while (injectcounter < 75);
}

void loop()
{
  if (injectcounter >= 75) {
    DriveLidStatus();
  }
  inject();
}
