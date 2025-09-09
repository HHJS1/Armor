//
// Created by 30427 on 2024/11/27.
//

#include "bsp.h"
#define GATE 3100                //定义MIC的触发门限值
uint32_t Mic_value,tmp;
uint8_t state;
//uint8_t CHG;					//定义8位无符号整型变量CHG存储充电状态
uint8_t color=1;				//定义8位无符号整型变量color存储LED颜色
//uint8_t light = 100;	//定义8位无符号整型变量light用于存储pwm占空比
//uint8_t add = 0;			//定义8位无符号整型变量add用于存储加减状态
uint8_t rx_buf[128];
uint8_t Id=3;
uint8_t Count[4]={0xFA,0,0,0xff};


void Light_OFF()
{
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,0);
}
void  RED_Light()
{
    color=1;
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,100);
}
void BLUE_Light()
{
    color=2;
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,100);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,0);
}
void PURPIE_Light()
{
    color=3;
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,80);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,100);
}

void Color_State(){
    switch (color) {
        case 1:
            RED_Light();
            break;
        case 2:
            BLUE_Light();
            break;
        case 3:
            PURPIE_Light();
            break;
        default:
            break;

    }
}

void Delay_us(uint32_t t)
{
    HAL_TIM_Base_Start(&htim3);
            __HAL_TIM_SetCounter(&htim3,0);
    while(t > __HAL_TIM_GetCounter(&htim3))
    {
    }
    HAL_TIM_Base_Stop(&htim3);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

    if(huart == &huart1)
    {
        if (rx_buf[0] == 0xFA&& rx_buf[2] == 0xFF) {
            switch (rx_buf[1]) {
                case 0xF1:
                    color=1;
                    break;
                case 0xF2:
                    color=2;
                    break;
                case 0xF3:
                    color=3;
                    break;
            }





        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buf, 128);
        __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin==GPIO_PIN_4){
        if (color==1)
        {
        color=2;
        } else if (color==2)
        {
        color=3;
        }else if (color==3)
        {
        color=1;
        }
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
    }

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//定时器中断服务函数
{
    if(htim->Instance == TIM1)
    {

        Color_State();                                          //判断当前颜色
        //--------采集MIC电压---------------
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,100);
        Mic_value= HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        //--------------------------------

        if (Mic_value > tmp)	tmp = Mic_value;				//记录MIC的最大值

        if (Mic_value >= GATE)			//若MIC的ADC值大于3850则判断受到打击
        {
            Count[2]++;
            Count[1]=Id;
            HAL_UART_Transmit(&huart1,Count,sizeof (Count),0xFF);//发送被击打次数
            switch (color)			//判断当前颜色
            {
                case 1:						//若为1红色
                {
                    Light_OFF();
                    Delay_us(40000);
                    RED_Light();
                    Delay_us(40000);
                    Light_OFF();
                    Delay_us(40000);
                    RED_Light();		//红色闪烁两次
                    break;
                }
                case 2:						//若为2蓝色
                {
                    Light_OFF();
                    Delay_us(40000);
                    BLUE_Light();
                    Delay_us(40000);
                    Light_OFF();
                    Delay_us(40000);
                    BLUE_Light();		//蓝色闪烁两次
                    break;
                }
                case 3:						//若为3紫色
                {
                    Light_OFF();
                    Delay_us(40000);
                    PURPIE_Light();
                    Delay_us(40000);
                    Light_OFF();
                    Delay_us(40000);
                    PURPIE_Light();		    //紫色闪烁两次
                    break;
                }
                default:
                    break;

            }
        }


    }
}
