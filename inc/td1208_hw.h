/*
 * td1208_hw.h
 *
 *  Created on: 14 oct. 2024
 *      Author: Ludo
 */

#ifndef __TD1208_HW_H__
#define __TD1208_HW_H__

#ifndef TD1208_DRIVER_DISABLE_FLAGS_FILE
#include "td1208_driver_flags.h"
#endif
#include "td1208.h"
#include "types.h"

#ifndef TD1208_DRIVER_DISABLE

/*** TD1208 HW structures ***/

/*!******************************************************************
 * \fn TD1208_HW_rx_irq_cb_t
 * \brief Byte reception interrupt callback.
 *******************************************************************/
typedef void (*TD1208_HW_rx_irq_cb_t)(uint8_t data);

/*!******************************************************************
 * \struct TD1208_HW_configuration_t
 * \brief TD1208 hardware interface parameters.
 *******************************************************************/
typedef struct {
    uint32_t uart_baud_rate;
    TD1208_HW_rx_irq_cb_t rx_irq_callback;
} TD1208_HW_configuration_t;

/*** TD1208 HW functions ***/

/*!******************************************************************
 * \fn TD1208_status_t TD1208_HW_init(TD1208_HW_configuration_t* configuration)
 * \brief Init TD1208 hardware interface.
 * \param[in]   configuration: Pointer to the hardware interface parameters structure.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_HW_init(TD1208_HW_configuration_t* configuration);

/*!******************************************************************
 * \fn TD1208_status_t TD1208_HW_de_init(void)
 * \brief Release TD1208 hardware interface.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_HW_de_init(void);

/*!******************************************************************
 * \fn TD1208_status_t TD1208_HW_send_message(uint8_t* message, uint32_t message_size_bytes)
 * \brief Send a message over the TD1208 control interface.
 * \param[in]   message: Byte array to send.
 * \param[in]   message_size_bytes: Number of bytes to send.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_HW_uart_write(uint8_t* data, uint32_t data_size_bytes);

/*!******************************************************************
 * \fn TD1208_status_t TD1208_HW_delay_milliseconds(uint32_t delay_ms)
 * \brief Delay function.
 * \param[in]   delay_ms: Delay to wait in ms.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TD1208_status_t TD1208_HW_delay_milliseconds(uint32_t delay_ms);

#endif /* TD1208_DRIVER_DISABLE */

#endif /* __TD1208_HW_H__ */
