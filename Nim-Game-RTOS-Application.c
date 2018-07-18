/*
 * Nim-Game-RTOS-Application.c
 *
 *  Created on: 12-May-2017
 *      Author: Harish Mohan
 */
  
/******************************************************************************
 * NOTE 1: The Win32 port is a simulation (or is that emulation?) only!  Do not
 * expect to get real time behaviour from the Win32 port or this demo
 * application.  It is provided as a convenient development and demonstration
 * test bed only.  This was tested using Windows XP on a dual core laptop.
 *
 * Windows will not be running the FreeRTOS simulator threads continuously, so
 * the timing information in the FreeRTOS+Trace logs have no meaningful units.
 * See the documentation page for the Windows simulator for an explanation of
 * the slow timing:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 * - READ THE WEB DOCUMENTATION FOR THIS PORT FOR MORE INFORMATION ON USING IT -
 *
 * NOTE 2:  This project provides two demo applications.  A simple blinky style
 * project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky style version.
 *
 * NOTE 3:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, are defined
 * in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 200 (simulated as far as the scheduler is concerned, but in
 * reality much longer - see notes above) milliseconds, before sending the
 * value 100 to the queue that was created within main_blinky().  Once the
 * value is sent, the task loops back around to block for another 200
 * (simulated) milliseconds.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * main_blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, outputs a message.  The
 * 'block time' parameter passed to the queue receive function specifies that
 * the task should be held in the Blocked state indefinitely to wait for data
 * to be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 200 (simulated - see notes above)
 * milliseconds, the queue receive task leaves the Blocked state every 200
 * milliseconds, and therefore outputs a message every 200 milliseconds.
 */

/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

/* Values passed to the two tasks just to check the task parameter
functionality. */
#define mainQUEUE_SEND_PARAMETER			( 0x1111UL )
#define mainQUEUE_RECEIVE_PARAMETER			( 0x22UL )

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvMainTask( void *pvParameters );
//static void prvQueueSendTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;
int stick=21;

/*-----------------------------------------------------------*/

void main_blinky( void )
{
	/* Create the queue. */
	/*	printf( "\r\nTrace started.  Hit a key to dump trace file to disk (does not work from Eclipse console).\r\n" );
	printf("\n\nRules for the game are as follows:\n There are 21 match sticks.\n The computer asks the player to pick 1, 2, 3, or 4 match sticks.\nAfter the person picks, the computer does its picking.\n Whoever is forced to pick up the last match stick loses the game\n");
	printf("Enter a value between 1 to 4:\n");
*/
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( int ) );

	if( xQueue != NULL )
	{
		printf("Enter a value between 1 to 4:\n");
		xQueueSend( xQueue, &stick, 0U );
		/* Start the two tasks as described in the comments at the top of this
		file. */
		xTaskCreate( prvMainTask,/* The function that implements the task. */
					"Main", 									/* The text name assigned to the task - for debug only as it is not used by the kernel. */
					configMINIMAL_STACK_SIZE, 				/* The size of the stack to allocate to the task. */
					NULL  , /* The parameter passed to the task - just to check the functionality. */
					mainQUEUE_RECEIVE_TASK_PRIORITY, 		/* The priority assigned to the task. */
					NULL );									/* The task handle is not required, so NULL is passed. */

		//xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, ( void * ) mainQUEUE_SEND_PARAMETER, mainQUEUE_SEND_TASK_PRIORITY, NULL );

		/* Start the tasks and timer running. */
		vTaskStartScheduler();
	}
												
	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for( ;; );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

static void prvMainTask( void *pvParameters )
{
//unsigned long ulReceivedValue;
int ll,x=0;
	/* Remove compiler warning in the case that configASSERT() is not
	defined. */
	( void ) pvParameters;

	/* Check the task parameter is as expected. */
	//configASSERT( ( ( unsigned long ) pvParameters ) == mainQUEUE_RECEIVE_PARAMETER );

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ll, portMAX_DELAY );
		scanf("%d",&x);
		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */
			/* Normally calling printf() from a task is not a good idea.  Here
			there is lots of stack space and only one task is using console  IO
			so it is ok. */
		ll-=5;
		xQueueSend( xQueue, &ll, 0U );
			printf( "User chosen Value :\r %d\n",x );
			printf( "Computer chosen value :\r %d\n",5-x );
			printf( "Remaining Sticks :\r %d\n",ll );
			if(ll!=1)
			printf("Enter a value between 1 to 4:\n");

			if(ll==1){ printf("\nGame over. Machine Wins! \nYou have the remaining stick to take!"); }
			fflush( stdout );
			if(ll==1)
			vTaskDelete(NULL);
	}


}

/*-----------------------------------------------------------*/



/*

static void pvrMainTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const unsigned long ulValueToSend = 100UL;
const TickType_t xBlockTime = pdMS_TO_TICKS( mainQUEUE_SEND_FREQUENCY_MS );

	/* Remove compiler warning in the case that configASSERT() is not
	defined. */
//	( void ) pvParameters;

	/* Check the task parameter is as expected. */
	//configASSERT( ( ( unsigned long ) pvParameters ) == mainQUEUE_SEND_PARAMETER );

	/* Initialise xNextWakeTime - this only needs to be done once. */
//	xNextWakeTime = xTaskGetTickCount();

	//for( ;; )
	//{
		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, the constant used converts ticks
		to ms.  While in the Blocked state this task will not consume any CPU
		time. */
		//vTaskDelayUntil( &xNextWakeTime, xBlockTime );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		/*xQueueSend( xQueue, &ulValueToSend, 0U );
	}
}
*/
