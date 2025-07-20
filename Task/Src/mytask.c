#include "mytask.h"
uint8_t target;
char read_data[4];
extern volatile int8_t encoder_flag;

void MainTask(void)
{
	int map[4];						// 用于存储行驶记录, 便于实现回程
	int turn_flag, cross_count = 0; // 分别为转弯flag和经过十字路口数目
	encoder_flag = 0;

	GetTargetNum();

	for (int i = 0; i < 4; i++)
	{
		reset_encoder(); // 重置编码器

		Trace(40); // 循迹

		Set_PWM(40, 40);
		HAL_Delay(300); // 向前继续一小段

		//		turn_flag = encoder_flag;//正常测试用
		turn_flag = 1; // debug用
		map[i] = turn_flag;

		TurnRightangel(turn_flag,40); // 转弯

		cross_count++;

		if (i < 2 && map[i] != 0)
		{
			break;
		}
	}

	// 进入送药最后部分循迹
	Trace(40); // 倒车

	Reverse(40); // 回程

	for (int i = 0; i < cross_count; i++)
	{
		if ((cross_count - 1 - i) == (3 - 1))
			TurnRightangel(-map[cross_count - 1 - i], 40); // 转弯
		else
			TurnRightangel(map[cross_count - 1 - i], 40);

		Trace(40); // 循迹

		Set_PWM(40, 40);
		HAL_Delay(300); // 向前继续一小段
	}
}

