/*
 * Copyright (c) 2022 感为智能科技(济南)
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

#ifndef INC_GW_GRAYSCALE_SENSOR_H_
#define INC_GW_GRAYSCALE_SENSOR_H_

#include <stdint.h>
#include <stdio.h>
#include <stm32h7xx_hal_rcc.h>
#include <stm32h7xx_hal_i2c.h>
#include <stm32h7xx_hal_gpio.h>

/* 默认地址 */
#define GW_GRAY_ADDR_DEF 0x4C
#define GW_GRAY_PING 0xAA
#define GW_GRAY_PING_OK 0x66
#define GW_GRAY_PING_RSP GW_GRAY_PING_OK

/* 开启开关数据模式 */
#define GW_GRAY_DIGITAL_MODE 0xDD

/* 开启连续读取模拟数据模式 */
#define GW_GRAY_ANALOG_BASE_ 0xB0
#define GW_GRAY_ANALOG_MODE  (GW_GRAY_ANALOG_BASE_ + 0)

/* 传感器归一化寄存器(v3.6及之后的固件) */
#define GW_GRAY_ANALOG_NORMALIZE 0xCF

/* 循环读取单个探头模拟数据 n从1开始到8 */
#define GW_GRAY_ANALOG(n) (GW_GRAY_ANALOG_BASE_ + (n))

/* 黑色滞回比较参数操作 */
#define GW_GRAY_CALIBRATION_BLACK 0xD0
/* 白色滞回比较参数操作 */
#define GW_GRAY_CALIBRATION_WHITE 0xD1

// 设置所需探头的模拟信号(CE: channel enable)
#define GW_GRAY_ANALOG_CHANNEL_ENABLE 0xCE
#define GW_GRAY_ANALOG_CH_EN_1 (0x1 << 0)
#define GW_GRAY_ANALOG_CH_EN_2 (0x1 << 1)
#define GW_GRAY_ANALOG_CH_EN_3 (0x1 << 2)
#define GW_GRAY_ANALOG_CH_EN_4 (0x1 << 3)
#define GW_GRAY_ANALOG_CH_EN_5 (0x1 << 4)
#define GW_GRAY_ANALOG_CH_EN_6 (0x1 << 5)
#define GW_GRAY_ANALOG_CH_EN_7 (0x1 << 6)
#define GW_GRAY_ANALOG_CH_EN_8 (0x1 << 7)
#define GW_GRAY_ANALOG_CH_EN_ALL (0xFF)

/* 读取错误信息 */
#define GW_GRAY_ERROR 0xDE

/* 设备软件重启 */
#define GW_GRAY_REBOOT 0xC0

/* 读取固件版本号 */
#define GW_GRAY_FIRMWARE 0xC1


/**
 * @brief 从I2C得到的8位的数字信号的数据 读取第n位的数据
 * @param sensor_value_8 数字IO的数据
 * @param n 第1位从1开始, n=1 是传感器的第一个探头数据, n=8是最后一个
 * @return
 */
#define GET_NTH_BIT(sensor_value, nth_bit) (((sensor_value) >> ((nth_bit)-1)) & 0x01)


/**
 * @brief 从一个变量分离出所有的bit
 */
#define SEP_ALL_BIT8(sensor_value, val1, val2, val3, val4, val5, val6, val7, val8) \
do {                                                                              \
val1 = GET_NTH_BIT(sensor_value, 1);                                              \
val2 = GET_NTH_BIT(sensor_value, 2);                                              \
val3 = GET_NTH_BIT(sensor_value, 3);                                              \
val4 = GET_NTH_BIT(sensor_value, 4);                                              \
val5 = GET_NTH_BIT(sensor_value, 5);                                              \
val6 = GET_NTH_BIT(sensor_value, 6);                                              \
val7 = GET_NTH_BIT(sensor_value, 7);                                              \
val8 = GET_NTH_BIT(sensor_value, 8);                                              \
} while(0)

/* 设置设备I2C地址 */
#define GW_GRAY_CHANGE_ADDR 0xAD

/* 广播重置地址所需要发的数据 */
#define GW_GRAY_BROADCAST_RESET "\xB8\xD0\xCE\xAA\xBF\xC6\xBC\xBC"

#if defined (ESP_PLATFORM)
/* ESP32 */


#endif

#endif /* INC_GW_GRAYSCALE_SENSOR_H_ */
