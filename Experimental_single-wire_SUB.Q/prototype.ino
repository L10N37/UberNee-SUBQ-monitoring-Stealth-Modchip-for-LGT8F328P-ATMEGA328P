#define subqislow (PIND == 0B000000000)
int bitdelay(3700);
#define wait delayMicroseconds(bitdelay);
int capturetime = 0;
int fallingedges = 0;
int risingedges = 0;
int timer = 0;
int subqlowcount = 0;


void setup() {
  DDRD = 0B00000000;
  PORTD = 0B00000000;
  bitClear(DDRD, 2);
  Serial.begin(19200);  // enable serial monitor as a type of debug console
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


void loop() {

  while (subqislow) { ; }  //wait for activity on Sub.Q

 if (capturetime == 0) attachinterrupts();  //attach interrupts to D2 and enable interrupts




  // 0xA0
  if (capturetime == 8 && risingedges == 4 && fallingedges == 4) {
    print();
    resetvariables();
  }

  // 0xA1 and 0xA2
  else if (capturetime == 10 && risingedges == 5 && fallingedges == 5) {
    print();
    resetvariables();
  }


  if (capturetime == 8 || capturetime >= 10) {
    noInterrupts();
    resetvariables();
  }
}
