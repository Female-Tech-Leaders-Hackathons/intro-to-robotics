#include "SimpleRSLK.h"


//-------------------------------------------------------------------------
#include "Appartment.h"

//Finite State Machine FSM states
#define goERROR -1
#define goLeft 0
#define goRight 1
#define goUp 2
#define goDown 3

#define noWall -1
#define wallUp 0
#define wallDown 1
#define wallLeft 2
#define wallRight 3

int numWaypoints = 0;
int waypointsList[100]; //50 waypoints max
int xVectorList[50];
int yVectorList[50];

int currentX = 0;
int currentY = 0;
int nextX = 0;
int nextY = 0;
int goalX = 0;
int goalY = 0;
int origGoalX = 0;
int origGoalY = 0;


//MOTOR STEUERUNG definitionen HIER UNTEN DRUNTER
#define globalOrientationUp 0
#define globalOrientationDown 1
#define globalOrientationLeft 2
#define globalOrientationRight 3


int currentOrientation = globalOrientationUp; //ganz am Anfang zeigt der Roboter nach oben


//-------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  setupRSLK();


  //-------------------------------------------------------------------------
  //Gebe die gesamte Karte via Serial aus
  for (int i = 0; i < 60; i++) {
    for (int j = 0; j < 60; j++) {
      Serial.print(obstaclePlan[i][j] == 0 ? "0" : "1");
    }
    Serial.println();
  }
  //-------------------------------------------------------------------------
  //Beispiel, von (41,58) nach (24,39);
  int START_X = 41, START_Y = 58;  //wird unten zur Berechnung der Vektoren wiederverwendet
  Pathfinding(START_X, START_Y, 24, 39);
  //Ergebnis ist in waypointsList[] enthalten. Es gibt "numWaypoints" Koordinatenpaare --> "2 * numWaypoints" Eintraege

  //Ausgabe aller Waypoints
  Serial.println("\n\nRESULT OF PATHFINDING:");
  for (int i = 0; i < numWaypoints; i++) {
    Serial.print("GOTO square with coordinates (x,y) = (");
    Serial.print(waypointsList[2 * i]);
    Serial.print(",");
    Serial.print(waypointsList[(2 * i) + 1]);
    Serial.println(")");
  }
  Serial.println("ALGORITHM DONE!");

  //Umwandlung der Waypoints in Vektoren
  //xVectorList
  //yVectorList
  for (int i = 0; i < numWaypoints; i++) {
    int prevX;
    int prevY;
    int currX = waypointsList[2 * i];
    int currY = waypointsList[(2 * i) + 1];
    if (i == 0) {
      prevX = START_X;
      prevY = START_Y;
    } else {
      prevX = waypointsList[2 * (i - 1)];
      prevY = waypointsList[(2 * (i - 1)) + 1];
    }
    xVectorList[i] = currX - prevX;
    yVectorList[i] = currY - prevY;
  }

  //Ausgabe der Vektoren
  for (int i = 0; i < numWaypoints; i++) {
    Serial.print("FOLLOW vectors (x,y) = (");
    Serial.print(xVectorList[i]);
    Serial.print(",");
    Serial.print(yVectorList[i]);
    Serial.println(")");
  }
  //-------------------------------------------------------------------------
  //Motorsteuerung nach Vektoren

  for (int i = 0; i < numWaypoints; i++) {
    
    int tempVectorOrientation = globalVectorOrientation(xVectorList[i], yVectorList[i]);
    
    robotWalk(currentOrientation, tempVectorOrientation, xVectorList[i], yVectorList[i]);
    
    currentOrientation = tempVectorOrientation;
    Serial.println("----");
  }
  //-------------------------------------------------------------------------
}

void loop() {

}

int globalVectorOrientation(int x, int y) {
  int ori = -1;
  if (x < 0 && y == 0) ori = globalOrientationLeft;
  else if (x > 0 && y == 0) ori = globalOrientationRight;
  else if (x == 0 && y < 0) ori = globalOrientationUp;
  else if (x == 0 && y > 0) ori = globalOrientationDown;
  return ori;
}


