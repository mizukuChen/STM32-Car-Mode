#ifndef __MODE_CONFIG_H__
#define __MODE_CONFIG_H__

#include <main.h>

void config_find_encoder();
/**
 * @brief 通过编码器调整出合适的设置速度，一轮速度应设为0，设定速度不为0的轮为参考轮
 * 
 * @param speedL 左轮速度
 * @param speedR 右轮速度
 */
void config_speed_by_encoder(float speedL, float speedR);
void config_show_encoder(float speed1, float speed2);
void config_show_Kyaw();
void config_show_Yaw();
void config_trace_sensor();
void config_trace_sensor_serial();
void config_trace_digital_i2c();
void config_trace_analog_i2c();
void config_moter_Pin();
void config_K230_read_nums();
void config_K230_read_name();

#endif