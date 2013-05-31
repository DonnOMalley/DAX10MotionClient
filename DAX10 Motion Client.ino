
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192.168.1.177); // Google
EthernetClient client;

int PIR_PIN;
int OutputPIN;
int TurnOffDelay; //in milliseconds
int TurnOffCounter;

boolean LightsOn;
boolean MovementDetected;
boolean TurnOffEnabled;

void setup() {
  
  TurnOffCounter = 0;
  TurnOffDelay = 5000;
  
  Serial.begin(9600);
  
  PIR_PIN = 5;
  OutputPIN = 7;
  
  pinMode(PIR_PIN, INPUT);
  pinMode(OutputPIN, OUTPUT);
  Serial.println("Input/Output Pins Initialized");
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  
  LightsOn = false;
  TurnOffEnabled = false;
}

void  SendLightCommand(boolean OnOff) {
  
  String strRequest;
  int millisConnecting;
  
   if (OnOff) {
       strRequest = "GET /?B81 HTTP/1.0"
   }
   else {
       strRequest = "GET /?B80 HTTP/1.0"
   }
   
  millisConnecting = millis();
  Serial.print("connecting...");

  if (client.connect(server, 80)) {
    millisConnecting = millis() - millisConnecting;
    Serial.print("connected (");
    Serial.print(millisConnecting, DEC);
    Serial.println("ms)");
    
    // Make the HTTP request:
    client.println(strRequest);
    client.println();
    delay(100); //give it enough time to get the request
    Serial.println("disconnecting.");
    client.stop();
    Serial.println("disconnected.");
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  
  MovementDetected = digitalRead(PIR_PIN);
  if(MovementDetected = HIGH) {
    digitalWrite(OutputPIN, HIGH);
    SendLightCommand(true);
  }
  else {
     if (!TurnOffEnabled) {
       TurnOffEnabled = true;
       TurnOffCounter = millis();
     }
     else {
       if((millis() - TurnOffCounter) => TurnOffDelay) {
         digitalWrite(OutputPIN, HIGH);
         SendLightCommand(false);
       }
     }
  }
  
  if (!client.connected() && client.available) {  
    client.println();
    Serial.println("disconnecting.");
    client.stop();
    Serial.println("disconnected.");
  }
}
