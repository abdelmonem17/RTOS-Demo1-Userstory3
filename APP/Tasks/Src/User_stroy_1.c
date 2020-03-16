

/****************************************************************************
 * @file Req1_Led.h
 * @brief this file includes some functions implementation to manipulate leds using RTOS
 *
 * @author Abdelmonem Mostafa
 * @date  NOV 2019
 *
 ****************************************************************************/

#include "FreeRTOS.h"
#include "queue.h"
#include "User_stroy_1.h"
#include "event_groups.h"
#include "task.h"
#include "led.h"
#include "lcd.h"
#include "Button.h"
#include "led.h"









/************************************************************************/
/*				macros to define tasks info                              */
/************************************************************************/
#define	 REQ2_LED_INIT_TASK_STACK_DEPTH									100
#define  REQ2_LED_TOGGLE_TASK_STACK_DEPTH								100
#define	 REQ2_BUTTON0_TASK_STACK_DEPTH									100
#define  REQ2_BUTTON1_TASK_STACK_DEPTH									100
#define  REQ2_LCD_TASK_STACK_DEPTH										100

#define	 REQ2_INIT_TASK_PRIORTIY										4
#define  REQ2_BUTTON0_TASK_PRIORTY										1
#define  REQ2_BUTTON1_TASK_PRIORTY										2
#define  REQ2_LCD_TASK_PRIORTY											3

#define	 REQ2_INPUT_PARAM_REF											NULL




#define LCD_START_COLUMN_BUTTON0_STATUS									1
#define LCD_START_ROW_BUTTON0_STATUS									1
#define LCD_START_COLUMN_BUTTON1_STATUS									1
#define LCD_START_ROW_BUTTON1_STATUS									2

#define LCD_DISPLAYING_AREA												16



#define INIT_TASK_PERIODICITY											15
#define BUTTON0_TASK_PERIODICITY										33
#define BUTTON1_TASK_PERIODICITY										40
#define LCD_TASK_PERIODICITY											3
#define LED_TASK_PERIODICITY											30



#define LCD_DISPLAY_STATE_WAIT_BUTTONS_QUEUE							(uint8)0
#define LCD_DISPLAY_STATE_CLEAR_AREA									(uint8)1
#define LCD_DISPLAY_STATE_GOTO_ROW_LOCATION					(uint8)2
#define LCD_DISPLAY_STATE_DISPLAY_STRING						(uint8)3

#define LCD_DISPLAY_STATE_GOTO_ROW_LOCATION_BUTTON_STATUS				(uint8)4
#define LCD_DISPLAY_STATE_DISPLAY_STRING_BUTTON_STATUS					(uint8)5

#define LCD_DISPLAY_STATE_LAST_STRING_DELAY								(uint8)4

#define BUTTON0_STATUS_FLAG												BIT0
#define BUTTON1_STATUS_FLAG												BIT1

#define BUTTON0			0
#define BUTTON1			1

#define BUTTONS_CHANGING_STATUS_LED_FLAG								BIT3
#define BUTTONS_CHANFING_STATUS_LCD_FLAG								BIT4

#define BUTTON_INFO_QUEUE_SIZE											5

#define NONE															5


/************************************************************************/
/*                         global static for tasks events                */
/************************************************************************/

static TaskHandle_t	req2_Lcd_TaskHandler;
static TaskHandle_t	req2_Init_TaskHandler;
static TaskHandle_t	req2_Button0_TaskHandler;
static TaskHandle_t	req2_Button1_TaskHandler;



static uint16 gu16_Ticks=0;
static uint8 gu8_TicksOverflowFlag=0;

typedef struct {
	
	uint8	btnNum;
	char	*btnStr;
	uint8 row;
	uint8 column;
	
	}ST_ButtonInfoqueueItem_t;

QueueHandle_t gQueueHandle_ButtonInf;


/****************************************************************************
*
*   @brief function acts as task which monitors LED0
*   this function takes void and return void to turn led on and off
*   @params  name : void
*
*   @return void
****************************************************************************/