//-----------------------------------------------------HERE DOWN BELOW----------------ADD MOTOR CONTROL FUNCTIONS where comments are-------------------------------
void robotWalk(int robotOrientation, int vectorOrientation, int x, int y) {
  int distance = abs(x) + abs(y);  //1 - Norm
  if (robotOrientation == vectorOrientation) {
    //HIER distance nach VORNE
    Serial.print("FAHREN um ");
    Serial.print(distance);
    Serial.println(" Schritte");
  } else if (
    (robotOrientation == globalOrientationUp && vectorOrientation == globalOrientationDown) ||
    (robotOrientation == globalOrientationDown && vectorOrientation == globalOrientationUp) ||
    (robotOrientation == globalOrientationLeft && vectorOrientation == globalOrientationRight) ||
    (robotOrientation == globalOrientationRight && vectorOrientation == globalOrientationLeft)
  ) {
    //HIER rotieren 180
    Serial.println("DREHEN um 180");
    //HIER distance nach VORNE
    Serial.print("FAHREN um ");
    Serial.print(distance);
    Serial.println(" Schritte");
  }
  else if (
    (robotOrientation == globalOrientationUp && vectorOrientation == globalOrientationLeft) ||
    (robotOrientation == globalOrientationDown && vectorOrientation == globalOrientationRight) ||
    (robotOrientation == globalOrientationLeft && vectorOrientation == globalOrientationDown) ||
    (robotOrientation == globalOrientationRight && vectorOrientation == globalOrientationUp)
  ) {
    //HIER rotieren 90 links
    Serial.println("DREHEN um 90 nach LINKS");
    //HIER distance nach VORNE
    Serial.print("FAHREN um ");
    Serial.print(distance);
    Serial.println(" Schritte");
  }
  else if (
    (robotOrientation == globalOrientationUp && vectorOrientation == globalOrientationRight) ||
    (robotOrientation == globalOrientationDown && vectorOrientation == globalOrientationLeft) ||
    (robotOrientation == globalOrientationLeft && vectorOrientation == globalOrientationUp) ||
    (robotOrientation == globalOrientationRight && vectorOrientation == globalOrientationDown)
  ) {
    //HIER rotieren 90 rechts
    Serial.println("DREHEN um 90 nach RECHTS");
    //HIER distance nach VORNE
    Serial.print("FAHREN um ");
    Serial.print(distance);
    Serial.println(" Schritte");
  }
}








