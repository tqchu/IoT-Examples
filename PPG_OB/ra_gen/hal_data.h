/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "rm_comms_i2c.h"
#include "rm_comms_api.h"
#include "rm_ob1203.h"
#include "rm_ob1203_api.h"
FSP_HEADER
/** UART on SCI Instance. */
extern const uart_instance_t g_uart0;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart0_ctrl;
extern const uart_cfg_t g_uart0_cfg;
extern const sci_uart_extended_cfg_t g_uart0_cfg_extend;

#ifndef user_uart_callback
void user_uart_callback(uart_callback_args_t *p_args);
#endif
/* I2C Communication Device */
extern const rm_comms_instance_t g_comms_i2c_device0;
extern rm_comms_i2c_instance_ctrl_t g_comms_i2c_device0_ctrl;
extern const rm_comms_cfg_t g_comms_i2c_device0_cfg;
#ifndef rm_ob1203_comms_i2c_callback
void rm_ob1203_comms_i2c_callback(rm_comms_callback_args_t *p_args);
#endif
/* OB1203 Light Proximity mode */
extern rm_ob1203_mode_extended_cfg_t g_ob1203_sensor0_extended_cfg;
/* OB1203 Sensor */
extern const rm_ob1203_instance_t g_ob1203_sensor0;
extern rm_ob1203_instance_ctrl_t g_ob1203_sensor0_ctrl;
extern const rm_ob1203_cfg_t g_ob1203_sensor0_cfg;
#ifndef ob1203_comms_i2c_callback
void ob1203_comms_i2c_callback(rm_ob1203_callback_args_t *p_args);
#endif
#ifndef RA_NOT_DEFINED
void RA_NOT_DEFINED(external_irq_callback_args_t *p_args);
#endif
#ifndef ob1203_irq_callback
void ob1203_irq_callback(rm_ob1203_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
