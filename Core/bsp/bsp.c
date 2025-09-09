//
// Created by 30427 on 2024/11/27.
//

#include "bsp.h"
#define GATE 3100                //����MIC�Ĵ�������ֵ
uint32_t Mic_value,tmp;
uint8_t state;
//uint8_t CHG;					//����8λ�޷������ͱ���CHG�洢���״̬
uint8_t color=1;				//����8λ�޷������ͱ���color�洢LED��ɫ
//uint8_t light = 100;	//����8λ�޷������ͱ���light���ڴ洢pwmռ�ձ�
//uint8_t add = 0;			//����8λ�޷������ͱ���add���ڴ洢�Ӽ�״̬
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


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//��ʱ���жϷ�����
{
    if(htim->Instance == TIM1)
    {

        Color_State();                                          //�жϵ�ǰ��ɫ
        //--------�ɼ�MIC��ѹ---------------
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,100);
        Mic_value= HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        //--------------------------------

        if (Mic_value > tmp)	tmp = Mic_value;				//��¼MIC�����ֵ

        if (Mic_value >= GATE)			//��MIC��ADCֵ����3850���ж��ܵ����
        {
            Count[2]++;
            Count[1]=Id;
            HAL_UART_Transmit(&huart1,Count,sizeof (Count),0xFF);//���ͱ��������
            switch (color)			//�жϵ�ǰ��ɫ
            {
                case 1:						//��Ϊ1��ɫ
                {
                    Light_OFF();
                    Delay_us(40000);
                    RED_Light();
                    Delay_us(40000);
                    Light_OFF();
                    Delay_us(40000);
                    RED_Light();		//��ɫ��˸����
                    break;
                }
                case 2:						//��Ϊ2��ɫ
                {
                    Light_OFF();
                    Delay_us(40000);
                    BLUE_Light();
                    Delay_us(40000);
                    Light_OFF();
                    Delay_us(40000);
                    BLUE_Light();		//��ɫ��˸����
                    break;
                }
                case 3:						//��Ϊ3��ɫ
                {
                    Light_OFF();
                    Delay_us(40000);
                    PURPIE_Light();
                    Delay_us(40000);
                    Light_OFF();
                    Delay_us(40000);
                    PURPIE_Light();		    //��ɫ��˸����
                    break;
                }
                default:
                    break;

            }
        }


    }
}
