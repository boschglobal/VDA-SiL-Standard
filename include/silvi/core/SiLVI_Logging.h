/******************************************************************
* FILE:            SiLVI_Logging.h
* VERSION:         3.0.0.0
* DATE:            07.03.2024
* DESCRIPTION:     Interface Description File
* COPYRIGHT:       (C) 2024 VDA SiLVI Workgroup
*
* SPDX-License-Identifier: Apache-2.0
*
******************************************************************/

#pragma once

/*
SiLVI API and ABI description

* Version history:
* MAJOR_ABI.MINOR_ABI.API.COMMENT version
* 3.0.0.0	Introduced separate file for the logging interface
*/

//SiLVI log levels
typedef enum SiLVI_LogLevel
{
    SiLVI_LOG_TRACE = 0,    //log tracing information and above, very detailed function trace can happen
    SiLVI_LOG_DEBUG = 1,    //log debug information and above, mostly interesting only for developers
    SiLVI_LOG_INFO = 2,     //log user information and above, can be ignored by the user
    SiLVI_LOG_WARNING = 3,  //log warnings and above, warning means the result might be not as expected, the user should act and at least check
    SiLVI_LOG_ERROR = 4,    //Log any kind of errors which are not fatal, further operation of the interface is possible
    SiLVI_LOG_FATAL = 5,    //Log only fatal errors, the interface is inoperable, no further communication is possible
}
SiLVI_LogLevel;

/*
* @brief Function for logging
* Logging must be enabled as early as possible and must be independent of any open interface.
* Each driver shall provide a default log function which simply logs to stdout or stderr.
* The default log level of the default log function must be 3 (SiLVI_LOG_WARNING).
* This default log function can then be replaced by a client specific log function by
* calling the register function below.
* The log function is expected to react on the log level information and to show only
* those messages which are expected to be shown.
* @param [in] log level to indicate the severity of the log message (how important is it?)
* @param [in] the message to be logged - may contain placeholders for arguments, similarly to printf format strings
* @param [in] additional arguments to replace placeholders
* @return success of the operation
*/
typedef SiLVI_status(*SiLVI_logCallbackFunction_p)(SiLVI_LogLevel, const char*, ...);

/*
* @brief Register a callback function for logging
* This function will register a new log function in the function table below.
* The original function (which shall call to stdout or stderr) will be replaced.
* The registration shall happen immediately after getting the pointer to the function table.
* It is not allowed to register a NULL here because the driver is not expected to check the
* pointers in the function table. It would crash then.
* @param [in] pointer to callback function to be called for each log message
* @return success of the operation
*/
typedef SiLVI_status(*SiLVI_registerLogCallbackFunction_p)(SiLVI_logCallbackFunction_p);
