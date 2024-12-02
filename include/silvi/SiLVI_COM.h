/******************************************************************
* FILE:            SiLVI_COM.h
* VERSION:         3.0.0.0
* DATE:            07.03.2024
* DESCRIPTION:     Interface Description File
* COPYRIGHT:       (C) 2024 VDA SiLVI Workgroup
*
* SPDX-License-Identifier: Apache-2.0
*
******************************************************************/

/*
SiLVI Communication API and ABI description

GENERAL NOTES:

1) The following API allows to be called from multiple threads. So implementations of SiLVI drivers are generally expected
   to be thread-safe.
   It is also allowed that SiLVI drivers call registered RX callback functions from different thread contexts.
   This means that multi-threading can be used on both sides: client (model) side and driver side.

2) The following API makes no statements about reentrant functions. None of the functions is expected to be reentrant.
   It is generally not allowed to call any of the functions e.g. in a UNIX signal handler or in another context expecting
   reentrant functions.

3) The following API is a pure C API and hence the caller does not expect (and likely cannot handle) any C++ exceptions
   So a possible C++ implementation must not throw exceptions and they must catch all exceptions internally before returning to
   the caller.

4) This header must be compilable by all modern C and C++ compilers.

5) The API/ABI of the driver must be an instance of the function table SiLVI_COM_driverFunctionTable_V3 defined below.
   It has to be exported as DLL/SO symbol "silvi_com_abi_3" where the last number is the major version number of the ABI.
   The same number must be assigned to the first uint16_t member of the function table SiLVI_COM_driverFunctionTable_V3.
   All function pointers in the function table must be valid and match the expected function signatures.
   It is not allowed to set any of them to NULL or leave them unimplemented. However, it is allowed to implement
   functions as empty functions returning only a return value.
   The function table for a particular bus type may be NULL if the bus type is not supported by the driver.

6) For the standardized protocols (CAN, LIN, FLEXRAY, ETHERNET) the API must be used with the serialization schemas
   shipped with this header file. Custom network protocols can use different serialization schemas that have to be
   aligned between the driver implementation and the client. Whatever serialization schema is used, the data must be
   representable as a raw byte stream: uint8_t* for data and a uint64_t for length.
   The function types to handle such data are SiLVI_COM_txFrame_p(), SiLVI_COM_rxFrame_p() and SiLVI_COM_rxCallbackFunction().
   None of the functions above will take the ownership of the passed byte sequence - the caller is expected to
   dispose the buffer after the respective function call when it is not needed anymore.

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
* 1.0.0.0	Initial version
*
* 2.0.0.0	Proposal from 11.04.2022
*			Removed function pointers: registerSyncPoint, continueSimulation, registerBusEvent
*			Reasons: Out of scope of VDA SiLVI which shall be an API for frame communication only
*			but not for stepping of and synchronizing with virtual buses.
*			A new API/ABI shall be created for this purpose.
*
* 2.0.1.0	Proposal from 25.04.2022
*			Names of typedefs in API changed according to feedback. Obsolete typedefs removed.
*			Comments on functions improved to reflect the independency of flatbuffer file format.
*
* 2.0.1.1	Minor fixes and changes in the comments, proposal for independent configuration based
*			on placeholders
*
* 2.1.0.0	Proposal from 10.11.2022: Configure function added. Changes in the specification of
*			the API and some functions to allow multi-threading in drivers and client code.
*			Switch from flatbuffer schema files version 1 to version 2.
*
* 2.2.0.0	Proposal for data structure instead of init string
*           Removal of deprecated APIs
*
* 3.0.0.0	Splitted header files and restructured driver function table
*           Removed mimetype flatbuffer schemas will be fixed within the SiLVI release
*           Use standard C/C++ data types
*           initialize with specific bus type
*           Auto config option in SiLVI_auto_initialize_..._p
*           Reconfiguration of VLAN and Multicast MAC addresses for Ethernet interfaces
*           Introduced SiLVI_COM_Ethernet_MAC_Addr
*           Enumeration SiLVI_Ethernet_Speed
*           Removed MIME type and key-value parts of the description of SiLVI_initialize_p
*           Removed configuration parts of the description of SiLVI_getInfo_p
*/

#pragma once

#include "silvi/core/SiLVI_BaseDefs.h"
#include "silvi/core/SiLVI_Status.h"
#include "silvi/core/SiLVI_Logging.h"
#include "silvi/com/SiLVI_COM_Generic.h"
#include "silvi/com/SiLVI_COM_CAN.h"
#include "silvi/com/SiLVI_COM_LIN.h"
#include "silvi/com/SiLVI_COM_FlexRay.h"
#include "silvi/com/SiLVI_COM_Ethernet.h"
#include "silvi/com/SiLVI_COM_CustomBus.h"

//SiLVI COM ABI Version 3
typedef struct SiLVI_COM_driverFunctionTable_V3
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

	//life cycle
	SiLVI_COM_terminate_p terminate;
	SiLVI_COM_getInfo_p getInfo;

	//time
	SiLVI_COM_getSimulationTime_p getSimulationTime;

	//communication
	SiLVI_COM_txFrame_p txFrame;
	SiLVI_COM_rxFrame_p rxFrame;
	SiLVI_COM_registerRxFrameCB_p registerRxFrameCallback;

	//function tables for the bus types
	SiLVI_COM_driverFunctionTable_CAN_V3 can;
	SiLVI_COM_driverFunctionTable_LIN_V3 lin;
	SiLVI_COM_driverFunctionTable_FlexRay_V3 flexray;
	SiLVI_COM_driverFunctionTable_Ethernet_V3 ethernet;
	SiLVI_COM_driverFunctionTable_CustomBus_V3 custom_bus;

	//extensions have to be added at the end
}
SiLVI_COM_driverFunctionTable_V3;

//The symbol name to be exported by the shared library of the driver, e.g. silvi_com_abi_3 for major ABI version 3.
//The minor version is not included in the symbol name because higher minor versions must guarantee backward
//compatibility to lower minor versions.
#define SiLVI_COM_DRIVER_MODULE_SYMBOL_3 (silvi_com_abi_3)
#define SiLVI_COM_DRIVER_MODULE_SYMBOL_3_STR ("silvi_com_abi_3")
