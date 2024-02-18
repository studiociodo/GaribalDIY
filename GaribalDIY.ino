#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Sim800l.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);


Sim800l Sim800l;

const byte ROWS = 4;
const byte COLS = 4;
char Keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad( makeKeymap(Keys), rowPins, colPins, ROWS, COLS);

unsigned long numerocell;
uint8_t zonaA;
uint8_t zonaB;
uint8_t zonaC;
uint8_t zonaD;
unsigned long Atime = millis();
unsigned long Btime;
boolean arresto = false;

void setup() {
  lcd.init();

  lcd.setCursor(0, 0);
  lcd.print(" GaribalDIY 1.0 ");
  lcd.setCursor(0, 1);
  lcd.print("di Chiodi Marco ");
  delay(2000);

  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  CARICAMENTO   ");
  lcd.setCursor(0, 1);
  lcd.print("   ATTENDERE    ");

  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
  pinMode(16, OUTPUT);
  digitalWrite(16, HIGH);
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);

  numerocell = EEPROMReadlong(0);
  zonaA = EEPROMReadlong(4);
  zonaB = EEPROMReadlong(5);
  zonaC = EEPROMReadlong(6);
  zonaD = EEPROMReadlong(7);

  // Serial.begin(9600);

  Sim800l.begin();
  delay(10000);
  Sim800l.delAllSms();
  potenzasegnale();

}

void loop() {
  Btime = millis();

  lcd.setCursor(0, 0);
  lcd.print("     PRONTO     ");
  char key = keypad.getKey();
  if (key != NO_KEY) {
    if (key == '#') {
      impostanumero();
    }
    if (key == 'A') {
      impostaA();
    }
    if (key == 'B') {
      impostaB();
    }
    if (key == 'C') {
      impostaC();
    }
    if (key == 'D') {
      impostaD();
    }
  }
  if ((Btime - Atime) >= 5000) {
    potenzasegnale();
    cercasms();
    Atime = millis();
    Btime = Atime;
  }
}

void EEPROMWritelong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void potenzasegnale() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  RICERCA DATI  ");
  lcd.setCursor(0, 1);
  lcd.print("SEGNALE: ");
  String valoredb = Sim800l.signalQuality();
  uint8_t intdb = valoredb.toInt();
  lcd.print(intdb);
  lcd.print("/31");

}

