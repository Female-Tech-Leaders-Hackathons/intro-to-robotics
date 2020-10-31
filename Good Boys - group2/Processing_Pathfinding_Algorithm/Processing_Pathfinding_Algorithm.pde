int size = 13; //<>// //<>// //<>//
int sizeX = 60, sizeY = 60;
int centimetersX = 500, centimetersY = 500;
char[][] array = new char[sizeX][sizeY];

Button importButton, solveButton;

void setup() {
  for (int i=0; i < sizeX; i++) {
    for (int j=0; j < sizeY; j++) {
      array[i][j] = 0;
    }
  }

  importButton = new Button(805, 705, 90, 90, "Import", 98);
  solveButton = new Button(805, 5, 90, 90, "Solve!", 99);

  size(901, 801, P2D);
  background(0);
}

void draw() {
  drawRects();
  processImportButton();
  processSolveButton();
  processMouseText();
}

void drawRects() {
  for (int i=0; i < sizeX; i++) {
    for (int j=0; j < sizeY; j++) {
      if (array[i][j] == 0) {
        fill(100);
      } else if (array[i][j] == 1) {
        fill(255, 0, 0);
      } else if (array[i][j] == 2) {
        fill(0, 0, 255);
      } else if (array[i][j] == 3) {
        fill(127, 0, 127);
      } else if (array[i][j] == 4) {
        fill(0, 127, 127);
      } else if (array[i][j] == 7) {
        fill(200);
      }
      rect(size*i, j*size, size, size);
    }
  }
}

boolean solvingAlreadyDone = false;

void processImportButton() {
  importButton.update();
  if (importButton.pressed) {
    importButton.pressed = false;
    selectInput("Select a file to read from:", "inputFileSelected");
  }
}

void inputFileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } else {
    println("User selected " + selection.getAbsolutePath());
    solvingAlreadyDone = false;

    String[] lines = loadStrings(selection.getAbsolutePath());
    String[][] entries = new String[sizeX][sizeY];
    println("there are " + (lines.length - 2) + " lines");
    for (int i = 0; i < sizeY; i++) {
      entries[i] = splitTokens(lines[1 + i], ",");
    }
    for (int i=0; i < lines.length - 2; i++) {
      for (int j=0; j < entries[0].length; j++) {
        entries[i][j] = entries[i][j].trim();
        print(entries[i][j]);
        array[j][i] = (char)Integer.parseInt(entries[i][j]);
      }
      println();
    }
  }
}



void processSolveButton() {
  solveButton.update();
  if (solveButton.pressed) {
    solveButton.pressed = false;
  } else return;
  if (solvingAlreadyDone) return;
  //check for valid start and goal positons
  PVector start = new PVector(0, 0), goal = new PVector(0, 0);
  boolean hasStart = false, hasGoal = false;
  for (int i=0; i < sizeX; i++) {
    for (int j=0; j < sizeY; j++) {
      if (array[i][j] == 2) {
        start = new PVector(i, j);
        println("Start: ["+i+"]["+j+"]");
        hasStart = true;
      } else if (array[i][j] == 3) {
        goal = new PVector(i, j);
        println("Goal: ["+i+"]["+j+"]");
        hasGoal = true;
      }
    }
  }
  if (!(hasStart && hasGoal)) {
    println("not a valid setup!");
    return;
  } else {
    solvingAlreadyDone = true;
    solve(start, goal);
  }
}

final int goERROR = -1;
final int goLeft = 0;
final int goRight = 1;
final int goUp = 2;
final int goDown = 3;

final int noWall = -1;
final int wallUp = 0;
final int wallDown = 1;
final int wallLeft = 2;
final int wallRight = 3;

int currentX = 0;
int currentY = 0;
int nextX = 0;
int nextY = 0;
int goalX = 0;
int goalY = 0;
int origGoalX = 0;
int origGoalY = 0;
int FSM_goState = -1;
int FSM_wallState = noWall;

ArrayList<PVector> waypoints = new ArrayList<PVector>();

