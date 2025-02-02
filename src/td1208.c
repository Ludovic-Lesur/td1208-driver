/*
 * td1208.c
 *
 *  Created on: 14 oct. 2024
 *      Author: Ludo
 */

#include "td1208.h"

#ifndef TD1208_DRIVER_DISABLE_FLAGS_FILE
#include "td1208_driver_flags.h"
#endif
#include "error.h"
#include "maths.h"
#include "parser.h"
#include "strings.h"
#include "td1208_hw.h"
#include "types.h"

#ifndef TD1208_DRIVER_DISABLE

/*** TD1208 local macros ***/

#define TD1208_UART_BAUD_RATE               9600

#define TD1208_BOOT_DELAY_MS                1000

#define TD1208_BUFFER_SIZE_BYTES            64

#define TD1208_REPLY_BUFFER_DEPTH           8
#define TD1208_REPLY_PARSING_DELAY_MS       500
#define TD1208_REPLY_PARSING_TIMEOUT_MS     10000

#define TD1208_SIGFOX_EP_ID_SIZE_CHAR       (TD1208_SIGFOX_EP_ID_SIZE_BYTES * MATH_U8_SIZE_HEXADECIMAL_DIGITS)

/*** TD1208 local structures ***/

/*******************************************************************/
typedef struct {
    volatile char_t buffer[TD1208_BUFFER_SIZE_BYTES];
    volatile uint8_t char_idx;
    PARSER_context_t parser;
} TD1208_reply_buffer_t;

/*******************************************************************/
typedef struct {
    // Reply buffers.
    TD1208_reply_buffer_t reply[TD1208_REPLY_BUFFER_DEPTH];
    uint8_t reply_idx;
} TD1208_context_t;

/*** TD1208 local global variables ***/

static TD1208_context_t td1208_ctx;

/*** TD1208 local functions ***/

/*******************************************************************/
static void _TD1208_rx_irq_callback(uint8_t rx_byte) {
    // Read current index.
    uint8_t char_idx = td1208_ctx.reply[td1208_ctx.reply_idx].char_idx;
    // Store incoming byte.
    td1208_ctx.reply[td1208_ctx.reply_idx].buffer[char_idx] = (char_t) rx_byte;
    // Manage index.
    char_idx = (uint8_t) ((char_idx + 1) % TD1208_BUFFER_SIZE_BYTES);
    td1208_ctx.reply[td1208_ctx.reply_idx].char_idx = char_idx;
    // Check ending characters.
    if (rx_byte == STRING_CHAR_LF) {
        // Switch buffer.
        td1208_ctx.reply_idx = (uint8_t) ((td1208_ctx.reply_idx + 1) % TD1208_REPLY_BUFFER_DEPTH);
    }
}

/*******************************************************************/
static void _TD1208_reset_replies(void) {
    // Local variables.
    uint8_t rep_idx = 0;
    uint8_t char_idx = 0;
    // Reset replies buffers.
    for (rep_idx = 0; rep_idx < TD1208_REPLY_BUFFER_DEPTH; rep_idx++) {
        for (char_idx = 0; char_idx < TD1208_BUFFER_SIZE_BYTES; char_idx++) {
            td1208_ctx.reply[rep_idx].buffer[char_idx] = STRING_CHAR_NULL;
        }
        td1208_ctx.reply[rep_idx].char_idx = 0;
        // Reset parser.
        td1208_ctx.reply[rep_idx].parser.buffer = (char_t*) td1208_ctx.reply[rep_idx].buffer;
        td1208_ctx.reply[rep_idx].parser.buffer_size = 0;
        td1208_ctx.reply[rep_idx].parser.separator_index = 0;
        td1208_ctx.reply[rep_idx].parser.start_index = 0;
    }
    // Reset index and count.
    td1208_ctx.reply_idx = 0;
}

