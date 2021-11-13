#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

const int BUTTON1 = 3;            // MOD 1
const int BUTTON2 = 4;           //  SELECKT
const int BUTTON3 = 2;          //   UP -
const int BUTTON5 = 5;         //    MOD 2
const int BUTTON7 = 6;        //     +
const int P1 = 7;            //      Time INFO (saat-dakika-gün-ay-yıl)
const int BUTTON10 = 8;     //       ON TUŞU (AKTİFLEME TUŞU) YANİ SAATİ AKTİF YAP

byte customChar1[] = {
  0x04, 0x04, 0x04, 0x15, 0x0E, 0x04, 0x00, 0x00
};
byte customChar2[] = {
  0x04, 0x08, 0x1F, 0x08, 0x04, 0x00, 0x00, 0x00
};

#include <OneWire.h>
#include <DallasTemperature.h>
#define Nem_PIN A0
int Nem_Deger = 0 ;

#define ISI_PIN 9
OneWire ISI (ISI_PIN);
DallasTemperature ISI_Sensor(&ISI);

#include <RTClib.h>
RTC_DS1307 RTC;

int hourupg, minupg, yearupg, monthupg, dayupg;
int menu = 0, setAll = 0, ekran = 0,  OK = 0, ON = 0, E = 0, N = 0, A = 0, K = 0, L = 0;
char DOW[][10] = {"Pazar    ", "Pazartesi", "Sali     ", "Carsamba ", "Persembe", "Cuma     ", "Cumartesi"};

int degerler[7 * 8];  // 0 .... 56

int lcdsaatx[] = {0, 13, 0, 13};
int lcdsaaty[] = {2, 2, 3, 3};
int lcddakikax[] = {3, 16, 3, 16};
int lcddakikay[] = {2, 2, 3, 3};
/*int virgulx[] = {2, 15, 2, 15};
  int virguly[] = {2, 2, 3, 3};*/

bool ekrandegisti;
int kursorblink = false;
unsigned long  kursorzamani = millis();

void EKRAN() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    ekran = ekran + 1;
    if (ekran == 2) {
      ekran = 0;
      A = 0;
    }
    ekrandegisti = true;
    last_interrupt_time = interrupt_time;
  }
}

void UP() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    A = A - 1;
    if (A <= 0) {
      A = 0;
    }
  }
  last_interrupt_time = interrupt_time;
}

void setup() {
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON5, INPUT_PULLUP);
  pinMode(BUTTON7, INPUT_PULLUP);
  pinMode(P1, INPUT_PULLUP);                                    // SET TIME
  pinMode(BUTTON10, INPUT_PULLUP);                             //  ACTIVE TIME
  pinMode(12, OUTPUT);                                        //   RELAY
  attachInterrupt(digitalPinToInterrupt(3), EKRAN, RISING);  //    EKRAN 0 - EKRAN 1
  attachInterrupt(digitalPinToInterrupt(2), UP, RISING);    //     -
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customChar1); lcd.createChar(1, customChar2);
  Wire.begin();
  RTC.begin();
  Serial.begin(9600);
  EEPROM.put(0, degerler);
  EEPROM.get(0, degerler);
  if (! RTC.isrunning()) {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}
