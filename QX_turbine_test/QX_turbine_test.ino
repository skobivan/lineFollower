#include <Servo.h>

Servo turbine; 
// Мини драйвер:
//  от 40 до 80
//  на 40 не запускается
//  на 50 - маленькая скорость
//  на 80 - максимум, дальше не ускоряется

void setup() {
  turbine.attach(9);
  Serial.begin(9600);
//  turbine.write(0);
//  for (int i = 0; i < 400; i+=10) {
//    turbine.write(i);
//    delay(1000); 
//  }
//  turbine.write(0);
}
int var = 0;
void loop() {
  if (Serial.available()) {
    var = 0;
    while (Serial.available()) {
      var = var*10 + Serial.read() - '0';
      delay(2);
    }
  }
  turbine.write(var);
}
