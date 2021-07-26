//VajskiDs consoles 2021
// timing is a bitch, it's a work in progress... took minutes to get a black screen on NTSC game, but couple hours to get one to boot.
// SO far only tested Vagrant story as perfect and Final Fantasy 9, gets past black screen then locks up.
// Only needs to be fine tuned now
// How's it work? D10 and D11 are actually tied together on the Arduino ... they're both set to inputs which lets the signal pass through
// but if either is set to an output we can pull the line low at the right time, it also needs releasing at the right time (here is after 44 injections)
// The D2 BIOS leg is LIFTED off the board, D10 goes to the lifted pin, D11 goes to the TEST pad next to the IC that this leads to
//People to see, places to be
// will fine tune another time :)

#define DRIVE_LID_SENS 8
#define GATE 2
#define DATA 12
#define P_SENS 4
#define PSOneBIOS2 10        //  ** PAL PSOne BIOS Patch pins  **
#define PSOneBIOS1 11        //  **                            **
#define mchipregister PIND    //register for monitoring high/low states (read only)
#define mchipregwrite PORTD   //register to write pin states to in setup
#define DRIVE_LID_SENS_REG PINB //the register with the DATA line and our drive lid sensor line
int bitdelay (3794); //delay between bits sent to drive controller (MS)
int stringdelay (140); //delay between string injections
const int Lid_Open = (0B000001); //so we don't have to refer to the states in bits further on
const int Lid_Closed = (0B000000);
int bitcounter = 0; //variable for injection loop storage
char SCEE[] = "S10011010100100111101001010111010010101110100";
char SCEA[] = "S10011010100100111101001010111010010111110100";
char SCEI[] = "S10011010100100111101001010111010010110110100";
int NTSC_FIX = 0; //variable to help time NTSC patch for PAL PSOne, basically counts full string injections, which is what should have being done initially instead of deciding to count bits :P

//DEFAULT 'SCEE' INJECTION, CHANGE IN INJECT() TO SUIT YOUR REGION (x 3 references to character string)

void setup() {
  //Set up the used pins

  pinMode (PSOneBIOS1, INPUT);
  pinMode (PSOneBIOS2, INPUT);
  pinMode (GATE, OUTPUT);
  pinMode (DATA, OUTPUT);
  digitalWrite (DATA, LOW);
  pinMode (P_SENS, INPUT);
  pinMode (DRIVE_LID_SENS, INPUT);
  digitalWrite (mchipregwrite, 0B00001011);
  digitalWrite (DRIVE_LID_SENS_REG, 0B000000);
}
void BIOSPATCH() {
  delay (2400); //inital bootup delay
  pinMode (PSOneBIOS1, OUTPUT);
  digitalWrite (PSOneBIOS1, LOW);
  delay (1400);                   //check pin signal low = NTSC during this time period
  //  pinMode (PSOneBIOS1, INPUT);    //then release
  inject();
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
        NTSC_FIX = (NTSC_FIX + 1);
      }
      else if (DRIVE_LID_SENS_REG == Lid_Open)
      {
        DriveLidStatus();
      }
      else if (NTSC_FIX == 32)  //after 32 full string injections release the BIOS D2 (black screen gets a little garbled on the bottom, WIP) (26 seems to be on the money for everygame I've tried bar FF9)
      {
        pinMode (PSOneBIOS1, INPUT);
      }
      else
      {
        pinMode (DATA, OUTPUT);
        digitalWrite (DATA, LOW);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
  }
  while (bitcounter < 3916);
  DriveLidStatus();
}
void loop() {
  if  (bitcounter >= 3916)
  {
    DriveLidStatus();
  }
  BIOSPATCH();             // else jump to here
}
