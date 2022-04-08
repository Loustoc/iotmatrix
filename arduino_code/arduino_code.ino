/*Copyright 2020, The MathWorks, Inc.
*/String test;
#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
unsigned char i;
 unsigned char j;
const int PAYLOAD_LENGTH = 12; // Known in advance
unsigned char disp1[PAYLOAD_LENGTH];
int Max7219_pinCLK = D5;
int Max7219_pinCS = D8;
int Max7219_pinDIN = D0;
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

// Weather station channel details

// Counting channel details
unsigned long counterChannelNumber = SECRET_CH_ID_COUNTER;
const char * myCounterReadAPIKey = SECRET_READ_APIKEY_COUNTER;
unsigned int counterFieldNumber = 1; 
void Write_Max7219_byte(unsigned char DATA) 
{   
            unsigned char i;
      digitalWrite(Max7219_pinCS,LOW);    
      for(i=8;i>=1;i--)
          {     
             digitalWrite(Max7219_pinCLK,LOW);
             digitalWrite(Max7219_pinDIN,DATA&0x80);// Extracting a bit data
             DATA = DATA<<1;
             digitalWrite(Max7219_pinCLK,HIGH);
           }                                 
}

void Write_Max7219(unsigned char address,unsigned char dat)
{
        digitalWrite(Max7219_pinCS,LOW);
        Write_Max7219_byte(address);           //address，code of LED
        Write_Max7219_byte(dat);               //data，figure on LED 
        digitalWrite(Max7219_pinCS,HIGH);
}
void Init_MAX7219(void)
{
 Write_Max7219(0x09, 0x00);       //decoding ：BCD
 Write_Max7219(0x0a, 0x03);       //brightness 
 Write_Max7219(0x0b, 0x07);       //scanlimit；8 LEDs
 Write_Max7219(0x0c, 0x01);       //power-down mode：0，normal mode：1
 Write_Max7219(0x0f, 0x00);       //test display：1；EOT，display：0
}
void setup() {
  Serial.begin(115200);// Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  pinMode(Max7219_pinCLK,OUTPUT);
  pinMode(Max7219_pinCS,OUTPUT);
  pinMode(Max7219_pinDIN,OUTPUT);
  delay(50);
  Init_MAX7219();
}

void loop() {

  int statusCode = 0;
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(String(ssid).c_str(), String(pass).c_str()); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
  }
  delay(500); // No need to read the temperature too often.

  String count = ThingSpeak.readStringField(counterChannelNumber, counterFieldNumber, myCounterReadAPIKey);  
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    Serial.println("matrice: " + String(count));
  }
  else{
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }
  test= "{"+String(count)+"}";
 const char* bufferPtr = test.c_str();
    for (int i = 0; i < PAYLOAD_LENGTH && sscanf(bufferPtr + 1, "%hhx", &disp1[i]); i++, bufferPtr += 5);
  for(i=1;i<9;i++){
    Write_Max7219(i,disp1[i-1]);}
   delay(500); 

  
}
