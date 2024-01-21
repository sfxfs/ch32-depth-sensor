#include "spi.h"

void spi_init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);

    // CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET);

    // SCK
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MISO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}

int spi_transmit(uint8_t *tx_buf, uint8_t *rx_buf, size_t size)
{
    int time;
    for (int i = 0; i < size; i++)
    {
        time = 100000;
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET) if ((--time) == 0) return -1;
        SPI_I2S_SendData(SPI1, tx_buf[i]);

        time = 100000;
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET)  if ((--time) == 0) return -2;
        rx_buf[i] = SPI_I2S_ReceiveData(SPI1);
    }
    return 0;
}
