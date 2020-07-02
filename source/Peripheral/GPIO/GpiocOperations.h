/**
 * @file GpiocOperations.h
 *
 * @brief public API for interacting with gpio operations for controlling the relays and other gpio operations.
 */

#ifndef GPIO_GPIOCOPERATIONS_H_
#define GPIO_GPIOCOPERATIONS_H_


#define RELAY_TRIP_ON 		(1<<0)
#define RELAY_TRIP_OFF 		(1<<1)
#define RELAY_NGR_ON 		(1<<2)
#define RELAY_NGR_OFF 		(1<<3)
#define RELAY_GND_ON 		(1<<4)
#define RELAY_GND_OFF 		(1<<5)
#define RELAY_AUX_ON 		(1<<6)
#define RELAY_AUX_OFF 		(1<<7)
#define TRIGGER_SELFTEST	(1<<8)
#define SELFTEST_DONE		(1<<9)
#define RESET_ON 			(1<<10)
#define RESET_OFF 			(1<<11)
#define PT_RELAYS_ON 		(1<<12)
#define PT_RELAYS_OFF		(1<<13)
#define PT_TEST_DIGITAL_IO  (1<<14)


typedef enum _GPIOSelfTest_e
{
	eGpioSelfTestCompleted = 0,
	eGpioSelfTestNotTriggered,
	eGpioSelfTestTestTrip,
	eGpioSelfTestTestGnd,
	eGpioSelfTestTestNGR,
	eGpioSelfTestTestAUX,
	eGpioSelfTestTestPending,
}eGPIOSelfTest_t;

TaskHandle_t GetGPIOTaskHandle();
void StartGpioTask();
eGPIOSelfTest_t GpioSelfTestResults();
void triggerEnergizeAllRelays();
void triggerReleaseAllRelays();

#endif /* GPIO_GPIOCOPERATIONS_H_ */
