/******************************************************************
* FILE:            SiLVI_COM_LIN.h
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
* 3.0.0.0	Introduced separate file for LIN
*/

//LIN parameters
typedef struct SiLVI_COM_LIN_Parameters
{
	uint8_t selfReception;   //SiLVI_True|SiLVI_False, self reception is used to receive a copy of the data that is sent to the bus
	uint32_t baudRate;       //baud rate
	uint8_t masterMode;      //SiLVI_True|SiLVI_False, if SiLVI_True the controller acts as master, otherwise as slave
}
SiLVI_COM_LIN_Parameters;

/*
* @brief Initialization of the bus interface
* @param [in] points to a handle to be returned
* @param [in] logical name of the interface to open, e.g. "LIN:0".
*             It is used to identify the interface/channel to be opened.
*             Useful if the model opens multiple interfaces.
* @param [in] the configration data structure
* @return status indicating success or failure of the operation
*
* Example
* int32_t lin_handle;
* SiLVI_COM_LINParameters lin_params;
* lin_params.selfReception = SiLVI_False;
* lin_params.baudRate = 9600;
* lin_params.masterMode = SiLVI_True;
* result = ptr->lin.initialize(&lin_handle, "LIN:0", lin_params);
*
* MULTIPLE CALLS AND MULTIPLE OPENS
*
* It is allowed to open the same interface (with same logical name) multiple times.
* In such cases different handles shall be returned by the driver.
*
* It is allowed to close and reopen the same interface during simulation, even multiple times.
* However, different handles shall be returned on subsequent calls of SiLVI_COM_initialize_lin_p().
* Existing handles must become invalid after they have been passed to SiLVI_COM_terminate_p().
*
* This function is allowed to block until the bus is available and/or the bus interface can be used.
* The virtual bus is expected to be fully operational when SiLVI_COM_initialize_lin_p() has successfully
* returned.
* A possible start-up or synchronisation phase of the virtual bus shall not be observable for the SiLVI
* client application.
*
* Each handle returned by a SiLVI_COM_initialize_lin_p() call must be closed by calling SiLVI_COM_terminate_p().
*/
typedef SiLVI_status(*SiLVI_COM_initialize_lin_p)(int32_t*, const char*, const SiLVI_COM_LIN_Parameters);

/*
* @brief Initialization of the bus interface with auto configuration
* @param [in]     points to a handle to be returned
* @param [in]     logical name of the interface to open, e.g. "LIN:0".
*                 It is used to identify the interface/channel to be opened.
*                 Useful if the model opens multiple interfaces.
* @param [in|out] a pointer to the data structure where the driver can store the configuration
*                 If this pointer is set to NULL then no configuration data will be written by the driver
* @return status indicating success or failure of the operation
*
* Example 1 - auto configuration without getting the configuration
* int32_t lin_handle;
* result = ptr->lin.auto_initialize(&lin_handle, "LIN:0", NULL);
*
* Example 2 - auto configuration with getting the configuration
* int32_t lin_handle;
* SiLVI_COM_LIN_Parameters lin_params;
* result = ptr->auto_initialize(&handle, "LIN:0", &lin_params);
*
* In addition to the described auto configuration options the implementation must provide the same
* behaviour like SiLVI_COM_initialize_lin_p
*
*/
typedef SiLVI_status(*SiLVI_COM_auto_initialize_lin_p)(int32_t*, const char*, SiLVI_COM_LIN_Parameters*);

//SiLVI ABI Version 3
typedef struct SiLVI_driverFunctionTable_LIN_V3
{
	//padding 32 bit word to avoid padding bytes inserted by the compiler
	uint32_t padding;

	//initialize and auto initialize
	SiLVI_COM_initialize_lin_p initialize;
	SiLVI_COM_auto_initialize_lin_p auto_initialize;

	// extensions have to be added at the end
}
SiLVI_COM_driverFunctionTable_LIN_V3;
