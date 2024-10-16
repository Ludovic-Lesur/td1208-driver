/*
 * td1208_hw.c
 *
 *  Created on: 14 oct. 2024
 *      Author: Ludo
 */

#include "td1208_hw.h"

#ifndef TD1208_DRIVER_DISABLE_FLAGS_FILE
#include "td1208_driver_flags.h"
#endif
#include "td1208.h"
#include "types.h"

#ifndef TD1208_DRIVER_DISABLE

/*** TD1208 HW functions ***/

/*******************************************************************/
TD1208_status_t __attribute__((weak)) TD1208_HW_init(TD1208_HW_configuration_t* configuration) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    /* To be implemented */
    UNUSED(configuration);
    return status;
}

/*******************************************************************/
TD1208_status_t __attribute__((weak)) TD1208_HW_de_init(void) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    /* To be implemented */
    return status;
}

/*******************************************************************/
TD1208_status_t __attribute__((weak)) TD1208_HW_uart_write(uint8_t* data, uint32_t data_size_bytes) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    /* To be implemented */
    UNUSED(data);
    UNUSED(data_size_bytes);
    return status;
}

/*******************************************************************/
TD1208_status_t __attribute__((weak)) TD1208_HW_delay_milliseconds(uint32_t delay_ms) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    /* To be implemented */
    UNUSED(delay_ms);
    return status;
}

#endif /* TD1208_DRIVER_DISABLE */
