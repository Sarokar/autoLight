


#include <ESP8266WiFi.h> //Using NodeMCU with ESP8266
#include <SPI.h>

//philips hue 
const char hueHubIP[] = "192.168.1.12";  // Hue hub IP
const int hueHubPort = 80;               //hue port
bool lightState = 1;
int lightMode = 0;

//wifi details
WiFiClient client;
const char *ssid = "NETGEAR69"; 
const char *password = "moderncanoe383";  
unsigned i;

//inputs
const int button1 = 16;
const int button2 = 5;
const int button3 = 4;
const int analogInPin = A0;  // potentimeteer
int sensorValue = 0;  // value read from the pot
int outputValue = 0;
int oldBright = 255;
bool isPattern = 0;

//led


  

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

const unsigned short tasksNum = 1;
task tasks[tasksNum];

enum WifiS_States {WifiS_init,WifiS_on,WifiS_off,WifiS_button,WifiS_preset,WifiS_adjust,WifiS_pattern};
int TickFct_WifiS(int state) { 
  switch(state) {
    case WifiS_init:
    break;

    case WifiS_button:
    Serial.println("init");

    //brightness adjust
    sensorValue = analogRead(analogInPin);
    outputValue = map(sensorValue, 0, 1023, 0, 255);
    Serial.println("sensor = ");
    Serial.print(outputValue);

    if (outputValue != oldBright) {
      state = WifiS_adjust;
      oldBright = outputValue;
      break;
    }

      //on/off
      if (digitalRead(button1) == HIGH) {
          Serial.print("buttonPress");
          if (lightState == 0) {
            Serial.print("1");
            state = WifiS_on;
          }
          else {
            Serial.print("2");
            state = WifiS_off;
          }
          break;
      }

      //light presets
      if (digitalRead(button2) == HIGH) {
            Serial.println("a2 press");
            state = WifiS_preset;
            break;
      }

       //random light show
      if (digitalRead(button3) == HIGH) {
            Serial.println("a3 press");
            isPattern = 1;
            state = WifiS_pattern;
            break;
      }
          
    break;

  

//    
    default:  
      break; 
    
   }
  
  switch(state) {
    case WifiS_init:
      state = WifiS_button;

    break;

    case WifiS_on:
      if (client.connect(hueHubIP, hueHubPort)) {
       Serial.println("Light On");
       client.println("PUT /api/ELqDQODw0Ih10TO0oMXpxXsHKV4ta6CHSuPEnS-0/lights/1/state HTTP/1.1"); 
       client.println("keep-alive");
       client.println("Host: 192.168.1.12"); 
//       client.println("Content-Length: 20");
        client.println("Content-Length: 11");
        client.println("Content-Type: text/plain;charset=UTF-8");
        client.println();  // blank line before body
        client.println("{\"on\":true}");
//        client.println("{\"on\":true,\"hue\":10}");
        //Serial.println("lights switched");
        client.stop();  
      }
      //digitalWrite(led1,LOW);
      lightState = 1;
      state = WifiS_button;
    break;

     case WifiS_off:
      if (client.connect(hueHubIP, hueHubPort)) {
      Serial.println("Light Off");
       client.println("PUT /api/ELqDQODw0Ih10TO0oMXpxXsHKV4ta6CHSuPEnS-0/lights/1/state HTTP/1.1");
       client.println("keep-alive");
       client.println("Host: 192.168.1.12"); 
//       client.println("Content-Length: 20");
        client.println("Content-Length: 12");
        client.println("Content-Type: text/plain;charset=UTF-8");
        client.println();  // blank line before body
        client.println("{\"on\":false}");
//        client.println("{\"on\":true,\"hue\":10}");
        //Serial.println("lights switched");
        client.stop();  
      }
     // digitalWrite(led1,HIGH);
      lightState = 0;
      state = WifiS_button;
    break;

    case WifiS_preset:
      if (client.connect(hueHubIP, hueHubPort)) {
       Serial.println("Light On");
       client.println("PUT /api/ELqDQODw0Ih10TO0oMXpxXsHKV4ta6CHSuPEnS-0/lights/1/state HTTP/1.1"); 
       client.println("keep-alive");
       client.println("Host: 192.168.1.12");
        client.println("Content-Length: 22");
        client.println("Content-Type: text/plain;charset=UTF-8");
        client.println();  // blank line before body
        if (lightMode == 0) {
          Serial.println("light 0");
          client.println("{\"xy\":[0.3096,0.3216]}");  //white
        }
        if (lightMode == 1) {
          Serial.println("light 1");
          client.println("{\"xy\":[0.3826,0.1597]}");  //purple
        }
        if (lightMode == 2) {
          Serial.println("light 2");
          client.println("{\"xy\":[0.3584,0.4180]}");  //purple
        }
        if (lightMode == 3) {
          Serial.println("light 3");
          client.println("{\"xy\":[0.675,0.322]}");  //red
        }

        lightMode++;
        if  (lightMode>=4) {
          lightMode = 0;
        }
        client.stop();  
      }
      state = WifiS_button;
    break;

    case WifiS_adjust:
      if (client.connect(hueHubIP, hueHubPort)) {
       Serial.println("Light On");
       client.println("PUT /api/ELqDQODw0Ih10TO0oMXpxXsHKV4ta6CHSuPEnS-0/lights/1/state HTTP/1.1"); 
       client.println("keep-alive");
       client.println("Host: 192.168.1.12"); 
        if (outputValue >= 100) {
          client.println("Content-Length: 11");
        }
        if (outputValue < 100) {
          client.println("Content-Length: 10");
        }
        if (outputValue < 10) {
          client.println("Content-Length: 9");
        }
        client.println("Content-Type: text/plain;charset=UTF-8");
        client.println();  // blank line before body
        client.println("{\"bri\":" + String(outputValue) + "}");
        client.stop();  
      }
      state = WifiS_button;
    break;

    case WifiS_pattern:
      Serial.println("pattern");
      while (isPattern) {
        if (client.connect(hueHubIP, hueHubPort)) {
          Serial.println("pattern1");
       client.println("PUT /api/ELqDQODw0Ih10TO0oMXpxXsHKV4ta6CHSuPEnS-0/lights/1/state HTTP/1.1");
       client.println("keep-alive");
       client.println("Host: 192.168.1.12"); 
        client.println("Content-Length: 19");
        client.println("Content-Type: text/plain;charset=UTF-8");
        client.println();  // blank line before body
        client.println("{\"xy\":[0.167,0.04]}"); 
        client.stop();  
        delay(1000);
        }

        if (digitalRead(button3) == HIGH) {
          isPattern = 0; 
        }

        Serial.println("pattern2");

         if (client.connect(hueHubIP, hueHubPort)) {

        client.println("PUT /api/ELqDQODw0Ih10TO0oMXpxXsHKV4ta6CHSuPEnS-0/lights/1/state HTTP/1.1");
       client.println("keep-alive");
       client.println("Host: 192.168.1.12"); 
        client.println("Content-Length: 22");
        client.println("Content-Type: text/plain;charset=UTF-8");
        client.println();  // blank line before body
        client.println("{\"xy\":[0.6725,0.3230]}"); 
        client.stop();  
        delay(1000);
         }

        if (digitalRead(button3) == HIGH) {
          isPattern = 0; 
        }

        Serial.println("pattern3");

        if (client.connect(hueHubIP, hueHubPort)) {
        client.println("PUT /api/ELqDQODw0Ih10TO0oMXpxXsHKV4ta6CHSuPEnS-0/lights/1/state HTTP/1.1");
       client.println("keep-alive");
       client.println("Host: 192.168.1.12"); 
        client.println("Content-Length: 22");
        client.println("Content-Type: text/plain;charset=UTF-8");
        client.println();  // blank line before body
        client.println("{\"xy\":[0.5567,0.4091]}"); 
        client.stop();  
        }

        if (digitalRead(button3) == HIGH) {
          isPattern = 0; 
        }
        delay(500);

        
      

      
        
        
      }

      state = WifiS_button;

    break;



 
    default:
      break;
  }
  
  return state; 
}



void setup()
{
  //inputs
  pinMode(button1, INPUT); 
  //pinMode(led1,OUTPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  //digitalWrite(led1,HIGH);

  //wifi connection
  delay(3000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  
  i = 0;
  tasks[i].state = WifiS_init;
  tasks[i].period = 200;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TickFct_WifiS; 
  i++;
  
}

void loop()
{
  
  Serial.println("loop start");
  for (i = 0; i < tasksNum; ++i) {
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }
delay(1000);


   
}