void loop()
{
  if (ekrandegisti) {
    lcd.clear(); ekrandegisti = false;
  }
  OK = digitalRead(BUTTON5); ON = digitalRead(BUTTON10);

  ////////////////set time//////////
  if (ON == 0) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("  ZAMAN AYARLARI    "); E = 1; N = 1; time();
  }
  if (OK == 0 && ekran == 0) {
    K = 0; L = 0; nem();
  }
  if (ekran == 0) {
    Nem_Deger = analogRead(Nem_PIN); ISI_Sensor.requestTemperatures(); DateTime now = RTC.now();
    lcd.setCursor(0, 3); lcd.print(" "); lcd.setCursor(0, 2); lcd.print("     "); lcd.setCursor(0, 0); lcd.print("   ");  lcd.setCursor(11, 1); lcd.print("  "); lcd.setCursor(15, 2); lcd.print("   ");
    lcd.setCursor(13, 1); lcd.print("Mod 1"); lcd.setCursor(3, 0);  lcd.print("NEM DEGER:"); lcd.setCursor(13, 0); lcd.print(Nem_Deger); lcd.setCursor(0, 1); lcd.print("ISI:"); lcd.setCursor(4, 1); lcd.print(ISI_Sensor.getTempCByIndex(0)); lcd.setCursor(9, 1); lcd.print("dC");
    lcd.setCursor(1, 3);
    if (now.hour() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.hour(), DEC);
    lcd.print(":");
    if (now.minute() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.minute(), DEC);
    lcd.print(":");
    if (now.second() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.second(), DEC);
    lcd.setCursor(5, 2);
    if (now.day() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.day(), DEC);
    lcd.print("/");
    if (now.month() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.month(), DEC);
    lcd.print("/");
    lcd.print(now.year(), DEC);
    lcd.setCursor(10, 3);
    lcd.print(DOW[now.dayOfTheWeek()]);

    for (int f = 0; f < 7; f++) {
      //0 : pazar
      if (now.dayOfTheWeek() == f  &&  now.hour() == degerler[f * 8] && now.minute() == degerler[f * 8 + 1]) {
        digitalWrite(12, HIGH);
        ekran = 0;
      }
      if (now.dayOfTheWeek() == f &&  now.hour() == degerler[f * 8 + 2] && now.minute() == degerler[f * 8 + 3]) {
        digitalWrite(12, LOW);
        ekran = 0;
      }
      if (now.dayOfTheWeek() == f  &&  now.hour() == degerler[f * 8 + 4] && now.minute() == degerler[f * 8 + 5]) {
        digitalWrite(12, HIGH);
        ekran = 0;
      }
      if (now.dayOfTheWeek() == f &&  now.hour() == degerler[f * 8 + 6] && now.minute() == degerler[f * 8 + 7]) {
        digitalWrite(12, LOW);
        ekran = 0;
      }
    }
    ////////////////////////////////////////////////////bitti////////////////////////////////////////////////
  }
  if (ekran == 1) {
    lcd.setCursor(2, 2); lcd.print(":"); lcd.setCursor(15, 2); lcd.print(":");
    lcd.setCursor(2, 3); lcd.print(":"); lcd.setCursor(15, 3); lcd.print(":");
    lcd.setCursor(0, 0); lcd.print(">"); lcd.setCursor(0, 1); lcd.print("BASLANGIC");
    lcd.setCursor(13, 1); lcd.print("BITIS"); lcd.setCursor(18, 3); lcd.print(" ");
    if (digitalRead(6) == LOW) {
      while (digitalRead(6) == LOW) {}
      A = A + 1;
      Serial.println(A);
    }
    lcd.setCursor(1, 0);
    lcd.print(DOW[A]);
    lcd.setCursor(0, 2);
    if (degerler[A * 8] <= 9)
    {
      lcd.print("0");
    }
    lcd.print(degerler[A * 8], DEC);
    lcd.setCursor(3, 2);
    if (degerler[A * 8 + 1] <= 9)
    {
      lcd.print("0");
    }
    lcd.print(degerler[A * 8 + 1], DEC);
    lcd.setCursor(13, 2);
    if (degerler[A * 8 + 2] <= 9)
    {
      lcd.print("0");
    }
    lcd.print(degerler[A * 8 + 2], DEC);
    lcd.setCursor(16, 2);
    if (degerler[A * 8 + 3] <= 9)
    {
      lcd.print("0");
    }
    lcd.print(degerler[A * 8 + 3], DEC);
    lcd.setCursor(0, 3);
    if (degerler[A * 8 + 4] <= 9)
    {
      lcd.print("0");
    }
    lcd.print(degerler[A * 8 + 4], DEC);
    lcd.setCursor(3, 3);
    if (degerler[A * 8 + 5] <= 9)
    {
      lcd.print("0");
    }
    lcd.print(degerler[A * 8 + 5], DEC);
    lcd.setCursor(13, 3);
    if (degerler[A * 8 + 6] <= 9)
    {
      lcd.print("0");
    }
    lcd.print(degerler[A * 8 + 6], DEC);
    lcd.setCursor(16, 3);
    if (degerler[A * 8 + 7] <= 9)
    {
      lcd.print("0");
    }
    lcd.print(degerler[A * 8 + 7], DEC);
    if ( digitalRead(BUTTON2) == 0) {
      while (digitalRead(BUTTON2) == 0) {}
      for (int f = 0; f < 4; f++) {
        DisplaySetHourAll(A, f);         // günlerin indisi
        DisplaySetMinuteAll(A, f);      // f: sira
      }
      ekran = 0;
    }
  }
  if (A == 7) {
    A = 0;
  }
}
void DisplaySetHourAll(int gun, int sira) {
  Serial.println("Gün : " + String(gun) + "  Sıra : " + String(sira));
  kursorblink = false;
  kursorzamani = millis();
  while (digitalRead(4) == HIGH) {
    lcd.setCursor(lcdsaatx[sira], lcdsaaty[sira]);
    if (millis() - kursorzamani > 300) {
      kursorblink = !kursorblink;
      kursorzamani = millis();
    }
    if (digitalRead(6) == LOW)
    {
      while (digitalRead(6) == LOW) {}
      if (degerler[gun * 7 + sira  ] == 23)
      {
        degerler[gun * 7 + sira  ] = 0;
      }
      else
      {
        degerler[gun * 7 + sira  ] = degerler[gun * 7 + sira ] + 1;
      }
    }
    if (digitalRead(2) == LOW)
    {
      while (digitalRead(2) == LOW) {}
      if (degerler[gun * 7 + sira ] == 0)
      {
        degerler[gun * 7 + sira ] = 23;
      }
      else
      {
        degerler[gun * 7 + sira ] = degerler[gun * 7 + sira ] - 1;
      }
    }
    lcd.setCursor(lcdsaatx[sira], lcdsaaty[sira]);
    if (kursorblink) {
      lcd.print("  ");
    } else {
      saatyaz(gun, sira);
    }
  }
  saatyaz(gun, sira);
  EEPROM.put(0, degerler);
  while (digitalRead(4) == LOW) {
  }
  delay(10);
}
void DisplaySetMinuteAll(int gun, int sira) {
  kursorblink = false;
  kursorzamani = millis();
  while (digitalRead(4) == HIGH) {
    if (millis() - kursorzamani > 300) {
      kursorblink = !kursorblink;
      kursorzamani = millis();
    }
    if (digitalRead(6) == LOW)
    {
      while (digitalRead(6) == LOW) {}
      if (degerler[gun * 7 + + 1] == 59)
      {
        degerler[gun * 7 +  + 1] = 0;
      }
      else
      {
        degerler[gun * 7 + sira  + 1] = degerler[gun * 7 + sira  + 1] + 1;
      }
    }
    if (digitalRead(2) == LOW)
    {
      while (digitalRead(2) == LOW) {}
      if (degerler[gun * 7 + sira  + 1] == 0)
      {
        degerler[gun * 7 + sira  + 1] = 59;
      }
      else
      {
        degerler[gun * 7 + sira  + 1] = degerler[gun * 7 + sira  + 1] - 1;
      }
    }
    lcd.setCursor(lcddakikax[sira], lcddakikay[sira]);
    if (kursorblink) {
      lcd.print("  ");
    } else {
      dakikayaz(gun, sira);
    }
  }
  dakikayaz(gun, sira);
  EEPROM.put(0, degerler);
  while (digitalRead(4) == LOW) {
  }
  delay(10);
}