void Main_Task(void)
{
	int8_t flag = 0;
	int8_t flag_far = 0;
	int8_t i = 1;
	float initial_angle = 0;
	float speed = 60;
	float turn_speed = 70;
	encoder_flag = 0;

	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	HAL_TIM_Encoder_Stop_IT(&htim1, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Stop_IT(&htim4, TIM_CHANNEL_ALL);

	GetTargetNum();
	HAL_Delay(3000);

	Set_PWM(speed, speed);
	HAL_Delay(150);
	Trace(speed);
	// 第一次循迹

	if (encoder_flag != 0)
	{
		initial_angle = Yaw;
		flag = encoder_flag;
		//car_brake();
		TurnRightangel(flag, turn_speed);
	}
	else{
		for (i = 2; i <= 3; i++)
		{
			reset_encoder();
			if(i == 3)
				__HAL_TIM_SET_COUNTER(&htim1, 7500);
			Set_PWM(speed, speed);
			HAL_Delay(150);
			Trace(speed);
			if (flag = encoder_flag)
			{
				initial_angle = Yaw;
				//car_brake();
				TurnRightangel(flag, turn_speed);
				Set_PWM(50,50);
    			HAL_Delay(300);
				break;
			}
			OLED_Clear();
		}
	}

	if (i == 3)
	{
		reset_encoder();
		__HAL_TIM_SET_COUNTER(&htim1, 1000);
		__HAL_TIM_SET_COUNTER(&htim4, 1000);
		Trace(speed);
		flag_far = flag;
		flag = encoder_flag;
		initial_angle = Yaw;
		//car_brake();
		TurnRightangel(flag, turn_speed);
		Set_PWM(50,50);
    	HAL_Delay(300);
	}

    Trace(50);
	// 等待卸药
	reverse_straight(initial_angle + 90 * flag, 50);
	// reverse(40);
	// reverse_straight(Kyaw,40);//陀螺仪version

	TurnRightangel(flag, turn_speed);
	if (i == 3)
	{
		Trace(speed);
		//car_brake();
		TurnRightangel(-flag_far, turn_speed);
	}
	while (i)
	{
		Set_PWM(speed, speed);
		HAL_Delay(150);
		Trace(speed);
		i--;
	}
}

void TurnRightangel(int flag, float turn_speed)
{
	if (flag == 0)
		return;
	float speed = flag * turn_speed;
	float start_angle = Yaw;
	uint8_t value[8];
	get_T_serial(value);

	while (value[4-1] || value[5-1])
	{
		get_T_serial(value);
		Set_PWM(-speed, speed);
		HAL_Delay(10);
	} // 先脱离当前轨道

	while (!(value[4-1] || value[5-1]) || (Yaw-start_angle)*flag < 70 )
	{
		get_T_serial(value);
		Set_PWM(-speed, speed);
		HAL_Delay(10);
	} // 中间两路寻到轨迹后判断中间靠边两路

	//while (!(get_T(3) || get_T(6)))
	//{
	//	Set_PWM(-speed, speed);
	//	HAL_Delay(10);
	//} // 3或6路寻到轨迹时停止旋转

	//stop();
}

/* void TurnRightangel(int flag, float turn_speed)
{
	if (flag == 0)
		return;
	float speed1 = 0;
	float speed2 = 0;

	if(flag == 1){
		speed2 = turn_speed;
	}
	if(flag == -1){
		speed1 = turn_speed;
	}

	while (get_T_ALL())
	{
		Set_PWM(speed1, speed2);
		HAL_Delay(10);
	} // 先脱离当前轨道

	while (!(get_T(4) || get_T(5)))
	{
		Set_PWM(speed1, speed2);
		HAL_Delay(10);
	} // 中间两路寻到轨迹时停止旋转

	stop();
} */



void Reverse(float speed)
{
	Set_PWM(-61.5, -60);
	HAL_Delay(1000);

	while (!((get_T(1) == 1 && get_T(2) == 1 && get_T(3) == 1 && get_T(4) == 1) ||
			 (get_T(5) == 1 && get_T(6) == 1 && get_T(7) == 1 && get_T(8) == 1)))
		;

	Set_PWM(62, 60);
	while (!((get_T(1) == 1 && get_T(2) == 1 && get_T(3) == 1 && get_T(4) == 1) ||
			 (get_T(5) == 1 && get_T(6) == 1 && get_T(7) == 1 && get_T(8) == 1)))
		; // 先直行一段，到合适位置再转向
}

int GetTargetNum(void)
{
	char msg[7] = {0}; // 发送信息

	HAL_UART_Transmit(&huart1, "name\r\n", 6, 30); // 给上位机信号
	HAL_UART_Receive(&huart1, msg, 1, HAL_MAX_DELAY); // 接受采样数据     

	OLED_ShowString(0, 30, msg, 8, 1); // 打印接收到信息
	OLED_Refresh();

	target = msg[0] - '0';

	for (int i = 0; i < 5; i++)
		HAL_UART_Receive(&huart1, msg, 7, 30); // 此处清除UART接受区

	if (target == 1)
	{
		encoder_flag = 1;
	}
	else if (target == 2)
	{
		encoder_flag = -1;
	}
	else{
		encoder_flag = 0;
	}

	return target;
}

int GetTurnFlag(void)
{
	char msg[7];   // 发送信息
	int turn_flag; // 转向flag

	OLED_ShowString(64, 30, "debug", 8, 1); // 可以去掉
	OLED_Refresh();

	HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart1, "get\r\n", 5, 20);

	for (int i = 0; msg[0] == 0 && i <= 10; i++)
	{
		HAL_UART_Receive(&huart1, msg, 7, 30);
	}
	//	HAL_UART_Receive(&huart1, msg, 7, HAL_MAX_DELAY);

	OLED_ShowString(64, 40, msg, 8, 1); // 显示接收到的方向信息, 非debug可去
	OLED_Refresh();

	switch (msg[0])
	{
	case 'L':
		turn_flag = 1;
		break;
	case 'R':
		turn_flag = -1;
		break;
	case 'M':
		turn_flag = 0;
		break;
	default:
		turn_flag = 1;
		break;
	}

	for (int i = 0; i < 5; i++)
	HAL_UART_Receive(&huart1, msg, 7, 30); // 此处清除UART接受区
	OLED_ShowString(64, 20, "finish", 8, 1);   // 此处debug用
	OLED_Refresh();
	return turn_flag;
}

char *GetTurnData()
{
	OLED_ShowString(64, 30, "debug", 8, 1); // 可以去掉
	OLED_Refresh();

	// HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, GPIO_PIN_SET);
	
	for(uint8_t i =0; i<10; i++){
		HAL_UART_Transmit(&huart1, "get\r\n", 5, 20); // 提示摄像头开始采样
		HAL_UART_Receive(&huart1, read_data, 4, 50); // 接受采样数据
	}

	OLED_ShowString(0, 40, read_data, 8, 1); // 显示接收到的方向信息, 非debug可去
	OLED_Refresh();

	return read_data;
}