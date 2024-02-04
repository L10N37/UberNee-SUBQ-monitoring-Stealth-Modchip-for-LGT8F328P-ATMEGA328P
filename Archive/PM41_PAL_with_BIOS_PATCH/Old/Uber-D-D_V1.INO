/*  =================================================VAJSKIDS CONSOLES 2021 =================================================
Stealth modchip with Single Wire BIOS patch for PM41 [Uber-dd V1.00] (this is a rushed release, will tidy up, debug and re-release)
Quote VajskiDs himself - "it's reliable enough" :P

....As for the WIP versions, they were so broken it was only a fluke they worked as well as they did. 

There could be irelevant function names or comments, haven't tidied this up yet....

Testing so far

FF9 (NTSC-U)
5 Good loads in a row
then a fail


Cotton (NTSC-J)
5 good loads in a row

RE3 (NTSC-U)
5 Good loads in a row

Duke Nukem TTK (NTSC-U)
started off failing multiple times ,then 5 good loads in a row?!

Dino Crisis (NTSC-U)
5 good loads in a row

Raiden DX (NTSC-J)
fail
then 5 good in a row

Original PAL games are fine but 9/10 times ALL games show 'd' 'd' on the black screen

*/

#define DRIVE_LID_SENS 8
#define GATE 2
#define DATA 12
#define P_SENS 4
#define PSOneBIOS1 11        //  **                            **
#define mchipregister PIND    //register for monitoring high/low states (read only)
#define mchipregwrite PORTD   //register to write pin states to in setup
#define DRIVE_LID_SENS_REG PINB //the register with the DATA line and our drive lid sensor line
int bitdelay (3794); //delay between bits sent to drive controller (MS)
int stringdelay (170); //delay between string injections
const int Lid_Open = (0B000001); //so we don't have to refer to the states in bits further on
const int Lid_Closed = (0B000000);
int bitcounter = 0; //variable for injection loop storage
char SCEE[] = "S10011010100100111101001010111010010101110100";
int NTSC_FIX = 0; //variable to help time NTSC patch for PAL PSOne, basically counts full string injections, which is what should have being done initially instead of deciding to count bits :P


void setup() {
  //Set up the used pins

  pinMode (PSOneBIOS1, INPUT);
  pinMode (DATA, OUTPUT);
  digitalWrite (DATA, LOW);
  pinMode (P_SENS, INPUT);
  pinMode (DRIVE_LID_SENS, INPUT);
  digitalWrite (mchipregwrite, 0B00001011);
  digitalWrite (DRIVE_LID_SENS_REG, 0B000000);
}
void BIOSPATCH() {
  delay (3800);
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
  pinMode (DATA, INPUT); // make certain we've released D2 after injection routine!
  NTSC_FIX = 0 ; // reset variables
  bitcounter = 0;
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





  if (DRIVE_LID_SENS_REG == Lid_Open) // Drive lid status check during injection routine
  {
    DriveLidStatus();
  }


//888888888888888888888888888888888888==   BIOS STUFF    =888888888888888888888888888888888888//

  if (NTSC_FIX > 8 && (NTSC_FIX < 26))                        // No rocket-science here! 
  {
    pinMode (PSOneBIOS1, OUTPUT);                             // Simply attempt to spoof NTSC console detection by pulling BIOS D2 low between
    digitalWrite (PSOneBIOS1, LOW);                           // these string injection iterations 
  }
                                          

   if (NTSC_FIX == 26)                                        
  {
    pinMode (PSOneBIOS1, INPUT);                              // Then release the line!
  }                                                           // This is EXTREMELY touchy! as non-precise as it seems, it's actually quite precise

//888888888888888888888888888888888888==   BIOS STUFF    =888888888888888888888888888888888888//


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
      else
      {
        pinMode (DATA, OUTPUT);
        digitalWrite (DATA, LOW);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
  }

  if (bitcounter > 3916)
  {
    DriveLidStatus();
  }
  inject();
}
void loop() {

  if  (bitcounter > 3916) // just in-case
  {
    DriveLidStatus();
  }
  BIOSPATCH();             // else jump to here
}
