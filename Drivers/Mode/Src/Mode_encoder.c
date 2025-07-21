// 通过编码器记录行驶长度的相关模组

#include <main.h>
#include <Mode_encoder.h>

#define ENCODER_TO_COORDINATE 1  // 实际长度与编码器数值的比值
#define CAR_LENGTH 1
#define ENCODER_PERIOD 8000

float coordinate_x = 0;
float coordinate_y = 0;
float left_encoder = 0;
float right_encoder = 0;
float last_angle = 0;

void trace_encoder(uint16_t encoder_max, float speed)
{
	PID TracePID;
	float TracePID_Kp = 30, TracePID_Ki = 0, TracePID_Kd = 5;
	float TracePID_Target = 0, TracePID_MinOutput = -35, TracePID_MaxOutput = 35;
	float IntergretPID_MinOutput = -10, IntergretPID_MaxOutput = 10;
	float TracePID_Input = 0;
	PID_Init(&TracePID, TracePID_Kp, TracePID_Ki, TracePID_Kd, TracePID_Target,
			 0, 0, TracePID_MinOutput, TracePID_MaxOutput);

	float transform = 0; // 两轮基于基础巡线速度的差值
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
	// HAL_TIM_Encoder_Stop(&htim1,TIM_CHANNEL_ALL);
	__HAL_TIM_SET_COUNTER(&htim4, 0);

	while (1)
	{
		if (__HAL_TIM_GET_COUNTER(&htim4) > encoder_max)
		{
			break;
		}
		transform = PID_Compute(&TracePID, get_Terror());
		Set_PWM(speed - transform, speed + transform);
		HAL_Delay(10);
	}
	return;
}

void reset_coordinates()
{
	left_encoder = __HAL_TIM_GET_COUNTER(&htim1);
	right_encoder = __HAL_TIM_GET_COUNTER(&htim4);
	last_angle = Yaw;
}

void update_coordinates()//更新坐标数值，第一次使用前需要进行复位
{
	float left_alter = fmod( (__HAL_TIM_GET_COUNTER(&htim1) - left_encoder), ENCODER_PERIOD ) * ENCODER_TO_COORDINATE;
	//左轮移动距离
	float right_alter = fmod( (__HAL_TIM_GET_COUNTER(&htim4) - right_encoder), ENCODER_PERIOD ) * ENCODER_TO_COORDINATE;
	//右轮移动距离
	float mid_value_alter = ( left_alter + right_alter ) /2;
	//两轮移动距离的平均值（可认为两轮中心移动距离）

	left_encoder = __HAL_TIM_GET_COUNTER(&htim1);
	right_encoder = __HAL_TIM_GET_COUNTER(&htim4);
	//记录本次编码器的值

	float alter_angle = (right_alter - left_alter) / CAR_LENGTH / 2;
	//两轮中心位移的相对朝向
	float orientation_angle = (Yaw + last_angle) / 2 + alter_angle;
	//两轮中心位移的绝对朝向（以Yaw == 0为参考方向）

	last_angle = Yaw;
	//记录本次的汽车朝向角

	coordinate_x -=  mid_value_alter * cosf(orientation_angle);
	coordinate_y -=  mid_value_alter * sinf(orientation_angle);
	//更新坐标
}













