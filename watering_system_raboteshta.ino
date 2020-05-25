// използвани библиотеки
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>
  #include <LCD_1602_RUS.h>
// Часовник  
  #include <RTClib.h>
// DHT11 сензор за влажност и температура
  #include <DHT.h>
  #define DHTTYPE DHT11

  
// тонове за зумера
  #define NOTE_C6  1047
  #define NOTE_C3  524
  #define NOTE_G3  1500

// дефиниция на изходи
//  int levelSensorPin = 0;       //Analog pin0  - не се ползва сензор за ниво на водата
  int moistureSensorPin1 = 0;   //Analog pin1   -  Сонда 1
  int moistureSensorPin2 = 1;   //Analog pin2   -  Сонда 2
  int audioPin = 2;             //Digital Pin2  -  Зумер
  int soggyLEDPin = 3;          //Digital Pin3  -  LED мокра почва
  int moistsoilLEDPin = 4;      //Digital Pin4  -  LED влажна почва
  int drysoilLEDPin = 5;        //Digital Pin5  -  LED суха почва
  int pumpLEDPin = 6;           //Digital Pin6  -  LED за клапа/реле
  int pumpPin1 = 7;             //Digital Pin7  -  Реле
  int DHTPin = 8;               //Digital Pin8  -  Сензор за температура и влага
  int menuButtonPin = 9;        //Digital Pin9  -  Копче за менюто
  int adjustButtonPin = 13;     //Digital Pin10 -  Копче за смяна на стойностите на числата

// деклариране на променливи
 // int levelSensorValue;        // stores the level sensor values
  int moistureSensorValue1;    // moisture sensor value bed 1 - Стойност за Сонда 1
  int moistureSensorValue2;    // moisture sensor value bed 2 - Стойност за Сонда 2
  int moistureSensorValue1_percentage;  // Стойност за Сонда 1 в проценти
  int moistureSensorValue2_percentage;  // Стойност за Сонда 2 в проценти
// стойности по подразбиране  
  int chas = 19;                  // Час за поливане
  int minuti = 00;                // И минути към часа по-горе 
  int vreme_za_polivane = 3;      // Времето за продължителността на поливането в минути, ако сондата не затвори клапата
  int menu_pages = 10;              // Броя на менютата за страниците в менюто
  int menuButtonPushCounter = 0;   // Брояч за брой натискания на копчето за менюто
  int menuButtonState = 0;         // Текущо състояние на копчето за менюто
  int menuLastButtonState = 0;     // Предишно състояние на копчето за менюто                 
  int adjustButtonState = 0;       // Текущо състояние на копчето за нагласяне на минутите

   
  int j = 0;

// системни съобщения
// таблица с имена, wchar_t и L(пред думите) 
// се ползва за принтиране на кирилица
const wchar_t *string_table[10] =
{   
  L"Версия 1.0",        //Table 0
  L"Температура:",      //Table 1
  L"Суха почва",        //Table 2
  L"Влажна почва",      //Table 3
  L"Мокра почва",       //Table 4
  L"Поливане...  ",     //Table 5
  L"Влажност:",         //Table 6
  L"Поливна система ",  //Table 7
  L"Сонда 1 Вл.:   %",  //Table 8
  L"Сонда 2 Вл.:   %"   //Table 9 
};

// деклариране на обектите
  // часовник ds1307
  DS1307 RTC;
  // дисплей, който използва руската библиотека
  // и модифицираната от нея LiquidCrystal_I2C базова библиотека
  LCD_1602_RUS      lcd(0x3F, 16, 2);
  // сензор за влага и температура
  DHT dht(DHTPin, DHTTYPE);

// Начална инициализация на програмата
void setup(){
// Старт на серийните данни в конзолата
   Serial.begin(9600);
// Старт на LCD дисплея
   lcd.init();
   lcd.backlight();
// Wire initialization
   Wire.begin();
// Старт на часовника
   RTC.begin();
   //RTC.adjust(DateTime(2018,5,10,5,18,0));
  // RTC.adjust(DateTime(__DATE__, __TIME__));
   if (!RTC.isrunning()){
// Ако часовникът не е бил сверяван, този ред го сверява с
// компютъра при компилиране
   RTC.adjust(DateTime(__DATE__, __TIME__)); 
   }
// Старт на сензора за влага и температура   
   dht.begin();
  
// Старт на Arduino изходите
  pinMode(audioPin,OUTPUT);
  pinMode(soggyLEDPin,OUTPUT);
  pinMode(moistsoilLEDPin,OUTPUT);
  pinMode(drysoilLEDPin,OUTPUT);
  pinMode(pumpLEDPin,OUTPUT);
  pinMode(pumpPin1,OUTPUT);
  pinMode(DHTPin,OUTPUT);
  pinMode(menuButtonPin, INPUT);
  pinMode(adjustButtonPin, INPUT);

  
//  Начално съобщениена екрана
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(string_table[7]);
  lcd.setCursor(0,1);
  lcd.print(string_table[0]); 
  delay(4000); 

//Изключи релето (и съответно клапата) при стартиране на програмата
   digitalWrite(pumpPin1, LOW);
}


