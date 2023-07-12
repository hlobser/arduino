// Пины подключения компонентов
#define pumpPin  2        // Пин, к которому подключено реле насоса
#define soilSensorPin  A0 // Аналоговый пин, к которому подключен датчик почвы

// Пороговые значения для полива и остановки полива
#define moistureThreshold  840      // Пороговое значение для включения полива
//#define dryThreshold  430           // Пороговое значение для остановки полива
                                           // Датчик инверсный: больше влажность - меньше значение.
// #define  MIN        595                    // Определяем минимальное показание датчика (в воздухе),
// #define  MAX        417                    // определяем максимальное показание датчика (в воде),
uint16_t soilMoisture;

void setup() {
  // Инициализация пина насоса как выходного
  pinMode(pumpPin, OUTPUT);
  
  // Инициализация последовательной связи с монитором порта
  Serial.begin(9600);
  digitalWrite(pumpPin, HIGH);
  delay(40000);
}
// Функция для включения полива
  
void startWatering() {
  digitalWrite(pumpPin, LOW);  // Включаем реле насоса
  Serial.println("Полив начат");
  delay(5000);
  digitalWrite(pumpPin, HIGH);  // Выключаем реле насоса
  Serial.println("Полив остановлен");
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