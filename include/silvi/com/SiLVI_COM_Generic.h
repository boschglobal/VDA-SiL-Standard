/******************************************************************
* FILE:            SiLVI_COM_Generic.h
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
* 3.0.0.0	Introduced separate file for the generic parts
*/

/*
 * @brief Termination to tear down a connections to virtual bus represented by the handle.
 * The handle is considered as invalid after this function call. To use the interface again a new
 * call of the respective init function has to happen - it shall return a new handle.
 * All pending messages waiting for reception must be disposed on termination - they are lost.
 *
 * @param [in] handle returned by the init function
 * @return status indicating success or failure of the operation
 */
typedef SiLVI_status(*SiLVI_COM_terminate_p)(int32_t);

/*
 * Returns information about the driver
 * @return Pointer to a multi-line string (UTF8) which contains human-readable details about the driver
 */
typedef const char* (*SiLVI_COM_getInfo_p)(void);

/*
 * @brief Returns the current simulation time of the virtual bus as integer number of nanoseconds
 * Note that this is for SiL and time is virtual
 * @param [in] handle returned by the init function
 * @param [out] pointer to a variable where the time stamp is to be stored
 * @return status indicating success or failure of the operation
 *		   Shall be SiLVI_ERROR_SIMULATION_NOT_RUNNING if the bus simulation is not yet running
 */
typedef SiLVI_status(*SiLVI_COM_getSimulationTime_p)(int32_t, uint64_t*);

/*
 * @brief Sends a sequence of frames in the format of the bus type or the custom serialization schema
 * We refer to that format simply as "buffer".
 * This function call shall trigger a sequence of send requests to the virtual bus.
 * It must not block or wait for arbitration of the frames!
 * A possible success/failure and return value must be calculated as soon as possible, e. g.
 * - indication whether the frame has been accepted by the virtual bus interface,
 * - send errors like send buffer overflow,
 * - malformed frames,
 * - payload too long etc.
 * must be reported immediately.
 * Bus errors and errors during arbitration shall not be reported here because there could be a long delay
 * between send request and the actual arbitration / bus interaction.
 *
 * Frames are not necessarily expected to be sent in the same order as the original order in the buffer.
 * The virtual bus interface is allowed to reorder frames, e. g. to send frames with highest priority first.
 * In case of multiple frame send requests in the buffer this function shall fail and return an error if
 * only one frame could not be sent or was not accepted by the virtual bus interface.
 * So sending shall behave like a transaction: Either all frames are accepted or - if one is not accepted -
 * the whole buffer shall be rejected and an error shall be returned. In that case the virtual interface
 * must remain in the same state as before.
 *
 * @param [in] handle returned by the init function
 * @param [in] pointer to a buffer containing the frames to be sent
 * @param [in] size of the buffer
 * @return status indicating success or failure of the operation
 */
typedef SiLVI_status(*SiLVI_COM_txFrame_p)(int32_t, const uint8_t*, uint64_t);

/*
 * @brief Receive a sequence of frames from the virtual bus in the format of the bus type or the custom
 * serialization schema. We refer to that format simply as "buffer".
 * Frames can be possibly copied from an internal temporary buffer inside the bus interface
 * and can be obtained by calling this function.
 * This function must not block or wait for frames but return immediately if there are no frames to receive.
 * In this case the value of the buffer size has to be set to 0 by the driver and SiLVI_OK is returned
 * If the buffer provided by the caller is too small to hold the entire sequence of frames then
 * SiLVI_ERROR_ALLOCATED_MEMORY_TOO_SMALL will be returned. In that case the driver has to set the value
 * of the buffer size to required size. The caller shall increase the buffer size and try again.
 * All frames will remain in the buffer in this case, no frames must be deleted in the buffer until
 * they have been successfully passed to the caller.
 * @param [in] handle returned by the init function
 * @param [in,out] pointer to a buffer to store the sequence of frames in the specified format
 * @param [in,out] pointer to size of the buffer. If the return value is SiLVI_ERROR_ALLOCATED_MEMORY_TOO_SMALL
 *        then the original value must be replaced by the required size of the buffer, so the caller could
 *        resize its buffer and try again to read the messages in a second function call.
 *        If there are no frames to receive the driver has to set the value to 0 and return SiLVI_OK.
 * @return status indicating success or failure of the operation:
 *         SiLVI_ERROR_ALLOCATED_MEMORY_TOO_SMALL if the buffer is too small, pending messages are
 *         considered as NOT received. The buffer must be left unchanged for a second try.
 *		   SiLVI_OK in case of success or if there are no frames to receive. The messages copied to caller's
 *         buffer are considered as received. The caller is expected to handle the received messages.
 *		   The driver is expected to delete the RX messages in its internal RX buffer.
 */
typedef SiLVI_status(*SiLVI_COM_rxFrame_p)(int32_t, uint8_t*, uint64_t*);

/*
 * @brief A callback function that gets called by the driver when RX frames arrive at the bus interface
 * Such callbacks can be registered via SiLVI_COM_registerRxFrameCB_p.
 * Registering a callback means that SiLVI_COM_rxFrame_p will never receive any frames because they will
 * be delivered directly to the receiver via this callback and they won't be stored in a temporary internal buffer.
 * It is allowed to call other SiLVI functions in the registered callback, e.g. to call txFrame described above,
 * so it is allowed to send new frames to the bus directly from the RX callback.
 *
 * Bear in mind that SiLVI drivers could call the registered callback function in a different thread context.
 * This typically requires to protect access to internal data structures by thread mutexes.
 *
 * @param [in] handle returned by the init function, can be used inside of the callback for further API calls
 * @param [in] pointer to a buffer to store the sequence of frames
 * @param [in] size of the buffer
 * @param [in] user defined context information to be passed to the callback, can be NULL if not needed
 * @return status indicating success or failure of the operation
 */

typedef void (*SiLVI_COM_rxCallbackFunction_p)(int32_t, const uint8_t*, uint64_t, void*);

/*
 * @brief register a callback function to receive frames
 * Received frames from the virtual bus can be immediately forwarded to the callback function of the model.
 * Retrun value of old registered callback or NULL if no callback was registered
 * @param [in] handle returned by the init function
 * @param [in] pointer to the callback function or NULL if all registered callbacks for the given handle
 *             should be unregistered
 * @param [in] pointer to context information to be passed to the callback, can be NULL if not needed
 * @return status indicating success or failure of the operation
 *
 * Note that registering RX callback functions is allowed any time, even during simulation steps.
 * In such cases all pending RX frames buffered in the background and waiting for SiLVI_COM_rxFrame_p must
 * be immediately passed to the just registered callback function. This is expected to happen in the context
 * of the SiLVI_COM_registerRxFrameCB_p function call!
 * The callback function can be called multiple times, once for each frame, or only once by passing all frames
 * in one go.
 *
 * In case the underlying bus simulation does not support callback-based frame reception
 * this function call must fail by returning SiLVI_ERROR_NOT_IMPLEMENTED. In that case frames shall be received
 * by calling SiLVI_COM_rxFrame_p.
 *
 * The opposite should happen when unregistering a callback function: All frames received from the interface
 * should be buffered internally until the client calls rxFrame() or registers another callback function.
 */
typedef SiLVI_status(*SiLVI_COM_registerRxFrameCB_p)(int32_t, SiLVI_COM_rxCallbackFunction_p, void*);
