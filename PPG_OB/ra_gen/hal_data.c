/* generated HAL source file - do not edit */
#include "hal_data.h"
sci_uart_instance_ctrl_t g_uart0_ctrl;

baud_setting_t g_uart0_baud_setting =
        {
        /* Baud rate calculated with 0.469% error. */.semr_baudrate_bits_b.abcse = 0,
          .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 53, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme =
                  false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart0_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart0_baud_setting, .flow_control =
          SCI_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
  .rs485_setting =
  { .enable = SCI_UART_RS485_DISABLE, .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
#if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
    .de_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
          }, };

/** UART interface configuration */
const uart_cfg_t g_uart0_cfg =
{ .channel = 5, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          user_uart_callback,
  .p_context = NULL, .p_extend = &g_uart0_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (12),
  .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
#if defined(VECTOR_NUMBER_SCI5_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI5_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI5_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI5_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI5_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI5_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI5_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI5_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart0 =
{ .p_ctrl = &g_uart0_ctrl, .p_cfg = &g_uart0_cfg, .p_api = &g_uart_on_sci };
/* I2C Communication Device */
#define RA_NOT_DEFINED (1)
rm_comms_i2c_instance_ctrl_t g_comms_i2c_device0_ctrl;

/* Lower level driver configuration */
const i2c_master_cfg_t g_comms_i2c_device0_lower_level_cfg =
{ .slave = 0x53, .addr_mode = I2C_MASTER_ADDR_MODE_7BIT, .p_callback = rm_comms_i2c_callback, };

const rm_comms_cfg_t g_comms_i2c_device0_cfg =
{ .semaphore_timeout = 0xFFFFFFFF, .p_lower_level_cfg = (void*) &g_comms_i2c_device0_lower_level_cfg, .p_extend =
          (void*) &g_comms_i2c_bus0_extended_cfg,
  .p_callback = rm_ob1203_comms_i2c_callback,
#if defined(g_ob1203_sensor0_ctrl)
    .p_context          = g_ob1203_sensor0_ctrl,
#else
  .p_context = (void*) &g_ob1203_sensor0_ctrl,
#endif
        };

const rm_comms_instance_t g_comms_i2c_device0 =
{ .p_ctrl = &g_comms_i2c_device0_ctrl, .p_cfg = &g_comms_i2c_device0_cfg, .p_api = &g_comms_on_comms_i2c, };
extern rm_ob1203_api_t const g_ob1203_on_ob1203_light_proximity;
rm_ob1203_mode_extended_cfg_t g_ob1203_sensor0_extended_cfg =
{ .p_api = (void*) &g_ob1203_on_ob1203_light_proximity,
  .mode_irq = RM_OB1203_OPERATION_MODE_LIGHT,
  .light_sensor_mode = RM_OB1203_LIGHT_SENSOR_MODE_CS,
  .light_interrupt_type = RM_OB1203_LIGHT_INTERRUPT_TYPE_THRESHOLD,
  .light_interrupt_source = RM_OB1203_LIGHT_INTERRUPT_SOURCE_CLEAR_CHANNEL,
  .light_interrupt_persist = 0x02,
  .light_sleep = RM_OB1203_SLEEP_AFTER_INTERRUPT_DISABLE,
  .light_gain = RM_OB1203_LIGHT_GAIN_3,
  .light_upper_threshold = 0x00CCC,
  .light_lower_threshold = 0x00000,
  .light_variance_threshold = RM_OB1203_VARIANCE_THRESHOLD_128_COUNTS,
  .light_resolution_period = RM_OB1203_LIGHT_RESOLUTION_18BIT_PERIOD_100MS,
  .ppg_prox_gain = RM_OB1203_PPG_PROX_GAIN_1,
  .led_order = RM_OB1203_LED_IR_FIRST_RED_SECOND,
  .prox_sleep = RM_OB1203_SLEEP_AFTER_INTERRUPT_DISABLE,
  .prox_interrupt_type = RM_OB1203_PROX_INTERRUPT_TYPE_NORMAL,
  .prox_interrupt_persist = 0x02,
  .prox_led_current = 0x100,
  .prox_ana_can = RM_OB1203_ANALOG_CANCELLATION_DISABLE,
  .prox_dig_can = 0x100,
  .prox_num_led_pulses = RM_OB1203_NUM_LED_PULSES_8,
  .prox_upper_threshold = 0x0600,
  .prox_lower_threshold = 0x0000,
  .prox_width_period = RM_OB1203_PROX_WIDTH_42US_PERIOD_100MS,
  .prox_moving_average = RM_OB1203_MOVING_AVERAGE_DISABLE,
  .prox_hysteresis = 0x00, };
#if BSP_CFG_RTOS
#if BSP_CFG_RTOS == 1
TX_SEMAPHORE g_ob1203_sensor0_semaphore_handle;
CHAR g_ob1203_sensor0_semaphore_name[] = "g_ob1203_sensor0 semaphore";
#elif BSP_CFG_RTOS == 2
SemaphoreHandle_t g_ob1203_sensor0_semaphore_handle;
StaticSemaphore_t g_ob1203_sensor0_semaphore_memory;
#endif

/* Semaphore to wait for callback  */
rm_ob1203_semaphore_t g_ob1203_sensor0_semaphore =
{
    .p_semaphore_handle = &g_ob1203_sensor0_semaphore_handle,
#if BSP_CFG_RTOS == 1 // ThreadX
    .p_semaphore_name = &g_ob1203_sensor0_semaphore_name[0],
#elif BSP_CFG_RTOS == 2 // FreeRTOS
    .p_semaphore_memory = &g_ob1203_sensor0_semaphore_memory,
#endif
};
#endif

rm_ob1203_instance_ctrl_t g_ob1203_sensor0_ctrl;
const rm_ob1203_cfg_t g_ob1203_sensor0_cfg =
{
#if BSP_CFG_RTOS
    .p_semaphore = &g_ob1203_sensor0_semaphore,
#endif
  .semaphore_timeout = 0xFFFFFFFF,
  .p_comms_instance = &g_comms_i2c_device0,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_irq_instance = NULL,
  .p_irq_callback = NULL,
#else
    .p_irq_instance = &RA_NOT_DEFINED,
    .p_irq_callback = ob1203_irq_callback,
#endif
#undef RA_NOT_DEFINED
  .p_comms_callback = ob1203_comms_i2c_callback,
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = &NULL,
#endif
  .p_extend = (void*) &g_ob1203_sensor0_extended_cfg, };

const rm_ob1203_instance_t g_ob1203_sensor0 =
{ .p_ctrl = &g_ob1203_sensor0_ctrl, .p_cfg = &g_ob1203_sensor0_cfg, .p_api = &g_ob1203_on_ob1203, };
void g_hal_init(void)
{
    g_common_init ();
}
