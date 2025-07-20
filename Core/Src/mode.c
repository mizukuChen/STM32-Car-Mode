#include "task.h"
#include "Trace.h"
#include "main.h"

#define BASIC_SPEED 40
#define STRAIGHT_ANGEL -180
#define MAGIC_ANGEL 38
#define LINEFOLLOWING_SPEED 40
#define COMPENSATE_SPEED 50



void go_straight(float angle, float speed)
{
	// read_angle();
	char msg[20], msg1[20];
	sprintf(msg, "%.2f", angle);
	OLED_ShowString(0, 20, msg, 8, 1);
	OLED_Refresh();
	PID *pid_straight = malloc(sizeof(PID));
	pid_straight->Kp = 1.8;
	pid_straight->Ki = 0;
	pid_straight->Kd = 2.5;
	pid_straight->target = angle; // 直行角度
	pid_straight->minIntegral = -5;
	pid_straight->maxIntegral = 5;
	pid_straight->minOutput = -10;
	pid_straight->maxOutput = 10;
	pid_straight->error = 0.0;		// 不需要修改
	pid_straight->last_error = 0.0; // 不需要修改
	pid_straight->integral = 0.0;	// 不需要修改
	float transform = 0;
	// read_angle();
	while (1) // get_T_ALL() == 0x00)
	{		  // 寻到轨迹是停止直行
		// read_angle();//更新Yaw
		sprintf(msg1, "%.2f", Yaw);
		OLED_ShowString(0, 30, msg1, 8, 1);
		OLED_Refresh();
		transform = PID_Compute(pid_straight, Yaw);
		if (transform > 0)
		{
			Set_PWM(speed - transform, speed); // Yaw偏小时transform>0,左轮减速可增大Yaw
		}
		else if (transform < 0)
		{
			Set_PWM(speed, speed + transform); // Yaw偏大时transform<0,右轮减速可减小Yaw
		}
		else
		{
			Set_PWM(speed, speed);
		}
		// OLED_ShowNum(10,40,transform,6,8,1);
		// OLED_Refresh();
		// Delay_Ms(10);
	}
}

int reverse(float speed)
{
	PID TracePID;
	TracePID.Kp = 20, TracePID.Ki = 0, TracePID.Kd = 0;
	TracePID.target = 0, TracePID.minOutput = -LINEFOLLOWING_SPEED, TracePID.maxOutput = LINEFOLLOWING_SPEED;
	TracePID.minIntegral = -10, TracePID.maxIntegral = 10;
	TracePID.error = 0;
	float transform = 0;
	Set_PWM(-speed, -speed);
	HAL_Delay(300);
	while (1)
	{
		if (get_T(3) == 0 && get_T(4) == 0 && get_T(5) == 0 && get_T(6) == 0)
		{
			Set_PWM(-speed, -speed);
		}
		else
		{
			// if ((get_T(1) == 1 || get_T(8) == 1) && (get_T(4) == 1 || get_T(5) == 1))
			//{
			//	Set_PWM(-speed, -speed);
			//	HAL_Delay(10);
			//	if ((get_T(1) == 1 || get_T(8) == 1) && (get_T(4) == 1 || get_T(5) == 1)){
			//		//break;
			//	}
			// }
			// if (get_T_ALL() == 0x00)
			//{
			//	//return 0;
			// }
			transform = PID_Compute(&TracePID, get_Terror());
			Set_PWM(-speed + transform, -speed - transform);
		}
		HAL_Delay(5);
	}
	Set_PWM(40, 40);
	HAL_Delay(500); // 先直行一段，到合适位置再转向
	stop();
	return 1; // 遇到十字路口返回1
}

