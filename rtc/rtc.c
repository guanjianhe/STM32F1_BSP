#include "rtc.h"

#define F_LSE   32768   /* LSE oscillator frequency */

#define _RTC_TDIF   +8.0 /* 时区，东八区 */

static const uint8_t samurai[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint8_t rtcok = 0;

int rtc_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    if (BKP_ReadBackupRegister(BKP_DR1) != 0x1232)
    {
        PWR_BackupAccessCmd(ENABLE);
        RTC_WaitForLastTask();
        BKP_DeInit();
        RCC_LSEConfig(RCC_LSE_ON);

        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForLastTask();
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        RTC_SetPrescaler(F_LSE - 1);
        RTC_WaitForLastTask();
        BKP_WriteBackupRegister(BKP_DR1, 0X1232);
        PWR_BackupAccessCmd(DISABLE);
    }
    else
    {
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
    }

    rtcok = 1;

    return rtcok;
}




int rtc_setutc(uint32_t tmr)
{
    int ret_val = 0;

    if (rtcok)
    {
        PWR_BackupAccessCmd(ENABLE);
        RTC_WaitForLastTask();
        RTC_SetCounter(tmr);
        RTC_WaitForLastTask();
        PWR_BackupAccessCmd(DISABLE);
        ret_val = 1;
    }

    return ret_val;
}


int rtc_getutc(uint32_t* tmr)
{
    uint32_t t1, t2;

    if (rtcok)
    {
        t1 = RTC_GetCounter();

        do
        {
            t2 = t1;
            t1 = RTC_GetCounter();
        } while (t1 != t2);

        *tmr = t1;
        return 1;
    }

    return 0;
}

int rtc_gettime(rtc_t* rtc)
{
    uint32_t utc, n, i, d;

    if (!rtc_getutc(&utc))
    {
        return 0;
    }

    utc += (int32_t)(_RTC_TDIF * 3600);

    rtc->sec = (uint8_t)(utc % 60);
    utc /= 60;
    rtc->min = (uint8_t)(utc % 60);
    utc /= 60;
    rtc->hour = (uint8_t)(utc % 24);
    utc /= 24;
    rtc->wday = (uint8_t)((utc + 4) % 7);
    rtc->year = (uint16_t)(1970 + utc / 1461 * 4);
    utc %= 1461;
    n = ((utc >= 1096) ? utc - 1 : utc) / 365;
    rtc->year += n;
    utc -= n * 365 + (n > 2 ? 1 : 0);

    for (i = 0; i < 12; i++)
    {
        d = samurai[i];

        if (i == 1 && n == 2)
        {
            d++;
        }

        if (utc < d)
        {
            break;
        }

        utc -= d;
    }

    rtc->month = (uint8_t)(1 + i);
    rtc->mday = (uint8_t)(1 + utc);

    return 1;
}

int rtc_settime(const rtc_t* rtc)
{
    uint32_t utc, i, y;


    y = rtc->year - 1970;

    if (y > 2106 || !rtc->month || !rtc->mday)
    {
        return 0;
    }

    utc = y / 4 * 1461;
    y %= 4;
    utc += y * 365 + (y > 2 ? 1 : 0);

    for (i = 0; i < 12 && i + 1 < rtc->month; i++)
    {
        utc += samurai[i];

        if (i == 1 && y == 2)
        {
            utc++;
        }
    }

    utc += rtc->mday - 1;
    utc *= 86400;
    utc += rtc->hour * 3600 + rtc->min * 60 + rtc->sec;

    utc -= (long)(_RTC_TDIF * 3600);

    return rtc_setutc(utc);
}

