/*******************************************************************************
 * Copyright (c) 2009-2016, MAV'RIC Development Team
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
 * \file control_command_telemetry.cpp
 *
 * \author MAV'RIC Team
 * \author Julien Lecoeur
 *
 * \brief   Telemetry for control_command
 *
 ******************************************************************************/


#include "control/control_command_telemetry.hpp"
#include "hal/common/time_keeper.hpp"

extern "C"
{
#include "util/print_util.h"
}


void control_command_telemetry_send(const command_t* command, const Mavlink_stream* mavlink_stream, mavlink_message_t* msg)
{
    float quat[4] = {   command->attitude.quat.s,
                        command->attitude.quat.v[0],
                        command->attitude.quat.v[1],
                        command->attitude.quat.v[2],
                    };

    mavlink_msg_control_command_pack(mavlink_stream->sysid(),
                                     mavlink_stream->compid(),
                                     msg,
                                     time_keeper_get_us(),
                                     command->thrust.thrust,
                                     command->torque.xyz,
                                     command->rate.xyz,
                                     command->attitude.rpy,
                                     quat,
                                     command->position.xyz,
                                     command->velocity.xyz);

    //print_util_dbg_print(" send command\r\n");
}

void control_command_2_telemetry_send(const command_t* command, const Mavlink_stream* mavlink_stream, mavlink_message_t* msg)
{
    mavlink_msg_debug_pack(mavlink_stream->sysid(),
                                mavlink_stream->compid(),
                                msg,
                                time_keeper_get_ms(),
                                0,
                                command->thrust.thrust);

    //print_util_dbg_print(" send command\r\n");
}

void control_command_3_telemetry_send(const command_t* command, const Mavlink_stream* mavlink_stream, mavlink_message_t* msg)
{
    mavlink_msg_debug_vect_pack(mavlink_stream->sysid(),
                                mavlink_stream->compid(),
                                msg,
                                "torque",
                                time_keeper_get_us(),
                                command->torque.xyz[0],
                                command->torque.xyz[1],
                                command->torque.xyz[2]);

    //print_util_dbg_print(" send command\r\n");
}