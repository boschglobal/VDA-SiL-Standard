/******************************************************************
* FILE:            SiLVI_COM_CustomBus.h
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
* 3.0.0.0	Introduced separate file for the custom bus type
*/

//Custom parameters, serialization and deserialization are contracted by the application and the SiLVI driver implementation
typedef struct SiLVI_COM_Custom_Bus_Parameters
{
	void* data;   //pointer to the configration data structure
	uint64_t len; //length of the data
}
SiLVI_COM_Custom_Bus_Parameters;


/*
* @brief Initialization of the bus interface
* @param [in] points to a handle to be returned
* @param [in] logical name of the interface to open, e.g. "CUSTOM:0".
*             It is used to identify the interface/channel to be opened.
*             Useful if the model opens multiple interfaces.
* @param [in] the configration data structure. The member data is only valid in the respective function call.
*             For this reason the driver implementation has to create and manage an internal copy of the
*             data if it is reused.
* @return status indicating success or failure of the operation
*
* MULTIPLE CALLS AND MULTIPLE OPENS
*
* It is allowed to open the same interface (with same logical name) multiple times.
* In such cases different handles shall be returned by the driver.
*
* It is allowed to close and reopen the same interface during simulation, even multiple times.
* However, different handles shall be returned on subsequent calls of SiLVI_COM_initialize_custom_bus_p().
* Existing handles must become invalid after they have been passed to SiLVI_COM_terminate_p().
*
* This function is allowed to block until the bus is available and/or the bus interface can be used.
* The virtual bus is expected to be fully operational when SiLVI_COM_initialize_custom_bus_p() has
* successfully returned.
* A possible start-up or synchronisation phase of the virtual bus shall not be observable for the SiLVI
* client application.
*
* Each handle returned by a SiLVI_COM_initialize_custom_bus_p() call must be closed by calling SiLVI_COM_terminate_p().
*/
typedef SiLVI_status(*SiLVI_COM_initialize_custom_bus_p)(int32_t*, const char*, const SiLVI_COM_Custom_Bus_Parameters);


/*
* @brief Initialization of the bus interface with auto configuration
* @param [in]     points to a handle to be returned
* @param [in]     logical name of the interface to open, e.g. "CUSTOM:0".
*                 It is used to identify the interface/channel to be opened.
*                 Useful if the model opens multiple interfaces.
* @param [in|out] a pointer to the data structure where the driver can store the configuration
*                 If this pointer is set to NULL then no configuration data will be written by the driver.
*                 The pointer for the configuration data must be initialized by the client. The value of the
*                 member len has to be set to the size of the allocated memory. The driver will replace
*                 the value by the actual required size and copy the data to the buffer. If the size is too
*                 small the driver will return with the SiLVI_ERROR_ALLOCATED_MEMORY_TOO_SMALL. The value of
*                 the member len is set to the required size by the driver. The client must reallocate the
*                 buffer and the second call should succeed.
* @return status indicating success or failure of the operation
*
* Example 1 - auto configuration without getting the configuration
* int32_t custom_handle;
* result = ptr->custom_bus.auto_initialize(&custom_handle, "CUSTOM:0", NULL);
*
* Example 2 - auto configuration, getting the configuration in a fixed anonymous buffer
* int32_t custom_handle;
* uint8_t busconfig_data[1024];
* SiLVI_COM_Custom_Bus_Parameters custom_params;
* custom_params.data = busconfig_data;
* custom_params.len = sizeof(busconfig_data)
* result = ptr->custom_bus.auto_initialize(&custom_handle, "CUSTOM:0", &custom_params);
*
* Example 3 - auto configuration, getting the configuration in a data structure that is known by the client and the driver
* int32_t custom_handle;
* CustomBusConfigData cfg_data;
* SiLVI_COM_Custom_Bus_Parameters custom_params;
* custom_params.data = &cfg_data;
* custom_params.len = sizeof(CustomBusConfigData)
* result = ptr->custom_bus.auto_initialize(&custom_handle, "CUSTOM:0", &custom_params);
*
* Example 4 - auto configuration, getting the configuration in a dynamic buffer
* int32_t custom_handle;
* SiLVI_COM_Custom_Bus_Parameters custom_params;
* custom_params.len = 1024;
* custom_params.data = malloc(params.len);
* result = ptr->custom_bus.auto_initialize(&custom_handle, "CUSTOM:0", &custom_params);
* if (result == SiLVI_ERROR_ALLOCATED_MEMORY_TOO_SMALL) {
*     custom_params.data  = realloc(custom_params.data, custom_params.len);
*     result = ptr->custom_bus.auto_initialize(&custom_handle, "CUSTOM:0", &custom_params);
* }
* if (result != SiLVI_OK) {
*     // advanced error handling required
* }
*
* In addition to the described auto configuration options the implementation must provide the same behaviour like SiLVI_COM_initialize_custom_bus_p
*
*/
typedef SiLVI_status(*SiLVI_COM_auto_initialize_custom_bus_p)(int32_t*, const char*, SiLVI_COM_Custom_Bus_Parameters*);

//SiLVI ABI Version 3
typedef struct SiLVI_driverFunctionTable_Custom_Bus_V3
{
	//padding 32 bit word to avoid padding bytes inserted by the compiler
	uint32_t padding;

	//initialize and auto initialize
	SiLVI_COM_initialize_custom_bus_p initialize;
	SiLVI_COM_auto_initialize_custom_bus_p auto_initialize;

	// extensions have to be added at the end
}
SiLVI_COM_driverFunctionTable_CustomBus_V3;
