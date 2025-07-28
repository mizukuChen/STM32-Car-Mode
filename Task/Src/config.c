#include "main.h"
#include <config.h>

void config1()
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

void config2()
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
            sprintf(msg,"%d\t",value[i]);
            HAL_UART_Transmit(&huart1, msg, 9, 30);
        }
        msg[0] = '\n';
        msg[1] = 0;
        HAL_UART_Transmit(&huart1, msg, 1, 30);
    }
}

void config3()
{
}

void config4()
{
}

void config5()
{
}

void config6()
{
}

void config7()
{
}

void config8()
{
}

void config9()
{
}