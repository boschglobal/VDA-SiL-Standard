/******************************************************************
* FILE:            SiLVI_TA.h
* VERSION:         3.0.0.0
* DATE:            07.03.2024
* DESCRIPTION:     Interface Description File
* COPYRIGHT:       (C) 2024 VDA SiLVI Workgroup
*
* SPDX-License-Identifier: Apache-2.0
*
******************************************************************/

/*
SiLVI Test Automation API and ABI description

GENERAL NOTES:

1) The mandatory SiLVI version is 3.0.0 or higher

2) The following API allows to be called from multiple threads. So implementations of SiLVI drivers are generally expected
   to be thread-safe.
   It is also allowed that SiLVI drivers call registered callback functions from different thread contexts.
   This means that multi-threading can be used on both sides: client (model) side and driver side.

3) The following API makes no statements about reentrant functions. None of the functions is expected to be reentrant.
   It is generally not allowed to call any of the functions e.g. in a UNIX signal handler or in another context expecting
   reentrant functions.

4) The following API is a pure C API and hence the caller does not expect (and likely cannot handle) any C++ exceptions
   So a possible C++ implementation must not throw exceptions and they must catch all exceptions internally before returning to
   the caller.

5) This header must be compilable by all modern C and C++ compilers.

6) The API/ABI of the driver must be an instance of the function table SiLVI_TA_driverFunctionTable_V3 defined below.
   It has to be exported as DLL/SO symbol "silvi_ta_abi_3" where the last number is the major version number of the ABI.
   The same number must be assigned to the first uint16_t member of the function table SiLVI_TA_driverFunctionTable_V3.
   All function pointers in the function table must be valid and match the expected function signatures.
   It is not allowed to set any of them to NULL or leave them unimplemented. However, it is allowed to implement
   functions as empty functions returning only a return value.

7) The API below does not have any operating system or hardware architecture dependencies.
   It is expected to work on all major operating systems on both, 32 bit and 64 bit architectures.
   The API should be compiler-agnostic and should work in the same way for debug and release builds.
   This means that no code changes should be necessary when recompiling client code for a different operating system
   or hardware architecture. Driver and client code are allowed to be built with different compilers and in different
   modes: debug and release.

8) The versioning schema of this header file is defined as follows:

   <ABI major version>.<ABI minor version>.<API version>.<Comment version>

   Where:
   <ABI major version>	is the major version of the Application Binary Interface (ABI),
						also included as numeric suffix in the exported DLL symbol silvi_abi_<ABI major vers.>
						and as first integer number in the function table defined below.
						Note that this number shall be increased in case of backward incompatible
						changes.

   <ABI minor version>  is the minor version number of the ABI, also included
						as second integer number in the function table defined below.
						Note that this number shall be increased in case of backward compatible
						changes, e. g. additional function pointers at the end of the table.

   <API version>		is the version number of the API (Application Programming Interface).
						Changes of the API typically require a recompilation of all source code that uses it.
						This number shall be increased in case of changes which don't affect the
						ABI - no changes in function signatures and in the order of function pointers.
						A typical API change which does not affect the ABI would be to rename
						any of the function pointers in the function table, e. g. init() -> initialize(),
						or to change names of typedefs.

   <Comment version>	any further changes which don't affect the compatibility, neither at runtime
						nor at compile time, shall cause an increase of this version number.
						Typically such changes involve comments, white spaces and source code
						formatting only.

* Version history:
* MAJOR_ABI.MINOR_ABI.API.COMMENT version
* 3.0.0.0	Initial version of the test automation interface
*/

#pragma once

#include "silvi/core/SiLVI_BaseDefs.h"
#include "silvi/core/SiLVI_Status.h"
#include "silvi/core/SiLVI_Logging.h"

typedef enum SiLVI_TA_BusType
{
    SiLVI_TA_Unknown = 0,
    SiLVI_TA_CAN = 1,
    SiLVI_TA_Ethernet = 2,
    SiLVI_TA_FlexRay_ChA = 3,
    SiLVI_TA_FlexRay_ChB = 4,
    SiLVI_TA_LIN = 5,
    SilVI_TA_Custom = 0x128
} SiLVI_TA_BusType;

typedef struct SiLVI_TA_BusInfo
{
    uint32_t busIndex;
    SiLVI_TA_BusType type;
    const char busName[256];
    void* configuration; // pointer to bus configuration data
}
SiLVI_TA_BusInfo;

typedef struct SiLVI_TA_InterfaceInfo
{
    uint32_t interfaceIndex;  //Must be unique across all interfaces listed for the current bus
    const char interfaceName[256];
    uint32_t busIndex;
    void* configuration; // pointer to interface configuration data
}
SiLVI_TA_InterfaceInfo;

typedef enum SiLVI_TA_Direction
{
    TX = 1,     //Send direction
    RX = 2,     //Receive direction
    TXRX = 3    //Both directions simultaneously, streams are synchronized (RX time stamps monotonically increasing)
} SiLVI_TA_Direction;

/**
 * Simulator connection
 */
//Requires a configured bus simulation (i.e. simulation tool has been started and simulation is loaded + configured but not necessarily running)
typedef SiLVI_status(*SiLVI_TA_ConnectSimulation)(int64_t* /*SimulationHandle*/, const char* /*connectionInfo*/);
//A asynchronous connection is also possible
typedef SiLVI_status(*SiLVI_TA_DisconnectSimulation)(int64_t /*SimulationHandle*/);

/**
 * Bus topology inspection
 */
//Valid bus indexes 0...SiLVI_TA_NumberOfBuses - 1
typedef SiLVI_status(*SiLVI_TA_GetNumberOfAvailableBuses)(int64_t /*SimulationHandle*/, size_t* /*Number of busses*/);
 //Info is written to the data structure provided by the caller. In case of FlexRay both channels A and B appear as separate buses
