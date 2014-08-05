/**
 * \page The MAV'RIC License
 *
 * The MAV'RIC Framework
 *
 * Copyright © 2011-2014
 *
 * Laboratory of Intelligent Systems, EPFL
 */


/**
 * \file servos.c
 *
 * Abstraction layer for servomotors.
 * This module does not write to hardware, it just holds data and configuration for servos.
 */


#include "servos.h"
#include "print_util.h"


void servos_init(servos_t* servos, const servos_conf_t* config)
{
	if ( config->servos_count <= MAX_SERVO_COUNT )
	{
		servos->servos_count = config->servos_count;
	
		for (int i = 0; i < servos->servos_count; ++i)
		{
			// Set default parameters for each type of servo
			switch ( config->types[i] )
			{
				case STANDARD_SERVO:
					servos->servo[i].trim          = 0.0f;
					servos->servo[i].min           = -1.0f;
					servos->servo[i].max           = 1.0f;
					servos->servo[i].failsafe      = 0.0f;
					servos->servo[i].pwm_neutral   = 1000;
					servos->servo[i].pwm_amplitude = 500;
					servos->servo[i].type 		   = STANDARD_SERVO;
					break;

				case MOTOR_CONTROLLER:
					servos->servo[i].trim          = 0.0f;
					servos->servo[i].min           = -0.9f;
					servos->servo[i].max           = 1.0f;
					servos->servo[i].failsafe      = -1.0f;
					servos->servo[i].pwm_neutral   = 1000;
					servos->servo[i].pwm_amplitude = 500;
					servos->servo[i].type 		   = MOTOR_CONTROLLER;
					break;

				case CUSTOM_SERVO:
					servos->servo[i].trim          = 0.0f;
					servos->servo[i].min           = -0.0f;
					servos->servo[i].max           = 0.0f;
					servos->servo[i].failsafe      = 0.0f;
					servos->servo[i].pwm_neutral   = 1000;
					servos->servo[i].pwm_amplitude = 0;
					servos->servo[i].type 		   = CUSTOM_SERVO;

					break;
			}

			// Set default value to failsafe
			servos->servo[i].value = servos->servo[i].failsafe;
		}
	}
	else
	{
		servos->servos_count = 0;
		print_util_dbg_print("[SERVOS] ERROR! Too many servos");
	}
}


void servos_set_value(servos_t* servos, uint32_t servo_id, float value)
{
	if ( servo_id <= servos->servos_count )
	{
		if ( value < servos->servo[servo_id].min )
		{
			servos->servo[servo_id].value = servos->servo[servo_id].min;
		}
		else if ( value > servos->servo[servo_id].max )
		{
			servos->servo[servo_id].value = servos->servo[servo_id].max;
		}
		else
		{
			servos->servo[servo_id].value = value;
		}
	}
}


void servos_failsafe(servos_t* servos)
{
	for (int i = 0; i < servos->servos_count; ++i)
	{
		servos->servo[i].value = servos->servo[i].failsafe;
	}
}