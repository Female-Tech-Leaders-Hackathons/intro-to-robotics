int navigationState = 0;

void resetNavigationState() {
  navigationState = 0;
}

void handleNavigation() {
  if (reachedTarget()) {
    if (currentGoal == LennartGoal && goal == TillGoal) {
      switch (navigationState) {
        case 0:
          driveXmm(4000);
          navigationState++;
          break;
        case 1:
          turnLeft();
          navigationState++;
          break;
        case 2:
          driveXmm(2800);
          navigationState++;
          break;
        case 3:
          turnLeft();
          navigationState++;
          break;
        case 4:
          driveXmm(4000);
          navigationState++;
          break;
        case 5:
          turnRight();
          navigationState++;
          break;
        case 6:
          driveXmm(1000);
          navigationState++;
          break;
        case 7:
          turnXDegrees(180);
          navigationState++;
        case 8: //finished
          currentGoal = goal;
          break;
      }
    }

    if (currentGoal == TillGoal && goal == LennartGoal) {
      switch (navigationState) {
        case 0:
          driveXmm(1000);
          navigationState++;
          break;
        case 1:
          turnLeft();
          navigationState++;
          break;
        case 2:
          driveXmm(4000);
          navigationState++;
          break;
        case 3:
          turnRight();
          navigationState++;
          break;
        case 4:
          driveXmm(2800);
          navigationState++;
          break;
        case 5:
          turnRight();
          navigationState++;
          break;
        case 6:
          driveXmm(4000);
          navigationState++;
          break;
        case 7:
          turnXDegrees(180);
          navigationState++;
        case 8: //finished
          currentGoal = goal;
          break;
      }
    }

    if (currentGoal == LennartGoal && goal == KitchenGoal) {
      switch (navigationState) {
        case 0:
          driveXmm(4000);
          navigationState++;
          break;
        case 1:
          turnLeft();
          navigationState++;
          break;
        case 2:
          driveXmm(3800);
          navigationState++;
          break;
        case 3:
          turnRight();
          navigationState++;
          break;
        case 4:
          driveXmm(1600);
          navigationState++;
          break;
        case 5:
          turnXDegrees(180);
          navigationState++;
          break;
        case 6:
          currentGoal = goal;
      }
    }

    if (currentGoal == KitchenGoal && goal == LennartGoal) {
      switch (navigationState) {
        case 0:
          driveXmm(1600);
          navigationState++;
          break;
        case 1:
          turnLeft();
          navigationState++;
          break;
        case 2:
          driveXmm(3800);
          navigationState++;
          break;
        case 3:
          turnRight();
          navigationState++;
          break;
        case 4:
          driveXmm(4000);
          navigationState++;
          break;
        case 5:
          turnXDegrees(180);
          navigationState++;
          break;
        case 6:
          currentGoal = goal;
      }
    }

    if (currentGoal == KitchenGoal && goal == TillGoal) {
      switch (navigationState) {
        case 0:
          driveXmm(1600);
          navigationState++;
          break;
        case 1:
          turnLeft();
          navigationState++;
          break;
        case 2:
          driveXmm(1000);
          navigationState++;
          break;
        case 3:
          turnRight();
          navigationState++;
          break;
        case 4:
          driveXmm(4000);
          navigationState++;
          break;
        case 5:
          turnRight();
          navigationState++;
        case 6:
          driveXmm(1000);
          navigationState++;
        case 7:
          turnXDegrees(180);
          navigationState++;
          break;
        case 8:
          currentGoal = goal;
      }

      if (currentGoal == TillGoal && goal == KitchenGoal) {
        switch (navigationState) {
          case 0:
            driveXmm(1000);
            navigationState++;
            break;
          case 1:
            turnLeft();
            navigationState++;
            break;
          case 2:
            driveXmm(4000);
            navigationState++;
            break;
          case 3:
            turnRight();
            navigationState++;
            break;
          case 4:
            driveXmm(1000);
            navigationState++;
            break;
          case 5:
            turnRight();
            navigationState++;
          case 6:
            driveXmm(1600);
            navigationState++;
          case 7:
            turnXDegrees(180);
            navigationState++;
            break;
          case 8:
            currentGoal = goal;
        }
      }

      if (goal == currentGoal) {
        robotState = doNothingState;
      }
    }
  }
}
