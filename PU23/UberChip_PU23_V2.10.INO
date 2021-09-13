//UberChip 4 wire stealth for PU23
//  V2.00
//  VajskiDs Consoles 2021
//  Refer to Github for install diagrams @ https://github.com/L10N37/tehUberChip_Another_PSX_Modchip
//  V1.1, Installed one today to ship off in this model, why wouldn't I? Spare arduinos on hand, doesn't interfere with the laser after boot and it takes 5 minutes to install.
//  A tidy Mayumi installation can take an hour if you're a trying to be *super neat*.

//  Gave it a quick test and it seems there was a slight oversight, like the PM41 version I have added genuine disc mode (start with lid open) as it was only working
//  with burnt games
//  and displaying Sony Computer Entertainment America with the SCEE string on the black Screen for NTSC US games, and the Sony Europe with SCEE string for copied PAL games
//  and finally, Sony Computer Entertainment Inc. for Japanese games :)
//  V2.00 Used some lower level type code for reading and changing pin states and some general optimisation, added link wire needed for genuine imports
//  at this stage, haven't been able to use software to produce this signal from an arduino.
//  V2.10 Added support for Genuine and Genuine imports without a genuine mode. The ability to disable the chip still remains.
//  The program used to make the calculations spits out a lot of broken code, used a pocket oscilliscope and uploaded a bunch of different settings and desired clock speeds until
//  I had something on the scope that was almost a 1:1 match of the WFCK signal. This signal gets turned off once injections are complete.

#define DATA 12
#define databit 4
#define gatebit 2
#define lidbit 0
#define DATAPORT PORTB
#define DATAIO DDRB
#define GATEINPUT PINB
int injectcounter = 0x00;
int bitdelay (3794); //delay between bits sent to drive controller (MS)
int stringdelay (160); //delay between string injections
char SCEE[] = "10011010100100111101001010111010010101110100S";
char SCEA[] = "10011010100100111101001010111010010111110100S";
char SCEI[] = "10011010100100111101001010111010010110110100";
//DEFAULT 'SCEE' INJECTION, CHANGE IN INJECT() TO SUIT YOUR REGION (x 3 references to character string)

void setup() {
  //Set up the used pins
  DATAIO = 0xFE;        // 0B11111110;         OPPOSITE OF TRISIO! set to ZERO for input, ONE for OUTPUT
  DATAPORT = 0x00;      // write all port b outputs low
}
void Genuine() {              //Genuine disc mode (start PSX with lid open, then close)
  Genuine();
}

void NewDisc() {
  do
  {
    delay (1500);
  }
  while (bitRead (DATAPORT, lidbit) == 1);
  inject();
}

void DriveLidStatus() {
  TCCR1B = 0x18;                // 0001 1000, Disable Timer Clock 
  bitClear (DATAIO, databit);  // OPPOSITE OF TRISIO! set to ZERO for input, ONE for OUTPUT
  injectcounter = 0;

  if (bitRead (DATAPORT, lidbit) == 0) {
    delay (1350) ;
  }
  else if (bitRead (DATAPORT, lidbit) == 1) {
    delay (1350);
    NewDisc();
  }
  DriveLidStatus();                           //forms a conditional if loop
}

void inject() {

// GATE WIRE SETUP***************************************
// RTM_TimerCalc 1.30,  RuntimeMicro.com
// Timer-1 Mode_14_16Bit_Fast_TOP_is_ICR

TCCR1B = 0x18; // 0001 1000, Disable Timer Clock 
TCCR1A = 0xA2; // 1010 0010

ICR1 = 2205-1;
OCR1A = (int) (ICR1 * 0.25);
OCR1B = (int) (ICR1 * 0.50);
TCNT1=0x0;
TCCR1B |= 1; // Prescale=1, Enable Timer Clock
//********************************************************
  do
  {

    for (byte i = 0; i < sizeof(SCEE) - 1; i++)

      if (SCEE[i] == '0')
      {
        bitWrite (DATAIO, databit, 1);  // OPPOSITE OF TRISIO! set to ZERO for input, ONE for OUTPUT
        bitClear (DATAPORT, databit);
        delayMicroseconds (bitdelay);
      }
      else if (SCEE[i] == '1')
      {
        bitClear (DATAIO, databit);     // OPPOSITE OF TRISIO! set to ZERO for input, ONE for OUTPUT
        delayMicroseconds(bitdelay);
      }
      else if (bitRead (DATAPORT, lidbit) == 1)
      {
        DriveLidStatus();
      }
      else if (SCEE[i] == 'S')
      {
        injectcounter++;
        delay (stringdelay);
      }
  }
  while (injectcounter < 86);
}

void loop()
{
  if (injectcounter >= 86) {
    DriveLidStatus();
  }

  if (bitRead (DATAPORT, lidbit) == 1) {
    pinMode (DATA, INPUT);
    Genuine();              // Disable uberchip
  }
  delay (4700); //start-up delay
  inject();
}
