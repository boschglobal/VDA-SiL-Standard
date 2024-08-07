/******************************************************************
* FILE:            SiLVI_BaseDefs.h
* VERSION:         3.0.0.0
* DATE:            07.03.2024
* DESCRIPTION:     Interface Description File
* COPYRIGHT:       (C) 2024 VDA SiLVI Workgroup
*
* SPDX-License-Identifier: Apache-2.0
*
******************************************************************/

#pragma once
#include<string.h>
#include<stdint.h>

/*
SiLVI API and ABI description

* Version history:
* MAJOR_ABI.MINOR_ABI.API.COMMENT version
* 3.0.0.0	Introduced separate file for the base definitions
*/

//dll import
#ifdef WIN32 //windows
#ifdef BUILD_SiLVI_DRIVER
#define EXPORT_SiLVI_SYMBOL __declspec(dllexport)
#else
#define EXPORT_SiLVI_SYMBOL __declspec(dllimport)
#endif
#else //linux
#define EXPORT_SiLVI_SYMBOL
#endif

//invalid handle
#define INVALID_SiLVI_HANDLE (-1)

//true and false
#define SiLVI_True 1
#define SiLVI_False 0