/****************************************************************************
*
*   @brief function acts as task which monitors button 0
*   this function takes pvParameters and return void 
*   @params  name : void
*
*   @return void
****************************************************************************/
void req2_Button0_Task( void * pvParameters)
{
	
	
	
	uint8 button_status=LOW;
	ST_ButtonInfoqueueItem_t aST_ButtonInfoqueueItem_button0info ={BTN0, "BUTTON 0:HERE",1,1};		
	ST_ButtonInfoqueueItem_t *ptr=&aST_ButtonInfoqueueItem_button0info;
	while(1)
	{
		
		 

		
		
			button_status = BTN_Read_Undebounce(BTN0);
			
					
				/* check if the change to high or to low */
				if (button_status == HIGH)
				{
					
					
					xQueueSend(
					gQueueHandle_ButtonInf,
					(void *)(&ptr),
					UINT16_MAX
					);		

				}
				else
				{
					
											
				}
		
		vTaskDelay(BUTTON0_TASK_PERIODICITY);
		
	}
}


/****************************************************************************
*
*   @brief function acts as task which monitors button 1
*   this function takes pvParameters and return void
*   @params  name : void
*
*   @return void
****************************************************************************/
void req2_Button1_Task( void * pvParameters)
{
	uint8 button_status=LOW;
	ST_ButtonInfoqueueItem_t aST_ButtonInfoqueueItem_button1Info ={BTN1, "BUTTON 1:HERE",2,1};
	ST_ButtonInfoqueueItem_t *ptr=&aST_ButtonInfoqueueItem_button1Info;
	while(1)
	{
		
		
		button_status = BTN_Read_Undebounce(BTN1);
		
		/* check if the change to high or to low */
		if (button_status == HIGH)
		{
			
			
			xQueueSend(
			gQueueHandle_ButtonInf,
			(void *)(&ptr),
			UINT16_MAX
			);


		}
		else
		{
			
			
		}	
		
		
		vTaskDelay(BUTTON1_TASK_PERIODICITY);
		
	}
}


