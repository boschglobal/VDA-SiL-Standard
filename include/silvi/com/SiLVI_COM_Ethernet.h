/******************************************************************
* FILE:            SiLVI_COM_Ethernet.h
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
* 3.0.0.0	Introduced separate file for Ethernet
*/

//ethernet
typedef struct { uint8_t bytes[6]; } SiLVI_COM_Ethernet_MAC_Addr;
typedef enum SiLVI_COM_Ethernet_Speed
{
	SiLVI_ETHERNET_10M = 0,
	SiLVI_ETHERNET_100M = 1,
	SiLVI_ETHERNET_1G = 2,
	SiLVI_ETHERNET_10G = 3
}
SiLVI_COM_Ethernet_Speed;

typedef struct SiLVI_COM_Ethernet_VLAN_Id_List
{
    uint16_t* ids;  //the values must be between 0...4095 and will add the interface to the IEEE 802.3q VLAN
                    //the pointer is only valid in the respective function call. For this reason the driver implementation
                    //has to create and manage an internal copy of the data if it is reused.
    uint64_t cnt;   //multiple keys are allowed. If cnt is 0 ids must be NULL.
} SiLVI_COM_Ethernet_VLAN_Id_List;

typedef struct SiLVI_COM_Ethernet_Multicast_Addr_List
{
    SiLVI_COM_Ethernet_MAC_Addr* addrs; //list of valid multicast MAC addresses.
                                        //the pointer is only valid in the respective function call. For this reason the
                                        //driver implementation has to create and manage an internal copy of the data if
                                        //it is reused.
    uint64_t cnt;                       //specifies the number of multicast addresses. If cnt is 0 addrs must be NULL
} SiLVI_COM_Ethernet_Multicast_Addr_List;

//Ethernet parameters
typedef struct SiLVI_COM_Ethernet_Parameters
{
	SiLVI_COM_Ethernet_MAC_Addr macAddr;              //valid MAC address
	SiLVI_COM_Ethernet_VLAN_Id_List vlan;
	SiLVI_COM_Ethernet_Multicast_Addr_List multicast;
	SiLVI_COM_Ethernet_Speed maxSpeed;                //maximum speed of the controller (10/100/1000/10000 MBit/s)
}
SiLVI_COM_Ethernet_Parameters;

/*
* @brief Initialization of the bus interface
* @param [in] points to a handle to be returned
* @param [in] logical name of the interface to open, e.g. "ETHERNET:0".
*             It is used to identify the interface/channel to be opened.
*             Useful if the model opens multiple interfaces.
* @param [in] the configration data structure
* @return status indicating success or failure of the operation
*
* MULTIPLE CALLS AND MULTIPLE OPENS
*
* It is allowed to open the same interface (with same logical name) multiple times.
* In such cases different handles shall be returned by the driver.
*
* It is allowed to close and reopen the same interface during simulation, even multiple times.
* However, different handles shall be returned on subsequent calls of SiLVI_COM_initialize_ethernet_p().
* Existing handles must become invalid after they have been passed to SiLVI_COM_terminate_p().
*
* This function is allowed to block until the bus is available and/or the bus interface can be used.
* The virtual bus is expected to be fully operational when SiLVI_COM_initialize_ethernet_p() has
* successfully returned.
* A possible start-up or synchronisation phase of the virtual bus shall not be observable for the SiLVI
* client application.
*
* Each handle returned by a SiLVI_COM_initialize_ethernet_p() call must be closed by calling SiLVI_COM_terminate_p().
*/
typedef SiLVI_status(*SiLVI_COM_initialize_ethernet_p)(int32_t*, const char*, const SiLVI_COM_Ethernet_Parameters);

