#ifndef hitecServo
#define hitecServo

#define HITECSERVO_TIME_PERIOD 20000
#define SERVO_ZERO_PULSE 550
#define SERVO_MAX_PULSE 2600

void hitecServoInit(void);
void setHitecServoAngle(float);

#endif