void reverse_straight(float angle, float speed)
{
	char msg[20], msg1[20];
	sprintf(msg, "%.2f", angle);
	OLED_ShowString(0, 20, msg, 8, 1);
	OLED_Refresh();
	Set_PWM(-speed, -speed);
	HAL_Delay(500);
	// read_angle();
	PID *pid_straight = malloc(sizeof(PID));
	pid_straight->Kp = 12, pid_straight->Ki = 0, pid_straight->Kd = 18;
	pid_straight->target = angle;
	pid_straight->minIntegral = -10, pid_straight->maxIntegral = 10;
	pid_straight->minOutput = -25, pid_straight->maxOutput = 25;
	pid_straight->error = 0.0;
	pid_straight->last_error = 0.0;
	pid_straight->integral = 0.0;
	float transform = 0;
	uint8_t value[8];

	while (1)
	{
		get_T_serial(value);
		if ((value[1-1] == 1 && value[2-1] == 1 && value[3-1] == 1 && value[4-1] == 1) ||
			(value[5-1] == 1 && value[6-1] == 1 && value[7-1] == 1 && value[8-1] == 1))
		{
			HAL_Delay(5);
			if ((value[1-1] == 1 && value[2-1] == 1 && value[3-1] == 1 && value[4-1] == 1) ||
			(value[5-1] == 1 && value[6-1] == 1 && value[7-1] == 1 && value[8-1] == 1))
			{
				break;
			}
		}
		if (get_T_ALL() == 0x00)
		{
			break;
		}
		sprintf(msg1, "%.2f", Yaw);
		OLED_ShowString(0, 30, msg1, 8, 1);
		OLED_Refresh();
		transform = PID_Compute(pid_straight, Yaw);
		if (transform > 0)
		{
			Set_PWM(-speed, -speed + transform); // Yaw偏小时transform>0,右轮减速可增大Yaw
		}
		else if (transform < 0)
		{
			Set_PWM(-speed - transform, -speed); // Yaw偏大时transform>0,左轮减速可减小Yaw
		}
		else
		{
			Set_PWM(-speed, -speed);
		}
		HAL_Delay(5);
	}
	Set_PWM(50, 50);
	while (!((value[1-1] == 1 && value[2-1] == 1 && value[3-1] == 1 && value[4-1] == 1) ||
			(value[5-1] == 1 && value[6-1] == 1 && value[7-1] == 1 && value[8-1] == 1)));
	HAL_Delay(350);
}

void reverse_encoder(float speed)
{
	char msg[20], msg1[20];
	__HAL_TIM_DISABLE_IT(&htim7,TIM_IT_UPDATE);
	int16_t encoder[2] = {0};
	//做好相关准备

	Set_PWM(-speed, -speed);
	HAL_Delay(500);
	//先离开黑线

	PID *pid_straight = malloc(sizeof(PID));
	pid_straight->Kp = 7, pid_straight->Ki = 0, pid_straight->Kd = 2.5;
	pid_straight->target = 0;
	pid_straight->minIntegral = -10, pid_straight->maxIntegral = 10;
	pid_straight->minOutput = -15, pid_straight->maxOutput = 15;
	pid_straight->error = 0.0;
	pid_straight->last_error = 0.0;
	pid_straight->integral = 0.0;
	float transform = 0;
	// read_angle();
	while (1)
	{
		if ((get_T(1) == 1 && get_T(2) == 1 && get_T(3) == 1 && get_T(4) == 1) ||
			(get_T(5) == 1 && get_T(6) == 1 && get_T(7) == 1 && get_T(8) == 1))
		{
			HAL_Delay(5);
			if ((get_T(1) == 1 && get_T(2) == 1 && get_T(3) == 1 && get_T(4) == 1) ||
				(get_T(5) == 1 && get_T(6) == 1 && get_T(7) == 1 && get_T(8) == 1))
			{
				break;
			}
		}
		if (get_T_ALL() == 0x00)
		{
			break;
		}
		//到十字路口口或空地时停止

		reset_encoder();
		transform = PID_Compute(pid_straight, encoder[1] - encoder[0]);
		if (transform > 0)
		{
			Set_PWM(-speed + transform, -speed); // Yaw偏小时transform>0,右轮减速可增大Yaw
		}
		else if (transform < 0)
		{
			Set_PWM(-speed, -speed - transform); // Yaw偏大时transform>0,左轮减速可减小Yaw
		}
		else
		{
			Set_PWM(-speed, -speed);
		}
		HAL_Delay(10);
		read_encoder(encoder);
	}
	//stop();

	Set_PWM(62, 60);
	HAL_Delay(350);
	// 直行至十字路口
}

