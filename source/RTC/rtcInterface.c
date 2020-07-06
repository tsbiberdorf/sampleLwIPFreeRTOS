/**
 * @file rtcInterface.c
 * @author Sierra Wahlin-Rhoades
 * @date March 5, 2020
 * @ingroup rtcInterface
 *
 * @details
 * Wrapper class for interacting with fsl_rtc to set RTC Values since MP8000 ported code references this. It may also contain
 * other helper functions not included in the fsl_rtc carried over from MP8000. Only copying over functions as they are called
 * by ported code.
 *
 *  Newer code should just directly communicate with
 * the fsl_rtc file where possible.
 *
 * @{
 */

/**@}*/

#include <stdint.h>

#include "fsl_rtc.h"

#include "rtcInterface.h"

/**
 * @details return the number of seconds since the last reset
 *
 * @return ageInSeconds
 */
uint32_t rtcGetRTCTime(){
	uint32_t ageInSeconds = RTC->TSR;
	return ageInSeconds;
}

uint32_t rtcGetRunTimeStateTime()
{
	uint32_t Seconds,subSeconds;
	Seconds = RTC->TPR;
	subSeconds = RTC->TSR;

	Seconds = Seconds<<16; // shift seconds time up 16 bits.
	subSeconds &= 0xFFFF; // mask off lower 16 bits of sub seconds
	Seconds |= subSeconds;
	return Seconds;
}

/**
 * @details provide a subSecond resolution to the time
 * The sub second will return the 100th second value
 */
void rtcGetMSecRTCTime(uint32_t *Seconds, uint32_t *mSeconds)
{
	long int temp;
	temp = RTC->TPR;
	*Seconds = RTC->TSR;
	*mSeconds = temp*1000 / (long int)32768;
	return;
}

void rtcInitialize()
{
    rtc_config_t rtcConfig;

	/* Init RTC */
    /*
     * rtcConfig.wakeupSelect = false;
     * rtcConfig.updateMode = false;
     * rtcConfig.supervisorAccess = false;
     * rtcConfig.compensationInterval = 0;
     * rtcConfig.compensationTime = 0;
     */
    RTC_GetDefaultConfig(&rtcConfig);
    RTC_Init(RTC, &rtcConfig);

    /* Select RTC clock source */
    /* Enable the RTC 32KHz oscillator */
    RTC->CR |= RTC_CR_OSCE_MASK;

    RTC_StartTimer(RTC);

}