void saatyaz(int gun, int sira) {
  lcd.setCursor(lcdsaatx[sira], lcdsaaty[sira]);
  if (degerler[gun * 7 + sira  ] <= 9)
  {
    lcd.print("0");
  }
  lcd.print(degerler[gun * 7 + sira ], DEC);
}

void dakikayaz(int gun, int sira) {
  lcd.setCursor(lcddakikax[sira], lcddakikay[sira]);
  if (degerler[gun * 7 + sira * 1 + 1] <= 9)
  {
    lcd.print("0");
  }
  lcd.print(degerler[gun * 7 + sira  + 1], DEC);
}

void time() {
  while (E == 1 && N == 1) {
    if (digitalRead(7) == LOW)
    {
      while (digitalRead(7) == LOW) {};
      menu = menu + 1;
    }
    if (menu == 0)
    {
      DisplayResult();
    }
    if (menu == 1)
    {
      DisplaySetDay();
    }
    if (menu == 2)
    {
      DisplaySetMonth();
    }
    if (menu == 3)
    {
      DisplaySetYear();
    }
    if (menu == 4)
    {
      DisplaySetHour();
    }
    if (menu == 5)
    {
      DisplaySetMinute();
    }
    if (menu == 6)
    {
      StoreAgg();
    }
  }
}

