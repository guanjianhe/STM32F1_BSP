#include "led.h"

typedef struct led_info
{
    uint32_t RCC_APB2Periph;
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    uint8_t ON_Sta;
} led_t;

static const led_t led[] =
{
    {
        .RCC_APB2Periph = RCC_APB2Periph_GPIOA,
        .GPIOx = GPIOA,
        .GPIO_Pin = GPIO_Pin_8,
        .ON_Sta = 0,
    },
    {
        .RCC_APB2Periph = RCC_APB2Periph_GPIOD,
        .GPIOx = GPIOD,
        .GPIO_Pin = GPIO_Pin_2,
        .ON_Sta = 0,
    },
};

static const uint8_t LED_NUM = sizeof(led) / sizeof(led[0]);

static void LED_GPIO_Configuration(void)
{
    int i;
    GPIO_InitTypeDef GPIO_InitStructure;

    for (i = 0; i < LED_NUM; i++)
    {
        RCC_APB2PeriphClockCmd(led[i].RCC_APB2Periph, ENABLE);
        GPIO_InitStructure.GPIO_Pin = led[i].GPIO_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(led[i].GPIOx, &GPIO_InitStructure);
        GPIO_WriteBit(led[i].GPIOx,
                      led[i].GPIO_Pin,
                      (BitAction)!led[i].ON_Sta);
    }
}

void led_init(void)
{
    LED_GPIO_Configuration();
}

int led_set(uint8_t led_id, uint8_t state)
{
    int retval = 0;

    if (led_id < LED_NUM)
    {
        if (state)
        {
            GPIO_WriteBit(led[led_id].GPIOx,
                          led[led_id].GPIO_Pin,
                          (BitAction)led[led_id].ON_Sta);
        }
        else
        {
            GPIO_WriteBit(led[led_id].GPIOx,
                          led[led_id].GPIO_Pin,
                          (BitAction)!led[led_id].ON_Sta);
        }

    }
    else
    {
        retval = -1;
    }

    return retval;

}

int led_on(uint8_t led_id)
{
    int retval = 0;

    if (led_id < LED_NUM)
    {
        GPIO_WriteBit(led[led_id].GPIOx,
                      led[led_id].GPIO_Pin,
                      (BitAction)led[led_id].ON_Sta);
    }
    else
    {
        retval = -1;
    }

    return retval;

}

int led_off(uint8_t led_id)
{
    int retval = 0;

    if (led_id < LED_NUM)
    {
        GPIO_WriteBit(led[led_id].GPIOx,
                      led[led_id].GPIO_Pin,
                      (BitAction)!led[led_id].ON_Sta);
    }
    else
    {
        retval = -1;
    }

    return retval;
}

int led_toggle(uint8_t led_id)
{
    int retval = 0;

    if (led_id < LED_NUM)
    {
        led[led_id].GPIOx->ODR ^= led[led_id].GPIO_Pin;
    }
    else
    {
        retval = -1;
    }

    return retval;
}
