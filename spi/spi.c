#include "spi.h"

/* Set SCLK = PCLK / 256 */
void spi_fclkslow(void)
{
    SPI1->CR1 = (SPI1->CR1 & ~0x38) | 0x38;
}

/* Set SCLK = PCLK / 2 */
void spi_fclkfast(void)
{
    SPI1->CR1 = (SPI1->CR1 & ~0x38) | 0x00;
}

void spi_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    /* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫�� */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    /* ����SPI����ģʽ:����Ϊ��SPI */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    /* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ */
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    /* ѡ���˴���ʱ�ӵ���̬:ʱ�����ո� */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    /* ���ݲ����ڵڶ���ʱ���� */
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    /* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    /* ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256 */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    /* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    /* CRCֵ����Ķ���ʽ */
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    /* ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ��� */
    SPI_Init(SPI1, &SPI_InitStructure);
    /* ʹ��SPI���� */
    SPI_Cmd(SPI1, ENABLE);
}

/* ����һ���ֽ� */
uint8_t spi_xchg(uint8_t dat)
{
    SPI1->DR = dat;

    while ((SPI1->SR & 0x83) != 0x03) ;

    return (uint8_t)SPI1->DR;
}

/* ��ȡ���ֽڣ�������ż���� */
void spi_readmulti(uint8_t* buff, uint16_t btr)
{
    uint16_t d;

    SPI1->CR1 &= ~(1 << 6);
    SPI1->CR1 |= ((1 << 6) | (1 << 11));

    SPI1->DR = 0xFFFF;
    btr -= 2;

    do
    {
        while ((SPI1->SR & 0x83) != 0x03);

        d = SPI1->DR;
        SPI1->DR = 0xFFFF;
        buff[1] = d;
        buff[0] = d >> 8;
        buff += 2;
    } while (btr -= 2);

    while ((SPI1->SR & 0x83) != 0x03);

    d = SPI1->DR;
    buff[1] = d;
    buff[0] = d >> 8;

    SPI1->CR1 &= ~((1 << 6) | (1 << 11));
    SPI1->CR1 |= 1 << 6;
}


/* д���ֽڣ�������ż���� */
void spi_writemulti(const uint8_t* buff, uint16_t btx)
{
    uint16_t d;

    SPI1->CR1 &= ~(1 << 6);
    SPI1->CR1 |= ((1 << 6) | (1 << 11));

    d = buff[0] << 8 | buff[1];
    buff += 2;
    SPI1->DR = d;
    btx -= 2;

    do
    {
        d = buff[0] << 8 | buff[1];
        buff += 2;

        while ((SPI1->SR & 0x83) != 0x03) ;

        SPI1->DR;
        SPI1->DR = d;
    } while (btx -= 2);

    while ((SPI1->SR & 0x83) != 0x03) ;

    SPI1->DR;

    SPI1->CR1 &= ~((1 << 6) | (1 << 11));
    SPI1->CR1 |= (1 << 6);
}

