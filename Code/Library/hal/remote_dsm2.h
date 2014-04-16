/*
 * remote_dsm2.h
 *
 *  Created on: Mar 2, 2010
 *      Author: felix
 */

#ifndef REMOTE_DSM2_
#define REMOTE_DSM2_
#include "compiler.h"
#include "buffer.h"
#include "stabilisation.h"

#define REMOTE_UART AVR32_USART1

#define DSM_RECEIVER_PIN AVR32_PIN_PD12

#define RECEIVER_POWER_ENABLE_PIN AVR32_PIN_PC01
//#define SPEKTRUM_10BIT

typedef struct Spektrum_Receiver {
	Buffer_t receiver;
	 int16_t channels[16];
	uint32_t last_update;
	uint8_t valid;
	uint32_t last_time;
	uint32_t duration;
} Spektrum_Receiver_t;

void rc_switch_power(bool on);

// function to set slave receiver into bind mode. has to be called 100ms after power-up
void rc_activate_bind_mode();

// initialise UART receiver for Spektrum/DSM2 slave receivers
void    rc_init (void);

int16_t rc_get_channel(uint8_t index);
void    rc_center_channel(uint8_t index);
int16_t rc_get_channel_neutral(uint8_t index);
int8_t  rc_check_receivers(void);


/*Control_Command_t get_command_from_spektrum();
float get_roll_from_spektrum();
float get_pitch_from_spektrum();
float get_yaw_from_spektrum();
float get_thrust_from_spektrum();

void get_channel_mode_spektrum(uint8_t *chanSwitch);
*/

#endif /* REMOTE_DSM2_ */
