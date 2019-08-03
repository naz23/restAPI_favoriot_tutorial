void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}/* In this code, you need to change FOUR things:
 *    1. Your wifi SSID
 *    2. Your wifi password
 *    3. Your API key (can be obtained from the account setting in favoriot platform)
 *    4. Your device developer ID (in void dataStream() )
 * Once done, you may compile and upload this code and check the data stream in your favoriot platform.
 * This tutorial is about sending two parameters which are temperature & light intensity.
 * Goodluck!
 */
  
#include <CytronWiFiShield.h>
#include <CytronWiFiClient.h>
#include <CytronWiFiServer.h>
#include <SoftwareSerial.h>
#define sensorPin A0
#define lightsensor A1

const char *ssid = "xxxxxxxx";  // **replace the SSID
const char *pass = "xxxxxxxx";  // **replace the password
IPAddress ip(192, 168, 1 ,242);
ESP8266Server server(80);
ESP8266Client client;

int sensorValue = 0;
int celcius = 0;
int light = 0;
unsigned long start, finished, elapsed;
String apikey = "";    // **replace with your api key from the FAVORIOT platform account setting
char serverAdd[] = "api.favoriot.com";
                        
void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(9600);  
  //pinMode(pushButton, INPUT);
  pinMode(sensorPin,INPUT);
  pinMode(lightsensor,INPUT);


  if(!wifi.begin(2, 3))
  {
    Serial.println(F("Error talking to shield"));
    while(2000);
  }
  
  Serial.println(wifi.firmwareVersion());
  Serial.print(F("Mode: "));Serial.println(wifi.getMode());// 1- station mode, 2- softap mode, 3- both
  Serial.println(F("Setup wifi config"));
  //wifi.config(ip); //static ip
  Serial.println(F("Start wifi connection to"));
  Serial.print(F("Wifi SSID: "));Serial.println(ssid);
 
  if(!wifi.connectAP(ssid, pass))
  {
    Serial.println(F("Error connecting to WiFi"));
    while(2000);
  } 
  
  Serial.print(F("Connected to "));Serial.println(wifi.SSID());
  Serial.println(F("IP address: "));
  Serial.println(wifi.localIP());
  wifi.updateStatus();
  Serial.println(wifi.status()); //2- wifi connected with ip, 3- got connection with servers or clients, 4- disconnect with clients or servers, 5- no wifi
  clientTest();
  server.begin();
}

void loop() {

  //if(digitalRead(pushButton) == LOW) //disable this line to retrieve data automatically without pressing the button 
  //{  
    //execute when the push button is pressed only
     celcius = analogRead(sensorPin) * 0.488;  //read the temperature and convert to celcius
     light  = analogRead(lightsensor); 
     start=millis();  //start counting time
     dataStream(celcius,light);  //send data to FAVORIOT platform
     finished=millis();  //stop counting time
     elapsed=(finished-start) / 1000;  //calculate time taken to send data
     Serial.print("        PROCESSING TIME : " + String(elapsed) + " seconds\n");  //display time taken
     Serial.print("**************************************************");
 // }
  
  delay(1000*30); //enable this line if you disable the 'if' condition
}

void dataStream(int celcius, int light)
{ 
  // Json Data to send to Platform
  String json = "{";
    json += "\"device_developer_id\":"; 
    json += "\"deviceDefault@favoriotplatform\"";
    json +=",";
    json +="\"data\":";
    json += "{";
    json += "\"Temperature\":";
    json += "\""+String(celcius)+"\"";
    json += ",";
    json += "\"Light\":";
    json += '"'+String(light)+'"';
    json += "}}";
    
  // display temperature value
  Serial.println("\n        TEMPERATURE : " + String(celcius)+ " Celcius");
  Serial.println("          Light :" + String(light));
      
  if (client.connect(serverAdd, 80)) {
    // Make a HTTP request:
    Serial.println("        STATUS : Sending data...");  //Display sending status
    client.println("POST /v1/streams HTTP/1.1");
    client.println("Host: api.favoriot.com");
    client.print(String("apikey: "));
    client.println(apikey);
    client.println("Content-Type: application/json");
    client.println("cache-control: no-cache");
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println();
    client.println(json);
    client.println("Connection:close");
    Serial.println("        STATUS : Data sent!");  //display sent status
    }
}

void clientTest()
{
  const char destServer[] = "api.favoriot.com";
  ESP8266Client client;
  
  Serial.print(F("Starting connection to: "));Serial.println(destServer);
  Serial.print("\n**************************************************");
  if (!client.connect(destServer, 80))
  {
    Serial.println(F("Failed to connect to server."));
    client.stop();
    return;
  }
   while (client.available()>0)
  {
    Serial.write(client.read());
  }
  
  if (!client.connected()) {
    client.stop();
  } 
}

/* Favoriot Sdn. Bhd. */
