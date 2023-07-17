#include "iarduino_RTC.h"
iarduino_RTC time(RTC_DS1302,6,8,7);  // для модуля DS1302 - RST, CLK, DAT
                                      //у экрана scl к A5, sda к A4

#include "Wire.h"                     // библиотека для устройств I2C 
#include "LiquidCrystal_I2C.h"        // подключаем библиотеку для дисплея
LiquidCrystal_I2C LCD(0x27,16,2);    // присваиваем имя дисплею


#define pumpPin  2        // Пин, к которому подключено реле насоса
#define soilSensorPin  A0 // Аналоговый пин, к которому подключен датчик почвы
#define piezoPin 3   // пин пьезодинамика


// Пороговые значения для полива и остановки полива
#define moistureThreshold  840      // Пороговое значение для включения полива
//#define dryThreshold  430           // Пороговое значение для остановки полива
                                           // Датчик инверсный: больше влажность - меньше значение.
// #define  MIN        595                    // Определяем минимальное показание датчика (в воздухе),
// #define  MAX        417                    // определяем максимальное показание датчика (в воде),
unsigned int soilMoisture;

void setup() {
  // Инициализация пина насоса как выходного
  pinMode(pumpPin, OUTPUT);
  pinMode(piezoPin, OUTPUT);

  LCD.init();            // инициализация LCD дисплея
  LCD.backlight();      // включение подсветки дисплея
  time.begin();
  
  // Инициализация последовательной связи с монитором порта
  Serial.begin(9600);
  digitalWrite(pumpPin, HIGH);
  delay(40000);
}
// Функция для включения полива
  
void startWatering() {
  digitalWrite(pumpPin, LOW);  // Включаем реле насоса
  Serial.println("Полив начат");
  for (int x = 1000; x <= 2000 ; x += 500){ // пищим что полив начался
    tone (piezoPin, x, 50);
     
  }
  // tone(piezoPin, 1000, 50);
  // tone(piezoPin, 1500, 50);
  // tone(piezoPin, 2000, 50);
  delay(5000);
  digitalWrite(pumpPin, HIGH);  // Выключаем реле насоса
  Serial.println("Полив остановлен");
  for (int x = 2000; x >= 1000 ; x +=500){ // пищим что полив закончился
    tone (piezoPin, x, 50);
  }
  // tone(piezoPin, 2000, 50);
  // tone(piezoPin, 1500, 50);
  // tone(piezoPin, 1000, 50);

}
  
// Функция для остановки полива
// void stopWatering() {
//   digitalWrite(pumpPin, HIGH);  // Выключаем реле насоса
//   Serial.println("Полив остановлен");
// }

void loop() {
  // Считываем значение с датчика почвы
  soilMoisture = analogRead(soilSensorPin);
  //soilMoisture = map(soilMoisture, MIN, MAX, 0, 100);

  if (millis() % 1000 == 0) {
    LCD.setCursor(0,0);                     // 0 столбец, 0 строка
    LCD.print(time.gettime("d M Y, D"));
    LCD.setCursor(0,1);                     // 0 столбец, 1 строка
    LCD.print(time.gettime("H:i:s"));
  }
  // Выводим значение в монитор порта
  Serial.print("Влажность почвы: ");
  Serial.println(soilMoisture);

  // Проверяем условия полива
  // if (soilMoisture < dryThreshold) {
  //   stopWatering();
  // }  
  if (soilMoisture > moistureThreshold) {
    startWatering();
  }

  // Задержка между измерениями
  delay(1800000);
}

// Функция для получения значения влажности почвы
//long getSoilMoisture() {
 // long moistureValue = soilSensor.capacitiveSensor(30); // Используем метод capacitiveSensor() для получения значения
 // return moistureValue;