// Начало на 
void loop(){
// RTC параметри за часовника
  DateTime myRTC = RTC.now();
  int H = myRTC.hour();
  int M = myRTC.minute();
  int S = myRTC.second();

// прочети данните от сензорите
  // levelSensorValue = analogRead(levelSensorPin);
   moistureSensorValue1 = analogRead(moistureSensorPin1);
   moistureSensorValue2 = analogRead(moistureSensorPin2);
   moistureSensorValue1_percentage = map(moistureSensorValue1, 1023 , 0 , 0 , 100);
   moistureSensorValue2_percentage = map(moistureSensorValue2, 1023 , 0 , 0 , 100);
    
// Тестване в конзолата  
   Serial.println("Moisture Value1");
   Serial.println(moistureSensorValue1);
   Serial.print("Procenti ");
   Serial.print(moistureSensorValue1_percentage);
   Serial.println("%");
   Serial.println( );
   Serial.println("Moisture Value2");
   Serial.println(moistureSensorValue2);
   Serial.print("Procenti ");
   Serial.print(moistureSensorValue2_percentage);
   Serial.println("%");
   Serial.println( );

// Принтирай на екрана
   lcd.clear();
   // час
   RightHour();
   lcd.clear();
   // температура и влажност
   TempAndHumidity();
   lcd.clear();
   // времето на поливане
   printWateringTime();
  
// провери данните на Сонда 1
  if(moistureSensorValue1 >= 700){
    // ако почвата е суха
    // напиши съобщение на дисплея
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(string_table[8]);
    lcd.setCursor(13,0);
    lcd.print(moistureSensorValue1_percentage);
    lcd.setCursor(0,1);
    lcd.print(string_table[2]);
    // светни правилното LED-че
    digitalWrite(drysoilLEDPin,HIGH);
    digitalWrite(moistsoilLEDPin,LOW);
    digitalWrite(soggyLEDPin,LOW);
    // зумерът дава звуков сигнал,
    // ако е суха почвата
    tone(audioPin, NOTE_C6, 100);
    delay(2000);
    noTone(audioPin);
  }
  if((moistureSensorValue1 < 700) && (moistureSensorValue1 >= 300)){
    // при влажна почва
    // изпиши съобщение
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(string_table[8]);
    lcd.setCursor(13,0);
    lcd.print(moistureSensorValue1_percentage);
    lcd.setCursor(0,1);
    lcd.print(string_table[3]);
    // светни правилното LED-че
    digitalWrite(drysoilLEDPin,LOW);
    digitalWrite(moistsoilLEDPin,HIGH);
    digitalWrite(soggyLEDPin,LOW);
    delay(2000);
  }
  if(moistureSensorValue1 < 300){
    // при мокра почва
    // изпиши съобщение
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(string_table[8]);
    lcd.setCursor(13,0);
    lcd.print(moistureSensorValue1_percentage);
    lcd.setCursor(0,1);
    lcd.print(string_table[4]);
    // светни правилното LED-че
    digitalWrite(drysoilLEDPin,LOW);
    digitalWrite(moistsoilLEDPin,LOW);
    digitalWrite(soggyLEDPin,HIGH);
    delay(2000);
}
  
// провери Сонда 2
// към този момент това е само за информация
// Сонда 2 не контролира релето и поливането
  if(moistureSensorValue2 >= 700){
    // при суха почва
    // изпиши съобщение
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(string_table[9]);
    lcd.setCursor(13,0);
    lcd.print(moistureSensorValue2_percentage);
    lcd.setCursor(0,1);
    lcd.print(string_table[2]);
    // светни правилното LED-че
    digitalWrite(drysoilLEDPin,HIGH);
    digitalWrite(moistsoilLEDPin,LOW);
    digitalWrite(soggyLEDPin,LOW);
    // зумерът дава звуков сигнал,
    // ако е суха почвата
    tone(audioPin, NOTE_C6, 100);
    delay(2000);
    noTone(audioPin);
  }
  if((moistureSensorValue2 < 700) && (moistureSensorValue2 >= 300)){
    // при влажна почва
    // изпиши съобщение
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(string_table[9]);
    lcd.setCursor(13,0);
    lcd.print(moistureSensorValue2_percentage);
    lcd.setCursor(0,1);
    lcd.print(string_table[3]);
    // светни правилното LED-че
    digitalWrite(drysoilLEDPin,LOW);
    digitalWrite(moistsoilLEDPin,HIGH);
    digitalWrite(soggyLEDPin,LOW);
    delay(2000);
  }
  if(moistureSensorValue2 < 300){
    // при мокра почва
    // изпиши съобщение
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(string_table[9]);
    lcd.setCursor(13,0);
    lcd.print(moistureSensorValue2_percentage);
    lcd.setCursor(0,1);
    lcd.print(string_table[4]); 
    // светни правилното LED-че
    digitalWrite(drysoilLEDPin,LOW);
    digitalWrite(moistsoilLEDPin,LOW);
    digitalWrite(soggyLEDPin,HIGH);
    delay(2000);
}
   // Настройка за времето на поливане
   menuButtonState = digitalRead(menuButtonPin);
   if (menuButtonState == HIGH){
      Menu();   
    }
   
  

//  Ако почвата е суха(стойност над 700 на сондата) в определения час, включи релето 
//  Релето стои отворено докато стойността на Сонда 1 не падне под 700 или времето
//  за поливане е надвишено.
if((H == chas) && (M == minuti) && (S >= 00 ) && (S <= 59 )){
    // Брояч за продължителността на поливане
    int pumpCounter = 0;
    while(moistureSensorValue1 >= 700 && pumpCounter <= (6*vreme_za_polivane)){
      // Системно съобщение
      lcd.clear();
      RightHour();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(L"Поливане...");
      lcd.setCursor(0,1);
      lcd.print(L"Мин.:    Вл.:  %");
      lcd.setCursor(6,1);
      lcd.print(pumpCounter/6);
      lcd.setCursor(13,1);
      lcd.print(moistureSensorValue1_percentage);
      // Включи релето
      digitalWrite(pumpPin1,HIGH);
      digitalWrite(pumpLEDPin,HIGH);
      // Изчакай 10 секунди
      delay(10000);
      // Провери стойността на Сонда 1
      moistureSensorValue1 = analogRead(moistureSensorPin1);
      moistureSensorValue1_percentage = map(moistureSensorValue1, 1023 , 0 , 0 , 100);
      // Добави 1 към брояча
      pumpCounter++;
      
    }
    // Изключи релето и LED-чето за него
    digitalWrite(pumpPin1,LOW);
    digitalWrite(pumpLEDPin,LOW);
  }

}   // КРАЙ НА VOID LOOP-A

