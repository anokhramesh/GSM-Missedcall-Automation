#include <SoftwareSerial.h>
const int relayPin=4;//connect digital pin 4 of Arduino to IN pin of Relay module
//bool relayStatus=1;
bool relayStatus=0;
const String phone="+971557407961";  // white list phone include +91 or your country code

SoftwareSerial gsmSerial(7,8);//Connect RXT pin of GSM module to D8 pin on Arduino 
                             // Connect TXD pin of GSM module to D7 pin of Arduino
String responseData;
String senderNumber;

void setup(){
  responseData.reserve(200);
  phone.reserve(20);
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,HIGH);
  //digitalWrite(relayPin,LOW);
  Serial.begin(9600);
  gsmSerial.begin(9600);
  delay(100);
  gsmSerial.write("AT\r");// because of SIM800L autobounding mode
  delay(100);
  gsmSerial.write("AT+IPR=9600\r");
  delay(100);
  gsmSerial.write("AT+CMGF=1\r");
  delay(500);
  gsmSerial.write("AT+CNMI=2,2,0,0,0\r");
  delay(100);
  Serial.print(gsmSerial.readString());
  gsmSerial.write("AT+CLIP=1\r\n");
  delay(100);
}

void loop(){
  if (gsmSerial.available()>0) {
    responseData = gsmSerial.readStringUntil('\n');
    Serial.println(responseData);
    parse();
    delay(1000);
  }
}

void parse(){
  if (responseData.indexOf("CLIP:")>0){
  senderNumber=responseData.substring(responseData.indexOf("+CLIP: ") +8,responseData.indexOf("+CLIP: ") +21); //PARSE CALLER ID 
  Serial.println("Caller number   :");
  Serial.println(senderNumber);
   if (senderNumber == phone){
    Serial.println("Sender number White list : ok");
    relayStatus=!relayStatus;
    digitalWrite(relayPin,relayStatus);
    gsmSerial.write("ATH\r");  // disconnect call & then send SMS
    gsmSerial.print("AT+CMGS=\""+phone+"\"\r");
    delay(1000);
    gsmSerial.print("DEVICE STATUS-");
  //gsmSerial.print("Relay status 1:off 0:on  ");
    gsmSerial.print(relayStatus);
    delay(200);
    gsmSerial.write(0x1A); //  ctrl+z to send message
    delay(100);
    
     }
    gsmSerial.write("ATH\r");
    delay(500);
  
  }
}
