/*
 * td1208.h
 *
 *  Created on: 14 oct. 2024
 *      Author: Ludo
 */

#ifndef __TD1208_H__
#define __TD1208_H__

#ifndef TD1208_DRIVER_DISABLE_FLAGS_FILE
#include "td1208_driver_flags.h"
#endif
#include "parser.h"
#include "strings.h"
#include "types.h"

/*** TD1208 macros ***/

#define TD1208_SIGFOX_EP_ID_SIZE_BYTES              4
#define TD1208_SIGFOX_UL_PAYLOAD_SIZE_BYTES_MAX     12

/*** TD1208 structures ***/

/*!******************************************************************
 * \enum TD1208_status_t
 * \brief TD1208 driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    TD1208_SUCCESS,
    TD1208_ERROR_NULL_PARAMETER,
    TD1208_ERROR_SIGFOX_UL_PAYLOAD_SIZE,
    TD1208_ERROR_REPLY_TIMEOUT,
    // Low level drivers errors.
    TD1208_ERROR_BASE_UART = 0x0100,
    TD1208_ERROR_BASE_DELAY = (TD1208_ERROR_BASE_UART + TD1208_DRIVER_UART_ERROR_BASE_LAST),
    TD1208_ERROR_BASE_STRING = (TD1208_ERROR_BASE_DELAY + TD1208_DRIVER_DELAY_ERROR_BASE_LAST),
    TD1208_ERROR_BASE_PARSER = (TD1208_ERROR_BASE_STRING + STRING_ERROR_BASE_LAST),
    // Last base value.
    TD1208_ERROR_BASE_LAST = (TD1208_ERROR_BASE_PARSER + PARSER_ERROR_BASE_LAST)
} TD1208_status_t;

#ifndef TD1208_DRIVER_DISABLE

/*** TD1208 functions ***/

/*!******************************************************************
 * \fn TD1208_status_t TD1208_init(void)
 * \brief Init TD1208 driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_init(void);

/*!******************************************************************
 * \fn TD1208_status_t TD1208_de_init(void)
 * \brief Release TD1208 driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_de_init(void);

/*!******************************************************************
 * \fn TD1208_status_t TD1208_reset(void)
 * \brief Reset TD1208 chip.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_reset(void);

/*!******************************************************************
 * \fn TD1208_status_t TD1208_get_sigfox_ep_id(uint8_t* sigfox_ep_id)
 * \brief Get TD1208 Sigfox EP ID.
 * \param[in]   none
 * \param[out]  sigfox_ep_id: Pointer to byte array that will contain the Sigfox EP ID.
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_get_sigfox_ep_id(uint8_t* sigfox_ep_id);

/*!******************************************************************
 * \fn TD1208_status_t TD1208_send_bit(uint8_t ul_bit)
 * \brief Send a bit over Sigfox network.
 * \param[in]   ul_bit: Bit to send.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_send_bit(uint8_t ul_bit);

/*!******************************************************************
 * \fn TD1208_status_t TD1208_send_frame(uint8_t* ul_payload, uint8_t ul_payload_size_bytes)
 * \brief Send a frame over Sigfox network.
 * \param[in]   ul_payload: Byte array to send.
 * \param[in]   ul_payload_size_bytes: Number of bytes to send.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_send_frame(uint8_t* ul_payload, uint8_t ul_payload_size_bytes);

/*******************************************************************/
#define TD1208_exit_error(base) { ERROR_check_exit(td1208_status, TD1208_SUCCESS, base) }

/*******************************************************************/
#define TD1208_stack_error(base) { ERROR_check_stack(td1208_status, TD1208_SUCCESS, base) }

/*******************************************************************/
#define TD1208_stack_exit_error(base, code) { ERROR_check_stack_exit(td1208_status, TD1208_SUCCESS, base, code) }

#endif /* TD1208_DRIVER_DISABLE */

#endif /* __TD1208_H__ */
