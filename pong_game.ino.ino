#include <EEPROM.h>

#include <LiquidCrystal.h>
#include<LedControl.h>

#define RS 8//am schimbat 2 cu 8
#define ENABLE 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7

#define ACTIV LOW


byte sad[] = { 
  B00000000,
  B01000100,
  B00010000,
  B00010000,
  B00000000,
  B00111000,
  B01000100,
  B00000000
  }; 
 
byte smile[] = { 
  B00000000,
  B01000100,
  B00010000,
  B00010000,
  B00010000,
  B01000100,
  B00111000,
  B00000000
  }; 
  
LiquidCrystal lcdq(RS, ENABLE, D4, D5, D6, D7);
LedControl lc=LedControl(12,11,10,1);


void afis(byte *sprite){
  for(int r = 0; r < 8; r++){
    lc.setRow(0, r, sprite[r]);
    }
 } 

const int buttonPin = 2;
int buttonState = 0;
 
//Viteza bilei
int ballDelayLimit = 400;
int ballDelay = 0;

//Control pin de intrare
int controller = 0;

//byte map pentru linie robot
byte bat = B11100000;

int cval = 0;//controller valoare
int pos = 0;
float epos = 2;
float eskill = 1;
float br = 4;
float bc = 2;
float brr = 1;
float bcc = 1;

//0 = player, 1 = robot
int score[2] = {0, 0};
boolean gameOver = false;


//shifteaza linia robotului bat cu offset
void drawBat(int offset, int row){
  offset = constrain(offset, 0, 5);
  lc.setRow(0, row, bat>>offset);
}


void bounce(){
  //verificare daca se buseste de peretii de sus/de jos
  if(bc <= 0){
    bcc = bcc * -1;
  }else if(bc >= 7){
    bcc = 0-bcc;
  }

  //Add bouncing off bats with angles for where on the bat got hit (row checks)
  if(br < 2 && br >= 1 && brr < 0){
    if((int)bc == pos){
      brr = brr * -1;
      if(bcc > 0){
        bcc = bcc * 0.9;
      }else{
        bcc = bcc * 1.1;
      }
    }else if((int)bc == pos+1){
      brr = brr * -1;
    }else if((int)bc == pos+2){
      brr = brr * -1;
      if(bcc > 0){
        bcc = bcc * 1.1;
      }else{
        bcc = bcc * 0.9;
      }
    }
    return;
  }else if(br < 7 && br >= 6 && brr > 0){
    if((int)bc == epos){
      brr = 0-brr;
      if(bcc > 0){
        bcc = bcc * 0.9;
      }else{
        bcc = bcc * 1.1;
      }
    }else if((int)bc == epos+1){
      brr = 0-brr;
    }else if((int)bc == epos+2){
      brr = 0-brr;
      if(bcc > 0){
        bcc = bcc * 1.1;
      }else{
        bcc = bcc * 0.9;
      }
    }
    return;
  }

  //verifica daca ai pierdut lovitura
  if(br < 0){
    //Game over, am pierdut
    score[1] = score[1]+1;
    gameOver = true;
  }else if(br > 7){
    //Game over, robotul a pierdut
    score[0] = score[0]+1;
    gameOver = true;
  }
}


//afiseaza scorul
void Scors(){
  lcdq.clear();
   lcdq.setCursor(0, 0); 
   lcdq.print("ROBOT");
  switch(score[1]) {
    case 0:
    {
       lcdq.setCursor(7, 0);
       lcdq.print("scor : 0");
    }
      break;
    case 1:
    {
       lcdq.setCursor(7, 0);
       lcdq.print("scor : 1");
      }
      break;
    case 2:
    {
       lcdq.setCursor(7, 0);
       lcdq.print("scor : 2");
    }
      break;
    case 3:{
       lcdq.setCursor(7, 0);
       lcdq.print("scor : 3");
      }
      break;
  }
  //delay(700);
   lcdq.setCursor(0, 1); 
   lcdq.print("PLAYER");
  switch(score[0]) {
    case 0:
    {
       lcdq.setCursor(8, 1);
       lcdq.print("scor : 0");
    }
    
      break;
    case 1:
    {
       lcdq.setCursor(8, 1);
       lcdq.print("scor : 1");
      }
      break;
    case 2:
    {
       lcdq.setCursor(8, 1);
       lcdq.print("scor : 2");
    }
      break;
    case 3:{
       lcdq.setCursor(8, 1);
       lcdq.print("scor : 3");
      }
      break;
  }
  delay(700);
}

