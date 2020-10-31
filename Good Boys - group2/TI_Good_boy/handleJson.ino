int rawDistance = 0;
int rawDriveDirection = 0;
int rawTurnDirection = 0;

boolean handleSerial() {
  if (Serial1.available())
  {
    DeserializationError err = deserializeJson(doc, Serial1);

    if (err == DeserializationError::Ok)
    {
      //Serial.println("Current Data");

      handleDirection(); //handle direction of driving and turning

      handleDegrees(); //handles the degrees for turning with direction

      rawDistance = doc["distance"].as<int>(); //update raw distance without unit conversion

      robotState = doc["mode"].as<int>(); //update the robotState

      handleUnit(); //make unit conversion for driving distance

      handleGoal();

      return true; // return that we had new command data
    }
    else
    {
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());

      while (Serial1.available() > 0)
        Serial1.read();
      return false;
    }
  }
  return false;
}

//go to goal
void handleGoal() {
  char buf[20];
  String bufString = doc["goal"].as<String>();
  bufString.toCharArray(buf, 20);
  Serial.print("Goal ");
  Serial.println(buf);

  if (!strcmp(buf, "Lennart") || !strcmp(buf, "lennart")) {
    goal = LennartGoal;
  }

  if (!strcmp(buf, "Robin") || !strcmp(buf, "robin")) {
    goal = TillGoal;
  }

  if (!strcmp(buf, "Kitchen") || !strcmp(buf, "kitchen")) {
    goal = KitchenGoal;
  }
}

//handles the degrees for turning with direction
void handleDegrees() {
  degressToTurn = doc["degree"].as<int>() * rawTurnDirection;
  //Serial.print("degree = ");
  //Serial.println(degressToTurn);
}

//handle the unit conversion for driving
void handleUnit() {
  char buf[20];
  String bufString = doc["unit"].as<String>();
  bufString.toCharArray(buf, 20);
  //Serial.print("unit = ");
  //Serial.println(buf);

  if (!strcmp(buf, "meters") || !strcmp(buf, "meter")) {
    distanceToDrive = 1000 * rawDistance;
  }


  if (!strcmp(buf, "centimeters")) {
    distanceToDrive = 10 * rawDistance;
  }

  distanceToDrive *= rawDriveDirection;
}

//handle direction of driving and turning
void handleDirection() {
  char buf[20];
  String bufString = doc["direction"].as<String>();
  bufString.toCharArray(buf, 20);
  //Serial.print("direction = ");
  //Serial.println(buf);

  if (!strcmp(buf, "forwards") || !strcmp(buf, "forward")) {
    rawDriveDirection = 1;
  }

  if (!strcmp(buf, "backwards") || !strcmp(buf, "backward")) {
    rawDriveDirection = -1;
  }


  if (!strcmp(buf, "to the right") || !strcmp(buf, "right")) {
    rawTurnDirection = 1;
  }

  if (!strcmp(buf, "to the left")  || !strcmp(buf, "left")) {
    rawTurnDirection = -1;
  }
}
