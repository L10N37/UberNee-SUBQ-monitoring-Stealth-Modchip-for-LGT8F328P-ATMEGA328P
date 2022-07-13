//            UberChip for PU23
//  V2.1J
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

//  V2.1J:  Small timing change, fixes: Legend of Mana (JP), was getting antimod flag 50% of the time.
//  13/07/2022 I realised some people might be trying to learn some basic coding using this, so added some more comments

#define DATA 12         // Because we are using pin 12 to send the wuns and zewos, you could use a global integer as well.
#define databit 4       // Because the 4th bit (well 5th, we start on '0') corresponds to the data pin (D12) in the port.
#define lidbit 0        // Because the the zero'eth bit  in PORTB corresponds to the lid sensing pin (D8), Make sure you check if it's little or big endian.
#define DATAPORT PORTB  // Renamed PORTB to DATAPORT as it's more project specific and makes more sense when I was reading/ writing code.
#define DATAIO DDRB     // Simply Renamed DDRB port to DATAIO (Data Input/ Output), now i can refer to the DDRB register as DATAIO, Cos i felt like it :P
int injectcounter = 0x00; //I initialised this in hex cos it's fancy :D
int bitdelay (3794);    //delay between bits sent to drive controller (MS)
int stringdelay (160);  //delay between string injections
char SCEE[] = "10011010100100111101001010111010010101110100S"; // Because these are the region strings when inverted (1's and Zero's are switched) and the endian is swapped
char SCEA[] = "10011010100100111101001010111010010111110100S"; // I just use the 's'character as an indication of hitting the end of the magic key and calling the delay between strings, before restarting next key injection.
char SCEI[] = "10011010100100111101001010111010010110110100S";
//DEFAULT 'SCEE' INJECTION, CHANGE IN INJECT() TO SUIT YOUR REGION (x 3 references to character string)

void setup() {
  //Set up the used pins
  DATAIO = 0xFE;        // 0B11111110;         OPPOSITE OF TRISIO! set to ZERO for input, ONE for OUTPUT 13/07/22 (These comments are only here because I'd just finished porting across to PIC ASM and it uses the opposite, after all that debugging I kept mixing these up)
  DATAPORT = 0x00;      // write all port b outputs low

// 13/07/2022; ^Because I want to confuse people more nooby than me, and it's lower level code (that's always more efficient)
// I am simply setting the I/O port bits to set the pins to Input/Output instead the high level way on previous versions
// After that, i simply converted it from bits to hex, just to be an asshole ...and once again, confuse the shit out of nooblets (I am a noob too :P )
  
}
void Genuine() {              //Genuine disc mode (start PSX with lid open, then close)
  Genuine();
}

void NewDisc() {
  do
  {
    delay (1500); //This delay isn't actually necessary, it just checks whether you've closed the drive lid every 1.5 seconds.
  }
  while (bitRead (DATAPORT, lidbit) == 1);
  inject();
}

void DriveLidStatus() {
  TCCR1B = 0x18;                // 0001 1000, Disable Timer Clock 
  bitClear (DATAIO, databit);  // OPPOSITE OF TRISIO! set to ZERO for input, ONE for OUTPUT
  injectcounter = 0;

  if (bitRead (DATAPORT, lidbit) == 0) {
    delay (1350) ; //This delay isn't actually necessary, it just checks whether the lid is closed every 1.35s
  }
  else if (bitRead (DATAPORT, lidbit) == 1) {
    delay (1350); //This delay isn't actually necessary, there's just a 1.35s gap between finding out the lid is now open, to jumping to NewDisc()
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
  while (injectcounter < 75);
}

void loop()
{
  if (injectcounter >= 75) {
    DriveLidStatus();
  }

  if (bitRead (DATAPORT, lidbit) == 1) {
    pinMode (DATA, INPUT);
    Genuine();              // Disable uberchip
  }
  delay (4700);             //start-up delay
  inject();
}
