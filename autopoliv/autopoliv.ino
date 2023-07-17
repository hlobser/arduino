#include "iarduino_RTC.h"
#include "Wire.h"                     // библиотека для устройств I2C 
#include "LiquidCrystal_I2C.h"        // подключаем библиотеку для дисплея
#include "SPI.h"
#include "SD.h"

LiquidCrystal_I2C LCD(0x27, 16, 2);    // присваиваем имя дисплею
iarduino_RTC time(RTC_DS1302, 6, 7, 8);  // для модуля DS1302 - RST, CLK, DAT
                                      //у экрана scl к A5, sda к A4

#define pumpPin  2        // Пин, к которому подключено реле насоса
#define soilSensorPin  A0 // Аналоговый пин, к которому подключен датчик почвы
#define piezoPin 3        // пин пьезодинамика


// Пороговые значения для полива и остановки полива
#define moistureThreshold  840      // Пороговое значение для включения полива
//#define dryThreshold  430           // Пороговое значение для остановки полива
                                           // Датчик инверсный: больше влажность - меньше значение.
// #define  MIN        595                    // Определяем минимальное показание датчика (в воздухе),
// #define  MAX        417                    // определяем максимальное показание датчика (в воде),
unsigned int soilMoisture;

void setup() {
  delay(300);
  // Инициализация пина насоса как выходного
  pinMode(pumpPin, OUTPUT);
  pinMode(piezoPin, OUTPUT);
  tone(piezoPin, 1000, 100);
  delay(200);
  tone(piezoPin, 2000, 100);
  delay(200);
  tone(piezoPin, 3000, 100);

  LCD.init();            // инициализация LCD дисплея
  // LCD.backlight();      // включение подсветки дисплея
  LCD.setCursor(0, 0);
  LCD.print("Humidity:");
  LCD.setCursor(0, 1);
  LCD.print("data and time");

  time.begin();
  //time.settime(0, 13, 14, 17, 7, 23, 2);  // 0  сек, 13 мин, 14 часов, 17, июля, 2023, понедельник
  Serial.begin(9600);
  digitalWrite(pumpPin, HIGH);
  delay(40000);
  tone(piezoPin, 3000, 100);
  delay(200);
  tone(piezoPin, 2000, 100);
  delay(200);
  tone(piezoPin, 1000, 100);
  
}
// Функция для включения полива
  
void startWatering() {
  digitalWrite(pumpPin, LOW);  // Включаем реле насоса
  Serial.println("Полив начат");
  tone(piezoPin, 1000, 100);
  delay(200);
  tone(piezoPin, 2000, 100);
  delay(200);
  tone(piezoPin, 3000, 100);
  
}
// Функция для остановки полива
void stopWatering() {
  digitalWrite(pumpPin, HIGH);  // Выключаем реле насоса
  Serial.println("Полив остановлен");
  tone(piezoPin, 3000, 100);
  delay(200);
  tone(piezoPin, 2000, 100);
  delay(200);
  tone(piezoPin, 1000, 100);
}
void printToScreen(){
  LCD.setCursor(9, 0);
  LCD.print(soilMoisture);
  LCD.setCursor(0, 1);
  LCD.print(time.gettime("d-m, H:i:s"));
}
void writeToSD(){

}
void loop() {
  // Считываем значение с датчика почвы
  soilMoisture = analogRead(soilSensorPin);

  // Выводим значение в монитор порта
  Serial.print("Влажность почвы: ");
  Serial.println(soilMoisture);
  Serial.println(time.gettime("d-m, H:i:s"));

  // Проверяем условия полива
  // if (soilMoisture < dryThreshold) {
  //   stopWatering();
  // }  
  if (soilMoisture > moistureThreshold) {
    startWatering();
    printToScreen();
    delay(5000);
    stopWatering();

  }

  // Задержка между измерениями
  delay(1800000);
  // delay(10000);
}

