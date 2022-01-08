/*
 * The following code snippet can be used to obtain the amount of garbage level in the bin,
 * air pollution around the bin using Ultrasonic sensor and push it to the Thingspeak server.
 * 
 */
#include <SoftwareSerial.h>          //Software Serial library

//Pins and variables initialisation
SoftwareSerial espSerial(2, 3);      //Pin 2 and 3 act as RX and TX. Connect them to TX and RX of ESP8266
#define DEBUG true

//Thingspeak 
String mySSID = "ig";                // WiFi SSID
String myPWD = "suwetha1";           // WiFi Password
String myAPI = "Z15YZG6JKY40ZX1D";   // API Key
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myFIELD = "field1";
String myFIELD1 = "field2";
int sendVal1, sendVal2;

//Ultrasonic sensor
int trigPin = 8;
int echoPin = 9;
long duration, cm;

void setup()
{
  //Initialise serial monitor
  Serial.begin(9600);

  //Initialise ESP8266
  espSerial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  espData("AT+RST", 1000, DEBUG);  //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000, DEBUG);  //Set the ESP mode as station mode
  espData("AT+CWJAP=\"" + mySSID + "\",\"" + myPWD + "\"", 1000, DEBUG); //Connect to WiFi network
  /*while(!esp.find("OK"))
    {
      //Wait for connection
    }*/
  delay(1000);
}

void loop()
{
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  int sensorValue = analogRead(A0);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = (duration / 2) / 29.1;
  sendVal1 = cm;
  sendVal2 = sensorValue;

  String sendData = "GET /update?api_key=" + myAPI + "&" + field1 + "=" + String(sendVal1) + "&" + field2 + "=" + String(sendVal2);
  espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
  espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG);
  espSerial.find(">");
  espSerial.println(sendData);
  Serial.print("Value to be sent: ");
  Serial.println(sendVal1);
  Serial.println(sendVal2);

  espData("AT+CIPCLOSE=0", 1000, DEBUG);
  delay(10000);
}

String espData(String command, const int timeout, boolean debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");

  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
    //Serial.print(response);
  }
  return response;
}