void DisplayResult()
{
  DateTime now = RTC.now();
  lcd.setCursor(1, 3);
  if (hourupg <= 9)
  {
    lcd.print("0");
  }
  lcd.print(hourupg, DEC);
  lcd.print(":");
  if (minupg <= 9)
  {
    lcd.print("0");
  }
  lcd.print(minupg, DEC);
  lcd.setCursor(5, 2);
  if (dayupg <= 9)
  {
    lcd.print("0");
  }
  lcd.print(dayupg, DEC);
  lcd.print("/");
  if (monthupg <= 9)
  {
    lcd.print("0");
  }
  lcd.print(monthupg, DEC);
  lcd.print("/");
  yearupg = now.year();
  lcd.print(yearupg, DEC);
}

void DisplaySetHour()
{
  //DateTime now = RTC.now();
  lcd.setCursor(2, 2);  lcd.write(byte(0)); lcd.setCursor(14, 1);  lcd.print(" ");
  lcd.setCursor(2, 2);
  lcd.write(byte(0));
  if (digitalRead(6) == LOW)
  {
    if (hourupg == 23)
    {
      hourupg = 0;
    }
    else
    {
      hourupg = hourupg + 1;
    }
  }
  if (digitalRead(2) == LOW)
  {
    if (hourupg == 0)
    {
      hourupg = 23;
    }
    else
    {
      hourupg = hourupg - 1;
    }
  }
  lcd.setCursor(1, 3);
  if (hourupg <= 9) {
    lcd.print("0");
  }
  lcd.print(hourupg, DEC);
  delay(200);
}
void DisplaySetMinute()
{
  lcd.setCursor(6, 3);  lcd.write(byte(1)); lcd.setCursor(2, 2);  lcd.print(" ");
  /*lcd.setCursor(1, 1);
    lcd.print(" ");*/
  if (digitalRead(6) == LOW)
  {
    if (minupg == 59)
    {
      minupg = 0;
    }
    else
    {
      minupg = minupg + 1;
    }
  }
  if (digitalRead(2) == LOW)
  {
    if (minupg == 0)
    {
      minupg = 59;
    }
    else
    {
      minupg = minupg - 1;
    }
  }
  lcd.setCursor(4, 3);
  if (minupg <= 9) {
    lcd.print("0");
  }
  lcd.print(minupg, DEC);
  delay(200);
}

void DisplaySetYear()
{
  lcd.setCursor(14, 1);  lcd.write(byte(0)); lcd.setCursor(9, 1);  lcd.print(" ");
  if (digitalRead(6) == LOW)
  {
    yearupg = yearupg + 1;
  }
  if (digitalRead(2) == LOW)
  {
    yearupg = yearupg - 1;
  }

  lcd.setCursor(11, 2);
  lcd.print(yearupg, DEC);
  delay(200);
}