/****************************************************************************
*
*   @brief function acts as task which monitors buttons and print on the LCD
*   this function takes pvParameters and return void
*   @params  name : void
*
*   @return void
****************************************************************************/
void req2_Lcd_Task( void * pvParameters)
{
	
	
	
	uint8 au8_State=LCD_DISPLAY_STATE_WAIT_BUTTONS_QUEUE;
	ST_ButtonInfoqueueItem_t * apButtonInfoqueueItem_ButtonInfo;
	
 	static volatile uint32	au32_Button0MessageDisplayingCounter=NUMBER_ZERO;
	 static volatile uint8	au8_Button0MessageDisplayingFlag=FALSE;
	
	static volatile uint32	au32_Button1MessageDisplayingCounter=NUMBER_ZERO;
	static volatile uint8	au8_Button1MessageDisplayingFlag=FALSE;
	
	uint8 au8_TickOverflowFlag;
	char *aStr_PrintedText=NULL;
	uint8 au8_row,au8_column;
	uint8 btn=NONE;
	
	 BaseType_t aBaseType_ReceivingFromQueueStatus;
	while(TRUE)
	{
		
		au8_TickOverflowFlag =gu8_TicksOverflowFlag;
		gu8_TicksOverflowFlag =FALSE;
		
		switch(au8_State)
		{
			/*	check  flag from a button that there is a change and don't wait any way	*/
			case LCD_DISPLAY_STATE_WAIT_BUTTONS_QUEUE:
				aBaseType_ReceivingFromQueueStatus=	xQueueReceive( gQueueHandle_ButtonInf,
												(void *)&apButtonInfoqueueItem_ButtonInfo,
												NUMBER_ZERO
												);
				
				
				
				if ( (au8_TickOverflowFlag == TRUE) && (au8_Button0MessageDisplayingFlag == TRUE) )
				{
					au32_Button0MessageDisplayingCounter =(uint16) au32_Button0MessageDisplayingCounter;
				}
				
				if ( (au8_TickOverflowFlag == 1) && (au8_Button1MessageDisplayingFlag==1) )
				{
					au32_Button1MessageDisplayingCounter =(uint16) au32_Button1MessageDisplayingCounter;
			    }
				
				
				 if (aBaseType_ReceivingFromQueueStatus == pdTRUE)
				 {
					
						
						 aStr_PrintedText = apButtonInfoqueueItem_ButtonInfo->btnStr;
						 au8_row		  =apButtonInfoqueueItem_ButtonInfo->row;
						 au8_column       =apButtonInfoqueueItem_ButtonInfo->column;
						 btn			 =apButtonInfoqueueItem_ButtonInfo->btnNum;
						 au8_State =	  LCD_DISPLAY_STATE_GOTO_ROW_LOCATION;
						 
						 
					 
				 }
				 else if ( au8_Button0MessageDisplayingFlag ==1 && (au32_Button0MessageDisplayingCounter <= gu16_Ticks) )
				 {
					 
					 /*  clear LCD for button 0 */
					  aStr_PrintedText = "                ";
					  au8_row			=LCD_START_ROW_BUTTON0_STATUS;
					  au8_column		=LCD_START_COLUMN_BUTTON0_STATUS;
					  btn =NONE;
					  au8_Button0MessageDisplayingFlag =FALSE;
					  au8_State =	  LCD_DISPLAY_STATE_GOTO_ROW_LOCATION;
					  
					  
				 }
				 else if ((au8_Button1MessageDisplayingFlag == 1) && (au32_Button1MessageDisplayingCounter <= gu16_Ticks) )
				 {
					  /*  clear LCD for button 1 */
					 aStr_PrintedText = "                 ";
					 au8_row			=LCD_START_ROW_BUTTON1_STATUS;
					 au8_column		=LCD_START_COLUMN_BUTTON1_STATUS;
					 btn=NONE;
					 au8_Button1MessageDisplayingFlag =FALSE;
					 au8_State =	  LCD_DISPLAY_STATE_GOTO_ROW_LOCATION;
					 
				 }
				 else
				 {
					 /* stay here nothing to do for LCD */
					 vTaskDelay(NUMBER_ONE);
					
				 }			
					break;
			
			
			case LCD_DISPLAY_STATE_GOTO_ROW_LOCATION:
				if( ( LCD_GoToRowColumn(au8_row,au8_column) == SEND_CMD_NOT_FINISHED))
				{
					vTaskDelay(NUMBER_ONE);
				
				}
				else
				{
					au8_State =LCD_DISPLAY_STATE_DISPLAY_STRING;
				}
				break;
				/*		display on LCD		*/
			case LCD_DISPLAY_STATE_DISPLAY_STRING:
				if( ( LCD_DisplayString(aStr_PrintedText) == SEND_DATA_NOT_FINISHED))
				{
					vTaskDelay(NUMBER_ONE);
				
				}
				else
				{
					//&& (au32_Button1MessageDisplayingCounter <= gu16_Ticks)
							/* We have finished accessing the shared resource.  Release the
							semaphore. */
					 
							//au8_State =LCD_DISPLAY_STATE_GOTO_ROW_LOCATION_BUTTON_STATUS;
							
							/*  if a Button on displayed reset it's counter  */
							if (btn !=NONE)
							{
								Leds_Toggle(LED0);
								
								switch(btn)
								{
									case BTN0 :
									Leds_Toggle(LED1);
										au32_Button0MessageDisplayingCounter = (uint32)((uint32)gu16_Ticks+(uint32)500);
										au8_Button0MessageDisplayingFlag =1;
										 
									break;
									case BTN1 :
									Leds_On(LED2);
										au32_Button1MessageDisplayingCounter =(uint32)((uint32)gu16_Ticks+(uint32)500);
										au8_Button1MessageDisplayingFlag =1;
									break;
									
								}
							}
							au8_State =LCD_DISPLAY_STATE_WAIT_BUTTONS_QUEUE;
							vTaskDelay(LCD_TASK_PERIODICITY);
				}
			break;
				
				
			
			
		}
		//vTaskDelay(LCD_TASK_PERIODICITY);
		
	}

}


