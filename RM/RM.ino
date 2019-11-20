#include <Wire.h>
#include <PS2X_lib.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>

/*
PS2 gamepad mapping
    PSS_LX, PSS_LY: movement
    PSB_L2, PSB_R2: rotate
    PSB_CROSS: shoot
*/

#define ENABLE_DEBUG 0

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
PS2X ps2x;
Adafruit_DCMotor *DCMotor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *DCMotor_2 = AFMS.getMotor(2);
Adafruit_DCMotor *DCMotor_3 = AFMS.getMotor(3);
Adafruit_DCMotor *DCMotor_4 = AFMS.getMotor(4);
bool is_shooting = false;

static inline int _max4(int a, int b, int c, int d)
{
    if (a > b && a > c && a > d) {
        return a;
    } else if (b > c && b > d) {
        return b;
    } else if (c > d) {
        return c;
    } else {
        return d;
    }
}

static inline int _min4(int a, int b, int c, int d)
{
    if (a < b && a < c && a < d) {
        return a;
    } else if (b < c && b < d) {
        return b;
    } else if (c < d) {
        return c;
    } else {
        return d;
    }
}

void setup()
{
#if ENABLE_DEBUG
    Serial.begin(57600);
#endif

    AFMS.begin(50);

    int error = 0;
    do {
        error = ps2x.config_gamepad(13, 11, 10, 12, true, true);
        if (error == 0) {
            break;
        } else {
            delay(100);
        }
    } while (1);
    for (size_t i = 0; i < 50; i++) {
        ps2x.read_gamepad(false, 0);
        delay(10);
    }

    pinMode(6, OUTPUT);
}

inline void _moto_set(Adafruit_DCMotor *DCMotor, int speed)
{
    if (speed > 20) {
        DCMotor->setSpeed(speed);
        DCMotor->run(FORWARD);
    } else if (speed < -20) {
        DCMotor->setSpeed(-speed);
        DCMotor->run(BACKWARD);
    } else {
        DCMotor->setSpeed(0);
        DCMotor->run(RELEASE);
    }

}

static int chassis_task(void *arg)
{
    /*
                   0
                   |
                   |
                   |
                   |
                127|    PSS_LX
    0-----------------------------> 255
                   |
                   |
                   |PSS_LY
                   |
                   v
                  255
    */
    int vector_x = map(ps2x.Analog(PSS_LX), 0, 255, -255, 255);
    int vector_y = map(ps2x.Analog(PSS_LY), 0, 255, -255, 255);
    int moto_speed[4];

    moto_speed[0] = 0;
    moto_speed[1] = 0;
    moto_speed[2] = 0;
    moto_speed[3] = 0;

    // pss_ly
    moto_speed[0] -= vector_y;
    moto_speed[1] -= vector_y;
    moto_speed[2] -= vector_y;
    moto_speed[3] -= vector_y;

    // pss_lx
    moto_speed[0] -= vector_x;
    moto_speed[1] += vector_x;
    moto_speed[2] -= vector_x;
    moto_speed[3] += vector_x;

    if (ps2x.Button(PSB_L2)) {
        moto_speed[0] -= 255;
        moto_speed[1] += 255;
        moto_speed[2] += 255;
        moto_speed[3] -= 255;
    }
    if (ps2x.Button(PSB_R2)) {
        moto_speed[0] += 255;
        moto_speed[1] -= 255;
        moto_speed[2] -= 255;
        moto_speed[3] += 255;
    }

    // scale and restrict the max and min to -255 ~ 255
    int max_speed = _max4(moto_speed[0], moto_speed[1], moto_speed[2], moto_speed[3]);
    int min_speed = _min4(moto_speed[0], moto_speed[1], moto_speed[2], moto_speed[3]);
    if (max_speed > 255) {
        if (min_speed < -255) {
            // scale both
            moto_speed[0] = map(moto_speed[0], min_speed, max_speed, -255, 255);
            moto_speed[1] = map(moto_speed[1], min_speed, max_speed, -255, 255);
            moto_speed[2] = map(moto_speed[2], min_speed, max_speed, -255, 255);
            moto_speed[3] = map(moto_speed[3], min_speed, max_speed, -255, 255);
        } else {
            // scale the upper bound
            moto_speed[0] = map(moto_speed[0], min_speed, max_speed, min_speed, 255);
            moto_speed[1] = map(moto_speed[1], min_speed, max_speed, min_speed, 255);
            moto_speed[2] = map(moto_speed[2], min_speed, max_speed, min_speed, 255);
            moto_speed[3] = map(moto_speed[3], min_speed, max_speed, min_speed, 255);
        }
    } else if (min_speed < -255) {
        // scale the lower bound
        moto_speed[0] = map(moto_speed[0], min_speed, max_speed, -255, max_speed);
        moto_speed[1] = map(moto_speed[1], min_speed, max_speed, -255, max_speed);
        moto_speed[2] = map(moto_speed[2], min_speed, max_speed, -255, max_speed);
        moto_speed[3] = map(moto_speed[3], min_speed, max_speed, -255, max_speed);
    }

    _moto_set(DCMotor_1, moto_speed[0]);
    _moto_set(DCMotor_2, moto_speed[1]);
    _moto_set(DCMotor_3, moto_speed[2]);
    _moto_set(DCMotor_4, moto_speed[3]);

#if ENABLE_DEBUG
    Serial.print("vector_x(");
    Serial.print(vector_x);
    Serial.print(") ");
    Serial.print("vector_y(");
    Serial.print(vector_y);
    Serial.print(") ");
    Serial.print("max_speed(");
    Serial.print(max_speed);
    Serial.print(") ");
    Serial.print("min_speed(");
    Serial.print(min_speed);
    Serial.print(") ");
    Serial.print("moto_speed(");
    Serial.print(moto_speed[0]);
    Serial.print(", ");
    Serial.print(moto_speed[1]);
    Serial.print(", ");
    Serial.print(moto_speed[2]);
    Serial.print(", ");
    Serial.print(moto_speed[3]);
    Serial.print(") ");
    Serial.println("");
#endif

    return 0;
}

static int shoot_task(void *arg)
{
    static unsigned long last_shoot_time_ms;
    unsigned long curr_time_ms = millis();
    bool is_btn_press = ps2x.Button(PSB_CROSS);

    if (is_btn_press) {
        is_shooting = true;
        digitalWrite(6, HIGH);
        last_shoot_time_ms = curr_time_ms;
    } else {
        if (curr_time_ms - last_shoot_time_ms > 150) {
            digitalWrite(6, LOW);
            is_shooting = false;
        }
    }

    return 0;
}

void loop()
{
    if (is_shooting) {
        ps2x.read_gamepad(true, 200);
    } else {
        ps2x.read_gamepad(false, 0);
    }

    chassis_task(NULL);
    shoot_task(NULL);

    delay(1);
}