//Game over 
void endGame(){
  lcdq.begin(16, 2);
  lcdq.clear();
  lcdq.setCursor(2, 1);
  lcdq.print("GAME OVER ! ");
  if(score[0] > score[1]){
    lcdq.begin(16, 2);
    lcdq.clear();
    lcdq.setCursor(2, 1);
    lcdq.print("YOU WIN ! ");
    afis(smile);
  }
  else{
      lcdq.begin(16, 2);
      lcdq.clear();
      lcdq.setCursor(2, 1);
      lcdq.print(" YOU LOSE ! ");
      afis(sad);
  }
  score[0] = 0;
  score[1] = 0;
}

void setup() {

  pinMode(buttonPin,INPUT);
  digitalWrite(buttonPin, HIGH);


  
  lc.shutdown(0, false);
  lc.setIntensity(0, 7);
  lc.clearDisplay(0);
  lcdq.begin(16, 2);
  lcdq.clear();
  lcdq.setCursor(3, 0);
  lcdq.print("INCEPEM ");
  //semnal de generare numere aleatorii
  randomSeed(analogRead(0));
}


void loop() {
  
  
  buttonState = digitalRead(buttonPin);
  //porneste din buton
if(buttonState == 0){
  
  if (gameOver) {
    lcdq.begin(16, 2);
    lcdq.clear();
    lcdq.setCursor(4, 2);
    lcdq.print("GAME OVER !");
    
    //ruleaza secventa endGame

    if (score[0] < 3 && score[1] < 3)
      Scors();
    else {
      Scors();
      endGame();
    }


    //Resetare joc
    br = (int)random(2, 5);
    bc = (int)random(2, 5);
    brr = 1;
    bcc = 1;
    eskill = 1;
    gameOver = false;
    ballDelayLimit = 400;
    delay(1000);
    lc.clearDisplay(0);
    lcdq.begin(16, 2);
    lcdq.clear();
    lcdq.setCursor(0, 0);
    lcdq.print("URMATOAREA RUNDA");
    lcdq.setCursor(6, 1);
    lcdq.print("....");
  }
  else {
    //citire valoare cotroller
    cval = analogRead(controller);
    //mapez valoarea
    pos = map(cval, 50, 950, 5, 0);
    //generez bara
    drawBat(pos, 0);

    //pozitia robotului
    if (ballDelayLimit == 70) {
      eskill = 0.9;
    }
    if (ballDelay > ballDelayLimit) {
      //oprire bila
      lc.setLed(0, br, bc, false);
      //creare pozitie noua
      br = br + brr;
      bc = bc + bcc;
      //cream noua bila
      lc.setLed(0, br, bc, true);

      //Reset the delay
      ballDelay = 0;

      //marire viteza bilei
      ballDelayLimit = max(ballDelayLimit - 0.5, 70);

      //misca bara robotului
      if (bc < epos) {
        epos = epos - eskill;
      } else if (bc > epos + 2) {
        epos = epos + eskill;
      }

      //afiseaza bara robotului
      drawBat(epos, 7);

      //next 
      bounce();
    }
    else {
      ballDelay++;

    }
  }


}else {
    lcdq.begin(16, 2);
    lcdq.clear();
    lcdq.setCursor(2, 0);
    lcdq.print("SFARSIT !");
    delay(1000);
     lcdq.clear();
    lc.clearDisplay(0);
    for (int i = 0; i <= 7; i++)
        for (int j = 0; j <= 7; j++) {
          lc.setLed(0, i, j, false);
      }
     
  
  


  
}
}
