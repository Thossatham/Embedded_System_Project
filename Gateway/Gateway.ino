
//BLYNK BLYNK BLYNK
#define BLYNK_TEMPLATE_ID "TMPL27y8JPLp4"
#define BLYNK_TEMPLATE_NAME "EmbeddedProject"
#define BLYNK_AUTH_TOKEN "BekhLMpMGb06DYtmjEgWGwHA_hV-MF0_"
char auth[] = BLYNK_AUTH_TOKEN;

#include <WiFi.h>
#include <Wire.h>
#include <BlynkSimpleEsp32.h>
#include <Preferences.h> 

float temp = 0;

// กำหนด Hardware Serial2 สำหรับสื่อสารระหว่างบอร์ด
#define RX_PIN 16
#define TX_PIN 17
#define SERIAL_BAUD 115200

char msg[] = "111.11,111.11,1111,11111,1111";
char *strings[5];
char *ptr = NULL;

float tempC = 0;
float tempF = 0;
int micValue = 0;
int hitValue = 0;
int joyValue = 0;
int servoValue = 0;
int lastServoValue = -1;

String url = "";

Preferences pref;

// 0 = tempC
// 1 = tempF
// 2 = Mic
// 3 = Hit Sensor
// 4 = Joystick (useless)
void decodeMsg(char *string){
  byte index = 0;
  ptr = strtok(string, ",");  // delimiter
   while (ptr != NULL)
   {
      strings[index] = ptr;
      index++;
      ptr = strtok(NULL, ",");
   }
   //Serial.println(index);
   // print all the parts

   //debugging

  //  Serial.println("The Pieces separated by strtok()");
  //  for (int n = 0; n < index; n++)
  //  {
  //     Serial.print(n);
  //     Serial.print("  ");
  //     Serial.println(strings[n]);
  //  }

  tempC = atof(strings[0]);
  tempF = atof(strings[1]);
  micValue = atof(strings[2]);
  hitValue = atof(strings[3]);
  //servoValue = atoi(strings[4]);

   ptr = NULL;
}

void printValues(){
  Serial.print(tempC);Serial.println(" C");
  Serial.print(tempF);Serial.println(" F");
  Serial.println(micValue);
  Serial.println(hitValue);
}

int handleServo(int value){
  int upper = 180;
  int lower = 0;
  if(value > upper){
    value = upper;
  }

  if(value < lower){
    value = lower;
  }

  return value;
}

void setup() {

  Serial.begin(115200);        

  // Read network credentials from Preferences
  pref.begin("network");
  String savedSSID = pref.getString("ssid", "wifiSusdam");
  String savedPassword = pref.getString("password", "12345678");
  pref.end();

  WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  
  url = "http://" + WiFi.localIP().toString() + ":81/stream";
  Serial.println(url);

  // Connect to Wi-Fi using saved credentials
  if (savedSSID != "" && savedPassword != "") {
   Blynk.begin(auth, savedSSID.c_str(),savedPassword.c_str() );
  } else {
    Serial.println("No saved credentials found !");
  }

  Blynk.setProperty(V6, "url", "http://10.46.129.172:81/stream");
  Blynk.setProperty(V6, "onImageUrl", "http://10.46.129.172:81/stream");
  Blynk.setProperty(V6, "offImageUrl", "http://10.46.129.172:81/stream");


  //SERIAL       // USB Serial ไปคอมพิวเตอร์
  Serial2.begin(SERIAL_BAUD, SERIAL_8N1, RX_PIN, TX_PIN); // UART2 ไปอีกบอร์ด

  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  Blynk.run();
  int bytesRead = Serial2.readBytesUntil('\n', msg, sizeof(msg) - 1);
  msg[bytesRead] = '\0';

  //debug
  //Serial.println(msg);

  decodeMsg(msg);

  //printValues();


  Blynk.virtualWrite(V0, tempC);
  Blynk.virtualWrite(V1, tempF);
  Blynk.virtualWrite(V2, micValue);
  Blynk.virtualWrite(V3, hitValue);

  //Serial.println(servoValue);
  // if(lastServoValue != servoValue){
  //   int i = 10000;
  //   Serial.println("sending shit");
  //   while(i--){
  //     Serial2.println(servoValue);
  //   }
  //   Serial.println("sending shit done");
  // }
  // lastServoValue = servoValue;
  Serial.println(servoValue);
  Serial2.println(servoValue);
  //Blynk.virtualWrite(V5, servoValue);

}

BLYNK_WRITE(V5) {
  int data = param.asInt();
  int pwmWave = map(data,  0, 90, 0, 90);
  servoValue = handleServo(pwmWave);

  // Blynk.virtualWrite(V5, servoValue);
}

BLYNK_WRITE(V6) {

  Blynk.setProperty(V6, "url", url.c_str());
  Blynk.setProperty(V6, "onImageUrl", url.c_str());
  Blynk.setProperty(V6, "offImageUrl", url.c_str());

  // Blynk.virtualWrite(V5, servoValue);
}