/****************************************************************************
*
*   @brief function acts as task which inits the system
*   this function takes pvParameters and return void
*   @params  name : void
*
*   @return void
****************************************************************************/
void req2_Init_Task(void * pvParameters)
{
	
	/*clear led and lcd flags flags*/
	
	BTN_init();
	
	while(TRUE)
	{
		
		/* execuit this until it finishs */
		if (LCD_init() == INIT_LCD_NOT_FINISHED)
		{
			vTaskDelay(NUMBER_ONE);
		}
		else
		{
			  vTaskResume(req2_Button0_TaskHandler);
			  vTaskResume(req2_Button1_TaskHandler);
			  vTaskSuspend(req2_Init_TaskHandler);
			
		}
		
		
		
	}
}



/****************************************************************************
*
*   @brief function creates tasks
*   this function takes errorBuffer to fill with the status of the tasks creation and returns status of the creation
*   @params  name : errorBuffer
*	type :uint8 *
*   @return uint8
****************************************************************************/
uint8 req2_tasksCreation(uint8 *errorBuffer)
{
	uint8 status=REQ2_TASKS_SUCCFULLY_CREATED;
	uint8 tasks_size=0;
	if (errorBuffer != NULL)
	{
	
	errorBuffer[tasks_size++] =	xTaskCreate(    req2_Init_Task,
						 "Init task",
						 REQ2_LED_INIT_TASK_STACK_DEPTH,
						 REQ2_INPUT_PARAM_REF,
						 REQ2_INIT_TASK_PRIORTIY,
						 &req2_Init_TaskHandler
						 );
	 
	 
	
					 
	 errorBuffer[tasks_size++]	=xTaskCreate(    req2_Button0_Task,
					 "BUTTON 0",
					 REQ2_BUTTON0_TASK_STACK_DEPTH,
					 REQ2_INPUT_PARAM_REF,
					 REQ2_BUTTON0_TASK_PRIORTY,
					 &req2_Button0_TaskHandler
					 );
	errorBuffer[tasks_size++]	=xTaskCreate(    req2_Button1_Task,
					 "BUTTON 1",
					 REQ2_BUTTON1_TASK_STACK_DEPTH,
					 REQ2_INPUT_PARAM_REF,
					 REQ2_BUTTON1_TASK_PRIORTY,
					 &req2_Button1_TaskHandler
					 );
   errorBuffer[tasks_size++]	=xTaskCreate(    req2_Lcd_Task,
					 "LCD",
					 REQ2_LCD_TASK_STACK_DEPTH,
					 REQ2_INPUT_PARAM_REF,
					 REQ2_LCD_TASK_PRIORTY,
					 &req2_Lcd_TaskHandler
					 );
			for (uint8 index=NUMBER_ZERO;index < tasks_size ;index++ )
			{
				if (errorBuffer[index] !=pdPASS)
				{
					errorBuffer[index]= REQ2_TASK_NOTSUCCFULLY_CREATED;
					status = REQ2_TASKS_NOTSUCCFULLY_CREATED;
				}
				else
				{
					errorBuffer[index]= REQ2_TASK_SUCCFULLY_CREATED;
				}
			}
			
			
			
			 gQueueHandle_ButtonInf = xQueueCreate(BUTTON_INFO_QUEUE_SIZE ,sizeof(ST_ButtonInfoqueueItem_t *) );
			 if (gQueueHandle_ButtonInf !=NULL)
			 {
				 vTaskSuspend(req2_Button0_TaskHandler);
				 vTaskSuspend(req2_Button1_TaskHandler);
			 }
			 else
			 {
				status	= REQ2_TASKS_NOTSUCCFULLY_CREATED_BUTTON_INFO_QUEUE_NULL; 
			 }
			
			
					 
		}
		else
		{
			status =REQ2_TASKS_ERROR_REPORT_BUFFER_NULL;
		}
	return status;
}


void vApplicationTickHook( void )
{
	/* detect over flow */
	if ((uint16)gu16_Ticks > (uint16)(gu16_Ticks+1) )
	{
		gu8_TicksOverflowFlag=1;
	}
	++gu16_Ticks;
}