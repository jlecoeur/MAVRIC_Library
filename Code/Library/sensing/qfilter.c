/** 
 * \page The MAV'RIC license
 *
 * The MAV'RIC Framework
 *
 * Copyright © 2011-2014
 *
 * Laboratory of Intelligent Systems, EPFL
 */
 
 
/**
 * \file qfilter.c
 *
 * This file implements a complementary filter for the attitude estimation
 */


#include "qfilter.h"
#include "conf_platform.h"
#include "coord_conventions.h"
#include "print_util.h"
 #include "time_keeper.h"
#include <math.h>
#include "maths.h"


float front_mag_vect_z;

uint8_t counter = 0;

void qfilter_init(qfilter_t* qf, Imu_Data_t* imu, AHRS_t* attitude_estimation) 
{
	uint8_t i;
	
	qf->imu = imu;
	qf->attitude_estimation = attitude_estimation;
	
	qf->imu->calibration_level = LEVELING;
	
	for (i = 0; i < 3; i++)
	{
		qf->attitude_estimation->linear_acc[i] = 0.0f;
	}

	qf->attitude_estimation->qe.s = 1.0f;
	qf->attitude_estimation->qe.v[0] = 0.0f;
	qf->attitude_estimation->qe.v[1] = 0.0f;
	qf->attitude_estimation->qe.v[2] = 0.0f;
	
	qf->kp = 0.07f;
	qf->ki = qf->kp / 15.0f;
	
	qf->kp_mag = 0.1f;
	qf->ki_mag = qf->kp_mag / 15.0f;
	
	print_util_dbg_print("qfilter initialized.\n");
}