typedef SiLVI_status(*SiLVI_TA_GetBusInfo)(int64_t /*SimulationHandle*/, uint32_t /*SiLVI_TA_BusIndex*/, SiLVI_TA_BusInfo*);
 //Valid interface indexes 0...SiLVI_TA_NumberOfInterfaces - 1
typedef SiLVI_status(*SiLVI_TA_GetNumberOfAvailableInterfaces)(int64_t /*SimulationHandle*/, uint32_t /*SiLVI_TA_BusIndex*/, uint64_t /*NumberOfInterfaces*/);
//Info is written to the data structure provided by the caller
typedef SiLVI_status(*SiLVI_TA_GetInterfaceInfo)(int64_t /*SimulationHandle*/, uint32_t /*SiLVI_TA_BusIndex*/, uint32_t /*InterfaceIndex*/, SiLVI_TA_InterfaceInfo*);


/**
 * Session handling
 */
typedef SiLVI_status(*SiLVI_TA_OpenBus)(int64_t /*SimulationHandle*/, uint32_t /*SiLVI_TA_BusIndex*/, int64_t* /*BusHandle*/);
typedef SiLVI_status(*SiLVI_TA_CloseBus)(int64_t /*SimulationHandle*/, int64_t /*BusHandle*/);
typedef SiLVI_status(*SiLVI_TA_OpenInterface)(int64_t /*SimulationHandle*/, uint32_t /*BusIndex*/, uint32_t /*InterfaceIndex*/, int64_t* /*InterfaceHandle*/);
typedef SiLVI_status(*SiLVI_TA_CloseInterface)(int64_t /*SimulationHandle*/,  int64_t /*InterfaceHandle*/);
//Start filtering and calling registered callbacks
typedef SiLVI_status(*SiLVI_TA_StartMonitoring)(int64_t /*BusHandle*/);
 //Don't call callbacks, don't use filters, simulation keeps running
typedef SiLVI_status(*SiLVI_TA_StopMonitoring)(int64_t /*BusHandle*/);
//Callbacks can slow down the simulation if they take too much time, but do not cause frame loss
typedef SiLVI_status(*SiLVI_TA_Callback)(const uint8_t* /*Data*/, uint64_t /*DataSize*/, void* /*UserPtr*/);
 //Only possible before StartMonitoring or after StopMonitoring with the same handle
typedef SiLVI_status(*SiLVI_TA_RegisterBusCallback)(int64_t /*BusHandle*/, SiLVI_TA_Callback, void* /*UserPtr*/);
 //Only possible before StartMonitoring or after StopMonitoring with the same handle
typedef SiLVI_status(*SiLVI_TA_UnregisterBusCallbacks)(int64_t /*BusHandle*/);
//Only possible before StartMonitoring or after StopMonitoring with the same handle
typedef SiLVI_status(*SiLVI_TA_RegisterInterfaceCallback)(int64_t /*InterfaceHandle*/, SiLVI_TA_Direction, SiLVI_TA_Callback, void* /*UserData*/);
 //Only possible before StartMonitoring or after StopMonitoring with the same handle
typedef SiLVI_status(*SiLVI_TA_UnregisterInterfaceCallbacks)(int64_t /*InterfaceHandle*/);


//SiLVI TA ABI Version 3
typedef struct SiLVI_TA_driverFunctionTable_V3
{
	//version information
	uint16_t majorVersion;
	uint16_t minorVersion;

	//padding 32 bit word to avoid padding bytes inserted by the compiler
	uint32_t padding;

	//logging
	SiLVI_logCallbackFunction_p defaultLogFunction;
	SiLVI_registerLogCallbackFunction_p registerLoggerCallback;

	//vendor error description
	SiLVI_getVendorErrorDescription_p getVendorErrorDescription;

	//simulation
	SiLVI_TA_ConnectSimulation connectSimulation;
	SiLVI_TA_DisconnectSimulation disconnectSimulation;

	//buses
	SiLVI_TA_GetNumberOfAvailableBuses getNumberOfAvailableBuses;
	SiLVI_TA_GetBusInfo getBusInfo;
	SiLVI_TA_OpenBus openBus;
	SiLVI_TA_CloseBus closeBus;

	//interfaces
   SiLVI_TA_GetNumberOfAvailableInterfaces getNumberOfAvailableInterfaces;
	SiLVI_TA_GetInterfaceInfo getInterfaceInfo;
	SiLVI_TA_OpenInterface openInterface;
	SiLVI_TA_CloseInterface closeInterface;

	//monitoring
	SiLVI_TA_StartMonitoring startMonitoring;
	SiLVI_TA_StopMonitoring stopMonitoring;

   //callbacks
   SiLVI_TA_RegisterBusCallback registerBusCallback;
   SiLVI_TA_UnregisterBusCallbacks unregisterBusCallbacks;
   SiLVI_TA_RegisterInterfaceCallback registerInterfaceCallback;
   SiLVI_TA_UnregisterInterfaceCallbacks unregisterInterfaceCallbacks;
}
SiLVI_TA_driverFunctionTable_V3;

//The symbol name to be exported by the shared library of the driver, e.g. silvi_ta_abi_3 for major ABI version 3.
//The minor version is not included in the symbol name because higher minor versions must guarantee backward
//compatibility to lower minor versions.
#define SiLVI_TA_DRIVER_MODULE_SYMBOL_3 (silvi_ta_abi_3)
#define SiLVI_TA_DRIVER_MODULE_SYMBOL_3_STR ("silvi_ta_abi_3")
