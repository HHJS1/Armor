//
// Created by 30427 on 2024/11/27.
//

#ifndef ARMOR_BSP_H
#define ARMOR_BSP_H
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

extern uint8_t rx_buf[128];
void Delay_us(uint32_t t);

#endif //ARMOR_BSP_H
