/*
 * main_app.c
 *
 *  Created on: 2024年1月21日
 *      Author: ssfxfss
 */

#include "debug.h"
#include "driver/ms5803.h"

float temp, press;

static void fail(void)
{
    RCC_ClearFlag();
    NVIC_SystemReset();
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    int ret = ms5803_init();
    if (ret != 0)
    {
        Delay_Ms(500);
        fail();
    }

    while(1)
    {
        ms5803_read_temp_and_press(&temp, &press);
        if (temp <= 0 || press <= 0)
        {
            Delay_Ms(500);
            fail();
        }
        printf("S%d/%dE", (int)temp * 100.0f, (int)press * 100.0f); // 全部为正数，小数点保留2位
        Delay_Ms(100);
    }
}
