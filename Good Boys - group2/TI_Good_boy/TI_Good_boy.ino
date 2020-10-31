#define ARDUINOJSON_ENABLE_STD_STRING 0
#define ARDUINOJSON_ENABLE_STD_STREAM 0

#include <ArduinoJson.h>
#include <SimpleRSLK.h>

StaticJsonDocument<300> doc;

int robotState = 0;
int direction = 0;
int distanceToDrive = 0;
int degressToTurn = 0;

enum {doNothingState, driveState, turnState, navigateState, goodBoyState};



enum {noGoal, LennartGoal, TillGoal, KitchenGoal};
int goal = noGoal;
int currentGoal = LennartGoal;

void setup() {
  Serial.begin(115200);
  while (!Serial) continue;

  Serial1.begin(115200);

  setupRSLK();

  Serial.print("finished Startup");
}

void loop() {

  boolean newData = handleSerial();
  if (newData) {
    Serial.println("new data");

    switch (robotState) {
      case doNothingState:
        //do nothing
        break;
      case driveState:
        driveXmm(distanceToDrive);
        Serial.print("mms to drive ");
        Serial.println(distanceToDrive);
        break;
      case turnState:
        turnXDegrees(degressToTurn);
        Serial.print("degrees to turn ");
        Serial.println(degressToTurn);
        break;
      case navigateState:
        Serial.println("Go to goal");
        Serial.print(goal);
        resetNavigationState();
        break;
      case goodBoyState:

        break;
      default:
        robotState = doNothingState;
        break;
    }
    newData = false;
  }

  if (robotState = navigateState) {
    handleNavigation();
  }

  handleMotors();
}