/*
* @brief Initialization of the bus interface with auto configuration
* @param [in]     points to a handle to be returned
* @param [in]     logical name of the interface to open, e.g. "ETHERNET:0".
*                 It is used to identify the interface/channel to be opened.
*                 Useful if the model opens multiple interfaces.
* @param [in|out] a pointer to the data structure where the driver can store the configuration
*                 If this pointer is set to NULL then no configuration data will be written by the driver
*                 Dynamic elements of the data structure (e.g. the number of multi cast addresses)
*                 are allocated by the client. If the size of the provided buffers is sufficient the
*                 driver will set the value of the member cnt to the number of the provided buffers.
*                 If the size of provided buffers is not sufficient the driver will return with the error
*                 SiLVI_ERROR_ALLOCATED_MEMORY_TOO_SMALL. In this case the driver has to set the size or
*                 count parameters to the expected value. The client has to provide sufficient buffers and
*                 the second call to this function should be successful.
* @return status indicating success or failure of the operation
*
* Example 1 - auto configuration without getting the configuration
* int32_t ethernet_handle;
* result = ptr->auto_initialize(&ethernet_handle, "ETHERNET:0", NULL);
*
* Example 2 - auto configuration with getting the configuration, but making an educated guess on a sufficient
*             buffer size for dynamic content (buffer for 3 VLAN IDs and buffer for 2 multi cast addresses
* int32_t ethernet_handle;
* SiLVI_COM_Ethernet_Parameters ethernet_params;
* uint16_t vlanids[3];
* SiLVI_COM_Ethernet_MAC_Addr macaddr[2];
* ethernet_params.vlan.cnt = 3;
* ethernet_params.vlan.ids = vlanids;
* ethernet_params.multicast.cnt = 3;
* ethernet_params.multicast.addrs = macaddr;
* result = ptr->auto_initialize(&ethernet_handle, "ETHERNET:0", &ethernet_params);
*
* Example 3 - auto configuration with getting the configuration and required buffer size for dynamic content from the driver
* int32_t ethernet_handle;
* SiLVI_COM_Ethernet_Parameters ethernet_params;
* uint16_t vlanids[3];
* SiLVI_COM_Ethernet_MAC_Addr macaddr[2];
* ethernet_params.vlan.cnt = 0;
* ethernet_params.vlan.ids = NULL;
* ethernet_params.multicast.cnt = 0;
* ethernet_params.multicast.addrs = NULL;
* result = ptr->auto_initialize(&ethernet_handle, "ETHERNET:0", &ethernet_params);
* if (result == SiLVI_ERROR_ALLOCATED_MEMORY_TOO_SMALL) {
*     // expected error
*
*     if (ethernet_params.vlan.cnt)
*         ethernet_params.vlan.ids = (uint16_t*)realloc(ethernet_params.vlan.ids, ethernet_params.vlan.cnt * sizeof(uint16_t));
*
*     if (ethernet_params.multicast.cnt)
*         ethernet_params.multicast.addrs = (SiLVI_COM_Ethernet_MAC_Addr*)realloc(ethernet_params.multicast.addrs, ethernet_params.multicast.cnt * sizeof(SiLVI_COM_Ethernet_MAC_Addr));
*
*     result = ptr->custom_bus.auto_initialize(&ethernet_handle, "ETHERNET:0", &ethernet_params);
* }
* if (result != SiLVI_OK) {
*     // advanced error handling required
* }
*
* In addition to the described auto configuration options the implementation must provide the same behaviour like SiLVI_initialize_p
*
*/
typedef SiLVI_status(*SiLVI_COM_auto_initialize_ethernet_p)(int32_t*, const char*, SiLVI_COM_Ethernet_Parameters*);

/*
* @brief Change the VLAN configuration of an existing interface
*
* @param [in] handle returned by the init function
* @param [in] the new configuration.
* @return Status indicating success or failure of the operation.
*
*/
typedef SiLVI_status(*SiLVI_COM_reconfigure_ethernet_vLan_p)(int32_t, const SiLVI_COM_Ethernet_VLAN_Id_List);

/*
* @brief Change the multicast configuration of an existing interface
*
* @param [in] handle returned by the init function
* @param [in] the new configuration.
* @return Status indicating success or failure of the operation.
*
*/
typedef SiLVI_status(*SiLVI_COM_reconfigure_ethernet_multiCast_p)(int32_t, const SiLVI_COM_Ethernet_Multicast_Addr_List);

//SiLVI ABI Version 3
typedef struct SiLVI_driverFunctionTable_Ethernet_V3
{
	//padding 32 bit word to avoid padding bytes inserted by the compiler
	uint32_t padding;

	//initialize and auto initialize
	SiLVI_COM_initialize_ethernet_p initialize;
	SiLVI_COM_auto_initialize_ethernet_p auto_initialize;

	//reconfigure
	SiLVI_COM_reconfigure_ethernet_vLan_p reconfigure_vLan;
	SiLVI_COM_reconfigure_ethernet_multiCast_p reconfigure_multiCast;

	// extensions have to be added at the end
}
SiLVI_COM_driverFunctionTable_Ethernet_V3;
