//************************************************* Stand-Alone version v1.5.1 PU18 for internal install / no debug ***********************************************//
// \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/ //
//                                                                                                                                                                 //
//  A STUPID DUMBED DOWN VERSION OF A PSX MODCHIP BY A TOTAL AMATEUR WITH A FEW DAYS OF ARDUINO TINKERING EXPERIENCE                                               //
//                                        I dub Thee: Teh UbeR ChiP  V1.5                                                                                          //
//                                            VAJSKIDS JULY 2021                                                                                                   //
//  SOURCES OF INFORMATION:                                                                                                                                        //
//                          https://github.com/vxbinaca/modavr/blob/master/modavr.asm                                                                              //
//                          http://www.oldcrows.net/mcc1.html                                                                                                      //
//                          https://github.com/Gadorach/PSX-MultiMode3-PIC12F683/blob/master/MPLAB%20X/MM3-12F683.X/main.c                                         //
//                          And an older version of PSNEE source code before it also covered OneChip/ psone , the latest one has IMO being over optimised          //
//                          and is harder to understand.                                                                                                           //
//tools used:                                                                                                                                                      //
//$17 ebay logic analyser (watched data line on an old crow + data lines on my arduino + data line on console with genuine disc - 11 injections)                   //
//Logic 2 (software)                                                                                                                                               //
//PU18 SCPH5502 with a drive from another playstation and an extension ribbon cable                                                                                //
// Arduino Nano (clone with old bootloader)                                                                                                                        //
// An old modchip pulled out of one of many systems I've flipped                                                                                                   //
//                                                                                                                                                                 //
// Most of the time was spent trying to understand "what to inject" as outlined below.... otherwise this would have being working within a couple of hours         //
// This is a great example for a learner / by a learner                                                                                                            //
// This is currently set up to watch on the serial monitor with a ~3.5v "power sensor" wire as oppose being hardwired into the console                             //
// which would be a very easy change.                                                                                                                              //
//                                                                                                                                                                 //                                                                                                                                                                //
//  https://imgur.com/eLf89y5                                                                                                                                      //
// IT BOOTS UM JAMMER LAMMY [NTSC], the only game I know that can detect a modchip!!                                                                               //
//                                                                                                                                                                 //
//V1.5: Some minor timing and magic key injection tweaks, added multidisc support. Extra lid sensor wire in defines.                                               //
// Audio CD Fix + Jumps straight into CD player if an audio disc is inserted, this was just a delay at boot before injections as an audio                          //
// CD is detected within the first 4 or 5 seconds and launches the player. Injecting the String / 'magic key' after this time                                      //
// doesn't affect boot time of a game disc what so ever. Releasing Gate/ Data pins breaks the program, possibly because of the register states used but            //
// releasing them isn't required - just can't keep pumping the 'magic key' down the DATA line or it's picked up by anti-mod.                                       //
// ...Will likely move onto a PSOne version after this. Proper Install diagrams will be coming!                                                                    //
// You won't be able to break this using the console normally, but if you really, really try you could probably flag an anti-mod                                   //
//                                                                                                                                                                 //
//  V1.5.1: Added disc drive monitoring in the injection cycle, can now break out of the routine if interrupted with a drive lid opening                           //
//  Not something you'd do in a real PSX use scenario but further bullet proofs the code, was just overlooked                                                      //
// \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/ //

//Define the used pins
#define DRIVE_LID_SENS 8
#define GATE 2
#define DATA 12
#define P_SENS 4
#define mchipregister PIND  //register for monitoring high/low states (read only)
#define mchipregwrite PORTD //register to write pin states to in setup
#define DRIVE_LID_SENS_REG PINB //the register with the DATA line and our drive lid sensor line
int bitdelay (3794); //delay between bits sent to drive controller (MS)
int stringdelay (116); //delay between string injections
const int System_Off = (0B00000011); //RX & TX are always high
const int System_On = (0B00001011); //
const int Lid_Open = (0B000001); //so we don't have to refer to the states in bits further on
const int Lid_Closed = (0B000000);
char Dots[1] = ".";
char equalsign[] = "==============================";
int bitcounter = 0; //variable for injection loop storage
char notice[] = "Playstation Console isn't Powered-On or the powers on and the Drive Lid is Open";
char blank[] = " "; //just to fix an annoying debugging bug
unsigned int DiscSwap = 0;
char SCEE[] = "10011010100100111101001010111010010101110100";   //the injection string bits in ASCII form - for a new injection method -
// based off https://www.arduino.cc/reference/en/language/variables/data-types/string/
// played with the example and re-wrote it to do an ascii comparison as it shifts
// the ascii character '1' from right to left and produce 2 different outcomes dependant on true/false condition
// due to limitations when using DEC/ HEX bitmask stuff.


