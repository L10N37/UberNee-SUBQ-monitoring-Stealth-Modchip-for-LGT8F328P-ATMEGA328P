
//UberChip 4 wire stealth for PU23 
//  V1.00
//  VajskiDs Consoles 2021
//  Refer to Github for install diagrams @ https://github.com/L10N37/tehUberChip_Another_PSX_Modchip
//  V1.1, Installed one today to ship off in this model, why wouldn't I? Spare arduinos on hand, doesn't interfere with the laser after boot and it takes 5 minutes to install.
//  A tidy Mayumi installation can take an hour if you're a trying to be *super neat*.

//  Gave it a quick test and it seems there was a slight oversight, like the PM41 version I have added genuine disc mode (start with lid open) as it was only working 
//  with burnt games
//  and displaying Sony Computer Entertainment America with the SCEE string on the black Screen for NTSC US games, and the Sony Europe with SCEE string for copied PAL games :) 


#define DRIVE_LID_SENS 8
#define GATE 2
#define DATA 12
#define P_SENS 4
#define mchipregister PIND  //register for monitoring high/low states (read only)
#define mchipregwrite PORTD //register to write pin states to in setup
#define DRIVE_LID_SENS_REG PINB //the register with the DATA line and our drive lid sensor line
int bitdelay (3794); //delay between bits sent to drive controller (MS)
int stringdelay (160); //delay between string injections
const int Lid_Open = (0B000001); //so we don't have to refer to the states in bits further on
const int Lid_Closed = (0B000000);
int bitcounter = 0; //variable for injection loop storage
char SCEE[] = "S10011010100100111101001010111010010101110100";
char SCEA[] = "S10011010100100111101001010111010010111110100";
char SCEI[] = "S10011010100100111101001010111010010110110100";
//DEFAULT 'SCEE' INJECTION, CHANGE IN INJECT() TO SUIT YOUR REGION (x 3 references to character string)

void setup() {
  //Set up the used pins
  pinMode (GATE, OUTPUT);
  pinMode (DATA, OUTPUT);
  digitalWrite (DATA, LOW);
  pinMode (P_SENS, INPUT);
  pinMode (DRIVE_LID_SENS, INPUT);
  digitalWrite (mchipregwrite, 0B00001011);
  digitalWrite (DRIVE_LID_SENS_REG, 0B000000);
}
void Genuine() {              //Genuine disc mode (start PSX with lid open, then close)  
  Genuine();
}
void NewDisc() {
  do
  {
    delay (1500);
  }
  while (DRIVE_LID_SENS_REG == Lid_Open);
  inject();
}
void DriveLidStatus() { //a conditional loop for the status of the drive lid sensor pin using the state of it's entire register
  //for multi-disc games
  pinMode (DATA, INPUT);
  if (DRIVE_LID_SENS_REG == Lid_Closed) { //state of this register after initial injection routine with lid closed
    bitcounter = 0;                  //This reset is in case of multidisc/ multiple injection routines
    delay (850);
  }
  else if (DRIVE_LID_SENS_REG == Lid_Open) {  //monitor register state with this pin for sensing purposes, data idles low after injection, only need to look for a change in one bit (lid sens pin)
    delay (1350);
    NewDisc();
  }
  DriveLidStatus();                           //forms a conditional if loop
}
  //void PowerUpDelay() {
  //    delay (4700);                                         //Audio-CD Fix boot delay, detects audio disc within this time frame
  // but squirting the region string in after this point is ok for a game disc
  // doesn't affect game boot time
  //  inject(); //ready to inject
  //}
void inject() {
  do
  {
    for (byte i = 0; i < sizeof(SCEE) - 1; i++)

      if (SCEE[i] == '1')
      {
        pinMode (DATA, INPUT);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
      else if (SCEE[i] == 'S')
      {
        delay(stringdelay); //string delay when it hits 's' (end of injection string bits)
      }
      else if (DRIVE_LID_SENS_REG == Lid_Open)
      {
        DriveLidStatus();
      }
      else
      {
        pinMode (DATA, OUTPUT);
        digitalWrite (DATA, LOW);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
  }
  while (bitcounter < 3740);
  DriveLidStatus();
}
void loop() {
 if (digitalRead(DRIVE_LID_SENS) == 1) {
    pinMode (DATA, INPUT);  //revert these lines to inputs so they don't interfere with the genuine disc string
    Genuine();              //jump to blinky light status indicating chips in genuine disc mode
  }
  delay (4700); //start-up delay
  inject();
}
