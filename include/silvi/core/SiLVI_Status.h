/******************************************************************
* FILE:            SiLVI_Status.h
* VERSION:         3.0.0.0
* DATE:            07.03.2024
* DESCRIPTION:     Interface Description File
* COPYRIGHT:       (C) 2024 VDA SiLVI Workgroup
*
* SPDX-License-Identifier: Apache-2.0
*
******************************************************************/

#pragma once
#include <stdint.h>
#include "SiLVI_BaseDefs.h"

/*
SiLVI API and ABI description

* Version history:
* MAJOR_ABI.MINOR_ABI.API.COMMENT version
* 3.0.0.0	Introduced separate file
*/

typedef enum SiLVI_status {
    // Error Code List
    SiLVI_OK                                   = 0,
    SiLVI_ERROR_TIMEOUT                        = 1,
    SiLVI_ERROR_NULLPTR                        = 2,
    SiLVI_ERROR_NOT_IMPLEMENTED                = 3,
    SiLVI_ERROR_ALLOCATED_MEMORY_TOO_SMALL     = 4,
    SiLVI_ERROR_INVALID_PARAMETERS             = 5,
    SiLVI_ERROR_INVALID_CONNECTION_INFO        = 6,
    SiLVI_ERROR_INVALID_INDEX                  = 7,
    SiLVI_ERROR_INVALID_HANDLE                 = 8,
    SiLVI_ERROR_INVALID_BUSTYPE                = 9,
    SiLVI_ERROR_INVALID_NAME                   = 10,
    SiLVI_ERROR_INVALID_DIRECTION              = 11,
    SiLVI_ERROR_INVALID_FRAME                  = 12,
    SiLVI_ERROR_TX_BUFFER_OVERFLOW             = 13,
    SiLVI_ERROR_BUS_MONITORING_ALREADY_STARTED = 14,          // e.g. SiLVI_TA_StartMonitoring was already called
    SiLVI_ERROR_BUS_MONITORING_NOT_RUNNING     = 15,          // e.g. SiLVI_TA_StartMonitoring was not previously called
    SiLVI_ERROR_SIMULATION_NOT_RUNNING         = 16,          // e.g. SiLVI_TA_ConnectSimulation was not previously called
    SiLVI_VENDOR_ERROR_START                   = (1 << 31),   // marker for start of vendor error range
    SiLVI_VEMDOR_ERROR_END                     = UINT32_MAX   // marker for end of vendor error range, ensures 32bit enum size
} SiLVI_status;

#define SiLVI_IS_VENDOR_ERROR(CODE) (CODE & SiLVI_VENDOR_ERROR_START) ? SiLVI_True : SiLVI_False

/*
 * @brief Returns information abount a vendor error code
 * @param [in] The error code
 * @return Pointer to a multi-line string (UTF8) which contains human-readable details about the error
 */
typedef const char* (*SiLVI_getVendorErrorDescription_p)(SiLVI_status);
