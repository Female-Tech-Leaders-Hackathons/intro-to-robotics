int size = 13;
int sizeX = 60, sizeY = 60;  
int centimetersX = 1200, centimetersY = 1200;
char[][] array = new char[sizeX][sizeY];

int numButtons = 4;  //nur die Mode Buttons, nicht der Export Button
Button[] buttons = new Button[numButtons];
Button importButton, exportButton;

void setup() {
  for (int i=0; i < sizeX; i++) {
    for (int j=0; j < sizeY; j++) {
      array[i][j] = 0;
    }
  }

  for (int i=0; i < numButtons; i++) {
    buttons[i] = new Button(805, 5 + 100*i, 90, 90, "", i /*ID*/);
  }
  buttons[0].label = "Freiraum";
  buttons[1].label = "Hindernis";
  buttons[2].label = "RoboterStart";
  buttons[3].label = "RoboterZiel";
  //buttons[4].label = "RoboterZiel 2";

  importButton = new Button(805, 605, 90, 90, "Import", 98);
  exportButton = new Button(805, 705, 90, 90, "Export", 99);

  size(901, 801, P2D);
  background(0);
}

void draw() {
  drawRects();
  processButtons();
  processImportButton();
  processExportButton();
  processDrawing();
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
      }
      rect(size*i, j*size, size, size);
    }
  }
}

char activeMode = 0;
void processButtons() {
  for (int i=0; i < numButtons; i++) {
    buttons[i].update();
    if (buttons[i].pressed) activeMode = (char)i;
  }
}

void processDrawing() {
  if (0 < mouseX && mouseX < sizeX * size) {
    if (0 < mouseY && mouseY < sizeX * size) {
      if (mousePressed && mouseButton == LEFT) {
        int i = floor(mouseX / size);
        int j = floor(mouseY / size);
        if (activeMode > 1) { //wenn der Mode ein Start oder Zielpunkt ist, lösche bitte den alten Punkt
          drawEventHandler(i, j, activeMode);
        }
        array[i][j] = activeMode;
      }
    }
  }
}

void processExportButton() {
  exportButton.update();
  if (exportButton.pressed) {
    exportButton.pressed = false;
    selectOutput("Select a file to write to:", "outputFileSelected");
  }
}
boolean CPP = true;
void outputFileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } else {
    println("User selected " + selection.getAbsolutePath());
    PrintWriter output = createWriter(selection.getAbsolutePath() /* + ".h"*/);
    
      if (CPP) { //C HEADER FILE EXPORT
      output.println("char obstaclePlan[" + sizeX + "][" + sizeY + "] = {");
      String s;
      for (int i=0; i < sizeY; i++) {
        s = "";
        for (int j=0; j < sizeX; j++) {
          s += str(int(array[j][i]));
          if (j + 1 < sizeY) s += " ,";
        }
        if (i + 1 < sizeX) s += " ,";
        output.println(s);
      }
      output.println("};");
    } else { //JAVASCRIPT EXPORT
      String s;
      for (int i=0; i < sizeY; i++) {
        s = "";
        for (int j=0; j < sizeX; j++) {
          if(j == 0) s += "[";
          s += array[j][i] == 0 ? "1" : "0";
          if (j + 1 < sizeY) s += " ,";
        }
        if (i + 1 < sizeX) s += " ],";
        else s += " ]";
        output.println(s);
      }
      output.println("};");
    }
    output.close();
  }
}

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

void processMouseText() {
  if (0 < mouseX && mouseX < sizeX * size) {
    if (0 < mouseY && mouseY < sizeX * size) {
      fill(255, 255, 0);
      text( round(mouseX * float(centimetersX)/float(size * sizeX)) + "cm, " + round(mouseY * float(centimetersY)/float(size * sizeY)) + "cm", 10 + mouseX, 10 + mouseY);
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

void buttonEventHandler(int invokerButton) {
  for (int i=0; i < numButtons; i++) {
    if (!(i == invokerButton)) buttons[i].pressed = false;
  }
}

void drawEventHandler(int locX, int locY, int mode) {
  for (int i=0; i < sizeX; i++) {
    for (int j=0; j < sizeY; j++) {
      if (array[i][j] == mode && (!(i == locX)) && (!(j == locY))) array[i][j] = 0;
    }
  }
}