void solve(PVector start, PVector goal) {
  waypoints.clear();
  //int Fcosts = Gcosts(trail) + Hcosts(start, goal);
  //PVector current = new PVector(start.x, start.y);

  /*int bestCost = 99999999;
   int bestMove = -1;
   int lastMove = -1;*/
  goalX = round(goal.x);
  goalY = round(goal.y);
  
  origGoalX = goalX;
  origGoalY = goalY;

  FSM_goState = -1;
  FSM_wallState = noWall;

  //raycasting function for the goal location to the next wall
  //this pathfinding algorithm only works if the goal is located on a wall
  boolean touchesWall = false;
  int offset = 0;
  while(!touchesWall){
    //println(offset);
    //check for above
    if(isWall(goalX, goalY - offset - 1)){
      touchesWall = true;
      //goalX = goalX;
      goalY = goalY - offset;
      continue;
    }
    //check for below
    if(isWall(goalX, goalY + offset + 1)){
      touchesWall = true;
      //goalX = goalX;
      goalY = goalY + offset;
      continue;
    }
    //check for left
    if(isWall(goalX - offset - 1, goalY)){
      touchesWall = true;
      goalX = goalX - offset;
      //goalY = goalY;
      continue;
    }
    //check for right
    if(isWall(goalX + offset + 1, goalY)){
      touchesWall = true;
      goalX = goalX + offset;
      //goalY = goalY;
      continue;
    }
    
    offset += 1;
  }
  array[goalX][goalY] = 3;  //mark the (new) goal

  currentX = round(start.x);
  currentY = round(start.y);

  

  //determine initial walk direction
  int minDist = 999999;
  int tempDist = 0;

  //check for left
  if (!isWall(currentX-1, currentY)) {
    tempDist = distGoal10(currentX-1, currentY);
    if (tempDist < minDist) {
      minDist = tempDist;
      FSM_goState = goLeft;
      //println("left: " + tempDist);
    }
  }
  //check for right
  if (!isWall(currentX+1, currentY)) {
    tempDist = distGoal10(currentX+1, currentY);
    if (tempDist < minDist) {
      minDist = tempDist;
      FSM_goState = goRight;
      //println("right: " + tempDist);
    }
  }
  //check for up
  if (!isWall(currentX, currentY-1)) {
    tempDist = distGoal10(currentX, currentY-1);
    if (tempDist < minDist) {
      minDist = tempDist;
      FSM_goState = goUp;
      //println("up: " + tempDist);
    }
  }
  //check for down
  if (!isWall(currentX, currentY+1)) {
    tempDist = distGoal10(currentX, currentY+1);
    if (tempDist < minDist) {
      minDist = tempDist;
      FSM_goState = goDown;
      //println("down: " + tempDist);
    }
  }
  //println("----");

  FSM_wallState = followingWallDetect(currentX, currentY, FSM_goState);

  int debugCounter = 0;
  while ((!(currentX == goalX && currentY == goalY)) && debugCounter < 150) {
    debugCounter += 1;
    //compute next potential step
    nextX = FSM_goState == goLeft ? currentX - 1 : FSM_goState == goRight ? currentX + 1 : currentX;
    nextY = FSM_goState == goDown ? currentY + 1 : FSM_goState == goUp ? currentY - 1 : currentY;

    //check if still following wall, if not, ask for new direction computation
    boolean recomputationRequired = false;
    if (FSM_wallState >= 0 && !stillFollowingWall(nextX, nextY, FSM_goState, FSM_wallState)) {
      FSM_wallState = noWall;
      recomputationRequired = true;
    }

    //check for wall in front
    boolean moveIsPossible = true;
    if (isWall(nextX, nextY)) {
      moveIsPossible = false;
      recomputationRequired = true;
    }

    //move forwards if possible
    if (moveIsPossible) {
      //println("currentX: "+currentX+" -> "+nextX);
      //println("currentY: "+currentY+" -> "+nextY);
      currentX = nextX;
      currentY = nextY;
      array[currentX][currentY] = 7;  //mark the path
    }

    if (recomputationRequired) {
      //save current waypoint
      waypoints.add(new PVector(currentX,currentY));
      //determine new walk direction
      minDist = 999999;
      tempDist = 0;
      int savedFSM_goState = FSM_goState;

      //check for left
      if ((!isWall(currentX-1, currentY)) && (!(savedFSM_goState == goRight))) {
        tempDist = distGoal10(currentX-1, currentY);
        if (tempDist < minDist) {
          minDist = tempDist;
          FSM_goState = goLeft;
          //println("left: " + tempDist);
        }
      }
      //check for right
      if ((!isWall(currentX+1, currentY)) && (!(savedFSM_goState == goLeft))) {
        tempDist = distGoal10(currentX+1, currentY);
        if (tempDist < minDist) {
          minDist = tempDist;
          FSM_goState = goRight;
          //println("right: " + tempDist);
        }
      }
      //check for up
      if ((!isWall(currentX, currentY-1)) && (!(savedFSM_goState == goDown))) {
        tempDist = distGoal10(currentX, currentY-1);
        if (tempDist < minDist) {
          minDist = tempDist;
          FSM_goState = goUp;
          //println("up: " + tempDist);
        }
      }
      //check for down
      if ((!isWall(currentX, currentY+1)) && (!(savedFSM_goState == goUp)) && (!specialCaseApplies(currentX, currentY, goalX, goalY))) {
        tempDist = distGoal10(currentX, currentY+1);
        if (tempDist < minDist) {
          minDist = tempDist;
          FSM_goState = goDown;
          //println("down: " + tempDist);
        }
      }
      //println("----");
    }
    //check every time whether we aligned with a wall
    FSM_wallState = followingWallDetect(currentX, currentY, FSM_goState);
  }
  waypoints.add(new PVector(goalX,goalY));
  if(!(goalX == origGoalX && goalY == origGoalY)) waypoints.add(new PVector(origGoalX,origGoalY));
  
  println("\n\nRESULT OF PATHFINDING:");
  for(PVector v : waypoints){
    println("GOTO square with coordinates (x,y) = (" + round(v.x) + "," + round(v.y) + ")");
  }
  println("ALGORITHM DONE!");
}