void qfilter_update(qfilter_t *qf)
{
	uint8_t i;
	float  omc[3], omc_mag[3] , tmp[3], snorm, norm, s_acc_norm, acc_norm, s_mag_norm, mag_norm;
	UQuat_t qed, qtmp1, up, up_bf;
	UQuat_t mag_global, mag_corrected_local;
	UQuat_t front_vec_global = 
	{
		.s = 0.0f, 
		.v = {1.0f, 0.0f, 0.0f}
	};

	float kp, kp_mag;

	// Update time
	uint32_t t = time_keeper_get_time_ticks();
	float dt = time_keeper_ticks_to_seconds(t - qf->attitude_estimation->last_update);
	qf->attitude_estimation->dt = dt;
	qf->attitude_estimation->last_update = t;

	// up_bf = qe^-1 *(0,0,0,-1) * qe
	up.s = 0; up.v[0] = UPVECTOR_X; up.v[1] = UPVECTOR_Y; up.v[2] = UPVECTOR_Z;
	up_bf = quaternions_global_to_local(qf->attitude_estimation->qe, up);
	
	// calculate norm of acceleration vector
	s_acc_norm = qf->imu->scaled_accelero.data[0] * qf->imu->scaled_accelero.data[0] + qf->imu->scaled_accelero.data[1] * qf->imu->scaled_accelero.data[1] + qf->imu->scaled_accelero.data[2] * qf->imu->scaled_accelero.data[2];
	if ( (s_acc_norm > 0.7f * 0.7f) && (s_acc_norm < 1.3f * 1.3f) ) 
	{
		// approximate square root by running 2 iterations of newton method
		acc_norm = maths_fast_sqrt(s_acc_norm);

		tmp[0] = qf->imu->scaled_accelero.data[0] / acc_norm;
		tmp[1] = qf->imu->scaled_accelero.data[1] / acc_norm;
		tmp[2] = qf->imu->scaled_accelero.data[2] / acc_norm;

		// omc = a x up_bf.v
		CROSS(tmp, up_bf.v, omc);
	}
	else
	{
		omc[0] = 0;
		omc[1] = 0;
		omc[2] = 0;
	}

	// Heading computation
	// transfer 
	qtmp1 = quaternions_create_from_vector(qf->imu->scaled_compass.data); 
	mag_global = quaternions_local_to_global(qf->attitude_estimation->qe, qtmp1);
	
	// calculate norm of compass vector
	//s_mag_norm = SQR(mag_global.v[0]) + SQR(mag_global.v[1]) + SQR(mag_global.v[2]);
	s_mag_norm = SQR(mag_global.v[0]) + SQR(mag_global.v[1]);

	if ( (s_mag_norm > 0.004f * 0.004f) && (s_mag_norm < 1.8f * 1.8f) ) 
	{
		mag_norm = maths_fast_sqrt(s_mag_norm);

		mag_global.v[0] /= mag_norm;
		mag_global.v[1] /= mag_norm;
		mag_global.v[2] = 0.0f;   // set z component in global frame to 0

		// transfer magneto vector back to body frame 
		qf->attitude_estimation->north_vec = quaternions_global_to_local(qf->attitude_estimation->qe, front_vec_global);		
		
		mag_corrected_local = quaternions_global_to_local(qf->attitude_estimation->qe, mag_global);		
		
		// omc = a x up_bf.v
		CROSS(mag_corrected_local.v, qf->attitude_estimation->north_vec.v,  omc_mag);
		
	}
	else
	{
		omc_mag[0] = 0;
		omc_mag[1] = 0;
		omc_mag[2] = 0;
	}


	// get error correction gains depending on mode
	switch (qf->imu->calibration_level)
	{
		case OFF:
			kp = qf->kp;//*(0.1f / (0.1f + s_acc_norm - 1.0f));
			kp_mag = qf->kp_mag;
			qf->ki = qf->kp / 15.0f;
			break;
			
		case LEVELING:
			kp = 0.5f;
			kp_mag = 0.5f;
			qf->ki = qf->kp / 10.0f;
			break;
			
		case LEVEL_PLUS_ACCEL:  // experimental - do not use
			kp = 0.3f;
			qf->ki = qf->kp / 10.0f;
			qf->imu->calib_accelero.bias[0] += dt * qf->kp * (qf->imu->scaled_accelero.data[0] - up_bf.v[0]);
			qf->imu->calib_accelero.bias[1] += dt * qf->kp * (qf->imu->scaled_accelero.data[1] - up_bf.v[1]);
			qf->imu->calib_accelero.bias[2] += dt * qf->kp * (qf->imu->scaled_accelero.data[2] - up_bf.v[2]);
			kp_mag = qf->kp_mag;
			break;
			
		default:
			kp = qf->kp;
			kp_mag = qf->kp_mag;
			qf->ki = qf->kp / 15.0f;
			break;
	}

	// apply error correction with appropriate gains for accelerometer and compass

	for (i = 0; i < 3; i++)
	{
		qtmp1.v[i] = (qf->imu->scaled_gyro.data[i] + kp * omc[i] + kp_mag * omc_mag[i]);//0.5f*
	}
	qtmp1.s = 0;

	// apply step rotation with corrections
	qed = quaternions_multiply(qf->attitude_estimation->qe,qtmp1);

	qf->attitude_estimation->qe.s = qf->attitude_estimation->qe.s + qed.s * dt;
	qf->attitude_estimation->qe.v[0] += qed.v[0] * dt;
	qf->attitude_estimation->qe.v[1] += qed.v[1] * dt;
	qf->attitude_estimation->qe.v[2] += qed.v[2] * dt;

/*
	float wx = qf->imu->scaled_gyro.data[X] + kp * omc[X] + kp_mag * omc_mag[X];
	float wy = qf->imu->scaled_gyro.data[Y] + kp * omc[Y] + kp_mag * omc_mag[Y];
	float wz = qf->imu->scaled_gyro.data[Z] + kp * omc[Z] + kp_mag * omc_mag[Z];

	float q0 = qf->attitude_estimation->qe.s;
	float q1 = qf->attitude_estimation->qe.v[0];
	float q2 = qf->attitude_estimation->qe.v[1];
	float q3 = qf->attitude_estimation->qe.v[2];

	qf->attitude_estimation->qe.s = q0 + dt / 2 * ( - q1 * wx - q2 * wy - q3 * wz);
	qf->attitude_estimation->qe.v[0] = q1 + dt / 2 * ( q0 * wx - q3 * wy + q2 * wz);
	qf->attitude_estimation->qe.v[1] = q2 + dt / 2 * ( q3 * wx + q0 * wy - q1 * wz);
	qf->attitude_estimation->qe.v[2] = q3 + dt / 2 * ( - q2 * wx + q1 * wy + q0 * wz);
*/

	snorm = qf->attitude_estimation->qe.s * qf->attitude_estimation->qe.s + qf->attitude_estimation->qe.v[0] * qf->attitude_estimation->qe.v[0] + qf->attitude_estimation->qe.v[1] * qf->attitude_estimation->qe.v[1] + qf->attitude_estimation->qe.v[2] * qf->attitude_estimation->qe.v[2];
	if (snorm < 0.0001f)
	{
		norm = 0.01f;
	}
	else
	{
		// approximate square root by running 2 iterations of newton method
		norm = maths_fast_sqrt(snorm);
	}
	qf->attitude_estimation->qe.s /= norm;
	qf->attitude_estimation->qe.v[0] /= norm;
	qf->attitude_estimation->qe.v[1] /= norm;
	qf->attitude_estimation->qe.v[2] /= norm;

	// bias estimate update
	qf->imu->calib_gyro.bias[0] += - dt * qf->ki * omc[0] / qf->imu->calib_gyro.scale_factor[0];
	qf->imu->calib_gyro.bias[1] += - dt * qf->ki * omc[1] / qf->imu->calib_gyro.scale_factor[1];
	qf->imu->calib_gyro.bias[2] += - dt * qf->ki * omc[2] / qf->imu->calib_gyro.scale_factor[2];

	// bias estimate update
	//qf->imu->calib_sensor.bias[6] += - dt * qf->ki_mag * omc[0];
	//qf->imu->calib_sensor.bias[7] += - dt * qf->ki_mag * omc[1];
	//qf->imu->calib_sensor.bias[8] += - dt * qf->ki_mag * omc[2];

	// set up-vector (bodyframe) in attitude
	qf->attitude_estimation->up_vec.v[0] = up_bf.v[0];
	qf->attitude_estimation->up_vec.v[1] = up_bf.v[1];
	qf->attitude_estimation->up_vec.v[2] = up_bf.v[2];
	
	//update angular_speed.
	qf->attitude_estimation->angular_speed[X] = qf->imu->scaled_gyro.data[X];
	qf->attitude_estimation->angular_speed[Y] = qf->imu->scaled_gyro.data[Y];
	qf->attitude_estimation->angular_speed[Z] = qf->imu->scaled_gyro.data[Z];
}
