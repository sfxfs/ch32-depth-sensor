/*
 * USART1 TX-->D.5   RX-->D.6
 * I2C1 SCL-->C.2   SDA-->C.1
*/

#include <application/ms5837_app.h>
#include "debug.h"

int8_t error_count;

float temp, press;

#ifdef EN_CMD
// C | A | 1 | \r\n
void recv_cmd(char data)
{
    static char rxBuffer[10] = {0};
    static uint8_t rxCount = 0;

    rxBuffer[rxCount++] = data;
    if (rxBuffer[0] != 'C')
    {
        rxCount = 0;
        return;
    }
    if (rxBuffer[rxCount-1] != '\n')
    {
        return;
    }

    switch (rxBuffer[1])
    {
    case 'A':
        break;
    default:
        break;
    }

    rxCount = 0;
}
#endif

static void ms5837_fail(void)
{
    printf("Init error, reinit after 1s\r\n\r\n");
    Delay_Ms(500);
    ms5837_basic_deinit();
    Delay_Ms(500);
    RCC_ClearFlag();
    NVIC_SystemReset();
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    if (ms5837_basic_init(MS5837_TYPE_30BA26) != 0)
        ms5837_fail();
    for (;;)
    {
        if (ms5837_basic_read(&temp, &press) != 0) {
            error_count++;
            printf("Cannot read from ms5837 for %d times\r\n", error_count);
        } else {
            printf("T%dP%d\r\n", (int)(temp * 100.0f), (int)(press * 100.0f));  // 保留两位小数
        }

        if (error_count == 5)
            ms5837_fail();

#ifdef EN_CMD
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
        {
            recv_cmd(USART_ReceiveData(USART1) & (uint8_t)0xff);
        }
#endif
    }
}
