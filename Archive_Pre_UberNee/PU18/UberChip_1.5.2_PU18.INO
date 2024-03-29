//************************************************* Stand-Alone version v1.5.1 for internal install / no debug ****************************************************//
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
//                                                                                                                                                                 //
//  https://imgur.com/eLf89y5                                                                                                                                      //
// IT BOOTS UM JAMMER LAMMY [NTSC], the only game I know that can detect a modchip!!                                                                               //
//                                                                                                                                                                 //
//  V1.5: Some minor timing and magic key injection tweaks, added multidisc support. Extra lid sensor wire in defines.                                             //
// Audio CD Fix + Jumps straight into CD player if an audio disc is inserted, this was just a delay at boot before injections as an audio                          //
// CD is detected within the first 4 or 5 seconds and launches the player. Injecting the String / 'magic key' after this time                                      //
// doesn't affect boot time of a game disc what so ever. Releasing Gate/ Data pins breaks the program, possibly because of the register states used but            //
// releasing them isn't required - just can't keep pumping the 'magic key' down the DATA line or it's picked up by anti-mod.                                       //
// ...Will likely move onto a PSOne version after this. Proper Install diagrams will be coming!                                                                    //
// You won't be able to break this using the console normally, but if you really, really try you could probably flag an anti-mod                                   //
//                                                                                                                                                                 //
//  V1.5.1: Injection routine now also monitors drive lid status, so can break out of the injection cycle early and restart it                                     //
//   not something you'd do in real PSX use, but further proofs the code                                                                                           //
//                                                                                                                                                                 //
// V1.5.2: Updated the injection method, re-coded to suit direct wiring to 3.3v line / no link wire needed anymore                                                 //
//  I haven't tested this personally as I CBF re-installing to a PU18 but it should be fine.                                                                       //
// \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/         \m/      \m/ //
//Define the used pins
#define DRIVE_LID_SENS 8
#define GATE 2
#define DATA 12
#define P_SENS 4
#define mchipregister PIND  //register for monitoring high/low states (read only)
#define mchipregwrite PORTD //register to write pin states to in setup
#define DRIVE_LID_SENS_REG PINB //the register with the DATA line and our drive lid sensor line
int bitdelay (3974); //delay between bits sent to drive controller (MS)
int stringdelay (67); //delay between string injections
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
  pinMode (P_SENS, INPUT);
  pinMode (DRIVE_LID_SENS, INPUT);
  //and sets true states of other unused pins in this register
  digitalWrite (DRIVE_LID_SENS_REG, 0B000000);
}
void NewDisc() {
  do
  {
    delay (1500);
  }
  while (DRIVE_LID_SENS_REG == Lid_Open);
  inject();
}
void DriveLidStatus() {
  if (DRIVE_LID_SENS_REG == Lid_Closed) { //state of this register after initial injection routine with lid closed
    bitcounter = 0;                  //This reset is in case of multidisc/ multiple injection routines
  }
  else if (DRIVE_LID_SENS_REG == Lid_Open) {  //monitor register state with this pin for sensing purposes, data idles low after injection, only need to look for a change in one bit (lid sens pin)
    delay (500);
    NewDisc();
  }
  DriveLidStatus();                           //forms a conditional if loop
}
void PowerUpDelay() {
  delay (7200);
  inject(); //ready to inject
}
void inject() {
  do
  {
    for (byte i = 0; i < sizeof(SCEE) - 1; i++)

      if (SCEE[i] == '1')
      {
        digitalWrite (DATA, HIGH);
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
        digitalWrite (DATA, LOW);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
  }
  while (bitcounter < 4400);
  DriveLidStatus();
}
void loop() {
  if  (bitcounter >= 4400)
  {
    DriveLidStatus();
  }
  PowerUpDelay();             // else jump to here
}
