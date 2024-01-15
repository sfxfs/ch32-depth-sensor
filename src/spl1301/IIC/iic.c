#include "debug.h"

#include "iic.h"

#define I2CT_FLAG_TIMEOUT         ((uint32_t)(100000))

static __IO uint32_t  I2CTimeout;

uint8_t spl1301_interface_iic_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure =
    { 0 };
    I2C_InitTypeDef I2C_InitTSturcture =
    { 0 };

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed = 400000;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1 = 0x01;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C1, &I2C_InitTSturcture);

    I2C_Cmd( I2C1, ENABLE);

    return 0;
}

uint8_t spl1301_interface_iic_deinit(void)
{
    I2C_DeInit(I2C1);

    return 0;
}

uint8_t spl1301_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf,
        uint16_t len)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET ) if ((--I2CTimeout) == 0) return 1;

    I2C_GenerateSTART( I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) ) if ((--I2CTimeout) == 0) return 2;

    I2C_Send7bitAddress( I2C1, addr, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) ) if ((--I2CTimeout) == 0) return 3;

    I2C_SendData( I2C1, reg);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) ) if ((--I2CTimeout) == 0) return 4;

    I2C_GenerateSTOP( I2C1, ENABLE);

    //----------------------------------------------------------------------
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET ) if ((--I2CTimeout) == 0) return 5;

    I2C_GenerateSTART( I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) ) if ((--I2CTimeout) == 0) return 6;

    I2C_Send7bitAddress( I2C1, addr, I2C_Direction_Receiver);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) ) if ((--I2CTimeout) == 0) return 7;

    while(len--)
    {
        if(len==0)
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
        }
        else
        {
            I2C_AcknowledgeConfig(I2C1, ENABLE);
        }

        I2CTimeout = I2CT_FLAG_TIMEOUT;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) if ((--I2CTimeout) == 0) return 8;

        *buf = I2C_ReceiveData(I2C1);
        buf++;
    }

    I2C_GenerateSTOP(I2C1, ENABLE);

    return 0;
}

uint8_t spl1301_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf,
        uint16_t len)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET ) if ((--I2CTimeout) == 0) return 1;

    I2C_GenerateSTART( I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) ) if ((--I2CTimeout) == 0) return 2;

    I2C_Send7bitAddress( I2C1, addr, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) ) if ((--I2CTimeout) == 0) return 3;

    I2C_SendData( I2C1, reg);

    for (uint8_t i = 0; i < len; i++)
    {
        I2CTimeout = I2CT_FLAG_TIMEOUT;
        while (!I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) if ((--I2CTimeout) == 0) return 4;

        I2C_SendData( I2C1, buf[i]);
    }

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) ) if ((--I2CTimeout) == 0) return 5;

    I2C_GenerateSTOP( I2C1, ENABLE);

    return 0;
}
