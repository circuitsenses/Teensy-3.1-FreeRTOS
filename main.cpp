#include "WProgram.h"

#define TEENSY_FREERTOS 

#ifdef TEENSY_FREERTOS 

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define UARTLog_Task_Priority				( tskIDLE_PRIORITY + 2 )
static xTimerHandle xLEDTimer = NULL;
#define mainLED_TIMER_PERIOD_MS		( 3000UL / portTICK_RATE_MS )
#define mainDONT_BLOCK	(0)


static void prvQueueReceiveTask(void *pvParameters)
{
	while(1)
		{
				digitalWriteFast(13, HIGH);
				delay(10);
				digitalWriteFast(13, LOW);
				delay(1000);
		}
}

static void UARTLogTask(void *pvParameters)
{
	while(1)
		{
			Serial.write("This is UART Log Task\n");
			delay(200);
		}

}

static void prvLEDTimerCallback( xTimerHandle xTimer )
{
	static int toggle = 0;

	if(toggle == 0) {
		digitalWriteFast(12, HIGH);
		toggle = 1;
	}
	else {
		digitalWriteFast(12, LOW);
		toggle = 0;
	}
}


#endif

extern "C" int main(void)
{
#if !defined(ARDUINO)

	// To use Teensy 3.0 without Arduino, simply put your code here.
	// For example:

	pinMode(13, OUTPUT);
	serial.begin(9600);
	while (1) {
		digitalWriteFast(13, HIGH);
		delay(500);
		digitalWriteFast(13, LOW);
		delay(500);
	}


#else

#ifdef TEENSY_FREERTOS
	pinMode(13, OUTPUT);
	pinMode(12, OUTPUT);

	xTaskCreate( prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
	xTaskCreate( UARTLogTask, "ULog", configMINIMAL_STACK_SIZE, NULL, UARTLog_Task_Priority, NULL );

	xLEDTimer = xTimerCreate( 	"LEDTimer", 					/* A text name, purely to help debugging. */
								( mainLED_TIMER_PERIOD_MS ),	/* The timer period, in this case 5000ms (5s). */
								pdTRUE,						/* This is a one shot timer, so xAutoReload is set to pdFALSE. */
								( void * ) 0,					/* The ID is not used, so can be set to anything. */
								prvLEDTimerCallback				/* The callback function that switches the LED off. */
								);
	
	xTimerStart( xLEDTimer, mainDONT_BLOCK );
	vTaskStartScheduler();
#else
	// Arduino's main() function just calls setup() and loop()....
	setup();
	while (1) {
		loop();
		yield();
	}
#endif /* TEENSY_FREERTOS */

#endif
}