#define Delayturn 500
void turn_both(float turn_angle)
{
	char msg[20], msg1[20];
	float initial = Yaw;
	PID *pid_turn = malloc(sizeof(PID));
	// read_angle();
	float angle_begin = Yaw;
	float angle_end = angle_begin + turn_angle; // 计算目标角度
	sprintf(msg, "%.2f %.2f", angle_begin, angle_end);
	OLED_ShowString(0, 20, msg, 8, 1);
	OLED_Refresh();
	pid_turn->Kp = 25;
	pid_turn->Ki = 0;
	pid_turn->Kd = 1.2;
	pid_turn->target = angle_end;
	pid_turn->minIntegral = -5;
	pid_turn->maxIntegral = 5;
	pid_turn->minOutput = -50;
	pid_turn->maxOutput = 50;
	pid_turn->error = 0.0;		// 不需要修改
	pid_turn->last_error = 0.0; // 不需要修改
	pid_turn->integral = 0.0;	// 不需要修改
	float transform = 0;
	// read_angle();
	while (1)
	{
		printf(msg1, "%.2f", Yaw);
		OLED_ShowString(0, 30, msg1, 8, 1);
		OLED_Refresh();
		// read_angle();//更新Yaw
		if (Yaw > angle_end - 0.5 && Yaw < angle_end + 0.5)
		{ // 允许误差值
			stop();
			break;
		} // 达到目标角度附近便停止
		else
		{
			transform = PID_Compute(pid_turn, Yaw);
			Set_PWM(-transform, transform); // Yaw偏小时transform>0，左轮倒转右轮正转
		}
		// OLED_ShowNum(10,40,transform,6,8,1);
		// OLED_Refresh();
		HAL_Delay(5);
	}
	transform = PID_Compute(pid_turn, initial);
	Set_PWM(transform, -transform); // 反向补偿
	HAL_Delay(Delayturn / 2);
	OLED_Clear();
	stop();
}

void turn_single(float turn)
{ // 只旋转外侧轮
	char msg[20], msg1[20];
	PID *pid_turn = malloc(sizeof(PID));
	// read_angle();
	float angle_begin = Yaw;
	float angle_end = angle_begin + turn; // 计算目标角度
	sprintf(msg, "%.2f %.2f", angle_begin, angle_end);
	OLED_ShowString(0, 20, msg, 8, 1);
	OLED_Refresh();
	pid_turn->Kp = 25;
	pid_turn->Ki = 0;
	pid_turn->Kd = 1.2;
	pid_turn->target = angle_end;
	pid_turn->minIntegral = -5;
	pid_turn->maxIntegral = 5;
	pid_turn->minOutput = -50;
	pid_turn->maxOutput = 50;
	pid_turn->error = 0.0;		// 不需要修改
	pid_turn->last_error = 0.0; // 不需要修改
	pid_turn->integral = 0.0;	// 不需要修改
	float transform = 0;
	// read_angle();
	if (turn > 0)
	{ // 向左转动，只转右轮
		while (1)
		{
			sprintf(msg1, "%.2f", Yaw);
			OLED_ShowString(0, 30, msg1, 8, 1);
			OLED_Refresh();
			// read_angle();//更新Yaw
			if (Yaw > angle_end - 0.5 && Yaw < angle_end + 0.5)
			{ // 允许误差值
				stop();
				break;
			} // 达到目标角度附近便停止
			else
			{
				transform = PID_Compute(pid_turn, Yaw);
				Set_PWM(0, transform); // Yaw偏小时transform>0，右轮正转
			}
			// OLED_ShowNum(10,40,transform,6,8,1);
			// OLED_Refresh();
			HAL_Delay(5);
		}
		Set_PWM(0, -50);
		HAL_Delay(Delayturn);
	}
	else if (turn < 0)
	{ // 向右转动，只转左轮
		while (1)
		{
			// read_angle();//更新Yaw
			if (Yaw > angle_end - 0.5 && Yaw < angle_end + 0.5)
			{ // 允许误差值
				stop();
				break;
			} // 达到目标角度附近便停止
			else
			{
				transform = PID_Compute(pid_turn, Yaw);
				Set_PWM(-transform, 0); // Yaw偏大时transform<0，左轮正转
			}
			// OLED_ShowNum(10,40,transform,6,8,1);
			// OLED_Refresh();
			HAL_Delay(5);
		}
		Set_PWM(-50, 0);
		HAL_Delay(Delayturn);
	}
	OLED_Clear();
	stop();
}

void turn(float turn)
{
	turn_single(turn);
}

void turnto(float aim_angle)
{
	float turn_angle = aim_angle - Yaw;
	turn(turn_angle);
}

void autoturn(float speed) // 从当前位置旋转至寻到下一个轨迹
{						   // speed>0时向左旋转，speed<时向右旋转
	while (get_T_ALL() != 0x00)
	{ // 先脱离当前轨道
		Set_PWM(-speed, speed);
		HAL_Delay(5);
	}
	while (get_T(4) == 0 && get_T(5) == 0)
	{ // 中间两路寻到轨迹时停止旋转
		Set_PWM(-speed, speed);
		HAL_Delay(5);
	}
	//		Set_PWM(turn_angle, -turn_angle);//反向补偿
	//		HAL_Delay(Delayturn);
	OLED_Clear();
	stop();
}