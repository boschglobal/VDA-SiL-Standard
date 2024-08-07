/******************************************************************
* FILE:            SiLVI_COM_FlexRay.h
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
* 3.0.0.0	Introduced separate file for Flexray
*/

//FlexRay channel
typedef enum SiLVI_COM_FlexRay_Channel
{
	SiLVI_FLEXRAY_CHANNEL_NONE = 0,
	SiLVI_FLEXRAY_CHANNEL_A = 1,
	SiLVI_FLEXRAY_CHANNEL_B = 2,
	SiLVI_FLEXRAY_CHANNEL_BOTH = 3
}
SiLVI_COM_FlexRay_Channel;

//FlexRay parameters
typedef struct SiLVI_COM_FlexRay_Parameters
{
	uint8_t selfReception;                     //SiLVI_True|SiLVI_False, self reception is used to receive a copy of the data that is sent to the bus
	uint16_t cycleSizeInMicroSec;              //<CYCLE>
	SiLVI_COM_FlexRay_Channel flexrayChannel;  //the flexray channel
	uint64_t bitsPerSecond;                    //Similar to baudrate <BAUDRATE>
	uint64_t bitsPerCycle;                     //Number of bits per cycle, derived from <BAUDRATE> and <CYCLE>
	uint16_t macroTicksPerCycle;               //8..16000 <MACRO-PER-CYCLE>
	uint16_t staticSlotsPerCycle;              //2..1023 <NUMBER-OF-STATIC-SLOTS>
	uint16_t macroTicksPerStaticSlot;          //3..664 <STATIC-SLOT-DURATION>
	uint8_t payloadWordsInStaticSegment;       //0..127 <PAYLOAD-LENGTH-STATIC>
	uint16_t miniSlotsPerCycle;                //0..7988 <NUMBER-OF-MINISLOTS>
	uint16_t macroTicksPerMiniSlot;            //2..63*<MACRO-PER-CYCLE>*<CYCLE> <MINISLOT-DURATION>
	uint16_t dynamicSlotIdlePhase;             //0..2*miniSlotsPerCycle <DYNAMIC-SLOT-IDLE-PHASE>
	uint8_t macroTicksInSymbolWindow;          //0..162 <SYMBOL-WINDOW>
}
SiLVI_COM_FlexRay_Parameters;

/*
* @brief Initialization of the bus interface
* @param [in] points to a handle to be returned
* @param [in] logical name of the interface to open, e.g. "FLEXRAY:0".
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
* However, different handles shall be returned on subsequent calls of SiLVI_COM_initialize_flexray_p().
* Existing handles must become invalid after they have been passed to SiLVI_COM_terminate_p().
*
* This function is allowed to block until the bus is available and/or the bus interface can be used.
* The virtual bus is expected to be fully operational when SiLVI_COM_initialize_flexray_p() has
* successfully returned.
* A possible start-up or synchronisation phase of the virtual bus shall not be observable for the SiLVI
* client application.
*
* Each handle returned by a SiLVI_COM_initialize_flexray_p() call must be closed by calling SiLVI_COM_terminate_p().
*/
typedef SiLVI_status(*SiLVI_COM_initialize_flexray_p)(int32_t*, const char*, const SiLVI_COM_FlexRay_Parameters);

/*
* @brief Initialization of the bus interface with auto configuration
* @param [in]     points to a handle to be returned
* @param [in]     logical name of the interface to open, e.g. "FLEXRAY:0".
*                 It is used to identify the interface/channel to be opened.
*                 Useful if the model opens multiple interfaces.
* @param [in|out] a pointer to the data structure where the driver can store the configuration
*                 If this pointer is set to NULL then no configuration data will be written by the driver
* @return status indicating success or failure of the operation
*
* The implementation must provide the same behaviour like SiLVI_COM_initialize_flexray_p
*
*/
typedef SiLVI_status(*SiLVI_COM_auto_initialize_flexray_p)(int32_t*, const char*, SiLVI_COM_FlexRay_Parameters*);

//SiLVI ABI Version 3
typedef struct SiLVI_driverFunctionTable_Flexray_V3
{
	//padding 32 bit word to avoid padding bytes inserted by the compiler
	uint32_t padding;

	//initialize and auto initialize
	SiLVI_COM_initialize_flexray_p initialize;
	SiLVI_COM_auto_initialize_flexray_p auto_initialize;

	// extensions have to be added at the end
}
SiLVI_COM_driverFunctionTable_FlexRay_V3;
