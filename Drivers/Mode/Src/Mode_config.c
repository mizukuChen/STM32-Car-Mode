// 直行模组

#include <main.h>
#include <Mode_config.h>

void config_find_encoder()
{
    int16_t encoder[2] = {0, 0};
    char msgA[20], msgB[20];
    __HAL_TIM_DISABLE(&htim7);
    reset_encoder();
    Set_PWM(60, 60);
    HAL_Delay(150);
    trace_to_cross(60);

    read_encoder(encoder);
    sprintf(msgA, "Encoder1: %d", encoder[0]);
    OLED_ShowString(0, 20, msgA, 8, 1);
    sprintf(msgB, "Encoder2: %d", encoder[1]);
    OLED_ShowString(0, 30, msgB, 8, 1);
    OLED_Refresh();
    stop();
    while (1)
    {
    }
}

void config_speed_by_encoder(float speedL, float speedR)
{
    char msg1[10], msg2[10];
    PID PWMPID;
    PWMPID.Kp = 0.01, PWMPID.Ki = 0, PWMPID.Kd = 0;
    PWMPID.target = 0, PWMPID.minOutput = -2, PWMPID.maxOutput = 2;
    PWMPID.error = 0, PWMPID.last_error = 0, PWMPID.integral = 0;

    int16_t encoder[2]; // 读编码器数值

    __HAL_TIM_DISABLE(&htim7); // 关tim7防止中断影响
    __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

    if (speedL == 0)
    {
        speedL = speedR;
        do
        {
            set_encoder(0, 0);
            Set_PWM(speedL, speedR);
            HAL_Delay(500);
            read_encoder(encoder);

            sprintf(msg1, ":%d,%d", encoder[1], encoder[0]);
            OLED_ShowString(0, 20, msg1, 8, 1);
            sprintf(msg2, "R:%.2f,L:%.2f", speedR, speedL);
            OLED_ShowString(0, 30, msg2, 8, 1);
            OLED_Refresh();

            speedL = speedL + PID_Compute(&PWMPID, encoder[0] - encoder[1]);

        } while (abs(encoder[0] - encoder[1]) > 2);
    }

    if (speedR == 0)
    {
        speedR = speedL;
        do
        {
            set_encoder(0, 0);
            Set_PWM(speedL, speedR);
            HAL_Delay(500);
            read_encoder(encoder);

            sprintf(msg1, ":%d,%d", encoder[1], encoder[0]);
            OLED_ShowString(0, 20, msg1, 8, 1);
            sprintf(msg2, "R:%.2f,L:%.2f", speedR, speedL);
            OLED_ShowString(0, 30, msg2, 8, 1);
            OLED_Refresh();

            speedR = speedR + PID_Compute(&PWMPID, encoder[1] - encoder[0]);

        } while (abs(encoder[0] - encoder[1]) > 2);
    }

    stop();
    {
        sprintf(msg1, ":%d,%d", encoder[1], encoder[0]);
        OLED_ShowString(0, 20, msg1, 8, 1);
        sprintf(msg2, "B:%.2f,A:%.2f", speedR, speedL);
        OLED_ShowString(0, 30, msg2, 8, 1);
        OLED_Refresh();
    }
    while (1)
        ;
}

void config_speed_by_encoder_global(float speedL, float speedR)
{
    PID PWMPID;
    PWMPID.Kp = 0.01, PWMPID.Ki = 0, PWMPID.Kd = 0;
    PWMPID.target = 0, PWMPID.minOutput = -2, PWMPID.maxOutput = 2;
    PWMPID.error = 0, PWMPID.last_error = 0, PWMPID.integral = 0;

    char msg1[10], msg2[10];
    int16_t encoder[2]; // 读编码器数值

    __HAL_TIM_DISABLE(&htim7); // 关tim7防止中断影响
    __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

    L_ratio = 1;
    R_ratio = 1;//复位两轮的速度参考值

    if (speedL == 0)
    {
        speedL = speedR;
        do
        {
            set_encoder(0, 0);
            Set_PWM(speedL, speedR);
            HAL_Delay(500);
            read_encoder(encoder);

            sprintf(msg1, ":%d,%d", encoder[1], encoder[0]);
            OLED_ShowString(0, 20, msg1, 8, 1);
            sprintf(msg2, "R:%.2f,L:%.2f", speedR, speedL);
            OLED_ShowString(0, 30, msg2, 8, 1);
            OLED_Refresh();

            speedL = speedL + PID_Compute(&PWMPID, encoder[0] - encoder[1]);

        } while (abs(encoder[0] - encoder[1]) > 2);

        L_ratio = speedL / speedR;//修改左轮的速度参考值
    }

    if (speedR == 0)
    {
        speedR = speedL;
        do
        {
            set_encoder(0, 0);
            Set_PWM(speedL, speedR);
            HAL_Delay(500);
            read_encoder(encoder);

            sprintf(msg1, ":%d,%d", encoder[1], encoder[0]);
            OLED_ShowString(0, 20, msg1, 8, 1);
            sprintf(msg2, "R:%.2f,L:%.2f", speedR, speedL);
            OLED_ShowString(0, 30, msg2, 8, 1);
            OLED_Refresh();

            speedR = speedR + PID_Compute(&PWMPID, encoder[1] - encoder[0]);

        } while (abs(encoder[0] - encoder[1]) > 2);

        R_ratio = speedR / speedL;//修改右轮的速度参考值
    }
}