/*******************************************************************/
static TD1208_status_t _TD1208_send_command(char_t* command) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    uint8_t command_buffer[TD1208_BUFFER_SIZE_BYTES] = { 0x00 };
    uint8_t command_size = 0;
    // Fill command buffer.
    while (*command) {
        command_buffer[command_size] = (uint8_t) (*(command++));
        command_size++;
    }
    command_buffer[command_size++] = STRING_CHAR_CR;
    command_buffer[command_size++] = STRING_CHAR_NULL;
    // Send command through UART.
    status = TD1208_HW_uart_write(command_buffer, command_size);
    if (status != TD1208_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
static TD1208_status_t _TD1208_wait_for_string(char_t* ref) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    PARSER_status_t parser_status = PARSER_SUCCESS;
    uint32_t parsing_time = 0;
    uint8_t reply_idx = 0;
    uint8_t reply_received = 0;
    while (1) {
        // Delay.
        status = TD1208_HW_delay_milliseconds(TD1208_REPLY_PARSING_DELAY_MS);
        if (status != TD1208_SUCCESS) goto errors;
        // Update parsing time.
        parsing_time += TD1208_REPLY_PARSING_DELAY_MS;
        // Loop on all replies.
        for (reply_idx = 0; reply_idx < TD1208_REPLY_BUFFER_DEPTH; reply_idx++) {
            // Check size.
            if (td1208_ctx.reply[reply_idx].char_idx != 0) {
                // Update local flag.
                reply_received = 1;
                // Parse reply.
                td1208_ctx.reply[reply_idx].parser.buffer_size = td1208_ctx.reply[reply_idx].char_idx;
                parser_status = PARSER_compare(&td1208_ctx.reply[reply_idx].parser, PARSER_MODE_HEADER, ref);
                // Update status.
                if (parser_status == PARSER_SUCCESS) {
                    status = TD1208_SUCCESS;
                    goto errors;
                }
                else {
                    status = (TD1208_ERROR_BASE_PARSER + parser_status);
                }
            }
        }
        // Manage timeout.
        if (parsing_time > TD1208_REPLY_PARSING_TIMEOUT_MS) {
            // Update error code.
            if (reply_received == 0) {
                status = TD1208_ERROR_REPLY_TIMEOUT;
            }
            goto errors;
        }
    }
errors:
    return status;
}

/*** TD1208 functions ***/

