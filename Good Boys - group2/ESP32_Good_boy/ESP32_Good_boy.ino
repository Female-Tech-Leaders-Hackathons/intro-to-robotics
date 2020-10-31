/* Anleitung für diesen Code
  1.  In Zeile 11 #include "MeineZugangsdaten.h" auskommentieren
  2.  In Zeile 18 und 19 SSID und Passwort definieren
*/


#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>

//defines for Serial Port to TI Board
#define RXD2 16
#define TXD2 17

StaticJsonDocument<300> doc;

//HIER DAS AUSKOMMENTIEREN
#include "MeineZugangsdaten.h"

// firebase info
#define FIREBASE_HOST "drive-e26b7.firebaseio.com"
#define FIREBASE_AUTH "LhuiPwiORMNN2ju4MhEDvpxLpYKGs2uBK7ffxhqE"


// Define FirebaseESP8266 data object for data sending and receiving
FirebaseData firebaseData;

/*Was bekomme ich von der Firebase? (alles vom Typ String)

  ESP32done: Daten vom ESP32 verarbeitet?
  "false","true"

  Flag: Bewegungsmodus
  "1" für Fahren,"2" für auf der Stelle drehen,"3"

  direction: Richtung
  "to the right","forward","to the left","backward"

  Degree: Richtung
  any valid positive integer

  distance: Distanz
  any valid positive integer

  unit: Einheit
  "centimeters","meters"
*/



class RobotFirebaseManager
{
  public:
    int robotDistance;
    int robotAngle;
    int robotMode;
    char robotDirection[20];
    char unit[20];
    char goal[20];
    bool newData;

    RobotFirebaseManager(); //Constructor
    bool poll(); //polls firebase and pulls new data if available

};

RobotFirebaseManager::RobotFirebaseManager() {
  robotDistance = 0;
  robotAngle = 0;
  robotMode = 0;
  //robotDirection = NULL;
  //unit = NULL;
  newData = false;
}

bool RobotFirebaseManager::poll() {
  bool returnVal = false;
  /*
     returns:
      false   no new data is available (or an error occurred, hopefully just for debugging)
      true    new data has been written to the class
  */

  //read firebase
  String receivedString;

  if (Firebase.get(firebaseData, "/ESP32done")) {
    receivedString = firebaseData.stringData();
    char buf_temp[10];
    receivedString.toCharArray(buf_temp, 10);
    if (!strncmp(buf_temp, "true", 4)) {
      Serial.println("nothing new to read.");
      returnVal = false;
      goto returnSection;
    } else if (!strncmp(buf_temp, "false", 5)) {
      Serial.println("new data is here!");
      RobotFirebaseManager::newData = true;  //set newData flag
      returnVal = true;
    }
  }
  else goto errorSection;

  if (Firebase.get(firebaseData, "/Degree")) {
    Serial.print("Degree = ");
    char buf[20];
    receivedString = firebaseData.stringData();
    receivedString.toCharArray(buf, 20);
    RobotFirebaseManager::robotAngle = atoi(buf);
    Serial.println(RobotFirebaseManager::robotAngle);
    doc["degree"] = RobotFirebaseManager::robotAngle;
  }
  else goto errorSection;

  if (Firebase.get(firebaseData, "/direction")) {
    Serial.print("direction = ");
    receivedString = firebaseData.stringData();
    char buf[20];
    receivedString.toCharArray(buf, 20);
    strcpy(RobotFirebaseManager::robotDirection, buf);
    Serial.println(RobotFirebaseManager::robotDirection);
    doc["direction"] = RobotFirebaseManager::robotDirection;
  }
  else goto errorSection;

  if (Firebase.get(firebaseData, "/Goal")) {
    Serial.print("goal = ");
    receivedString = firebaseData.stringData();
    char buf[20];
    receivedString.toCharArray(buf, 20);
    strcpy(RobotFirebaseManager::goal, buf);
    Serial.println(RobotFirebaseManager::goal);
    doc["goal"] = RobotFirebaseManager::goal;
  }
  else goto errorSection;

  if (Firebase.get(firebaseData, "/unit")) {
    Serial.print("unit = ");
    receivedString = firebaseData.stringData();
    char buf[20];
    receivedString.toCharArray(buf, 20);
    strcpy(RobotFirebaseManager::unit, buf);
    Serial.println(RobotFirebaseManager::unit);
    doc["unit"] = RobotFirebaseManager::unit;
  }
  else goto errorSection;

  if (Firebase.get(firebaseData, "/distance")) {
    Serial.print("Distance = ");
    char buf[20];
    receivedString = firebaseData.stringData();
    receivedString.toCharArray(buf, 20);
    RobotFirebaseManager::robotDistance = atoi(buf);
    Serial.println(RobotFirebaseManager::robotDistance);
    doc["distance"] = RobotFirebaseManager::robotDistance;
  }
  else goto errorSection;

  if (Firebase.get(firebaseData, "/Flag")) {
    Serial.print("Robot Mode = ");
    char buf[20];
    receivedString = firebaseData.stringData();
    receivedString.toCharArray(buf, 20);
    RobotFirebaseManager::robotMode = atoi(buf);
    Serial.println(RobotFirebaseManager::robotMode);
    doc["mode"] = RobotFirebaseManager::robotMode;
  }
  else goto errorSection;

  //write to firebase (set flag)
  if (Firebase.setString(firebaseData, "/ESP32done", "true")) {
    Serial.println("Set Firebase Flag to true succeded!");
  }
  else goto errorSection;

  serializeJson(doc, Serial2);
  Serial.println("Sending over Serial2 to TI Board");

returnSection:
  return returnVal;

errorSection:
  Serial.print("Yikes! Something really went wrong here! :");
  Serial.println(firebaseData.errorReason());
  return false;
}


//Firebase Robot Object
RobotFirebaseManager man = RobotFirebaseManager();
void setup()
{
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  int startMillis = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
    if (millis() - startMillis > 5000) {
      Serial.println("Cant connect to WiFi restarting");
      ESP.restart();
    }
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //init Serial port for communication with ti board
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);


  //3. Set your Firebase info

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //4. Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);


  //6. Try to get int data from Firebase
}

void loop()
{
  man.poll();
  Serial.print("\n");

  //serializeJson(doc, Serial2);

  delay(100);
}
