// experimental WIP
// seems to reliably detect laserhead in TOC area
// no black screen yet

#define subqislow (PIND == 0B00000000)
#define wait delayMicroseconds(bitdelay);
#define LowBit bitWrite(DDRD, 4, 1), delayMicroseconds(bitdelay)
#define HighBit bitClear(DDRD, 4), delayMicroseconds(bitdelay)
#define EndOfMagicKey injectcounter++, delay(stringdelay)
int injectcounter = 0x00;
int bitdelay(3970);
int stringdelay(160);  //delay between string injections
int capturetime = 0;
int fallingedges = 0;
int risingedges = 0;
int timer = 0;


char ALLREGIONS[] = "10011010100100111101001010111010010101110100S10011010100100111101001010111010010111110100S10011010100100111101001010111010010110110100";
char SCEE[] = "10011010100100111101001010111010010101110100";
char SCEA[] = "10011010100100111101001010111010010111110100";
char SCEI[] = "10011010100100111101001010111010010110110100";



//***************** MAGIC KEYS *****************************
// EUROPE / AUSTRALIA / NEWZEALAND = SCEE
// USA = SCEA
// JP = SCEI
// ALL= ALLREGIONS
//**********************************************************


#define SELECT_MAGICKEY SCEE  //<---------------------------------------------REGION SELECT!! ENTER CONSOLE REGION




void setup() {

  Serial.begin(115200);  // enable serial monitor as a type of debug console
  monitorpinstates();
}

void monitorpinstates() {
  bitClear(DDRD, 4);      //  high-z data
}


void attachinterrupts() {
  interrupts();
  attachInterrupt(digitalPinToInterrupt(2), subqrising, RISING);
  attachInterrupt(digitalPinToInterrupt(2), subqfalling, FALLING);
}


void resetvariables() {
  capturetime = 0;
  fallingedges = 0;
  risingedges = 0;
  injectcounter = 0;
}

void subqrising() {

  capturetime++;
  risingedges++;


  switch (risingedges) {
    case 1:
      wait break;
    case 2:
      wait break;
    case 3:
      wait break;
    case 4:
      wait break;
    case 5:
      wait break;
  }
}

void subqfalling() {

  capturetime++;
  fallingedges++;


  switch (fallingedges) {
    case 1:
      wait wait wait wait wait break;
    case 2:
     wait wait wait wait wait wait wait wait wait break;
    case 3:
      wait break;
    case 4:
      wait break;
  }
}

void print() {
  Serial.print("LASER HEAD IN TOC AREA  ");
}


void inject() {


  bitWrite(DDRD, 4, 1);  //  set data as output for injections
  bitClear(PORTD, 4);     // set data low

  do {

    for (byte i = 0; i < sizeof(SELECT_MAGICKEY); i++)

      switch (SELECT_MAGICKEY[i]) {
        case '1':
          HighBit;   Serial.print(injectcounter);
          break;
        case '0':
          LowBit;   Serial.print(injectcounter);
          break;
      }

    EndOfMagicKey;

  } while (injectcounter <= 3);
}

void loop() {

  
  if (capturetime >=11 ) {capturetime = 0; noInterrupts(); resetvariables(); wait}
  attachinterrupts();  //attach interrupts to D2 and enable interrupts



  // 0xA0
  if (capturetime == 8 && risingedges == 4 && fallingedges == 4) {
    noInterrupts();
    print();
    inject();
    monitorpinstates();
    resetvariables();
    

  }

  // 0xA1 and 0xA2
  else if (capturetime == 10 && risingedges == 5 && fallingedges == 5) {
    noInterrupts();
    print();
    inject();
    monitorpinstates();
    resetvariables();
    
  }

}
