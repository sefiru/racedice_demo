#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void (*rightAction)();
void (*upAction)();
void (*downAction)();
void (*leftAction)();
void (*selectAction)();

struct GAME {
  int id;
  String _name;
  int gameStatus;
  int gameSpeed;
  int p1;
  int p2;
  int p3;
  int p4;
  byte dice1;
  byte dice2;
  byte dice3;
  byte dice4;
};

struct RACE {
  byte road[2][16];
  char bullet[2];
};

RACE raceGame;

GAME dice = {1, "Dice", 0, 500, 0, 0, 0, 0, 0, 0, 0, 0};

GAME race = {2, "Race", 0, 300, 0, 0, 0, 0, 0, 0, 0, 0};

int presed = 0;

int programmPosition = -1;
int programmPositionSet = 0;

unsigned long timer = 0;

GAME (*gameSelected) = &dice;

byte left[8] = {
  B00010,
  B00110,
  B01110,
  B11110,
  B11110,
  B01110,
  B00110,
  B00010,
};

byte right[8] = {
  B01000,
  B01100,
  B01110,
  B01111,
  B01111,
  B01110,
  B01100,
  B01000,
};

byte car[8] = {
  B00000,
  B00000,
  B10010,
  B11111,
  B11111,
  B10010,
  B00000,
  B00000,
};

