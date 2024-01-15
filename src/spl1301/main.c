#include "debug.h"
#include "SPL1301/spl1301.h"
#include "IIC/iic.h"

float press, temp;
int dis_press;
int error_time;

static void fail(void)
{
    spl1301_interface_iic_deinit();
    RCC_ClearFlag();
    NVIC_SystemReset();
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    if (spl1301_init() != 0)
    {
        Delay_Ms(500);
        fail();
    }
    for (int i = 0; i < 5; i++)
    {
        Delay_Ms(100);
        get_spl1301_pressure();
    }
    Delay_Ms(100);
    float initial_press = get_spl1301_pressure();

    for (;;)
    {
        Delay_Ms(30);
        press = get_spl1301_pressure();
        temp = get_spl1301_temperature();
        if (press < 0 || temp < 0)
            if (error_time++ == 5)
                fail();
        dis_press = (press - initial_press) / 101.325f;
        printf("P%dD%dT%d\r\n", (int)press, dis_press > 0 ? dis_press : 0, (int)temp); // Pa cm °„C
    }
}
