/*
 * Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_data.h"
#include <stdio.h>

void R_BSP_WarmStart(bsp_warm_start_event_t event);

void user_uart_callback(uart_callback_args_t *p_args);
void console_write(const char *buffer);
static volatile bool is_transfer_complete = false;

void g_comms_i2c_bus0_quick_setup(void);
uint32_t find_max(uint32_t r, uint32_t g, uint32_t b);
void g_ob1203_sensor0_quick_setup(void);
void g_ob1203_sensor0_quick_getting_light_mode_data(rm_ob1203_light_data_t *p_ob1203_data,
        rm_ob1203_light_data_type_t data_type);
void g_ob1203_sensor0_quick_getting_proximity_mode_data(rm_ob1203_prox_data_t *p_ob1203_data);
extern bsp_leds_t g_bsp_leds;
/*******************************************************************************************************************//**
 * @brief  Blinky example application
 *
 * Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
 *
 **********************************************************************************************************************/
void hal_entry(void)
{
#if BSP_TZ_SECURE_BUILD

   /* Enter non-secure code */
   R_BSP_NonSecureEnter();
#endif

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    /* If this board has no LEDs then trap here */
    if (0 == leds.led_count)
    {
        while (1)
        {
            ;                          // There are no LEDs on this board
        }
    }

    /* Define the units to be used with the software delay function */
    const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

    /* Set the blink frequency (must be <= bsp_delay_units */
    const uint32_t freq_in_hz = 2;

    /* Calculate the delay in terms of bsp_delay_units */
    const uint32_t delay = bsp_delay_units / freq_in_hz;

    char write_buffer[200] =
    { };
    /* Initialize UART */
    R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);

    /* Initialize I2C */
    g_comms_i2c_bus0_quick_setup ();

    /* Initialize ob1203 */
    g_ob1203_sensor0_quick_setup ();

    while (1)
    {
        R_BSP_PinAccessEnable ();

        /* OB1203 DATA */
        rm_ob1203_light_data_t p_ob1203_data;
        rm_ob1203_light_data_type_t data_type = RM_OB1203_LIGHT_DATA_TYPE_ALL;
        g_ob1203_sensor0_quick_getting_light_mode_data (&p_ob1203_data, data_type);

        sprintf (write_buffer, "\n\rOB1203 LIGHT:\n\r");
        console_write (write_buffer);

        sprintf (write_buffer, "clear_data: %lu \n\r", p_ob1203_data.clear_data);
        console_write (write_buffer);
        sprintf (write_buffer, "green_data: %lu \n\r", p_ob1203_data.green_data);
        console_write (write_buffer);
        sprintf (write_buffer, "blue_data: %lu \n\r", p_ob1203_data.blue_data);
        console_write (write_buffer);
        sprintf (write_buffer, "red_data: %lu \n\r", p_ob1203_data.red_data);
        console_write (write_buffer);
        sprintf (write_buffer, "comp_data: %lu \n\r", p_ob1203_data.comp_data);
        console_write (write_buffer);

        uint32_t index = find_max (p_ob1203_data.red_data, p_ob1203_data.green_data, p_ob1203_data.blue_data);
        sprintf (write_buffer, "max index: %lu \n\r", index);
        console_write (write_buffer);

        rm_ob1203_prox_data_t p_ob1203_data_2;
        g_ob1203_sensor0_quick_getting_proximity_mode_data (&p_ob1203_data_2);

        sprintf (write_buffer, "\n\rOB1203 Proximity:\n\r");
        console_write (write_buffer);

        sprintf (write_buffer, "proximity_data: %u \n\r", p_ob1203_data_2.proximity_data);
        console_write (write_buffer);

        /* Delay */
        R_BSP_SoftwareDelay (delay, bsp_delay_units);

        /* Send clear screen message & cursor home command */
        sprintf (write_buffer, "\x1b[H");
        console_write (write_buffer);


        for (uint32_t i = 0; i < 3; i++)
                   {
                       /* Get pin to toggle */
                       uint32_t pin = leds.p_leds[i];

                       /* Turn off the current LED */
                       R_BSP_PinWrite((bsp_io_port_pin_t)pin, BSP_IO_LEVEL_LOW);
                   }
        /* Write to this pin */
        R_BSP_PinWrite ((bsp_io_port_pin_t) leds.p_leds[index], BSP_IO_LEVEL_HIGH);  // Turn on the current LED



//       /* Send clear screen message & cursor home command */
//       sprintf (write_buffer, "\x1b[H");
//       console_write (write_buffer);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

       /* Enable reading from data flash. */
       R_FACI_LP->DFLCTL = 1U;

       /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
        * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}

