//UberChip for PU22 WIP
// Shortened delay until injection crucial for game to load, not for black screen. Very easy to get black screen with a single injection at the right time.
//Define the used pins


#define DRIVE_LID_SENS 8
#define GATE 2
#define DATA 12
#define P_SENS 4
#define mchipregister PIND  //register for monitoring high/low states (read only)
#define mchipregwrite PORTD //register to write pin states to in setup
#define DRIVE_LID_SENS_REG PINB //the register with the DATA line and our drive lid sensor line
int Stealth = 0;
int bitdelay (3794); //delay between bits sent to drive controller (MS)
int stringdelay (100); //delay between string injections
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
char SCEE[] = "S10011010100100111101001010111010010101110100";   //the injection string bits in ASCII form - for a new injection method -
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

  pinMode (DATA, INPUT);
  

  if (DRIVE_LID_SENS_REG == Lid_Closed) { //state of this register after initial injection routine with lid closed
    //pinMode (DATA, INPUT);
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
  delay (2200);                                         //Audio-CD Fix boot delay, detects audio disc within this time frame
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
        Serial.println (" Release DATA line (high) ");
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
        Serial.println (" '0' bit ");
        bitcounter = (bitcounter + 1);
      }
    Serial.println (" Full String iteration ");
    //  delay(stringdelay); //delay
  }
  while (bitcounter < 3740);
  DriveLidStatus();
}

void loop() {
  if  (bitcounter >= 3740)
  {
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