boolean stillFollowingWall(int current_x, int current_y, int goState, int wallState) {
  int cx = current_x, cy = current_y;

  if (goState == goLeft || goState == goRight) {
    if (wallState == wallRight && isWall(cx, cy-1)) return true;
    if (wallState == wallLeft && isWall(cx, cy+1)) return true;
  } else {
    if (wallState == wallUp && isWall(cx-1, cy)) return true;
    if (wallState == wallDown && isWall(cx+1, cy)) return true;
  }
  return false;
}

int followingWallDetect(int current_x, int current_y, int goState) {
  int cx = current_x, cy = current_y;

  if (goState == goLeft || goState == goRight) {
    if (isWall(cx, cy-1)) return wallRight;
    if (isWall(cx, cy+1)) return wallLeft;
  } else {
    if (isWall(cx-1, cy)) return wallUp;
    if (isWall(cx+1, cy)) return wallDown;
  }
  return noWall;
}

int distGoal10(int x, int  y) {
  return round(10 * sqrt((x - goalX) * (x - goalX) + (y - goalY) * (y - goalY) ));
}

boolean isWall(int x, int y) {
  if (array[x][y] == 1) return true;
  //bugfix #2
  if (22 <= x && x <= 27 && y == 28){
    if(specialCaseApplies(x, y+1, goalX, goalY)) return true;
  }
  return false;
}

//due to the algorithm's constraints (both coding time and RAM space on the microcontroller),
//a specific area has to be treated specially to not confuse the path finder
//-> with more coding time, the algorithm could be improved quite a bit
boolean specialCaseApplies(int X, int Y, int goalX, int goalY){
  if(isInSpecialArea(X,Y) && (!isInSpecialArea(goalX, goalY))) return true;
  return false;
}

boolean isInSpecialArea(int x, int y){
  if((19 <= x && x <= 27) && (29 <= y && y <= 40)) return true;
  return false;
}






void processMouseText() {
  if (0 < mouseX && mouseX < sizeX * size) {
    if (0 < mouseY && mouseY < sizeX * size) {
      fill(255, 255, 0);
      text("[" + round(mouseX / size) + "," + round(mouseY / size) + "]", 10 + mouseX, 10 + mouseY);
    }
  }
}


class Button {
  int X, Y, sizeX, sizeY, ID;
  String label;
  boolean pressed = false;
  private boolean buttonReadyFlag = false;
  Button(int x, int y, int sizex, int sizey, String lab, int id) {
    this.X = x;
    this.Y = y;
    this.sizeX = sizex;
    this.sizeY = sizey;
    this.label = lab;
    this.ID = id;
  }
  void update() {
    if (this.X < mouseX && mouseX < this.X + this.sizeX) {
      if (this.Y < mouseY && mouseY < this.Y + this.sizeY) {
        if (!mousePressed) {
          this.buttonReadyFlag = true;
        } else if (mousePressed && mouseButton == LEFT && this.buttonReadyFlag) {
          this.buttonReadyFlag =false;
          this.pressed = true;
          buttonEventHandler(this.ID);
        } else {
          this.buttonReadyFlag =false;
        }
      }
    }
    if (this.pressed) {
      fill(0, 255, 0);
    } else {
      fill(50);
    }
    rect(this.X, this.Y, this.sizeX, this.sizeY);
    fill(255);
    text(this.label, this.X + 5, this.Y + this.sizeY / 2 - 8);
  }
};

void buttonEventHandler(int id) {
  //nothing here
}
