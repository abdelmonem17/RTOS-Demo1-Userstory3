/*
 * free RTOS challenge.c
 *
 * Created: 11/30/2019 1:53:58 PM
 * Author : AVE-LAP-062
 */ 

#include <avr/io.h>

#include "task.h"

#include "User_stroy_1.h"
#include "led.h"


int main(void)
{
   // Leds_Init();
	
	 uint8 arr[REQ2_TASKS_COUNTS];
	 
	 
	//req1_TasksCreation(arr);
	if( req2_tasksCreation(arr) == REQ2_TASKS_SUCCFULLY_CREATED)
	{
		//Leds_On(LED3);
	 vTaskStartScheduler();
	 
	}
    while (1)
    {
    }
}