void setup() {


  Serial.begin(57600); //serial monitor for debugging
  //Set up the used pins
  pinMode (GATE, OUTPUT);
  pinMode (DATA, OUTPUT);
  digitalWrite (DATA, LOW);
  pinMode (P_SENS, INPUT);
  pinMode (DRIVE_LID_SENS, INPUT);
  //setup default register state (psx off)
  digitalWrite (mchipregwrite, 0B00001011);
  //otherwise LED pin 13 will go high when flashing and break code
  //and sets true states of other unused pins in this register
  digitalWrite (DRIVE_LID_SENS_REG, 0B000000);
}
void NewDisc() {
  do
  {
    delay (1500);
    Serial.println ("   ...Awaiting disc swap and drive lid closure  " );
  }
  while (DRIVE_LID_SENS_REG == Lid_Open);
  inject();
}
void DriveLidStatus() { //a conditional loop for the status of the drive lid sensor pin using the state of it's entire register
  //for multi-disc games

  if (DRIVE_LID_SENS_REG == Lid_Closed) { //state of this register after initial injection routine with lid closed

    bitcounter = 0;                  //This reset is in case of multidisc/ multiple injection routines
    Serial.println (" Injection cycles complete ");
    Serial.println (" ...Gate will remain closed "); //print more debug stuff
    Serial.println (equalsign);
    delay (850);
    Serial.println ("   Stealth chip enabled, monitoring disc drive lid status   ");
  }
  else if (DRIVE_LID_SENS_REG == Lid_Open) {  //monitor register state with this pin for sensing purposes, data idles low after injection, only need to look for a change in one bit (lid sens pin)
    Serial.println (" Drive Lid is Open!" );  //print this to serial monitor while drive lid is open and wait for it to close again
    delay (850);
    DiscSwap = (DiscSwap + 1);                   // **debug purposes**
    Serial.println ("Disc swapped ");            // **              **
    Serial.print (DiscSwap);
    Serial.print (  "   time/s!"  );
    delay (500);
    NewDisc();
  }
  DriveLidStatus();                           //forms a conditional if loop
}

void PowerUpDelay() { //A power up delay with serial monitor stuff, self explanatory
  Serial.println ("  PSX Console Powered-On & Drive lid is closed ...Data Gate Closed, Preparing to Inject!   ");
  delay (500);
  Serial.println (Dots);
  delay (500);
  Serial.println (Dots);
  delay (500);
  Serial.println (Dots);
  delay (500);
  Serial.println (Dots);
  delay (250);
  Serial.println (Dots);
  delay (250);
  Serial.println (Dots);
  delay (250);
  Serial.println (Dots);
  delay (250);
  Serial.println (Dots);
  delay (150);
  Serial.println (Dots);
  delay (150);
  Serial.println ("  STARTING INJECTION CYCLES   ");
  Serial.println (equalsign);
  delay (4000);                                         //Audio-CD Fix boot delay, detects audio disc within this time frame
  // but squirting the region string in after this point is ok for a game disc
  // doesn't affect game boot time
  inject(); //ready to inject
}

void inject() {
  do
  {
    for (byte i = 0; i < sizeof(SCEE) - 1; i++)

      if (SCEE[i] == '1')
      {
        pinMode (DATA, INPUT);
        Serial.println (" WFCK bit (high) ");
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
      else
      {
        pinMode (DATA, OUTPUT);
        digitalWrite (DATA, LOW);
        delayMicroseconds (bitdelay);
        Serial.println (" '0' bit ");
        bitcounter = (bitcounter + 1);
      }
    Serial.println (" Full String iteration ");
    delay(stringdelay); //delay
  }
  while (bitcounter < 1500);
}
void loop() {
  if  (bitcounter>= 1500)
  {
    pinMode (DATA, INPUT);
    DriveLidStatus();
  }
  delay(1800);
  if (DRIVE_LID_SENS_REG == Lid_Open || mchipregister == System_Off) //if power off or lid closed
  {
    Serial.println (notice);  //keep printing this to serial monitor
    Serial.print (blank);     //otherwise it prints a dot along with 'notice' 9 times after jumping to powerupdelay, instead of just the 9 new line dots (?!?!)
  }
  else
  {
    PowerUpDelay();             // else jump to here
  }
}