byte wall[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte target[8] = {
  B10101,
  B10101,
  B10101,
  B10101,
  B10101,
  B10101,
  B10101,
  B10101,
};

byte bullet[8] = {
  B00000,
  B00000,
  B11110,
  B11111,
  B11111,
  B11110,
  B00000,
  B00000,
};

void setup () {
  pinMode(0, INPUT);
  digitalWrite(0, HIGH);
 
  lcd.createChar(0, left);
  lcd.createChar(1, right);
  lcd.createChar(2, car);
  lcd.createChar(3, wall);
  lcd.createChar(4, target);
  lcd.createChar(5, bullet);
  lcd.begin(16, 2);
 
}
 
void loop () {
  checkButtons();
  checkProgrammPosition();
  gameCycle();
}

void raceReset () {
  raceGame = {{{2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {-1, -1}};
}

boolean isBulletExist () {
  for ( byte y = 0; y < 2; y++ ) {
    for ( byte x = 0; x < 16; x++ ) {
      if ( raceGame.road[y][x] == 5 )
        return true;
    }
  }
  return false;
}

void rightActionDefault () {
  if ( gameSelected == &dice ) {
    gameSelected = &race;
  } else {
    gameSelected = &dice;
  }
  printGameName();
  delay(200);
}
void upActionDefault () {}
void downActionDefault () {}
void leftActionDefault () {
  if ( gameSelected == &dice ) {
    gameSelected = &race;
  } else {
    gameSelected = &dice;
  }
  printGameName();
  delay(200);
}
void selectActionDefault () {
  programmPositionSet = 1;
  delay(200);
}

void diceAction (int plrNum) {
  switch ( plrNum ) {
    case 1: gameSelected->p1++; break;
    case 2: gameSelected->p2++; break;
    case 3: gameSelected->p3++; break;
    case 4: gameSelected->p4++; break;
  }
}

void rightActionGame () {
  if ( gameSelected == &dice ) {
    diceAction(4);
  } else {
    if ( !isBulletExist() ) {
      if ( raceGame.road[0][0] == 2 ) {
        if ( raceGame.road[0][1] != 3 )
          raceGame.road[0][1] = 5;
      } else {
        if ( raceGame.road[1][1] != 3 )
          raceGame.road[1][1] = 5;
      }
    }
  }
}
void upActionGame () {
  if ( gameSelected == &dice ) {
    diceAction(3);
  } else {
    if ( raceGame.road[0][0] == 0 ) {
      raceGame.road[0][0] = 2;
      raceGame.road[1][0] = 0;
    }
  }
}
void downActionGame () {
  if ( gameSelected == &dice ) {
    diceAction(2);
  } else {
    if ( raceGame.road[1][0] == 0 ) {
      raceGame.road[1][0] = 2;
      raceGame.road[0][0] = 0;
    }
  }
}
void leftActionGame () {
  if ( gameSelected == &dice ) {
    diceAction(1);
  }
}
void selectActionGame () {
  if ( gameSelected == &dice ) {
    byte results[4] = {dice.dice1, dice.dice2, dice.dice3, dice.dice4};
    for ( byte i = 0; i < 4; i++ ) {
      for ( byte k = 0; k < 3; k++ ) {
        if ( results[k] < results[k + 1] ) {
          byte temp = results[k];
          results[k] = results[k + 1];
          results[k + 1] = temp;
        }
      }
    }
    lcd.clear();
    if ( results[0] == results[1] ) {
      lcd.setCursor(7 - ((sizeof("Draw") - 1) / 2 - 1), 0);
      lcd.print("Draw");
    } else if ( results[0] == dice.dice1 ) {
      lcd.setCursor(7 - ((sizeof("Left Win") - 1) / 2 - 1), 0);
      lcd.print("Left Win");
    } else if ( results[0] == dice.dice2 ) {
      lcd.setCursor(7 - ((sizeof("Down Win") - 1) / 2 - 1), 0);
      lcd.print("Down Win");
    } else if ( results[0] == dice.dice3 ) {
      lcd.setCursor(7 - ((sizeof("Up Win") - 1) / 2 - 1), 0);
      lcd.print("Up Win");
    } else if ( results[0] == dice.dice4 ) {
      lcd.setCursor(7 - ((sizeof("Right Win") - 1) / 2 - 1), 0);
      lcd.print("Right Win");
    }
    delay(3000);
  } else {
    lcd.clear();
    lcd.setCursor(7 - ((sizeof("The End") - 1) / 2 - 1), 0);
    lcd.print("The End");
    delay(3000);
  }
  programmPositionSet = 0;
  gameSelected->gameStatus = 0;
  delay(200);
}

void screenMain () {
  rightAction = &rightActionDefault;
  upAction = &upActionDefault;
  downAction = &downActionDefault;
  leftAction = &leftActionDefault;
  selectAction = &selectActionDefault;
  
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("SELEGD  GAEM");
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(15, 1);
  lcd.write(byte(1));
  
  printGameName();
}

void printGameName () {
  lcd.setCursor(6, 1);
  lcd.print(gameSelected->_name);
}

void diceReset () {
    dice.p1 = 0;
    dice.p2 = 0;
    dice.p3 = 0;
    dice.p4 = 0;
    dice.dice1 = 0;
    dice.dice2 = 0;
    dice.dice3 = 0;
    dice.dice4 = 0;
}

void screenGame () {
  rightAction = &rightActionGame;
  upAction = &upActionGame;
  downAction = &downActionGame;
  leftAction = &leftActionGame;
  selectAction = &selectActionGame;
  
  lcd.clear();
  if ( gameSelected == &dice ) {
    diceReset();
    printDice();
  } else {
    raceReset();
    printRace();
  }
}

void printDice () {
  lcd.print("Left: 0 ");
  lcd.print("Up:    0");
  lcd.setCursor(0, 1);
  lcd.print("Down: 0 ");
  lcd.print("Right: 0");
}

void printRace () {
  if ( raceGame.road[1][0] == 1 ) {
    lcd.setCursor(0, 1);
  }
  lcd.write(byte(2));
}

void printFrame () {
  lcd.clear();
  for ( byte y = 0; y < 2; y++ ) {
    for ( byte x = 0; x < 16; x++ ) {
      if ( raceGame.road[y][x] != 0 ) {
        lcd.setCursor(x, y);
        lcd.write(byte(raceGame.road[y][x]));
      }
    }
  }
}

void bulletFly () {
  for ( byte y = 0; y < 2; y++ ) {
    for ( byte x = 0; x < 16; x++ ) {
      if ( raceGame.road[y][x] == 5 ) {
        if ( x == 15 ) {
          raceGame.road[y][x] = 0;
          return;
        }
        if ( raceGame.road[y][x + 1] == 0 ) {
          raceGame.road[y][x] = 0;
          raceGame.road[y][x + 1] = 5;
          return;
        } else if ( raceGame.road[y][x + 1] == 3 ) {
          raceGame.road[y][x] = 0;
          return;
        } else {
          raceGame.road[y][x] = 0;
          raceGame.road[y][x + 1] = 0;
          return;
        }
      }
    }
  }
}

void wallTick () {
  for ( byte y = 0; y < 2; y++ ) {
    for ( byte x = 0; x < 16; x++ ) {
      if ( raceGame.road[y][x] == 3 || raceGame.road[y][x] == 4 ) {
        if ( x == 0 && ( raceGame.road[y][x] == 3 || raceGame.road[y][x] == 4 ) ) {
          raceGame.road[y][x] = 0;
          continue;
        }
        if ( raceGame.road[y][x - 1] == 5 ) {
          if ( raceGame.road[y][x] == 4 )
            raceGame.road[y][x - 1] = 0;
          else
            raceGame.road[y][x - 1] = 3;
          raceGame.road[y][x] = 0;
        } else if ( raceGame.road[y][x - 1] == 2 ) {
          lcd.clear();
          lcd.setCursor(7 - ((sizeof("The End") - 1) / 2 - 1), 0);
          lcd.print("The End");
          delay(3000);
          programmPositionSet = 0;
          gameSelected->gameStatus = 0;
        } else {
          raceGame.road[y][x - 1] = raceGame.road[y][x];
          raceGame.road[y][x] = 0;
        }
      }
    }
  }
  
  if ( random(100) < 50 ) {
    if ( random(100) < 50 ) {
      if ( random(100) < 50 ) {
        if ( raceGame.road[1][14] != 3 && raceGame.road[1][15] != 3 )
          raceGame.road[0][15] = 3;
      } else {
        if ( raceGame.road[0][14] != 3 && raceGame.road[0][14] != 4 )
          raceGame.road[0][15] = 4;
      }
    } else {
      if ( random(100) < 50 ) {
        if ( raceGame.road[0][14] != 3 && raceGame.road[0][15] != 3 )
          raceGame.road[1][15] = 3;
      } else {
        if ( raceGame.road[1][14] != 3 && raceGame.road[1][14] != 4 )
          raceGame.road[1][15] = 4;
      }
    }
  }
}

unsigned long raceFrameTimer = 0;
unsigned long raceBulletTimer = 0;
unsigned long raceWallTimer = 0;

void gameCycle () {
  if ( gameSelected == &dice && programmPosition > 0 && millis() >= timer + gameSelected->gameSpeed ) {
    timer = millis();
    if ( gameSelected->p1 == 1 ) {
      gameSelected->dice1 = random(10);
      lcd.setCursor(6, 0);
      lcd.print(gameSelected->dice1);
    }
    if ( gameSelected->p2 == 1 ) {
      gameSelected->dice2 = random(10);
      lcd.setCursor(6, 1);
      lcd.print(gameSelected->dice2);
    }
    if ( gameSelected->p3 == 1 ) {
      gameSelected->dice3 = random(10);
      lcd.setCursor(15, 0);
      lcd.print(gameSelected->dice3);
    }
    if ( gameSelected->p4 == 1 ) {
      gameSelected->dice4 = random(10);
      lcd.setCursor(15, 1);
      lcd.print(gameSelected->dice4);
    }
  }

  if ( gameSelected == &race && programmPosition > 0 ) {
    if ( millis() >= raceBulletTimer + 150 ) {
      raceBulletTimer = millis();
      bulletFly();
    }
    if ( millis() >= raceWallTimer + 300 ) {
      raceWallTimer = millis();
      wallTick();
    }
    if ( millis() >= raceFrameTimer + 100 ) {
      raceFrameTimer = millis();
      printFrame();
    }
  }
}

void checkProgrammPosition () {
  if ( programmPosition != programmPositionSet ) {
    programmPosition = programmPositionSet;
    switch ( programmPosition ) {
      case 0: screenMain(); break;
      case 1: screenGame(); break;
    }
  }
}

void checkButtons () {
  int buttonPressed = analogRead(A0);
  if ( checkButton(buttonPressed, 1, "right", 110, rightAction) )
    return;
  else if ( checkButton(buttonPressed, 2, "up", 310, upAction) )
    return;
  else if ( checkButton(buttonPressed, 3, "down", 450, downAction) )
    return;
  else if ( checkButton(buttonPressed, 4, "left", 600, leftAction) )
    return;
  else 
    checkButton(buttonPressed, 5, "select", 800, selectAction);
}

boolean checkButton (int analogResult, int buttonNum, String buttonName, int analogData, void (*test)()) {
  if ( analogResult < analogData  ) {
    if ( presed != buttonNum ) {
      presed = buttonNum;
      test();
    }
    return true;
  } else {
    if ( presed == buttonNum )
      presed = -1;
  }
  return false;
}

