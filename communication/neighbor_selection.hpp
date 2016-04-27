/*******************************************************************************
 * Copyright (c) 2009-2015, MAV'RIC Development Team
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/
 
/*******************************************************************************
 * \file neighbor_selection.hpp
 * 
 * \author MAV'RIC Team
 * \author Nicolas Dousse
 * 
 * \brief This file decodes the message from the neighbors and computes the relative position and velocity in local coordinates
 * 
 ******************************************************************************/


#ifndef NEIGHBOR_SEL_H__
#define NEIGHBOR_SEL_H__

#include "communication/mavlink_communication.hpp"
#include "communication/mavlink_message_handler.hpp"
#include "communication/mavlink_stream.hpp"
#include "communication/state.hpp"
#include "sensing/position_estimation.hpp"

#define MAX_NUM_NEIGHBORS 15                                        ///< The maximum number of neighbors

#define SIZE_VHC 2.0f                                               ///< The safety size for the collision avoidance strategy
#define safe_size__VHC 3.0f                                         ///< The safety size for the collision avoidance strategy

#define MAXSPEED 4.5f                                               ///< The maximum speed of the vehicle
#define CRUISESPEED 3.0f                                            ///< The cruise speed of the vehicle in m/s

#define ORCA_TIME_STEP_MILLIS 10.0f                                 ///< The time step for the task

#define NEIGHBOR_TIMEOUT_LIMIT_MS 4000                              ///< 4 seconds timeout limit

#define FREQ_LPF 0.2f

/**
 * \brief The track neighbor structure
 */
typedef struct
{
    uint8_t neighbor_ID;                                            ///< The MAVLink ID of the vehicle
    float position[3];                                              ///< The 3D position of the neighbor in m
    float velocity[3];                                              ///< The 3D velocity of the neighbor in m/s
    float size;                                                     ///< The physical size of the neighbor in m
    uint32_t time_msg_received;                                     ///< The time at which the message was received in ms
    float comm_frequency;                                           ///< The frequency of message reception
    uint32_t msg_count;                                             ///< The number of message received
    float extrapolated_position[3];                                 ///< The 3D position of the neighbor
} track_neighbor_t;                                                 ///< The structure of information about a neighbor 

/**
 * \brief The collision log structure
 */
typedef struct
{
    bool near_miss_flag[MAX_NUM_NEIGHBORS];
    bool collision_flag[MAX_NUM_NEIGHBORS];
    bool transition_flag[MAX_NUM_NEIGHBORS];
    uint32_t count_collision;
    uint32_t count_near_miss;
    
} collision_log_t;

/**
 * \brief The neighbor structure
 */
class Neighbors
{
public:
    uint8_t number_of_neighbors_;                               ///< The actual number of neighbors at a given time step
    float safe_size_;                                           ///< The safe size for collision avoidance
    track_neighbor_t neighbors_list_[MAX_NUM_NEIGHBORS];        ///< The list of neighbors structure
    float comm_frequency_list_[MAX_NUM_NEIGHBORS];
    float mean_comm_frequency_;                                 ///< The mean value of the communication frequency
    float variance_comm_frequency_;                             ///< The variance of the communication frequency
    uint32_t previous_time_;                                    ///< The time of the previous loop
    uint32_t update_time_interval_;                             ///< The time between two communication frequency update, in ms
    float min_dist_;                                            ///< The minimal distance with all neighbors at each time step
    float max_dist_;                                            ///< The maximal distance with all neighbors at each time step
    float local_density_;                                       ///< The instantaneous local density
    collision_log_t collision_log_;                             ///< The collision log structure
    float collision_dist_sqr_;                                  ///< The square of the collision distance
    float near_miss_dist_sqr_;                                  ///< The square of the near-miss distance

    Position_estimation& position_estimation_;                  ///< The pointer to the position estimator structure

    State& state_;                                              ///< The pointer to the state structure
    const Mavlink_stream& mavlink_stream_;

    /**
     * \brief   Initialize the neighbor selection module
     *
     * \param neighbors             The pointer to the neighbor struct
     * \param position_estimation   The pointer to the position estimator struct
     * \param message_handler       The pointer to the message handler structure
     *
     * \return  True if the init succeed, false otherwise
     */
    Neighbors(Position_estimation& position_estimation, State& state, const Mavlink_stream& mavlink_stream);

    /**
     * \brief   Extrapolate the position of each UAS between two messages, deletes the message if time elapsed too long from last message
     *
     * \param   neighbors           The pointer to the neighbors struct
     */
    void extrapolate_or_delete_position(void);

    /**
     * \brief   Computes the mean and the variance of the communication frequency
     *
     * \param   neighbors           The pointer to the neighbors struct
     */
    void compute_communication_frequency(void);

    /**
     * \brief   Update the log of the near miss and collisions and compute the minimal distance between all MAVs
     *
     * \param   neighbors           The pointer to the neighbors struct
     */
    void collision_log_smallest_distance(void);

private:
} ;

/**
 * \brief   Decode the message and parse to the neighbor array
 *
 * \param   neighbors           The pointer to the neighbors struct
 * \param   sysid               The system ID
 * \param   msg                 The pointer to the MAVLink message
 */
void neighbor_selection_read_message_from_neighbors(Neighbors* neighbors, uint32_t sysid, mavlink_message_t* msg);

/**
 * \brief   Initialize the neighbor selection telemetery module
 *
 * \param neighbors             The pointer to the neighbor struct
 * \param message_handler       The pointer to the message handler structure
 *
 * \return  True if the init succeed, false otherwise
 */
bool neighbor_selection_telemetry_init(Neighbors* neighbors, Mavlink_message_handler* message_handler);

#endif // NEIGHBOR_SEL_H__