// Изпиши часа и датата на екрана
void RightHour()
{
  DateTime Now = RTC.now();
  String clock_date = "";
  String clock_hour = "";
  
  int _day = Now.day();
  int _month = Now.month();
  int _year = Now.year();
  
  clock_date += fixZero(_day);
  clock_date += "/";
  clock_date += fixZero(_month);
  clock_date += "/";
  clock_date += _year;
  
  int _hour = Now.hour();
  int _minute = Now.minute();
  int _second = Now.second();
  
  clock_hour += fixZero(_hour);
  clock_hour += ":";
  clock_hour += fixZero(_minute);
  clock_hour += ":";
  clock_hour += fixZero(_second);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(L"Дата:");
  lcd.setCursor(6, 0);
  lcd.print(clock_date);  
  lcd.setCursor(0, 1);
  lcd.print(L"Час:");
  lcd.setCursor(6, 1);
  lcd.print(clock_hour);
  delay(2000);
}

// Изпиши температурата и влажността на екрана
void TempAndHumidity()
{
   float humidity = dht.readHumidity();
   float temperature = dht.readTemperature();
   lcd.setCursor(0,0);
   lcd.print(string_table[1]);
   lcd.setCursor(12,0);
   lcd.print(temperature);
   lcd.setCursor(14,0);
   lcd.print(" C");
   lcd.setCursor(0,1);
   lcd.print(string_table[6]);
   lcd.setCursor(12,1);
   lcd.print(humidity);
   lcd.setCursor(14,1);
   lcd.print(" %"); 
   delay(2000);
}

