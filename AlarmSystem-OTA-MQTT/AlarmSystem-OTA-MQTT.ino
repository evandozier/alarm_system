#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <arduino.h>
#include "DataToMaker.h"

const char* host = "esp8266-ota-alarm";
const char* ssid = "DozNET-N24";
const char* pass = "CHANGE_ME";
const char* myKey = "DUeiq8yYvR9Ce93tb_pzp"; // your maker key here
const uint16_t aport = 8266;

WiFiServer TelnetServer(aport);
WiFiClient Telnet;
WiFiUDP OTA;

//Sensors that actually are addressable
#define D1 5
#define D2 4
#define D5 14
#define D6 12
#define D7 13
#define NUMBER_OF_SENSORS 5 

int sensors[NUMBER_OF_SENSORS] = {D1, D2, D5, D6, D7};
int previous_values[NUMBER_OF_SENSORS];

// declare new maker events
DataToMaker Zone1Event(myKey, "Zone1");
DataToMaker Zone2Event(myKey, "Zone2");
DataToMaker Zone3Event(myKey, "Zone3");
DataToMaker Zone4Event(myKey, "Zone4");
DataToMaker Zone5Event(myKey, "Zone5");

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Arduino OTA Test");

  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());

  WiFi.begin(ssid, pass);
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    MDNS.begin(host);
    MDNS.addService("arduino", "tcp", aport);
    OTA.begin(aport);
    TelnetServer.begin();
    TelnetServer.setNoDelay(true);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

   //Initializing the pins as inputs and turning on pullups
  for (int i = 0; i < NUMBER_OF_SENSORS ; i++)
  {
    pinMode(sensors[i], INPUT_PULLUP);
    previous_values[i] = 0;
  }
}

void loop() {
  //OTA Sketch
  if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size   = OTA.parseInt();

    Serial.print("Update Start: ip:");
    Serial.print(remote);
    Serial.printf(", port:%d, size:%d\n", port, size);
    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(size)){
      Serial.println("Update Begin Error");
      return;
    }

    WiFiClient client;
    if (client.connect(remote, port)) {

      uint32_t written;
      while(!Update.isFinished()){
        written = Update.write(client);
        if(written > 0) client.print(written, DEC);
      }
      Serial.setDebugOutput(false);

      if(Update.end()){
        client.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(client);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }
  }
  //IDE Monitor (connected to Serial)
  if (TelnetServer.hasClient()){
    if (!Telnet || !Telnet.connected()){
      if(Telnet) Telnet.stop();
      Telnet = TelnetServer.available();
    } else {
      WiFiClient toKill = TelnetServer.available();
      toKill.stop();
    }
  }
  if (Telnet && Telnet.connected() && Telnet.available()){
    while(Telnet.available())
      Serial.write(Telnet.read());
  }
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t * sbuf = (uint8_t *)malloc(len);
    Serial.readBytes(sbuf, len);
    if (Telnet && Telnet.connected()){
      Telnet.write((uint8_t *)sbuf, len);
      yield();
    }
    free(sbuf);
  }

  //Debug
//  Serial.println("OTA Code Completed - Starting Monitor Code");
  int val;
  bool changed = false;
  //Monitor Code Start

  for (int i = 0; i < NUMBER_OF_SENSORS; i++)
  {
//    Serial.printf(" -- About to read sensor #%d (pin %d)", i, sensors[i]);
//    Serial.printf(" -- Free Mem: %d", ESP.getFreeHeap());
    val = digitalRead(sensors[i]);
//    Serial.printf(" -- Finished read of sensor %d: Value %d -- ", sensors[i], val);
    changed = DetectChange(val, i);
    if (changed)
    {
      Serial.printf("Detected change of sensor %d: Value %d", i, val);
      Serial.println();
      previous_values[i] = val;
   
      if(i == 0)
      {
        if (val == 0) Zone1Event.setValue(1, "Closed");
        else if (val == 1) Zone1Event.setValue(1, "Opened");

        //Establish connection with IFTTT
        Serial.println("connecting to IFTTT..");
        
        if(Zone1Event.connect())
        {
          Serial.println("Connected to Maker - Reporting Zone1 Trigger");
          Zone1Event.post();
        }
        else Serial.println("Failed to connect to Maker");
      }
      else if(i == 1)
      {
        if (val == 0) Zone2Event.setValue(1, "Closed");
        else if (val == 1) Zone2Event.setValue(1, "Opened");

        //Establish connection with IFTTT
        Serial.println("connecting to IFTTT..");
        
        if(Zone2Event.connect())
        {
          Serial.println("Connected to Maker - Reporting Zone2 Trigger");
          Zone2Event.post();
        }
        else Serial.println("Failed to connect to Maker");
      }
      else if(i == 2)
      {
        if (val == 0) Zone3Event.setValue(1, "Closed");
        else if (val == 1) Zone3Event.setValue(1, "Opened");

        //Establish connection with IFTTT
        Serial.println("connecting to IFTTT..");
        
        if(Zone3Event.connect())
        {
          Serial.println("Connected to Maker - Reporting Zone3 Trigger");
          Zone3Event.post();
        }
        else Serial.println("Failed to connect to Maker");
      }
      else if(i == 3)
      {
        if (val == 0) Zone4Event.setValue(1, "Closed");
        else if (val == 1) Zone4Event.setValue(1, "Opened");

        //Establish connection with IFTTT
        Serial.println("connecting to IFTTT..");
        
        if(Zone4Event.connect())
        {
          Serial.println("Connected to Maker - Reporting Zone4 Trigger");
          Zone4Event.post();
        }
        else Serial.println("Failed to connect to Maker");
      }
      else if(i == 4)
      {
        if (val == 0) Zone5Event.setValue(1, "Closed");
        else if (val == 1) Zone5Event.setValue(1, "Opened");

        //Establish connection with IFTTT
        Serial.println("connecting to IFTTT..");
        
        if(Zone5Event.connect())
        {
          Serial.println("Connected to Maker - Reporting Zone5 Trigger");
          Zone5Event.post();
        }
        else Serial.println("Failed to connect to Maker");
      }
      
    }
    //Debug
//    Serial.printf(" -- Finished compare of sensor %d", i);
//    Serial.println("");
  }

  //Debug
//  Serial.println("OTA Code Completed - Finished Monitor Code");

  delay(1000);
}

bool DetectChange(int val, int i)
{
  bool changed = false;
  if (val != previous_values[i])
  {
    previous_values[i] = val;
    changed = true;
  }

//  if (!changed) Serial.println("No Changes Detected");
  return changed;
}

