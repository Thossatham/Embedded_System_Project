//Temperature
//Microphone
//Hit Sensor
//Joystick

// comms message
// tempC,tempF,microphone,hit,joystick

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 32 //same as temp

//communication
#define RX_PIN 16
#define TX_PIN 17
#define SERIAL_BAUD 115200

#include <ESP32Servo.h>

int pinTemp = ONE_WIRE_BUS; // same as one_wire_bus
int pinMic = 35;
int pinHit = 22;
int pinJoy = 34;
int pinServo = 12;

//microphone ---> idk 
//hit sensor ---> HIGH when สั่น
//temperature
//joystick >= 3500 right ;;;;; <= 2500 left

float tempC = 0;
float tempF = 0;
int micValue = 0;
int hitValue = 0;
int joyValue = 0;

//actuator Servo
int servoValue = 0; // min = 5 ; max = 175 ;
int servoValue_buffer = 0;

//comms from gateway
char msg[] = "123";

//debounce
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 1500;    // the debounce time; increase if the output flickers

bool lastJoyState = false;

Servo myservo;


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void printValues(int SerialNo, float tempC, float tempF, int mV, int hV, int jV){
  if(SerialNo == 2){
    Serial2.print(tempC);Serial2.print(",");Serial2.print(tempF);Serial2.print(",");Serial2.print(mV);Serial2.print(",");Serial2.print(hV);Serial2.print(",");Serial2.print(jV);Serial2.println();
  }else{
    Serial.print(tempC);Serial.print(",");Serial.print(tempF);Serial.print(",");Serial.print(mV);Serial.print(",");Serial.print(hV);Serial.print(",");Serial.print(jV);Serial.println();
  }
}


void joyStickServo(int jV){
  //joystick >= 3000 right ;;;;; <= 2000 left
  //max 4000
  int power = 0;
  int target = servoValue;
  if(jV >= 3000){
    //4000 - jV >= 500
    // jV = 4095 - jV;
    // power = map(jV, 0, 1100, 0, 10);
    target = servoMove('R');
  }
  else if(jV <= 2500){
    // power = map(jV, 0, 2000, 0, 10);
    target = servoMove('L');
  }

  servoValue = target;
  
  myservo.write(servoValue);
}

int servoMove(char dir ){
  int newValue = servoValue;
  int power = 10;
  if( dir == 'R' ){
    newValue = newValue + power;
  }else{
    newValue = newValue - power;
  }
  if( newValue >= 90 ){
    newValue = 90;
  }
  if( newValue <= 0 ){
    newValue = 0;
  }

  return newValue;
}

int servoHandle(int v){
  int upper = 90;
  int lower = 0;
  if( v > upper){
    v = upper;
  }
  if( v < lower){
    v = lower;
  }

  return v;
}

void setup() {

  //Serial
  Serial.begin(115200);               // USB Serial ไปคอมพิวเตอร์
  Serial2.begin(SERIAL_BAUD, SERIAL_8N1, RX_PIN, TX_PIN); // UART2 ไปอีกบอร์ด

  myservo.setPeriodHertz(50);
  myservo.attach(pinServo, 500, 2400);

  sensors.begin();
  
}


void loop() {

  // int bytesRead = Serial2.readBytesUntil('\n', msg, sizeof(msg) - 1);
  // msg[bytesRead] = '\0';

  String s_msg = Serial2.readStringUntil('\n');

  //Serial.print("msg=");
  //Serial.println(msg);

  joyValue = analogRead(pinJoy);
  Serial.println(joyValue);
  if(joyValue >= 3000 || joyValue <= 2000){
    joyStickServo(joyValue);
    lastDebounceTime = millis();
  }else if ((millis() - lastDebounceTime) > debounceDelay) {
  //convert msg to servo
      if(s_msg!=""){
        servoValue_buffer = s_msg.toInt();
        servoValue = servoHandle(servoValue_buffer);
        myservo.write(servoValue); 
      }
  }


  Serial.println(servoValue);

  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  tempF = sensors.getTempFByIndex(0);
  micValue = analogRead(pinMic);

  // micMax = 0;
  // for(int x = 0 ; x < 20 ; x++){
  //   int mValue = analogRead(pinMic);
  //   if(mValue > micMax){
  //     micMax = mValue;
  //   }
  // }
  // micValue = micMax;

  //micValue = 4095 - micValue; //since its inversed

  // if( digitalRead(pinMic)){
  //   micValue = 1;
  // }else{
  //   micValue = 0;
  // }

  if(micValue > 0){
    micValue=1;
  }

  

  if( digitalRead(pinHit) == HIGH){
    hitValue = 1;
  }else{hitValue = 0;}

  // hitValue = analogRead(pinHit);

  // comms message
  // temp,microphone,hit,joystick 

  //for debugging
  //printValues(1, tempC, tempF, micValue, hitValue, joyValue);

  printValues(2, tempC, tempF, micValue, hitValue, servoValue);


}