void user_uart_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:
        {
            is_transfer_complete = true;
            break;
        }
        default:
        {
            /* Do nothing */
            break;
        }
    }
}

uint32_t find_max(uint32_t r, uint32_t g, uint32_t b)
{
    uint32_t max_val = r;

    if (g > max_val)
    {
        max_val = g;
    }

    if (b > max_val)
    {
        max_val = b;
    }

    if (max_val == r)
    {
        return 0;
    }

    else if (max_val == g)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

void console_write(const char *buffer)
{
    is_transfer_complete = false;

    R_SCI_UART_Write (&g_uart0_ctrl, (uint8_t*) buffer, strlen (buffer));

    while (!is_transfer_complete)
    {
        R_BSP_SoftwareDelay (1, BSP_DELAY_UNITS_MICROSECONDS);
    }
}

/* TODO: Enable if you want to open I2C bus */

/* Quick setup for i2c_bus_name. */
void g_comms_i2c_bus0_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t *p_driver_instance = (i2c_master_instance_t*) g_comms_i2c_bus0_extended_cfg.p_driver_instance;

    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open (p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    assert(FSP_SUCCESS == err);
}

/* TODO: Enable if you want to open OB1203 */
#define G_OB1203_SENSOR0_NON_BLOCKING (1)
#define G_OB1203_SENSOR0_IRQ_ENABLE   (0)

#if G_OB1203_SENSOR0_NON_BLOCKING
volatile bool g_ob1203_i2c_completed = false;
#endif
#if G_OB1203_SENSOR0_IRQ_ENABLE
volatile bool g_ob1203_irq_completed = false;
#endif

/* TODO: Enable if you want to use a callback */
#define G_OB1203_SENSOR0_CALLBACK_ENABLE (1)
#if G_OB1203_SENSOR0_CALLBACK_ENABLE
void ob1203_comms_i2c_callback(rm_ob1203_callback_args_t *p_args)
{
#if G_OB1203_SENSOR0_NON_BLOCKING
    if (RM_OB1203_EVENT_ERROR != p_args->event)
    {
        g_ob1203_i2c_completed = true;
    }
#else
   FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* TODO: Enable if you want to use a IRQ callback */
#define G_OB1203_SENSOR0_IRQ_CALLBACK_ENABLE (0)
#if G_OB1203_SENSOR0_IRQ_CALLBACK_ENABLE
void ob1203_irq_callback(rm_ob1203_callback_args_t * p_args)
{
#if ob1203_name_upper_IRQ_ENABLE
   FSP_PARAMETER_NOT_USED(p_args);
   g_ob1203_irq_completed = true;
#else
   FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* Quick setup for ob1203_name.
 * - i2c_bus_name must be setup before calling this function
 *     (See Developer Assistance -> ob1203_name -> OB1203 ***** on rm_ob1203 -> i2c_device_name -> i2c_bus_name -> Quick Setup).
 */

/* Quick setup for ob1203_name. */
void g_ob1203_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open OB1203 instance, this must be done before calling any OB1203 API */
    err = g_ob1203_sensor0.p_api->open (g_ob1203_sensor0.p_ctrl, g_ob1203_sensor0.p_cfg);
    assert(FSP_SUCCESS == err);
}

/* Quick getting Light mode values for ob1203_name.
 * - ob1203_name must be setup before calling this function.
 */

/* Quick getting Light data for ob1203_name. */
void g_ob1203_sensor0_quick_getting_light_mode_data(rm_ob1203_light_data_t *p_ob1203_data,
        rm_ob1203_light_data_type_t data_type)
{
    fsp_err_t err;
    rm_ob1203_raw_data_t ob1203_raw_data;
#if 0 == ob1203_name_upper_IRQ_ENABLE
    rm_ob1203_device_status_t device_status;
#endif

    /* Clear I2C callback flag */
#if G_OB1203_SENSOR0_NON_BLOCKING
    g_ob1203_i2c_completed = false;
#endif

    /* Start the measurement */
    /* If the MeasurementStart API is called once, a second call is not required. */
    err = g_ob1203_sensor0.p_api->measurementStart (g_ob1203_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
    while (!g_ob1203_i2c_completed)
    {
        ;
    }
    g_ob1203_i2c_completed = false;
#endif

    /* If IRQ interrupt is enabled, wait IRQ callback */
#if G_OB1203_SENSOR0_IRQ_ENABLE
   while (!g_ob1203_irq_completed)
   {
       ;
   }
   g_ob1203_irq_completed = false;
#else
    /* Wait until the measurement is complete */
    do
    {
        /* Get device status */
        err = g_ob1203_sensor0.p_api->deviceStatusGet (g_ob1203_sensor0.p_ctrl, &device_status);
        assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
        while (!g_ob1203_i2c_completed)
        {
            ;
        }
        g_ob1203_i2c_completed = false;
#endif
    }
    while (false == device_status.light_measurement_complete);
#endif

    /* Read ADC data */
    err = g_ob1203_sensor0.p_api->lightRead (g_ob1203_sensor0.p_ctrl, &ob1203_raw_data, data_type);
    assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
    while (!g_ob1203_i2c_completed)
    {
        ;
    }
    g_ob1203_i2c_completed = false;
#endif

    /* Calculate Light data */
    err = g_ob1203_sensor0.p_api->lightDataCalculate (g_ob1203_sensor0.p_ctrl, &ob1203_raw_data, p_ob1203_data);
    assert(FSP_SUCCESS == err);
}

/* Quick getting Proximity mode values for ob1203_name.
 * - ob1203_name must be setup before calling this function.
 */

/* Quick getting Proximity data for ob1203_name. */
void g_ob1203_sensor0_quick_getting_proximity_mode_data(rm_ob1203_prox_data_t *p_ob1203_data)
{
    fsp_err_t err;
    rm_ob1203_raw_data_t ob1203_raw_data;
#if 0 == G_OB1203_SENSOR0_IRQ_ENABLE
    rm_ob1203_device_status_t device_status;
#endif

    /* Clear I2C callback flag */
#if G_OB1203_SENSOR0_NON_BLOCKING
    g_ob1203_i2c_completed = false;
#endif

    /* Start the measurement */
    /* If the MeasurementStart API is called once, a second call is not required. */
    err = g_ob1203_sensor0.p_api->measurementStart (g_ob1203_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
    while (!g_ob1203_i2c_completed)
    {
        ;
    }
    g_ob1203_i2c_completed = false;
#endif

    /* If IRQ interrupt is enabled, wait IRQ callback */
#if G_OB1203_SENSOR0_IRQ_ENABLE
   while (!g_ob1203_irq_completed)
   {
       ;
   }
   g_ob1203_irq_completed = false;
#else
    /* Wait until the measurement is complete */
    do
    {
        /* Get device status */
        err = g_ob1203_sensor0.p_api->deviceStatusGet (g_ob1203_sensor0.p_ctrl, &device_status);
        assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
        while (!g_ob1203_i2c_completed)
        {
            ;
        }
        g_ob1203_i2c_completed = false;
#endif
    }
    while (false == device_status.prox_measurement_complete);
#endif

    /* Read ADC data */
    err = g_ob1203_sensor0.p_api->proxRead (g_ob1203_sensor0.p_ctrl, &ob1203_raw_data);
    assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
    while (!g_ob1203_i2c_completed)
    {
        ;
    }
    g_ob1203_i2c_completed = false;
#endif

    /* Calculate Proximity data */
    err = g_ob1203_sensor0.p_api->proxDataCalculate (g_ob1203_sensor0.p_ctrl, &ob1203_raw_data, p_ob1203_data);
    assert(FSP_SUCCESS == err);
}