/*******************************************************************/
TD1208_status_t TD1208_init(void) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    TD1208_HW_configuration_t hw_config;
    // Init buffers.
    _TD1208_reset_replies();
    // Init hardware interface.
    hw_config.uart_baud_rate = TD1208_UART_BAUD_RATE;
    hw_config.rx_irq_callback = &_TD1208_rx_irq_callback;
    status = TD1208_HW_init(&hw_config);
    if (status != TD1208_SUCCESS) goto errors;
    // Boot delay.
    status = TD1208_HW_delay_milliseconds(TD1208_BOOT_DELAY_MS);
    if (status != TD1208_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TD1208_status_t TD1208_de_init(void) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    status = TD1208_HW_de_init();
    if (status != TD1208_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TD1208_status_t TD1208_reset(void) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    // Reset parser.
    _TD1208_reset_replies();
    // Send command.
    status = _TD1208_send_command("ATZ");
    if (status != TD1208_SUCCESS) goto errors;
    // Wait for command completion.
    status = _TD1208_wait_for_string("OK");
errors:
    return status;
}

/*******************************************************************/
TD1208_status_t TD1208_get_sigfox_ep_id(uint8_t* sigfox_ep_id) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    PARSER_status_t parser_status = PARSER_SUCCESS;
    char_t temp_char = STRING_CHAR_NULL;
    uint8_t rep_idx = 0;
    uint8_t line_size = 0;
    uint32_t extracted_size = 0;
    int8_t idx = 0;
    // Check parameter.
    if (sigfox_ep_id == NULL) {
        status = TD1208_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Reset parser.
    _TD1208_reset_replies();
    // Send command.
    status = _TD1208_send_command("ATI7");
    if (status != TD1208_SUCCESS) goto errors;
    // Wait for command completion.
    status = _TD1208_wait_for_string("OK");
    if (status != TD1208_SUCCESS) goto errors;
    // Search ID in all replies.
    for (rep_idx = 0; rep_idx < TD1208_REPLY_BUFFER_DEPTH; rep_idx++) {
        // Replace unwanted characters.
        for (idx = 0; idx < TD1208_BUFFER_SIZE_BYTES; idx++) {
            temp_char = td1208_ctx.reply[rep_idx].buffer[idx];
            if ((temp_char == STRING_CHAR_CR) || (temp_char == STRING_CHAR_LF)) {
                td1208_ctx.reply[rep_idx].buffer[idx] = STRING_CHAR_NULL;
            }
        }
        // Count the number of characters.
        line_size = 0;
        for (idx = 0; idx < TD1208_BUFFER_SIZE_BYTES; idx++) {
            if (td1208_ctx.reply[rep_idx].buffer[idx] == STRING_CHAR_NULL) {
                break;
            }
            else {
                line_size++;
            }
        }
        // Check length.
        if ((line_size > 0) && (line_size <= TD1208_SIGFOX_EP_ID_SIZE_CHAR)) {
            // Shift buffer.
            for (idx = (TD1208_SIGFOX_EP_ID_SIZE_CHAR - 1); idx >= (TD1208_SIGFOX_EP_ID_SIZE_CHAR - line_size); idx--) {
                td1208_ctx.reply[rep_idx].buffer[idx] = td1208_ctx.reply[rep_idx].buffer[idx - line_size];
            }
            // Pad most significant symbols with zeroes.
            for (; idx >= 0; idx--) {
                td1208_ctx.reply[rep_idx].buffer[idx] = '0';
            }
            // Try parsing ID.
            td1208_ctx.reply[rep_idx].parser.buffer_size = TD1208_SIGFOX_EP_ID_SIZE_CHAR;
            parser_status = PARSER_get_byte_array(&td1208_ctx.reply[rep_idx].parser, STRING_CHAR_NULL, TD1208_SIGFOX_EP_ID_SIZE_BYTES, 1, sigfox_ep_id, &extracted_size);
            // Update status.
            if (parser_status == PARSER_SUCCESS) {
                status = TD1208_SUCCESS;
                goto errors;
            }
            else {
                status = (TD1208_ERROR_BASE_PARSER + parser_status);
            }
        }
        else {
            status = (TD1208_ERROR_BASE_PARSER + PARSER_ERROR_BYTE_ARRAY_SIZE);
        }
    }
errors:
    return status;
}

/*******************************************************************/
TD1208_status_t TD1208_send_bit(uint8_t ul_bit) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    char_t command[TD1208_BUFFER_SIZE_BYTES] = { 0x00 };
    // Reset parser.
    _TD1208_reset_replies();
    // Build AT command.
    command[0] = 'A';
    command[1] = 'T';
    command[2] = '$';
    command[3] = 'S';
    command[4] = 'B';
    command[5] = '=';
    command[6] = ul_bit ? '1' : '0';
    command[7] = ',';
    command[8] = '2';
    command[9] = STRING_CHAR_NULL;
    // Send command through UART.
    status = _TD1208_send_command(command);
    if (status != TD1208_SUCCESS) goto errors;
    // Wait for reply.
    status = _TD1208_wait_for_string("OK");
    if (status != TD1208_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TD1208_status_t TD1208_send_frame(uint8_t* ul_payload, uint8_t ul_payload_size_bytes) {
    // Local variables.
    TD1208_status_t status = TD1208_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
    char_t command[TD1208_BUFFER_SIZE_BYTES] = { 0x00 };
    uint8_t idx = 0;
    // Check parameters.
    if (ul_payload == NULL) {
        status = TD1208_ERROR_NULL_PARAMETER;
        goto errors;
    }
    if (ul_payload_size_bytes > TD1208_SIGFOX_UL_PAYLOAD_SIZE_BYTES_MAX) {
        status = TD1208_ERROR_SIGFOX_UL_PAYLOAD_SIZE;
        goto errors;
    }
    // Reset parser.
    _TD1208_reset_replies();
    // Header.
    command[idx++] = 'A';
    command[idx++] = 'T';
    command[idx++] = '$';
    command[idx++] = 'S';
    command[idx++] = 'F';
    command[idx++] = '=';
    // Data.
    string_status = STRING_byte_array_to_hexadecimal_string(ul_payload, ul_payload_size_bytes, 0, (char_t*) &(command[idx]));
    STRING_exit_error(TD1208_ERROR_BASE_STRING);
    // AT command end.
    idx += (uint8_t) (ul_payload_size_bytes * MATH_U8_SIZE_HEXADECIMAL_DIGITS);
    command[idx++] = STRING_CHAR_NULL;
    // Send command through UART.
    status = _TD1208_send_command(command);
    if (status != TD1208_SUCCESS) goto errors;
    // Wait for reply.
    status = _TD1208_wait_for_string("OK");
    if (status != TD1208_SUCCESS) goto errors;
errors:
    return status;
}

#endif /* TD1208_DRIVER_DISABLE */
