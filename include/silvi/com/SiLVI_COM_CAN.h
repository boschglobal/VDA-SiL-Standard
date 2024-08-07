/******************************************************************
* FILE:            SiLVI_COM_CAN.h
* VERSION:         3.0.0.0
* DATE:            07.03.2024
* DESCRIPTION:     Interface Description File
* COPYRIGHT:       (C) 2024 VDA SiLVI Workgroup
*
* SPDX-License-Identifier: Apache-2.0
*
******************************************************************/

#pragma once
#include "silvi/core/SiLVI_BaseDefs.h"

/*
SiLVI API and ABI description

* Version history:
* MAJOR_ABI.MINOR_ABI.API.COMMENT version
* 3.0.0.0	Introduced separate file for CAN
*/

//CAN parameters - bus configuration is part of the network simulation
typedef struct SiLVI_COM_CAN_Parameters
{
	uint8_t selfReception;     //SiLVI_True|SiLVI_False, self reception is used to receive a copy of the data that is sent to the bus
	uint32_t baudRate;         //baud rate in bps
	uint8_t fastDataEnabled;   //SiLVI_True|SiLVI_False, flag if CAN-FD is enabled, if set to false fastBaudRate is ignored
	uint64_t fastBaudRate;     //CAN-FD baud rate in bps
}
SiLVI_COM_CAN_Parameters;

/*
* @brief Initialization of the bus interface
* @param [in] points to a handle to be returned
* @param [in] logical name of the interface to open, e.g. "CAN:0".
*             It is used to identify the interface/channel to be opened.
*             Useful if the model opens multiple interfaces.
* @param [in] the configration data structure
* @return status indicating success or failure of the operation
*
* Example
* int32_t can_handle;
* SiLVI_COM_CANParameters can_params;
* can_params.selfReception = SiLVI_False;
* can_params.baudRate = 250*1024;
* can_params.fastDataEnabled = SiLVI_True;
* can_params.fastBaudRate = 2*1024*1024;
* result = ptr->can.initialize(&can_handle, "CAN:0", can_params);
*
* MULTIPLE CALLS AND MULTIPLE OPENS
*
* It is allowed to open the same interface (with same logical name) multiple times.
* In such cases different handles shall be returned by the driver.
*
* It is allowed to close and reopen the same interface during simulation, even multiple times.
* However, different handles shall be returned on subsequent calls of SiLVI_COM_initialize_can_p().
* Existing handles must become invalid after they have been passed to SiLVI_COM_terminate_p().
*
* This function is allowed to block until the bus is available and/or the bus interface can be
* used. The virtual bus is expected to be fully operational when SiLVI_COM_initialize_can_p() has
* successfully returned.
* A possible start-up or synchronisation phase of the virtual bus shall not be observable for the
* SiLVI client application.
*
* Each handle returned by a SiLVI_COM_initialize_can_p() call must be closed by calling
* SiLVI_COM_terminate_p().
*/
typedef SiLVI_status(*SiLVI_COM_initialize_can_p)(int32_t*, const char*, const SiLVI_COM_CAN_Parameters);

/*
* @brief Initialization of the bus interface with auto configuration
* @param [in]     points to a handle to be returned
* @param [in]     logical name of the interface to open, e.g. "CAN:0".
*                 It is used to identify the interface/channel to be opened.
*                 Useful if the model opens multiple interfaces.
* @param [in|out] a pointer to the data structure where the driver can store the configuration
*                 If this pointer is set to NULL then no configuration data will be written by the
*                 driver
* @return status indicating success or failure of the operation
*
* Example 1 - auto configuration without getting the configuration
* int32_t can_handle;
* result = ptr->can.auto_initialize(&can_handle, "CAN:0", NULL);
*
* Example 2 - auto configuration with getting the configuration
* int32_t can_handle;
* SiLVI_COM_CANParameters can_params;
* result = ptr->auto_initialize(&can_handle, "CAN:0", &can_params);
*
* In addition to the described auto configuration options the implementation must provide the same
* behaviour like SiLVI_COM_initialize_can_p
*
*/
typedef SiLVI_status(*SiLVI_COM_auto_initialize_can_p)(int32_t*, const char*, SiLVI_COM_CAN_Parameters*);

//SiLVI ABI Version 3
typedef struct SiLVI_driverFunctionTable_CAN_V3
{
	//padding 32 bit word to avoid padding bytes inserted by the compiler
	uint32_t padding;

	//initialize and auto initialize
	SiLVI_COM_initialize_can_p initialize;
	SiLVI_COM_auto_initialize_can_p auto_initialize;

	// extensions have to be added at the end
}
SiLVI_COM_driverFunctionTable_CAN_V3;