void config_show_encoder(float speed1, float speed2)
{
    int16_t encoder[2] = {0, 0};
    char msgA[20], msgB[20], msgspeed[20];
    __HAL_TIM_DISABLE(&htim7);
    reset_encoder();
    while (1)
    {
        reset_encoder();
        Set_PWM(speed1, speed2);
        HAL_Delay(30);
        read_encoder(encoder);

        sprintf(msgA, "Encoder1: %d", encoder[1]);
        OLED_ShowString(0, 20, msgA, 8, 1);
        sprintf(msgB, "Encoder2: %d", encoder[0]);
        OLED_ShowString(0, 30, msgB, 8, 1);
        OLED_Refresh();
    }
}

void config_show_Kyaw()
{
    char msg[9] = {0};
    while (1)
    {
        sprintf(msg, "%f", Kyaw);
        OLED_ShowString(0, 20, msg, 8, 1);
        OLED_Refresh();
        HAL_Delay(10);
    }
}

void config_show_Yaw()
{
    char msg[9] = {0};
    while (1)
    {
        sprintf(msg, "%f", Yaw);
        OLED_ShowString(0, 20, msg, 8, 1);
        OLED_Refresh();
        HAL_Delay(10);
    }
}

void config_trace_sensor()
{
    char msg[9] = {0};
    while (1)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            msg[i] = get_T(7 - i) + '0';
            OLED_ShowString(0, 20, msg, 8, 1);
            OLED_Refresh();
        }
    }
}

void config_trace_sensor_serial()
{
    uint8_t value[8];
    char msg[9] = {0};
    while (1)
    {
        get_T_eight_serial(value);
        for (uint8_t i = 0; i < 8; i++)
        {
            msg[i] = value[7 - i] + '0';
            OLED_ShowString(0, 20, msg, 8, 1);
            OLED_Refresh();
        }
    }
}

void config_trace_digital_i2c()
{
    uint8_t value[8] = {0};
    char msg[10] = {0};
    HAL_Delay(200);
    HAL_UART_Transmit(&huart1, "begin\n", 6, 30);
    path_digital_init_i2c();
    while (1)
    {
        get_T_eight_i2c(value);
        for (uint8_t i = 0; i < 8; i++)
        {
            msg[i] = value[7 - i] + '0';
        }
        msg[8] = '\n';
        HAL_UART_Transmit(&huart1, msg, 9, 30);
        HAL_Delay(100);
    }
}

void config_trace_analog_i2c()
{
    uint8_t value[8] = {0};
    char msg[10] = {0};
    HAL_Delay(200);
    HAL_UART_Transmit(&huart1, "begin\n", 6, 30);
    path_analog_init_i2c(0);
    while (1)
    {
        get_T_analog_eight_i2c(value);
        for (uint8_t i = 0; i < 8; i++)
        {
            sprintf(msg, "%d\t", value[i]);
            HAL_UART_Transmit(&huart1, msg, 9, 30);
        }
        msg[0] = '\n';
        msg[1] = 0;
        HAL_UART_Transmit(&huart1, msg, 1, 30);
    }
}

void config_moter_Pin()
{
    while (1)
    {
        Set_PWM(50, 0);
        HAL_Delay(2000);
        Set_PWM(0, 50);
        HAL_Delay(2000);
        Set_PWM(50, 50);
        HAL_Delay(2000);
    }
}

void config_K230_read_nums()
{
    char data[4];
    while (1)
    {
        HAL_UART_Transmit(&huart1, "get\r\n", 5, 30);
        HAL_UART_Receive(&huart1, data, 4, HAL_MAX_DELAY);
        OLED_ShowString(0, 20, "vision", 8, 1);
        OLED_ShowString(0, 30, data, 8, 1);
        OLED_Refresh();
        HAL_Delay(100);
    }
}

void config_K230_read_name()
{
    char data[4];
    while (1)
    {
        HAL_UART_Transmit(&huart1, "name\r\n", 6, 30);
        HAL_UART_Receive(&huart1, data, 1, HAL_MAX_DELAY);
        OLED_ShowString(0, 20, "vision", 8, 1);
        OLED_ShowString(0, 30, data, 8, 1);
        OLED_Refresh();
        HAL_Delay(100);
    }
}
