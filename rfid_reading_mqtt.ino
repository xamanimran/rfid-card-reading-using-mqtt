/*working*/
#include <SPI.h>
#include <MFRC522.h>
#include<ESP8266WiFi.h>
#include<PubSubClient.h>
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above
#define wifi_ssid ""
#define wifi_password ""
#define mqtt_server "test.mosquitto.org" //its a cloud server just for you to temporarily run the code and understand, as implementing it locally takes some efforts.
#define tagn "TagNumber"
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(115200);    
    SPI.begin(); // Init SPI bus
    setup_wifi();
    rfid.PCD_Init(); // Init MFRC522
    client.setServer(mqtt_server, 1883);
}
void setup_wifi() {
    delay(10);
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT Connection...");
    if (client.connect("nodeMcuDHT22")){
      Serial.println("connected");}
      else
      {Serial.print("failed,rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
      }
    }
  }

long lastMsg=0;
void loop()
{ if (!client.connected())
  {reconnect();
  }
  client.loop();
  long now=millis();
  if(now-millis()>30000)
  {lastMsg=now;
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
     client.publish(tagn, String(tag).c_str(),true); //publish to mqtt
        delay(5000);
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

       
        }
    }