void DisplaySetMonth()
{
  lcd.setCursor(9, 1);  lcd.write(byte(0)); lcd.setCursor(6, 1);  lcd.print(" ");
  if (digitalRead(6) == LOW)
  {
    if (monthupg == 12)
    {
      monthupg = 1;
    }
    else
    {
      monthupg = monthupg + 1;
    }
  }
  if (digitalRead(2) == LOW)
  {
    if (monthupg == 1)
    {
      monthupg = 12;
    }
    else
    {
      monthupg = monthupg - 1;
    }
  }
  lcd.setCursor(8, 2);
  if (monthupg <= 9) {
    lcd.print("0");
  }
  lcd.print(monthupg, DEC);
  delay(200);
}

void DisplaySetDay()
{
  lcd.setCursor(6, 1);
  lcd.write(byte(0));
  if (digitalRead(6) == LOW)
  {
    if (dayupg == 31)
    {
      dayupg = 1;
    }
    else
    {
      dayupg = dayupg + 1;
    }
  }
  if (digitalRead(2) == LOW)
  {
    if (dayupg == 1)
    {
      dayupg = 31;
    }
    else
    {
      dayupg = dayupg - 1;
    }
  }
  lcd.setCursor(5, 2);
  if (dayupg <= 9) {
    lcd.print("0");
  }
  lcd.print(dayupg, DEC);
  delay(200);
}

void StoreAgg()
{
  lcd.setCursor(6, 1);   lcd.print("YAPILDI  ");   lcd.setCursor(6, 3);   lcd.print(" ");
  RTC.adjust(DateTime(yearupg, monthupg, dayupg, hourupg, minupg, 0));
  delay(2000);
  //ekran = 0;
  asm volatile ("  jmp 0");
}
void nem() {
  while (K == 0 && L == 0) {
    while (digitalRead(5) == LOW) {}
    Nem_Deger = analogRead(Nem_PIN); ISI_Sensor.requestTemperatures();  DateTime now = RTC.now();
    lcd.setCursor(0, 3); lcd.print(" "); lcd.setCursor(0, 2); lcd.print("     "); lcd.setCursor(0, 0); lcd.print("   ");  lcd.setCursor(11, 1); lcd.print("  "); lcd.setCursor(15, 2); lcd.print("   ");
    lcd.setCursor(13, 1); lcd.print("Mod 2"); lcd.setCursor(3, 0);  lcd.print("NEM DEGER:"); lcd.setCursor(13, 0); lcd.print(Nem_Deger); lcd.setCursor(0, 1); lcd.print("ISI:"); lcd.setCursor(4, 1); lcd.print(ISI_Sensor.getTempCByIndex(0)); lcd.setCursor(9, 1); lcd.print("dC");
    lcd.setCursor(1, 3);
    if (now.hour() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.hour(), DEC);
    hourupg = now.hour();
    lcd.print(":");
    if (now.minute() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.minute(), DEC);
    minupg = now.minute();
    lcd.print(":");
    if (now.second() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.second(), DEC);

    lcd.setCursor(5, 2);
    if (now.day() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.day(), DEC);
    dayupg = now.day();
    lcd.print("/");
    if (now.month() <= 9)
    {
      lcd.print("0");
    }
    lcd.print(now.month(), DEC);
    monthupg = now.month();
    lcd.print("/");
    lcd.print(now.year(), DEC);
    yearupg = now.year();
    lcd.setCursor(10, 3);
    lcd.print(DOW[now.dayOfTheWeek()]);
    if (Nem_Deger >= 600 ) {
      digitalWrite(6, LOW);
    }
    if (Nem_Deger <= 400 ) {
      digitalWrite(6, HIGH);
    }
    if (digitalRead(5) == LOW) {
      while (digitalRead(5) == LOW) {}
      asm volatile ("  jmp 0");
    }
  }
}
