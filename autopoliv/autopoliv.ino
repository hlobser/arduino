#include "iarduino_RTC.h"
#include "Wire.h"                     // библиотека для устройств I2C 
#include "LiquidCrystal_I2C.h"        // подключаем библиотеку для дисплея
#include "SPI.h"
#include "SD.h"

LiquidCrystal_I2C LCD(0x27, 16, 2);      // присваиваем имя дисплею
iarduino_RTC time(RTC_DS1302, 6, 7, 8);  // для модуля DS1302 - RST, CLK, DAT
                                         //у экрана SCL к A5, SDA к A4

#define pumpPin  2        // Пин, к которому подключено реле насоса
#define soilSensorPin  A0 // Аналоговый пин, к которому подключен датчик почвы
#define piezoPin 3        // пин пьезодинамика


// Пороговые значения для полива и остановки полива
#define moistureThreshold  840      // Пороговое значение для включения полива
//#define dryThreshold  430           // Пороговое значение для остановки полива
                                           // Датчик инверсный: больше влажность - меньше значение.
// #define  MIN        595                    // Определяем минимальное показание датчика (в воздухе),
// #define  MAX        417                    // определяем максимальное показание датчика (в воде),

// подключение sd карты:
/* 
MOSI - пин 11
MISO - пин 12
CLK - пин 13
CS - пин 4
*/
int soilMoisture;
const int sdCardPin = 4;
String datalog_filename = "datalog.csv";

bool warning = false;     // флаг ошибки

void soundUp();
void soundDown();
void startWatering();
void stopWatering();
void printToScreen();
void writeToSD(bool watering);

byte l_warning[] = {0x01, 0x02, 0x02, 0x04, 0x08, 0x08, 0x10, 0x1F}; //левая и правая граница треугольника символа ⚠
byte r_warning[] = {0x10, 0x08, 0x08, 0x04, 0x02, 0x02, 0x01, 0x1F};

void setup() {
  delay(300);
  soundUp();
  pinMode(pumpPin, OUTPUT);       // насос
  pinMode(piezoPin, OUTPUT);      // пищалка
  pinMode(sdCardPin, OUTPUT);     // сд карта
  digitalWrite(pumpPin, HIGH);

  LCD.init();            // инициализация LCD дисплея
// LCD.backlight();      // включение подсветки дисплея

  LCD.createChar(0, l_warning);
  LCD.createChar(1, r_warning);

  if (!SD.begin(sdCardPin)) { 
    warning = true;
    LCD.clear();                      // Пытаемся проинициализировать sd карту
    LCD.print("NO SD card");          // Если что-то пошло не так, выдаем на экран
    // return;
  }
  else {
  LCD.setCursor(0, 0);
  LCD.print("Humidity:");
  LCD.setCursor(0, 1);
  LCD.print("data and time");
  }
  // Serial.println("card initialized.");

  time.begin();
  //time.settime(0, 13, 10, 18, 7, 23, 3);  // сек, мин, часы, число, месяц, год, день недели(0 - суббота)
  // Serial.begin(9600);
  if (!SD.exists(datalog_filename)) {
    File dataFile = SD.open(datalog_filename, FILE_WRITE);
      if (dataFile) {
      dataFile.println("date,time,humidity,watering");
      dataFile.close();
    }
  }
  delay(40000);
  // delay(4000);
  //soundDown();
}

  

void loop() {
  if (String(time.gettime("d-m-Y")) == "00-00-2000" or String(time.gettime("H:i:s")) == "45:85:85" ){ // проверка не отвалилоь ли питание у часов
    warning = true;
    File dataFile = SD.open(datalog_filename, FILE_WRITE);
    if (dataFile) {
      dataFile.println("RTC module error");
      dataFile.close();
    }
  }
  soilMoisture = analogRead(soilSensorPin);         // Считываем значение с датчика почвы

  // if (!(soilMoisture <= 1000 or soilMoisture >= 200)){
  //   warning = true;
  //   File dataFile = SD.open(datalog_filename, FILE_WRITE);
  //   if (dataFile) {
  //     dataFile.println("soil moisture sensor error");
  //     dataFile.close();  
  //   }   
  // }
  if (warning) {
    LCD.setCursor(13, 0);
    LCD.write(0);
    LCD.print("!");
    LCD.write(1);
  }
  

  // Serial.print("Влажность почвы: ");             // Выводим значение в монитор порта
  // Serial.println(soilMoisture);
  // Serial.println(time.gettime("d-m-Y H:i:s"));

  writeToSD(0);
  printToScreen();

  if (soilMoisture > moistureThreshold) {
    startWatering();
    printToScreen();
    writeToSD(1);
    delay(5000);
    stopWatering();

  }
  // Задержка между измерениями
  delay(1800000);
  // delay(100000);
}


void soundUp(){
  tone(piezoPin, 1000, 100);
  delay(200);
  tone(piezoPin, 2000, 100);
  delay(200);  
  tone(piezoPin, 3000, 100);
  delay(200);  
}
void soundDown(){
  tone(piezoPin, 3000, 100);
  delay(200);
  tone(piezoPin, 2000, 100);
  delay(200);  
  tone(piezoPin, 1000, 100);
  delay(200);
}
void startWatering() {          // Функция для включения полива
  digitalWrite(pumpPin, LOW);   // Включаем реле насоса
  // Serial.println("Полив начат");
  soundUp();
}
void stopWatering() {           // Функция для остановки полива
  digitalWrite(pumpPin, HIGH);  // Выключаем реле насоса
  // Serial.println("Полив остановлен");
  soundDown();
}
void printToScreen(){
  LCD.setCursor(0, 0);
  LCD.print("Humidity:" + String(soilMoisture));
  LCD.setCursor(0, 1);
  LCD.print(time.gettime("d-m, H:i:s"));
}
void writeToSD(bool watering){
  String logData = String(time.gettime("d-m-Y,H:i:s,")) + soilMoisture + "," + watering;
  File dataFile = SD.open(datalog_filename, FILE_WRITE);
    if (dataFile) {
    dataFile.println(logData);
    dataFile.close();
    // Публикуем в мониторе порта для отладки
    // Serial.println(logData);
  }
  else {
    warning = true;
    // Serial.println("error opening " + datalog_filename);
  }
}