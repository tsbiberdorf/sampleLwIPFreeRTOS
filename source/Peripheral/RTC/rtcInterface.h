/**
 * @file rtcInterface.h
 * @author Sierra Wahlin-Rhoades
 * @date Mar 5, 2020
 * @ingroup rtcInterface 
 */
#ifndef RTCINTERFACE_H_
#define RTCINTERFACE_H_


#include <stdint.h>
#include "fsl_rtc.h"

void rtcInitialize();
#define RTC_TSR         (RTC->TSR)

uint32_t rtcGetRTCTime();
void rtcGetMSecRTCTime(uint32_t *Seconds, uint32_t *mSeconds);
#endif /* RTCINTERFACE_H_ */
