#include <Wire.h>
#include <PS2X_lib.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
PS2X ps2x;
Adafruit_DCMotor *DCMotor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *DCMotor_2 = AFMS.getMotor(2);
Adafruit_DCMotor *DCMotor_3 = AFMS.getMotor(3);
Adafruit_DCMotor *DCMotor_4 = AFMS.getMotor(4);

void setup()
{
  AFMS.begin(50);
         
  int error = 0;
  do{
    error = ps2x.config_gamepad(13,11,10,12, true, true);
    if(error == 0){
      break;
    }else{
      delay(100);
    }
  }while(1); 
  for(size_t i = 0; i < 50; i++)
  {
        ps2x.read_gamepad(false, 0);
        delay(10);
  }

  pinMode(6, OUTPUT);
}

void loop()
{
  ps2x.read_gamepad(false, 0);
  delay(3);
  digitalWrite(6,LOW);
  if (ps2x.Analog(PSS_LY) < 10) {
    DCMotor_1->setSpeed(255);
    DCMotor_1->run(FORWARD);
    DCMotor_2->setSpeed(255);
    DCMotor_2->run(FORWARD);
    DCMotor_3->setSpeed(255);
    DCMotor_3->run(FORWARD);
    DCMotor_4->setSpeed(255);
    DCMotor_4->run(FORWARD);

  } else if (ps2x.Analog(PSS_LY) > 240) {
    DCMotor_1->setSpeed(255);
    DCMotor_1->run(BACKWARD);
    DCMotor_2->setSpeed(255);
    DCMotor_2->run(BACKWARD);
    DCMotor_3->setSpeed(255);
    DCMotor_3->run(BACKWARD);
    DCMotor_4->setSpeed(255);
    DCMotor_4->run(BACKWARD);
  } else if (ps2x.Analog(PSS_LX) > 240) {
    DCMotor_1->setSpeed(255);
    DCMotor_1->run(BACKWARD);
    DCMotor_2->setSpeed(255);
    DCMotor_2->run(FORWARD);
    DCMotor_3->setSpeed(255);
    DCMotor_3->run(BACKWARD);
    DCMotor_4->setSpeed(255);
    DCMotor_4->run(FORWARD);
  } else if (ps2x.Analog(PSS_LX) < 10) {
    DCMotor_1->setSpeed(255);
    DCMotor_1->run(FORWARD);
    DCMotor_2->setSpeed(255);
    DCMotor_2->run(BACKWARD);
    DCMotor_3->setSpeed(255);
    DCMotor_3->run(FORWARD);
    DCMotor_4->setSpeed(255);
    DCMotor_4->run(BACKWARD);
  } else if (ps2x.Button(PSB_L2)) {
    DCMotor_1->setSpeed(255);
    DCMotor_1->run(BACKWARD);
    DCMotor_2->setSpeed(255);
    DCMotor_2->run(FORWARD);
    DCMotor_3->setSpeed(255);
    DCMotor_3->run(FORWARD);
    DCMotor_4->setSpeed(255);
    DCMotor_4->run(BACKWARD);
  } else if (ps2x.Button(PSB_R2)) {
    DCMotor_1->setSpeed(255);
    DCMotor_1->run(FORWARD);
    DCMotor_2->setSpeed(255);
    DCMotor_2->run(BACKWARD);
    DCMotor_3->setSpeed(255);
    DCMotor_3->run(BACKWARD);
    DCMotor_4->setSpeed(255);
    DCMotor_4->run(FORWARD);
  } else if (ps2x.Button(PSB_CROSS)) {
    ps2x.read_gamepad(true, 200);
    delay(300);
    ps2x.read_gamepad(false, 0);
    digitalWrite(6,HIGH);
    delay(300);
    digitalWrite(6,LOW);
  } else {
    DCMotor_1->setSpeed(0);
    DCMotor_1->run(RELEASE);
    DCMotor_2->setSpeed(0);
    DCMotor_2->run(RELEASE);
    DCMotor_3->setSpeed(0);
    DCMotor_3->run(RELEASE);
    DCMotor_4->setSpeed(0);
    DCMotor_4->run(RELEASE);

  }
  delay(10);

}