#include <SoftwareSerial.h>
#include <Time.h>

// Ограничение значений мотора и джойстика
#define MOTOR_MAX 255                
#define JOY_MAX 40                  

// Модуль bluetooth
#define BT_RX_PIN 10
#define BT_TX_PIN 11

// Выходы к драйверу моторов, по два выхода на каждую сторону - вперед и назад
#define L_F 3
#define L_B 5
#define R_F 6
#define R_B 9

SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

boolean doneParsing, startParsing;
short dataX, dataY;
String string_convert;

void setup() {
  pinMode(L_F, OUTPUT);
  pinMode(L_B, OUTPUT);
  pinMode(R_F, OUTPUT);
  pinMode(R_B, OUTPUT);
  
  BTSerial.begin(9600);
  Serial.begin(9600);
  delay(500);
}

bool parse_input(){
  char incomingChar = BTSerial.read();
    if (startParsing) {      
      if (incomingChar == ' ') {  
        dataX = string_convert.toInt();
        string_convert = "";
      }
      else if (incomingChar == ';') {
        dataY = string_convert.toInt();
        string_convert = "";
        startParsing = false;
        doneParsing = true;
      } else {
        string_convert += incomingChar;
      }
    }
    if (incomingChar == '$') {          
      startParsing = true;
    }
}
 
void loop() {
  // Если модуль принял значения, получаем из них положения джойстика по X и Y
  if (BTSerial.available()) {
    parse_input();
  }
  if (doneParsing) {
    doneParsing = false;
    // Приводим значения положения джойстика к виду от -MOTOR_MAX до MOTOR_MAX
    int joystickX = map((dataX), -JOY_MAX, JOY_MAX, -MOTOR_MAX / 2, MOTOR_MAX / 2); 
    int joystickY = map((dataY), -JOY_MAX, JOY_MAX, -MOTOR_MAX, MOTOR_MAX);

    int dutyR = joystickY + joystickX;
    int dutyL = joystickY - joystickX;
    dutyR = -(constrain(dutyR, -MOTOR_MAX, MOTOR_MAX));
    dutyL = -(constrain(dutyL, -MOTOR_MAX, MOTOR_MAX)); 

    // Значение положительное - едем вперед, отрицательное - назад
    if (dutyR > 0) {
      analogWrite(L_F, abs(dutyR));
      analogWrite(L_B, 0);
    } else {
      analogWrite(L_B, abs(dutyR));
      analogWrite(L_F, 0);
    }
    if (dutyL > 0){
      analogWrite(R_F, abs(dutyL));
      analogWrite(R_B, 0);
    } else {
      analogWrite(R_B, abs(dutyL));
      analogWrite(R_F, 0);
    }
  }
}
