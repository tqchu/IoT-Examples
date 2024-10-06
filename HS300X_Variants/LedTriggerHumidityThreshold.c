    /*
    * Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
    *
    * SPDX-License-Identifier: BSD-3-Clause
    */

    #include "hal_data.h"
    #include "stdio.h"

    void R_BSP_WarmStart(bsp_warm_start_event_t event);

    void user_uart_callback(uart_callback_args_t *p_args);
    void console_write(const char *buffer);

    static volatile bool is_transfer_complete = false;
    extern bsp_leds_t g_bsp_leds;

    void hs300x_callback(rm_hs300x_callback_args_t * p_args);
    static volatile bool hs300x_complete = false;

    /*******************************************************************************************************************//**
    * @brief  Blinky example application
    *
    * Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
    *
    **********************************************************************************************************************/
    void hal_entry (void)
    {
    #if BSP_TZ_SECURE_BUILD

        /* Enter non-secure code */
        R_BSP_NonSecureEnter();
    #endif

        /* Define the units to be used with the software delay function */
        const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

        /* Set the blink frequency (must be <= bsp_delay_units */
        const uint32_t freq_in_hz = 1;

        /* Calculate the delay in terms of bsp_delay_units */
        const uint32_t delay = bsp_delay_units * 2 / freq_in_hz;

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


        //  char write_buffer[4][200] = {};
        char write_buffer[200] = {};
        fsp_err_t            err = FSP_SUCCESS;
        rm_hs300x_raw_data_t hs300x_raw_data;
        rm_hs300x_data_t     hs300x_data;
        uint8_t              calculated_flag = 0;

        R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
        rm_comms_i2c_bus_extended_cfg_t * p_extend_hs300x =
        (rm_comms_i2c_bus_extended_cfg_t *)g_hs300x_sensor0_cfg.p_instance->p_cfg->p_extend;
        i2c_master_instance_t * p_driver_instance_hs300x = (i2c_master_instance_t *) p_extend_hs300x->p_driver_instance;
        p_driver_instance_hs300x->p_api->open(p_driver_instance_hs300x->p_ctrl, p_driver_instance_hs300x->p_cfg);


        /* Initialize sensor */
        RM_HS300X_Open(&g_hs300x_sensor0_ctrl, &g_hs300x_sensor0_cfg);

        R_BSP_PinAccessEnable();
        while(1){


            /* Delete old log in tera term */
           sprintf(write_buffer,"\x1b[H");
           console_write(write_buffer);

           hs300x_complete = false;
           /* Start Measurement */
           RM_HS300X_MeasurementStart(&g_hs300x_sensor0_ctrl);
           while (false == hs300x_complete)
           {
               /* Wait callback */
           }
           do
           {
               hs300x_complete = false;
               /* Read ADC Data from HS300X */
               RM_HS300X_Read(&g_hs300x_sensor0_ctrl, &hs300x_raw_data);
               while (false == hs300x_complete)
               {
                   /* Wait callback */
               }
               /* Calculate Humidity and Temperatuere values from ADC data */
               err = RM_HS300X_DataCalculate(&g_hs300x_sensor0_ctrl, &hs300x_raw_data, &hs300x_data);
               if (FSP_SUCCESS == err)
               {
                   calculated_flag = 1;

               }
               else if (FSP_ERR_SENSOR_INVALID_DATA == err)
               {
                   /* Stale data */
                   calculated_flag = 0;
               }
           } while (0 == calculated_flag);

           uint32_t pinRed = leds.p_leds[0];
          uint32_t pinGreen = leds.p_leds[1];

          /* Get the same RGB pin*/
               uint32_t RGBRedPin = leds.p_leds[3];
               uint32_t RGBGreenPin = leds.p_leds[4];

               //turn of blue led
               uint32_t pinBlue = leds.p_leds[5];
               R_BSP_PinWrite((bsp_io_port_pin_t)pinBlue, BSP_IO_LEVEL_HIGH);

           if(hs300x_data.humidity.integer_part>65){
                          R_BSP_PinWrite((bsp_io_port_pin_t)pinRed, BSP_IO_LEVEL_HIGH);
                          R_BSP_PinWrite((bsp_io_port_pin_t)pinGreen, BSP_IO_LEVEL_LOW);

                          //RGB
                          R_BSP_PinWrite((bsp_io_port_pin_t)RGBRedPin, BSP_IO_LEVEL_LOW);

                          R_BSP_PinWrite((bsp_io_port_pin_t)RGBGreenPin, BSP_IO_LEVEL_HIGH);

           }else{
               R_BSP_PinWrite((bsp_io_port_pin_t)pinRed, BSP_IO_LEVEL_LOW);
             R_BSP_PinWrite((bsp_io_port_pin_t)pinGreen, BSP_IO_LEVEL_HIGH);

             R_BSP_PinWrite((bsp_io_port_pin_t)RGBGreenPin, BSP_IO_LEVEL_LOW);
                                  R_BSP_PinWrite((bsp_io_port_pin_t)RGBRedPin, BSP_IO_LEVEL_HIGH);
           }


           sprintf(write_buffer, "Humidity value: %d.%d \n\r",hs300x_data.humidity.integer_part, hs300x_data.humidity.decimal_part);
            console_write(write_buffer);


             sprintf(write_buffer, "Temperature value: %d.%d \n\r",hs300x_data.temperature.integer_part, hs300x_data.temperature.decimal_part);
             console_write(write_buffer);

             R_BSP_SoftwareDelay(delay, bsp_delay_units);
        }

            /* Protect PFS registers */
            R_BSP_PinAccessDisable();

    }

    /*******************************************************************************************************************//**
    * This function is called at various points during the startup process.  This implementation uses the event that is
    * called right before main() to set up the pins.
    *
    * @param[in]  event    Where at in the start up process the code is currently at
    **********************************************************************************************************************/
    void R_BSP_WarmStart (bsp_warm_start_event_t event)
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
            R_IOPORT_Open(&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
        }
    }

    void user_uart_callback(uart_callback_args_t *p_args)
    {
        switch (p_args->event){
            case UART_EVENT_TX_COMPLETE:
            {
                is_transfer_complete = true;
                break;
            }
            default:
            {

            }
        }
    }
    void console_write(const char *buffer){
        is_transfer_complete = false;
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) buffer, strlen(buffer));
        while (!is_transfer_complete){
            R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
        }
    }

    void hs300x_callback(rm_hs300x_callback_args_t * p_args){
        switch (p_args->event){
            case RM_HS300X_EVENT_SUCCESS:
            {
                hs300x_complete = true;
                break;
            }
            default:
            {

            }
        }
    }

