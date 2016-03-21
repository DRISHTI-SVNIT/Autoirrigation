#include "hitecServo.h"
#include "asf.h"

volatile float angle2pwm_factor = (SERVO_MAX_PULSE-SERVO_ZERO_PULSE)/180;
struct tcc_module tcc_instance;

void hitecServoInit(void)
{
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC0);
	config_tcc.counter.clock_source = GCLK_GENERATOR_3;
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1;
	config_tcc.counter.period = HITECSERVO_TIME_PERIOD;
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	config_tcc.pins.enable_wave_out_pin[6] = true;
	config_tcc.pins.wave_out_pin[6] = PIN_PB12F_TCC0_WO6 ;
	config_tcc.pins.wave_out_pin_mux[6] = MUX_PB12F_TCC0_WO6;
	tcc_init(&tcc_instance, TCC0, &config_tcc);
	tcc_enable(&tcc_instance);
}

void setHitecServoAngle(float angle)
{
	uint32_t angle_ = (angle*angle2pwm_factor)+SERVO_ZERO_PULSE;
	printf("%d",(int)(angle_*1000));	
	tcc_set_compare_value(&tcc_instance,2,angle_);
}