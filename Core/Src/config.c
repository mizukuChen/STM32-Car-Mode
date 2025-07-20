#include "main.h"

void config_encoder()
{
    int16_t encoder[2] = {0, 0};
    char msgA[20], msgB[20];
    __HAL_TIM_DISABLE(&htim7);
    reset_encoder();
    Set_PWM(60, 60);
	    HAL_Delay(150);
    Trace(60);
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

void config1()
{
    char msg1[10], msg2[10];
    float PWMAtest, PWMBtest; // 目标PWM输出值
    PID PWMPID;
    float PWMPID_Kp = 0.01, PWMPID_Ki = 0, PWMPID_Kd = 0;
    float PWMPID_Target = 0, PWMPID_MinOutput = -2, PWMPID_MaxOutput = 2;
    int16_t Pencoder_num[2]; // 读编码器的
    int16_t targetB, targetA;
    PWMBtest = -60.0f;
    PWMAtest = PWMBtest;

    __HAL_TIM_DISABLE(&htim7); // 关tim7防止中断影响
    __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

    reset_encoder();
    Set_PWM(PWMBtest, PWMAtest);
    HAL_Delay(500);
    read_encoder(Pencoder_num);

    PID_Init(&PWMPID, PWMPID_Kp, PWMPID_Ki, PWMPID_Kd, PWMPID_Target, 0, 0, PWMPID_MinOutput, PWMPID_MaxOutput);

    do
    {  
        targetA = Pencoder_num[0];
        targetB = Pencoder_num[1];
        PWMAtest = PWMBtest - PID_Compute(&PWMPID, targetB - targetA);

        sprintf(msg1, ":%d,%d", Pencoder_num[1], Pencoder_num[0]);
        OLED_ShowString(0, 20, msg1, 8, 1);
        sprintf(msg2, "B:%.2f,A:%.2f", PWMBtest, PWMAtest);
        OLED_ShowString(0, 30, msg2, 8, 1);
        OLED_Refresh();

        reset_encoder();
        Set_PWM(PWMBtest, PWMAtest);
        HAL_Delay(500);
        read_encoder(Pencoder_num);
    }while (abs(Pencoder_num[0] - Pencoder_num[1]) > 2);

    stop();
    {
        sprintf(msg1, ":%d,%d", Pencoder_num[1], Pencoder_num[0]);
        OLED_ShowString(0, 20, msg1, 8, 1);
        sprintf(msg2, "B:%.2f,A:%.2f", PWMAtest, PWMBtest);
        OLED_ShowString(0, 30, msg2, 8, 1);
        OLED_Refresh();
    }
    while(1);
}

void config2() // 显示编码器数值
{
    //while(1){
    //    Set_PWM(50,0);
    //    HAL_Delay(2000);
    //    Set_PWM(0,50);
    //    HAL_Delay(2000);
    //    Set_PWM(50,50);
    //    HAL_Delay(2000);
    //}
    int16_t encoder[2] = {0, 0};
    char msgA[20], msgB[20], msgspeed[20];
    float speed = 60;
    float Inc = 4, Del = 0;
    __HAL_TIM_DISABLE(&htim7);
    reset_encoder();
    while (1)
    {
        reset_encoder();
        Set_PWM(61.5, 60);
        HAL_Delay(30);
        read_encoder(encoder);

        sprintf(msgA, "Encoder1: %d", encoder[1]);
        OLED_ShowString(0, 20, msgA, 8, 1);
        sprintf(msgB, "Encoder2: %d", encoder[0]);
        OLED_ShowString(0, 30, msgB, 8, 1);
        sprintf(msgspeed, "%.0f  %.0f", speed, speed+Inc-Del);
        OLED_ShowString(0, 40, msgspeed, 8, 1);
        OLED_Refresh();
    }
}

#define GET_NTH_BIT(sensor_value, nth_bit) (((sensor_value) >> ((nth_bit)-1)) & 0x01)
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

void config3()
{
    HAL_TIM_Encoder_Stop_IT(&htim1, TIM_CHANNEL_ALL);
    Trace(60);
    //HAL_Delay(500);
    float initial_angle = Yaw;
    //trace_encoder(500,60);
    TurnRightangel(1,70);
    Set_PWM(50,50);
    HAL_Delay(300);
    Trace(50);
    reverse_straight(initial_angle+90, 50);
    //trace_encoder(1000,60);
    TurnRightangel(1,70);
    Trace(50);
    stop();
    //uint8_t digital_data;
	//
    //uint8_t gray_sensor[8];
    //while(1){
    //	digital_data = GW_digital_TempRead();
    //	/* 把字节里的8个开关量存到八个变量里，这里为gray_sensor[0] ~ gray_sensor[7], 
    //		 * 也可以是变量val1 ~ val8, 因为是宏定义 */
    //	SEP_ALL_BIT8(digital_data, 
    //		gray_sensor[0], //探头1
    //		gray_sensor[1], //探头2
    //		gray_sensor[2], //探头3
    //		gray_sensor[3], //探头4
    //		gray_sensor[4], //探头5
    //		gray_sensor[5], //探头6
    //		gray_sensor[6], //探头7
    //		gray_sensor[7]  //探头8
    //	);
    //    char msg[10] = {0};
    //    for(uint8_t i =0;i<8;i++){
    //        msg[i] = gray_sensor[7-i] + '0';
    //    }
    //    OLED_ShowString(0,20,msg,8,1);
    //    OLED_Refresh();
    //    HAL_Delay(5);
    //}

    //__HAL_TIM_DISABLE_IT(&htim7,TIM_IT_UPDATE);
    //Trace(60);
    //config_encoder();

    //int16_t encoder[2] = {0, 0};
    //char msgA[20], msgB[20], msgspeed[20];
    //float speed = 60;
    //float Inc = 4, Del = 0;
    //__HAL_TIM_DISABLE(&htim7);
    //reset_encoder();
    //while (1)
    //{
    //    reset_encoder();
    //    Set_PWM(62, 60);
    //    HAL_Delay(30);
    //    read_encoder(encoder);
//
    //    sprintf(msgA, "Encoder1: %d", encoder[1]);
    //    OLED_ShowString(0, 20, msgA, 8, 1);
    //    sprintf(msgB, "Encoder2: %d", encoder[0]);
    //    OLED_ShowString(0, 30, msgB, 8, 1);
    //    sprintf(msgspeed, "%.0f  %.0f", speed, speed+Inc-Del);
    //    OLED_ShowString(0, 40, msgspeed, 8, 1);
    //    OLED_Refresh();
    //}
}

void config4()
{
    //HAL_TIM_Encoder_Stop_IT(&htim1, TIM_CHANNEL_ALL);
    //reverse_straight(Yaw, 50);
    //trace_encoder(3000,70);
    //stop();
    uint8_t value[8];
    char msg[9] = {0};
    while(1){
        get_T_serial(value);
        for(uint8_t i = 0; i<8; i++){
            msg[i] = value[7-i] +'0';
            OLED_ShowString(0,20,msg,8,1);
            OLED_Refresh();
        }
    }
}

void config5()
{
    reverse_straight(Yaw+90, 50);
    //trace_encoder(1000,60);
    TurnRightangel(1,70);
    Trace(50);
    stop();
    //char data[4];
    //while(1){
    //    HAL_UART_Transmit(&huart1, "get\r\n", 5, 30);
    //    HAL_UART_Receive(&huart1, data, 4, HAL_MAX_DELAY);
    //    OLED_ShowString(0, 20, "vision", 8, 1);
    //    OLED_ShowString(0, 30, data, 8, 1);
    //    OLED_Refresh();
    //    HAL_Delay(100);    
    //}
}

void config6()
{
    char data[4];
    while(1){
        HAL_UART_Transmit(&huart1, "name\r\n", 6, 30);
        HAL_UART_Receive(&huart1, data, 1, HAL_MAX_DELAY);
        OLED_ShowString(0, 20, "vision", 8, 1);
        OLED_ShowString(0, 30, data, 8, 1);
        OLED_Refresh();
        HAL_Delay(100);    
    }
}

void config7()
{
    
}

void config8()
{
    char msg[8];
    while (1)
    {
        OLED_ShowString(0, 10, "Running", 8 ,1);
        HAL_UART_Receive(&huart3, msg, 5, 100);
        OLED_ShowString(0, 20, msg, 8, 1);
        OLED_Refresh();
    }
}

void config9()
{
    char kmsg[20];
    for (int i = 0; i < 10000; i++)
    {
        sprintf(kmsg, "%f", Kyaw);
        OLED_ShowString(0, 20, kmsg, 8, 0);
        OLED_Refresh();
        HAL_Delay(10);
        OLED_Clear();
    }
}