void Pathfinding(int start_X, int start_Y, int goal_X, int goal_Y) {
  int listEntryCounter = 0;

  //waypointsList[listEntryCounter++] =

  currentX = 0;
  currentY = 0;
  nextX = 0;
  nextY = 0;
  goalX = goal_X;
  goalY = goal_Y;
  origGoalX = goalX;
  origGoalY = goalY;

  int FSM_goState = -1;
  int FSM_wallState = noWall;

  //raycasting function for the goal location to the next wall
  //without this function this pathfinding algorithm only works if the goal is located on a wall
  bool touchesWall = false;
  int offset = 0;
  while (!touchesWall) {
    //Serial.println(offset);
    //check for above
    if (isWall(goalX, goalY - offset - 1)) {
      touchesWall = true;
      //goalX = goalX;
      goalY = goalY - offset;
      continue;
    }
    //check for below
    if (isWall(goalX, goalY + offset + 1)) {
      touchesWall = true;
      //goalX = goalX;
      goalY = goalY + offset;
      continue;
    }
    //check for left
    if (isWall(goalX - offset - 1, goalY)) {
      touchesWall = true;
      goalX = goalX - offset;
      //goalY = goalY;
      continue;
    }
    //check for right
    if (isWall(goalX + offset + 1, goalY)) {
      touchesWall = true;
      goalX = goalX + offset;
      //goalY = goalY;
      continue;
    }

    offset += 1;
  }
  //array[goalX][goalY] = 3;  //mark the (new) goal

  currentX = start_X;
  currentY = start_Y;


  //determine initial walk direction
  int minDist = 999999;
  int tempDist = 0;

  //check for left
  if (!isWall(currentX - 1, currentY)) {
    tempDist = distGoal10(currentX - 1, currentY);
    if (tempDist < minDist) {
      minDist = tempDist;
      FSM_goState = goLeft;
      //Serial.println("left: " + tempDist);
    }
  }
  //check for right
  if (!isWall(currentX + 1, currentY)) {
    tempDist = distGoal10(currentX + 1, currentY);
    if (tempDist < minDist) {
      minDist = tempDist;
      FSM_goState = goRight;
      //Serial.println("right: " + tempDist);
    }
  }
  //check for up
  if (!isWall(currentX, currentY - 1)) {
    tempDist = distGoal10(currentX, currentY - 1);
    if (tempDist < minDist) {
      minDist = tempDist;
      FSM_goState = goUp;
      //Serial.println("up: " + tempDist);
    }
  }
  //check for down
  if (!isWall(currentX, currentY + 1)) {
    tempDist = distGoal10(currentX, currentY + 1);
    if (tempDist < minDist) {
      minDist = tempDist;
      FSM_goState = goDown;
      //Serial.println("down: " + tempDist);
    }
  }
  //Serial.println("----");

  FSM_wallState = followingWallDetect(currentX, currentY, FSM_goState);

  int debugCounter = 0;
  while ((!(currentX == goalX && currentY == goalY)) && debugCounter < 150) {
    debugCounter += 1;
    //terminate search if too many waypoints
    if (listEntryCounter + 1 >= 100) {
      Serial.println("YIKES!! Abort!! took too many steps. Not plausible. Maybe corrdinates are not in bounds?");
      return;
    }

    //compute next potential step
    nextX = FSM_goState == goLeft ? currentX - 1 : FSM_goState == goRight ? currentX + 1 : currentX;
    nextY = FSM_goState == goDown ? currentY + 1 : FSM_goState == goUp ? currentY - 1 : currentY;

    //check if still following wall, if not, ask for new direction computation
    bool recomputationRequired = false;
    if (FSM_wallState >= 0 && !stillFollowingWall(nextX, nextY, FSM_goState, FSM_wallState)) {
      FSM_wallState = noWall;
      recomputationRequired = true;
    }

    //check for wall in front
    bool moveIsPossible = true;
    if (isWall(nextX, nextY)) {
      moveIsPossible = false;
      recomputationRequired = true;
    }

    //move forwards if possible
    if (moveIsPossible) {
      //Serial.print("currentX: "); Serial.print(currentX); Serial.print(" -> "); Serial.println(nextX);
      //Serial.print("currentY: "); Serial.print(currentY); Serial.print(" -> "); Serial.println(nextY);
      currentX = nextX;
      currentY = nextY;
      //array[currentX][currentY] = 7;  //mark the path
    }

    if (recomputationRequired) {
      //save current waypoint
      waypointsList[listEntryCounter++] = currentX;
      waypointsList[listEntryCounter++] = currentY;
      //determine new walk direction
      minDist = 999999;
      tempDist = 0;
      int savedFSM_goState = FSM_goState;

      //check for left
      if ((!isWall(currentX - 1, currentY)) && (!(savedFSM_goState == goRight))) {
        tempDist = distGoal10(currentX - 1, currentY);
        if (tempDist < minDist) {
          minDist = tempDist;
          FSM_goState = goLeft;
          //Serial.println("left: " + tempDist);
        }
      }
      //check for right
      if ((!isWall(currentX + 1, currentY)) && (!(savedFSM_goState == goLeft))) {
        tempDist = distGoal10(currentX + 1, currentY);
        if (tempDist < minDist) {
          minDist = tempDist;
          FSM_goState = goRight;
          //Serial.println("right: " + tempDist);
        }
      }
      //check for up
      if ((!isWall(currentX, currentY - 1)) && (!(savedFSM_goState == goDown))) {
        tempDist = distGoal10(currentX, currentY - 1);
        if (tempDist < minDist) {
          minDist = tempDist;
          FSM_goState = goUp;
          //Serial.println("up: " + tempDist);
        }
      }
      //check for down
      if ((!isWall(currentX, currentY + 1)) && (!(savedFSM_goState == goUp)) && (!specialCaseApplies(currentX, currentY, goalX, goalY))) {
        tempDist = distGoal10(currentX, currentY + 1);
        if (tempDist < minDist) {
          minDist = tempDist;
          FSM_goState = goDown;
          //Serial.println("down: " + tempDist);
        }
      }
      //Serial.println("----");
    }
    //check every time whether we aligned with a wall
    FSM_wallState = followingWallDetect(currentX, currentY, FSM_goState);
  }
  //waypoints.add(new PVector(goalX,goalY));
  waypointsList[listEntryCounter++] = goalX;
  waypointsList[listEntryCounter++] = goalY;

  if (!(goalX == origGoalX && goalY == origGoalY))
  {
    //waypoints.add(new PVector(origGoalX,origGoalY));
    waypointsList[listEntryCounter++] = origGoalX;
    waypointsList[listEntryCounter++] = origGoalY;
  }
  numWaypoints = listEntryCounter / 2;

  /*Serial.println("\n\nRESULT OF PATHFINDING:");
    for(int i = 0; i < numWaypoints; i++){
    //Serial.println("GOTO square with coordinates (x,y) = (" + waypointsList[2*i] + "," + waypointsList[(2*i) + 1] + ")");
    Serial.print("GOTO square with coordinates (x,y) = ("); Serial.print(waypointsList[2*i]); Serial.print(","); Serial.print(waypointsList[(2*i) + 1]); Serial.println(")");
    }
    Serial.println("ALGORITHM DONE!");*/
}