void impostanumero () {
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("IMPOSTA NUMERO: ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(numerocell);
  String tempstring;
  boolean daimpostare = true;
  while (daimpostare) {
    char key = keypad.getKey();
    if (key != NO_KEY ) {
      if (key == '*' ) {
        delay(200);
        lcd.setCursor(0, 1);
        lcd.print("OK!             ");
        delay(1000);
        daimpostare = false;
        if (tempstring != "") {
          numerocell = tempstring.toInt();
          EEPROMWritelong(0, numerocell);
        }
        Atime = 0;

      } else {
        if ((key == 'A') || (key == 'B') || (key == 'C') || (key == 'D') || (key == '#') ) {
          delay(200);

        } else {
          delay(200);
          tempstring = String(tempstring + key);
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print(tempstring);
        }
      }

    }


  }
}

void impostaA () {
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("MINUTI ZONA A:  ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(zonaA);
  String tempstring;
  boolean daimpostare = true;
  while (daimpostare) {
    char key = keypad.getKey();
    if (key != NO_KEY ) {
      if (key == '*' ) {
        delay(200);
        lcd.setCursor(0, 1);
        lcd.print("OK!             ");
        daimpostare = false;
        delay(1000);
        if ((tempstring != "") && (tempstring.toInt() <= 255)){
          zonaA = tempstring.toInt();
          EEPROM.write(4, zonaA);
        }
        Atime = 0;

      } else {
        if ((key == 'A') || (key == 'B') || (key == 'C') || (key == 'D') || (key == '#') ) {
          delay(200);

        } else {
          delay(200);
          tempstring = String(tempstring + key);
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print(tempstring);
        }
      }

    }


  }
}

void impostaB () {
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("MINUTI ZONA B:  ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(zonaB);
  String tempstring;
  boolean daimpostare = true;
  while (daimpostare) {
    char key = keypad.getKey();
    if (key != NO_KEY ) {
      if (key == '*' ) {
        delay(200);
        lcd.setCursor(0, 1);
        lcd.print("OK!             ");
        daimpostare = false;
        delay(1000);
        if ((tempstring != "") && (tempstring.toInt() <= 255)) {
          zonaB = tempstring.toInt();
          EEPROM.write(5, zonaB);
        }
        Atime = 0;

      } else {
        if ((key == 'A') || (key == 'B') || (key == 'C') || (key == 'D') || (key == '#') ) {
          delay(200);

        } else {
          delay(200);
          tempstring = String(tempstring + key);
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print(tempstring);
        }
      }

    }


  }
}

void impostaC () {
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("MINUTI ZONA C:  ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(zonaC);
  String tempstring;
  boolean daimpostare = true;
  while (daimpostare) {
    char key = keypad.getKey();
    if (key != NO_KEY ) {
      if (key == '*' ) {
        delay(200);
        lcd.setCursor(0, 1);
        lcd.print("OK!             ");
        daimpostare = false;
        delay(1000);
        if ((tempstring != "") && (tempstring.toInt() <= 255)) {
          zonaC = tempstring.toInt();
          EEPROM.write(6, zonaC);
        }
        Atime = 0;

      } else {
        if ((key == 'A') || (key == 'B') || (key == 'C') || (key == 'D') || (key == '#') ) {
          delay(200);

        } else {
          delay(200);
          tempstring = String(tempstring + key);
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print(tempstring);
        }
      }

    }


  }
}

void impostaD () {
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("MINUTI ZONA D:  ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(zonaD);
  String tempstring;
  boolean daimpostare = true;
  while (daimpostare) {
    char key = keypad.getKey();
    if (key != NO_KEY ) {
      if (key == '*' ) {
        delay(200);
        lcd.setCursor(0, 1);
        lcd.print("OK!             ");
        daimpostare = false;
        delay(1000);
        if ((tempstring != "") && (tempstring.toInt() <= 255)) {
          zonaD = tempstring.toInt();
          EEPROM.write(7, zonaD);
        }
        Atime = 0;

      } else {
        if ((key == 'A') || (key == 'B') || (key == 'C') || (key == 'D') || (key == '#') ) {
          delay(200);

        } else {
          delay(200);
          tempstring = String(tempstring + key);
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print(tempstring);
        }
      }

    }


  }
}

void cercasms() {
  String textSms = Sim800l.readSms(1);
  String numerocellS = String(numerocell);
  if ((textSms.indexOf("OK") != -1) && (textSms.indexOf(numerocellS) != -1))
  {
    if ((textSms.indexOf("utte") != -1) || (textSms.indexOf("ona a") != -1) || (textSms.indexOf("ona b") != -1) || (textSms.indexOf("ona c") != -1) || (textSms.indexOf("ona d") != -1)) {
      char numerocellC[11];
      numerocellS.toCharArray(numerocellC, 11);

      if ((textSms.indexOf("Tutte") != -1) || (textSms.indexOf("tutte") != -1)) {
        arresto = false;
        Sim800l.sendSms( numerocellC , "Obbedisco! (A+B+C+D)");
        timerzona(zonaA, 14);
        timerzona(zonaB, 15);
        timerzona(zonaC, 16);
        timerzona(zonaD, 17);
        lcd.setCursor(0, 0);
        lcd.print("FATTO! (A+B+C+D)");
        delay(5000);
      }

      if ((textSms.indexOf("Zona a") != -1) || (textSms.indexOf("zona a") != -1)) {
        arresto = false;
        Sim800l.sendSms( numerocellC , "Obbedisco! (A)");
        timerzona(zonaA, 14);
        lcd.setCursor(0, 0);
        lcd.print("   FATTO! (A)   ");
        delay(5000);
      }

      if ((textSms.indexOf("Zona b") != -1) || (textSms.indexOf("zona b") != -1)) {
        arresto = false;
        Sim800l.sendSms( numerocellC , "Obbedisco! (B)");
        timerzona(zonaB, 15);
        lcd.setCursor(0, 0);
        lcd.print("   FATTO! (B)   ");
        delay(5000);
      }
      if ((textSms.indexOf("Zona c") != -1) || (textSms.indexOf("zona c") != -1)) {
        arresto = false;
        Sim800l.sendSms( numerocellC , "Obbedisco! (C)");
        timerzona(zonaC, 16);
        lcd.setCursor(0, 0);
        lcd.print("   FATTO! (C)   ");
        delay(5000);
      }
      if ((textSms.indexOf("Zona d") != -1) || (textSms.indexOf("zona d") != -1)) {
        arresto = false;
        Sim800l.sendSms( numerocellC , "Obbedisco! (D)");
        timerzona(zonaD, 17);
        lcd.setCursor(0, 0);
        lcd.print("   FATTO! (D)   ");
        delay(5000);
      }

      Sim800l.delAllSms();

    } else {
      Sim800l.delAllSms();
    }


  } else {
    Sim800l.delAllSms();
  }
}

void timerzona ( uint8_t zona, uint8_t pin ) {
  if (arresto == false) {
    lcd.setCursor(0, 0);
    lcd.print("   AZIONAMENTO  ");
    lcd.setCursor(0, 1);
    lcd.print("IN CORSO ZONA ");
    switch (pin) {
      case 14:
        lcd.print("A");
        break;
      case 15:
        lcd.print("B");
        break;
      case 16:
        lcd.print("C");
        break;
      case 17:
        lcd.print("D");
        break;
    }

    digitalWrite(pin, LOW);
    for (int i = 0; i < zona; i++) {
      delay(57000);
      smsarresto();
      if (arresto == true) {
        break;
      }
    }
    digitalWrite(pin, HIGH);
    lcd.clear();
  }
}

void smsarresto() {
  String textSms = Sim800l.readSms(1);
  String numerocellS = String(numerocell);
  if ((textSms.indexOf("OK") != -1) && (textSms.indexOf(numerocellS) != -1) && (textSms.indexOf("erma") != -1))
  {
    if ((textSms.indexOf("Ferma") != -1) || (textSms.indexOf("ferma") != -1)) {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("ARRESTO IN CORSO");
      char numerocellC[11];
      numerocellS.toCharArray(numerocellC, 11);
      Sim800l.sendSms( numerocellC , "Obbedisco! (Arresto)");
      digitalWrite(14, HIGH);
      digitalWrite(15, HIGH);
      digitalWrite(16, HIGH);
      digitalWrite(17, HIGH);
      arresto = true;
      delay(3000);
      Sim800l.delAllSms();
    } else {
      Sim800l.delAllSms();
    }
  } else {
    Sim800l.delAllSms();
  }
}

