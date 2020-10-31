#define SPEED 35
#define GRIDSIZE 200

#define COUNTSPERMM 1000/610.0
//#define COUNTSPERDEGRE 183/90.0
#define COUNTSPERDEGRE 186/90.0

#define MINSPEED 8
#define DAMPENING 3.0

int targetL = 0;
int targetR = 0;

int32_t countL = 0;
int32_t countR = 0;

int speedL = 0 ;
int speedR = 0 ;

boolean motorLfinished = false;
boolean motorRfinished = false;

void turnLeft() {
  turnXDegrees(-90);
}

void turnRight() {
  turnXDegrees(90);
}

void driveForward() {
  driveXmm(GRIDSIZE);
}

void breakMotors() {
  targetL = countL;
  targetR = countR;
}

//positive to the right negative to the left
void turnXDegrees(int deg) {
  targetL += deg * COUNTSPERDEGRE;
  targetR  -= deg * COUNTSPERDEGRE;
}

//positive forward negativ backwards
void driveXmm(int mmToDrive) {
  targetL += mmToDrive * COUNTSPERMM;
  targetR += mmToDrive * COUNTSPERMM;
}

void handleMotors() {
  driveToCounter(targetL, targetR, SPEED);
}

boolean reachedTarget() {
  return motorLfinished && motorRfinished;
}

void driveToCounter(int32_t targetCountL, int32_t targetCountR, int speed) {

  countL = getEncoderLeftCnt();
  countR = getEncoderRightCnt();
  /*
    Serial.print(countL);
    Serial.print(",");
    Serial.print(countR);
    Serial.println();
  */
  setMotorDirections(targetCountL, targetCountR);

  int countDif = abs(countL - targetCountL) - abs(countR - targetCountR );
  int speedDif = countDif;

  speedL = constrain(speed + speedDif, 0, 100);
  speedR = constrain(speed - speedDif, 0, 100);

  speedL = constrain(speedL * abs(targetCountL - countL) / (DAMPENING * speed), MINSPEED, speedL);
  speedR = constrain(speedR * abs(targetCountR - countR) / (DAMPENING * speed), MINSPEED, speedR);

  setMotorSpeed(LEFT_MOTOR, speedL);
  setMotorSpeed(RIGHT_MOTOR, speedR);
  /*
    Serial.print(speedL);
    Serial.print(",");
    Serial.print(speedR);
    Serial.println();
  */
  if (abs(countL - targetCountL) < 5) {
    pauseMotor(LEFT_MOTOR);
    motorLfinished = true;
  }
  else {
    enableMotor(LEFT_MOTOR);
    motorLfinished = false;
  }

  if (abs(countR - targetCountR) < 5) {
    pauseMotor(RIGHT_MOTOR);
    motorRfinished = true;
  }
  else {
    enableMotor(RIGHT_MOTOR);
    motorRfinished = false;
  }
}

void setMotorDirections(int32_t targetCountL, int32_t targetCountR) {
  if (targetCountL > countL) { //set direction of left motor
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
  }
  else {
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
  }

  if (targetCountR > countR) { //set direction of right motor
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
  }
  else {
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
  }
}
