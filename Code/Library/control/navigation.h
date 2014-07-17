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
 * \file navigation.h
 * 
 * Waypoint navigation controller 
 */


#ifndef NAVIGATION_H_
#define NAVIGATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mavlink_waypoint_handler.h"
#include "stabilisation.h"
#include "quaternions.h"
#include "mavlink_waypoint_handler.h"
#include "position_estimation.h"
#include "orca.h"
#include <stdbool.h>

/**
 * \brief The navigation structure
 */
typedef struct
{
	float dist2vel_gain;								///< The gain linking the distance to the goal to the actual speed
	float cruise_speed;									///< The cruise speed in m/s
	float max_climb_rate;								///< Max climb rate in m/s
	float softZoneSize;									///< Soft zone of the velocity controller
	
	uint8_t loopCount;									///< A counter for sending mavlink messages at a lower rate than the function
	
	
	Control_Command_t *controls_nav;					///< The pointer to the navigation control structure
	UQuat_t *qe;										///< The pointer to the attitude quaternion structure
	mavlink_waypoint_handler_t *waypoint_handler;		///< The pointer to the waypoint handler structure
	position_estimator_t *position_estimator;			///< The pointer to the position estimation structure in centralData
	orca_t *orcaData;									///< The pointer to the ORCA structure in centralData
}navigation_t;

/**
 * \brief						Initialization
 *
 * \param	navigationData		The pointer to the navigation structure in centralData
 * \param	controls_nav		The pointer to the control structure in centralData
 * \param	qe					The pointer to the attitude quaternion structure in centralData
 * \param	waypoint_handler	The pointer to the waypoint handler structure in centralData
 * \param	position_estimator	The pointer to the position estimation structure in centralData
 * \param	orcaData			The pointer to the ORCA structure in centralData
 */
void navigation_init(navigation_t* navigationData, Control_Command_t* controls_nav, UQuat_t* qe, mavlink_waypoint_handler_t* waypoint_handler, position_estimator_t* position_estimator, orca_t* orcaData);


/**
 * \brief						Navigates the robot towards waypoint waypoint_input in 3D velocity command mode
 *
 * \param	waypoint_input		Destination waypoint in local coordinate system
 * \param	navigationData		The navigation structure
 */
void navigation_run(local_coordinates_t waypoint_input, navigation_t* navigationData);


#ifdef __cplusplus
}
#endif

#endif // NAVIGATION_H_