bool stillFollowingWall(int current_x, int current_y, int goState, int wallState) {
  int cx = current_x, cy = current_y;

  if (goState == goLeft || goState == goRight) {
    if (wallState == wallRight && isWall(cx, cy - 1)) return true;
    if (wallState == wallLeft && isWall(cx, cy + 1)) return true;
  } else {
    if (wallState == wallUp && isWall(cx - 1, cy)) return true;
    if (wallState == wallDown && isWall(cx + 1, cy)) return true;
  }
  return false;
}

int followingWallDetect(int current_x, int current_y, int goState) {
  int cx = current_x, cy = current_y;

  if (goState == goLeft || goState == goRight) {
    if (isWall(cx, cy - 1)) return wallRight;
    if (isWall(cx, cy + 1)) return wallLeft;
  } else {
    if (isWall(cx - 1, cy)) return wallUp;
    if (isWall(cx + 1, cy)) return wallDown;
  }
  return noWall;
}

int distGoal10(int x, int  y) {
  return round(10 * sqrt((x - goalX) * (x - goalX) + (y - goalY) * (y - goalY) ));
}

bool isWall(int x, int y) {
  if (/*obstaclePlan[x][y]*/obstaclePlan[y][x] == 1) return true;
  //bugfix #2
  if (22 <= x && x <= 27 && y == 28) {
    if (specialCaseApplies(x, y + 1, goalX, goalY)) return true;
  }
  return false;
}


//due to the algorithm's constraints (both coding time and RAM space on the microcontroller),
//a specific area has to be treated specially to not confuse the path finder
//-> with more coding time, the algorithm could be improved quite a bit
boolean specialCaseApplies(int X, int Y, int goalX, int goalY) {
  if (isInSpecialArea(X, Y) && (!isInSpecialArea(goalX, goalY))) return true;
  return false;
}

boolean isInSpecialArea(int x, int y) {
  if ((19 <= x && x <= 27) && (29 <= y && y <= 40)) return true;
  return false;
}
