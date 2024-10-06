#include "hal_data.h"
#include "stdio.h"
#include "string.h"

// Khai báo các hàm và biến
void R_BSP_WarmStart(bsp_warm_start_event_t event);
void user_uart_callback(uart_callback_args_t *p_args);
void console_write(const char *buffer);
void console_read(char *buffer, uint32_t length);
void hs300x_callback(rm_hs300x_callback_args_t * p_args);

static volatile bool is_transfer_complete = false;
extern bsp_leds_t g_bsp_leds;
static volatile bool hs300x_complete = false;

void hal_entry (void)
{
#if BSP_TZ_SECURE_BUILD
    R_BSP_NonSecureEnter();
#endif

    const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;
    const uint32_t freq_in_hz = 1;
    const uint32_t delay = bsp_delay_units * 2 / freq_in_hz;
    bsp_leds_t leds = g_bsp_leds;

    if (0 == leds.led_count)
    {
        while (1) { ; }
    }

    char write_buffer[200] = {};
    char read_buffer[10] = {};
    uint8_t humidity_threshold = 58;  // Ngưỡng độ ẩm mặc định

    fsp_err_t            err = FSP_SUCCESS;
    rm_hs300x_raw_data_t hs300x_raw_data;
    rm_hs300x_data_t     hs300x_data;
    uint8_t              calculated_flag = 0;

    // Mở UART và cảm biến
    R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    rm_comms_i2c_bus_extended_cfg_t * p_extend_hs300x =
    (rm_comms_i2c_bus_extended_cfg_t *)g_hs300x_sensor0_cfg.p_instance->p_cfg->p_extend;
    i2c_master_instance_t * p_driver_instance_hs300x = (i2c_master_instance_t *) p_extend_hs300x->p_driver_instance;
    p_driver_instance_hs300x->p_api->open(p_driver_instance_hs300x->p_ctrl, p_driver_instance_hs300x->p_cfg);
    RM_HS300X_Open(&g_hs300x_sensor0_ctrl, &g_hs300x_sensor0_cfg);

    R_BSP_PinAccessEnable();
    while(1)
    {
        // Đo nhiệt độ và độ ẩm từ cảm biến HS300x
        hs300x_complete = false;
        RM_HS300X_MeasurementStart(&g_hs300x_sensor0_ctrl);
        while (false == hs300x_complete) { ; }
        do
        {
            hs300x_complete = false;
            RM_HS300X_Read(&g_hs300x_sensor0_ctrl, &hs300x_raw_data);
            while (false == hs300x_complete) { ; }
            err = RM_HS300X_DataCalculate(&g_hs300x_sensor0_ctrl, &hs300x_raw_data, &hs300x_data);
            if (FSP_SUCCESS == err)
            {
                calculated_flag = 1;
            }
            else if (FSP_ERR_SENSOR_INVALID_DATA == err)
            {
                calculated_flag = 0;
            }
        } while (0 == calculated_flag);

        sprintf(write_buffer, "Humidity value: %d.%d \n\r",hs300x_data.humidity.integer_part, hs300x_data.humidity.decimal_part);
        console_write(write_buffer);

        sprintf(write_buffer, "Temperature value: %d.%d \n\r",hs300x_data.temperature.integer_part, hs300x_data.temperature.decimal_part);
        console_write(write_buffer);

        if (hs300x_data.humidity.integer_part > humidity_threshold)
        {
            R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[0], BSP_IO_LEVEL_HIGH); // Bật đèn đỏ
        }
        else
        {
            R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[0], BSP_IO_LEVEL_LOW);  // Tắt đèn đỏ
        }

        // Thêm đoạn mã mới để nhận số từ người dùng
        char input_buffer[200] = {};
        int input_number = 0;
        sprintf(write_buffer, "Nhap vao so: ");
        console_write(write_buffer);

        // Đọc số từ người dùng
        console_read(input_buffer, 199);
        input_buffer[2] = '\0';
        input_number = atoi(input_buffer);
        printTree(input_number);
        // Hiển thị số nhận được

        sprintf(write_buffer, "Da nhan: %d \n\r", input_number);
        console_write(write_buffer);

//        // Điều khiển đèn đỏ dựa trên số nhận được
//        if (input_number == 0)
//        {
//            R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[0], BSP_IO_LEVEL_HIGH); // Bật đèn đỏ
//        }
//        else if (input_number == 1)
//        {
//            R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[1], BSP_IO_LEVEL_HIGH);  // Tắt đèn đỏ
//        }
//        else if (input_number == 2)
//        {
//            R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[2], BSP_IO_LEVEL_HIGH);  // Tắt đèn đỏ
//        }

        R_BSP_SoftwareDelay(delay, bsp_delay_units);
    }

    R_BSP_PinAccessDisable();
}

// Hàm khởi động
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0
        R_FACI_LP->DFLCTL = 1U;
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        R_IOPORT_Open(&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}

// Callback cho UART
void user_uart_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event){
        case UART_EVENT_TX_COMPLETE:
        case UART_EVENT_RX_COMPLETE:
        {
            is_transfer_complete = true;
            break;
        }
        default:
        {
        }
    }
}

// Hàm ghi dữ liệu ra console
void console_write(const char *buffer){
    is_transfer_complete = false;
    R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) buffer, strlen(buffer));
    while (!is_transfer_complete){
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }
}

// Hàm đọc dữ liệu từ console
void console_read(char *buffer, uint32_t length){
    is_transfer_complete = false;
    R_SCI_UART_Read(&g_uart0_ctrl, (uint8_t *) buffer, length);
    while (!is_transfer_complete){
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }
}

// Callback cho cảm biến HS300x
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

void printTree(int height) {
    char write_buffer[256];  // Đệm để chứa chuỗi định dạng
    sprintf(write_buffer, "\r\n");
   console_write(write_buffer);
    for (int i = 0; i < height; i++) {
        // In khoảng trắng
        for (int j = 0; j < height - i - 1; j++) {
            sprintf(write_buffer, " ");
            console_write(write_buffer);
        }


        // In các số
        for (int k = 0; k < (2 * i + 1); k++) {
            sprintf(write_buffer, "*");
            console_write(write_buffer);
        }

        // Kết thúc dòng
        sprintf(write_buffer, "\r\n");
        console_write(write_buffer);

    }
}