// Влез в менюто за настройка на час за поливане,
// продължителността на поливане и сверяване на
// час и дата.
void Menu()
{
   int lastCounter = 0;
   
   DateTime Vreme = RTC.now();
   int nov_chas = Vreme.hour();
   int novi_minuti = Vreme.minute();
   int nov_den = Vreme.day();
   int nov_mesec = Vreme.month();
   int nova_godina = Vreme.year();
   lcd.clear();
   // Стой в менюто докато копчето за меню
   // не е натиснато 9 пъти. 
   while (menuButtonPushCounter != menu_pages){
   menuButtonState = digitalRead(menuButtonPin);
   adjustButtonState = digitalRead(adjustButtonPin);
   // Брой колко пъти копчето за меню е натиснато.
   // Ново натискане се брои само след като копчето е
   // било пуснато.
   if (menuButtonState != menuLastButtonState) {
    if (menuButtonState == HIGH){
      if (menuButtonPushCounter >= menu_pages){
        menuButtonPushCounter = 1;
      } else{
        menuButtonPushCounter++;
      }

    } else {
      Serial.println("off");
    }
      delay(50);
   }
  // Актуализирай състоянието на копчето за меню. 
  menuLastButtonState = menuButtonState;

  // Всяко натискане на копчето за меню променя
  // страницата в менюто. При 9-тото натискане 
  // програмата излиза от функцията.

  // Страница 1 - променя часа в 'Час на поливане'
  if (menuButtonPushCounter  == 1) {
    
        if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      lcd.setCursor(0,0);
      lcd.print(L"Час на поливане:");
      lcd.setCursor(0,1);
      lcd.print(L"час");
      lcd.setCursor(10,1);
         if (adjustButtonState == HIGH){
           if(chas==23) {
              chas=0;
           } else {
              chas++;
             }
          }
      lcd.setCursor(7,1);
      lcd.print(fixZero(chas));
      lcd.setCursor(9,1);
      lcd.print(":");
      lcd.setCursor(10,1);
      //lcd.blink();
      lcd.print(fixZero(minuti));
      delay(500);    
      }

   // Страница 2 - променя минутите в 'Час на поливане'
   if (menuButtonPushCounter  == 2) {
        
      if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      lcd.setCursor(0,0);
      lcd.print(L"Час на поливане:");
      lcd.setCursor(0,1);
      lcd.print(L"мин");
         if (adjustButtonState == HIGH){
            if(minuti==59)
           {
              minuti=0;
            } else {
              minuti++;
            }
           }      
      lcd.setCursor(7,1);
      lcd.print(fixZero(chas));
      lcd.setCursor(9,1);
      lcd.print(":");
      lcd.setCursor(10,1);
      lcd.print(fixZero(minuti));  
       
      delay(500);
    }

  // Страница 3 - променя минутите, които 
  // определят продължителността на поливането. 
    if (menuButtonPushCounter  == 3) {
        // Тази част избягва мигането на екрана в страница 3 и
        // чисти екрана само, ако страницата в менюто
        // е сменена. 
       if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      
      lcd.setCursor(0,0);
      lcd.print(L"Поливане:");
      lcd.setCursor(3,1);
      lcd.print(L"минути");
         if (adjustButtonState == HIGH){
            if(vreme_za_polivane==59)
           {
              vreme_za_polivane=0;
            } else {
              vreme_za_polivane++;
            }
           }      
      lcd.setCursor(0,1);
      lcd.print(fixZero(vreme_za_polivane));        
      delay(500);
    }   

  // Страница 4 - сверяване на час     
  if (menuButtonPushCounter  == 4) {
        // Тази част избягва мигането на екрана в страница 3 и
        // чисти екрана само, ако страницата в менюто
        // е сменена. 
      if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      
      lcd.setCursor(0,0);
      lcd.print(L"Сверяване час:");
      lcd.setCursor(0,1);
      lcd.print(L"час");
         if (adjustButtonState == HIGH){
            if(nov_chas==23)
           {
              nov_chas=0;
            } else {
              nov_chas++;
            }
           }      
      lcd.setCursor(7,1);
      lcd.print(fixZero(nov_chas));
      lcd.setCursor(9,1);
      lcd.print(":");
      lcd.setCursor(10,1);
      lcd.print(fixZero(novi_minuti));        
      delay(500);
    }
  
  // Страница 5 - сверяване на час     
  if (menuButtonPushCounter  == 5) {
      if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      
      lcd.setCursor(0,0);
      lcd.print(L"Сверяване час:");
      lcd.setCursor(0,1);
      lcd.print(L"мин");
         if (adjustButtonState == HIGH){
            if(novi_minuti==59)
           {
              novi_minuti=0;
            } else {
              novi_minuti++;
            }
           }      
      lcd.setCursor(7,1);
      lcd.print(fixZero(nov_chas));
      lcd.setCursor(9,1);
      lcd.print(":");
      lcd.setCursor(10,1);
      lcd.print(fixZero(novi_minuti));        
      delay(500);
    }

    // Страница 6 - сверяване на ден     
  if (menuButtonPushCounter  == 6) {
      if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      
      lcd.setCursor(0,0);
      lcd.print(L"Сверяване дата:");
      lcd.setCursor(0,1);
      lcd.print(L"ден");
         if (adjustButtonState == HIGH){
            if(nov_den==31)
           {
              nov_den=1;
            } else {
              nov_den++;
            }
           }      
      lcd.setCursor(6,1);
      lcd.print(fixZero(nov_den));
      lcd.setCursor(8,1);
      lcd.print("/");
      lcd.setCursor(9,1);
      lcd.print(fixZero(nov_mesec)); 
      lcd.setCursor(11,1);
      lcd.print("/"); 
      lcd.setCursor(12,1);  
      lcd.print(nova_godina);     
      delay(500);
    }

    // Страница 7 - сверяване на месец     
  if (menuButtonPushCounter  == 7) {
      if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      
      lcd.setCursor(0,0);
      lcd.print(L"Сверяване дата:");
      lcd.setCursor(0,1);
      lcd.print(L"мес");
         if (adjustButtonState == HIGH){
            if(nov_mesec==12)
           {
              nov_mesec=1;
            } else {
              nov_mesec++;
            }
           }      
      lcd.setCursor(6,1);
      lcd.print(fixZero(nov_den));
      lcd.setCursor(8,1);
      lcd.print("/");
      lcd.setCursor(9,1);
      lcd.print(fixZero(nov_mesec)); 
      lcd.setCursor(11,1);
      lcd.print("/"); 
      lcd.setCursor(12,1);  
      lcd.print(nova_godina);     
      delay(500);
    }

        // Страница 8 - сверяване на година     
  if (menuButtonPushCounter  == 8) {
      if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      
      lcd.setCursor(0,0);
      lcd.print(L"Сверяване дата:");
      lcd.setCursor(0,1);
      lcd.print(L"год");
         if (adjustButtonState == HIGH){
            if(nova_godina==2030)
           {
              nova_godina=2017;
            } else {
              nova_godina++;
            }
           }      
      lcd.setCursor(6,1);
      lcd.print(fixZero(nov_den));
      lcd.setCursor(8,1);
      lcd.print("/");
      lcd.setCursor(9,1);
      lcd.print(fixZero(nov_mesec)); 
      lcd.setCursor(11,1);
      lcd.print("/"); 
      lcd.setCursor(12,1);  
      lcd.print(nova_godina);     
      delay(500);
    }

  // Страница 9 - запазване на сверяването на часа и датата
  if (menuButtonPushCounter  == 9) {
      if (menuButtonPushCounter !=lastCounter ){
      lcd.clear();
      tone(audioPin, NOTE_C3, 100);}
      
      lcd.setCursor(0,0);
      lcd.print(L"Запази");
      lcd.setCursor(0,1);
      lcd.print(L"сверяването?");
         if (adjustButtonState == HIGH){
           RTC.adjust(DateTime(nova_godina,nov_mesec,nov_den,nov_chas,novi_minuti,0));
           lcd.clear();
           lcd.setCursor(2,0);
           lcd.print(L"Час и дата");
           lcd.setCursor(3,1);
           lcd.print(L"сверени");
           tone(audioPin, NOTE_G3, 150);
           delay(300);
           tone(audioPin, NOTE_G3, 150);
           // Това излиза от while loop-a
           menuButtonPushCounter = menu_pages;
           delay(1500);
           }      
      delay(500);
    }
    
    lastCounter = menuButtonPushCounter;
  }   
  menuButtonPushCounter = 0;
  menuLastButtonState = 0;
  tone(audioPin, NOTE_G3, 150);
  delay(300);
  tone(audioPin, NOTE_G3, 150);
}

// Изпиши часа на поливане на екрана
void printWateringTime()
{
   lcd.setCursor(0,0);
   lcd.print(L"Час на поливане:");
   lcd.setCursor(0,1);
   lcd.setCursor(5,1);
   lcd.print(fixZero(chas));
   lcd.setCursor(7,1);
   lcd.print(":");
   lcd.setCursor(8,1);
   lcd.print(fixZero(minuti)); 
   delay(2000);
}

// Функция, която принтира числата с 2 цифри.
// Примерно за час (02:08:05)
String fixZero(int i)
{
  String ret;
  if (i < 10) ret += "0";
  ret += i;
  return ret;
}

