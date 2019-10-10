/*
 * FreeRtos.c
 *
 * Created: 03-06-2019 18:05:35
 * Author : abc
 */ 

#include <tgmath.h>
#include <stdlib.h>
#include "sam.h"
#include "src/asf.h"
#include "I2C.h"
#include "USART.h"
#include "board.h"
#define  RAD_TO_DEG 180/M_PI
void ClocksInit(void);
void AppInit(void);
void vTaskAngle( void *pvParameters );
static void vcalAngles( void *pvParameters);
static void vdebug( void *pvParameters);

static void prvSetupHardware( void )
{
	/* Initialisation is performed by the Atmel board support package. */
	
	
	/* Application hardware and software initialization */
	AppInit();
}

void AppInit(void){
	
	ClocksInit();
	i2cInit();
	InitUart();
	
	// Assign the LED0 pin as OUTPUT
	PORT->Group[LED0_PORT].DIRSET.reg = LED0_PIN_MASK;
	PORT->Group[LED0_PORT].DIRSET.reg = PORT_PA12;
	PORT->Group[LED0_PORT].OUTSET.reg = PORT_PA12;
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	PORT->Group[LED0_PORT].OUTSET.reg = LED0_PIN_MASK;
} // AppInit()






int main( void )
{
	prvSetupHardware();
	/* Create one of the two tasks. */


	xTaskCreate(	vcalAngles,		/* Pointer to the function that implements the task. */
	(const char *)"Angles",	/* Text name for the task.  This is to facilitate debugging only. */
	1000,		/* Stack depth - most small microcontrollers will use much less stack than this. */
	NULL,		/* We are not using the task parameter. */
	2,			/* This task will run at priority 1. */
	NULL );		/* We are not using the task handle. */

xTaskCreate(	vdebug,		/* Pointer to the function that implements the task. */
(const char *)"Debug",	/* Text name for the task.  This is to facilitate debugging only. */
1000,		/* Stack depth - most small microcontrollers will use much less stack than this. */
NULL,		/* We are not using the task parameter. */
1,			/* This task will run at priority 1. */
NULL );		/* We are not using the task handle. */



	/* Start the scheduler to start the tasks executing. */
	vTaskStartScheduler();	

	/* The following line should never be reached because vTaskStartScheduler() 
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	PORT->Group[LED0_PORT].OUTCLR.reg = PORT_PA12;
	for( ;; );
	
	return 0;
}
/*-----------------------------------------------------------*/

double roll_angle=0;
double pitch_angle=0;



static void vcalAngles( void *pvParameters){
	
	TickType_t  xLastWakeTime=xTaskGetTickCount();
	double accl_x,accl_y,accl_z,temp,gyro_x,gyro_y,gyro_z,dt,gyroX_angle,gyroY_angle,roll,pitch;
	double err_accX=153, err_accY=-57, err_accZ=-468;
	double err_gyroX=-68, err_gyroY=40, err_gyroZ=-96;
	start_write_transmission();
	write_i2c(0x6B);
	write_i2c(0x00);
	end_transmission();
	start_write_transmission();
	write_i2c(0x1B);
	write_i2c(0x08);
	end_transmission();
	start_write_transmission();
	write_i2c(0x1C);
	write_i2c(0x10);
	end_transmission();
	start_write_transmission();
	write_i2c(0x1A);
	write_i2c(0x03);
	end_transmission();
	for(;;){
		vTaskDelayUntil(&xLastWakeTime,100);
		start_write_transmission();
		write_i2c(0x3B);
		// step 4: request reading from sensor
		requestFrom(SLAVE_ADDR, 14);    // request 2 bytes from slave device #112
		accl_x=(int16_t)(rx_buf[0]<<8|rx_buf[1]);
		accl_y=(int16_t)(rx_buf[2]<<8|rx_buf[3]);
		accl_z=(int16_t)(rx_buf[4]<<8|rx_buf[5]);
		temp=(int16_t)(rx_buf[6]<<8|rx_buf[7]);
		gyro_x=(int16_t)(rx_buf[8]<<8|rx_buf[9]);
		gyro_y=(int16_t)(rx_buf[10]<<8|rx_buf[11]);
		gyro_z=(int16_t)(rx_buf[12]<<8|rx_buf[13]);

		accl_x-=err_accX;
		accl_y-=err_accY;
		accl_z-=err_accZ;
		gyro_x-=err_gyroX;
		gyro_y-=err_gyroY;
		gyro_z-=err_gyroZ;
		
		dt=1/250;
		
		gyroX_angle= gyro_x*65.5*dt;
		gyroY_angle= gyro_y*65.5*dt;
		
		
		roll  = atan2(accl_y, accl_z) *RAD_TO_DEG ;
		pitch = atan(-accl_x / sqrt(accl_y * accl_y + accl_z * accl_z)) *RAD_TO_DEG ;
		
		roll_angle=(roll_angle+gyroX_angle)*0.93+roll*0.07;
		pitch_angle=(pitch_angle+gyroY_angle)*0.93+pitch*0.07;
		PORT->Group[LED0_PORT].OUTTGL.reg = PORT_PA13;

	}
}

static void vdebug( void *pvParameters){
	
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		vTaskDelayUntil(&xLastWakeTime,1000);

		printFloat(roll_angle,2);
		uart_putc('\t');
		printFloat(pitch_angle,2);
		uart_putc('\n');
		PORT->Group[LED0_PORT].OUTTGL.reg = PORT_PA12;
		
	}
}
/*-----------------------------------------------------------*/



void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/
