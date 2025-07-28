// 检测轨迹模组(串行)

#include <main.h>
#include <path_i2c.h>

#define ALTERA1 0.5
#define ALTERA2 1.2
#define ALTERA3 4.0
#define INC_ALTERA1 0.5
#define INC_ALTERA2 0.7
#define INC_ALTERA3 1.0
#define INC_ALTERA4 1.5

#define GW_SENSOR_ADDRESS 0x98
#define HI2C hi2c1

#define GET_BIT(sensor_value, nth_bit) (((sensor_value) >> ((nth_bit)-1)) & 0x01)
static inline void get_all_bit(uint8_t sensor_value, uint8_t value[]) 
{                                
    value[0] = GET_BIT(sensor_value, 1);
    value[1] = GET_BIT(sensor_value, 2);
    value[2] = GET_BIT(sensor_value, 3);
    value[3] = GET_BIT(sensor_value, 4);
    value[4] = GET_BIT(sensor_value, 5);
    value[5] = GET_BIT(sensor_value, 6);
    value[6] = GET_BIT(sensor_value, 7);
    value[7] = GET_BIT(sensor_value, 8);
} 

void path_digital_init_i2c()
{
    uint8_t digital_init = 0xDD;
    HAL_I2C_Master_Transmit(&HI2C, GW_SENSOR_ADDRESS, &digital_init, 1, 30);
}

uint8_t get_T_char_i2c()
{
    uint8_t digit = 0;
    HAL_I2C_Master_Receive(&HI2C, GW_SENSOR_ADDRESS, &digit, 1, HAL_MAX_DELAY);
    return digit;
} 

void get_T_eight_i2c(uint8_t value[])
{
    uint8_t digital_data;
    digital_data = get_T_char_i2c();
    get_all_bit(digital_data,value);
}

uint8_t get_T_Sum_i2c(uint8_t value[])
{
    uint8_t sum = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        sum += value[i];
    }
    return sum;
}

float get_Terror_i2c()
{
    float error;
    uint8_t trace;

    trace = get_T_char_i2c();

    switch (trace)
    {
    case 0x18:
    case 0x10:
    case 0x08: // 00011000 00010000 00001000正
        error = 0;
        break;
    case 0x30:
    case 0x38: // 00110000 00111000 偏左
        error = -ALTERA1;
        break;
    case 0x60:
    case 0x70:
    case 0x20: // 01100000 01110000 00100000左
        error = -ALTERA2;
        break;
    case 0xC0:
    case 0xE0:
    case 0x80: // 11000000 11100000 10000000更大偏左
        error = -ALTERA3;
        break;
    case 0x0C:
    case 0x1C: // 00001100 00011100 偏右
        error = +ALTERA1;
        break;
    case 0x06:
    case 0x0E:
    case 0x04: // 00000110 00001110 00000100右
        error = +ALTERA2;
        break;
    case 0x03:
    case 0x07:
    case 0x01: // 00000011 00000111 00000001 更大偏右
        error = +ALTERA3;
        break;
    default:
        error = 0;
        break;
    }
    return error;
}

void path_analog_init_i2c(uint8_t path){
    uint8_t digital_init = 0xB0;// + path;
    HAL_I2C_Master_Transmit(&HI2C, GW_SENSOR_ADDRESS, &digital_init, 1, 30);
}

uint8_t get_T_analog_single_i2c()
{
    uint8_t digit = 0;
    HAL_I2C_Master_Receive(&HI2C, GW_SENSOR_ADDRESS, &digit, 1, HAL_MAX_DELAY);
    return digit;
} 

void get_T_analog_eight_i2c(uint8_t value[])
{
    HAL_I2C_Master_Receive(&HI2C, GW_SENSOR_ADDRESS, value, 8, HAL_MAX_DELAY);
}





