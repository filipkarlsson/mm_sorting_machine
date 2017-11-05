int wheelSteps;
int armPos;
int armNewPos;
int armSteps;

int color[2];

void setup() {
  wheelSteps = 0;
  armPos = 0;
  armNewPos = 0;
  armSteps = 0;

  //Do setup stuff
}

void loop() {

  if ((wheelSteps == 0) && (armPos == armNewPos)) {
    //Get candy color
    if (false) { //check candy color, if white continue to next candy, if wheel havn´t rotated do a wiggle,
      ;
    }

    else {
      int c = 1; //the new color


      color[1] = color[0];  //updating color
      color[0] = c;

      armNewPos = color[1] * 360 / 5; //if there are 5 bowls
      armSteps = (armNewPos - armPos) * 4096 / 360; //How many steps the motor have to take

      wheelSteps += 4096 / (2 * 8);
    }
  }

  else if (armSteps == 0) {
    armPos = armNewPos;
    wheelSteps += 4096 / (2 * 8);
  }
}
