#ifndef __PATH_I2C_H__
#define __PATH_I2C_H__

#include <main.h>

void path_digital_init_i2c();
uint8_t get_T_char_i2c();
void get_T_eight_i2c(uint8_t value[]);
uint8_t get_T_Sum_i2c(uint8_t value[]);
float get_Terror_i2c(uint8_t value[]);

/**
 * @brief 启动i2c模拟通信
 * 
 * @param path 代表需要读取的端口（0代表读取所有端口）
 */
void path_analog_init_i2c(uint8_t path);
uint8_t get_T_analog_single_i2c();
void get_T_analog_eight_i2c(uint8_t value[]);


#endif