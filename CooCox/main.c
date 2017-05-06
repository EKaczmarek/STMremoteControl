#include "main.h"

int lastCommand;
int possible;
/*Struct for IR-Data*/
IRMP_DATA  myIRData;

int main(void)
{
	/* Set up the system clocks */
	SystemInit();

	/*Led init*/
	Led_Init();

	/*IR Init*/
	UB_IRMP_Init();

	/*Init VCP*/
	init();

	/*Init Timer */
	TIMER_1HZ_init(9999);
	TIMER_Interrupt_init();
	int i;

	while (1)
	{
		 // IR Data pollen
		    if(UB_IRMP_Read(&myIRData)==TRUE) {
		      // If IR data has been received
		      if(myIRData.address==65280) {
		        // If address from IR device is correct
					// Button "4"
					if(myIRData.command==8)
							LED_GREEN_ON;

					// Button "2"
					if(myIRData.command==24)
							LED_ORANGE_ON;

					// Button "6"
					if(myIRData.command==90)
							LED_RED_ON;

					// Button "8"
					if(myIRData.command==82)
							LED_BLUE_ON;

					//Button "5"
					if(myIRData.command==28)
							ALL_OFF;

					for(i=0;i<7500000;i++);//bylo 7500000
					if(possible == 1)
						VCP_send_buffer(&myIRData.command,1);

		      }
		    }
	}
	return 0;
}
void init()
{
	/* Setup USB */
	USBD_Init(&USB_OTG_dev,
	            USB_OTG_FS_CORE_ID,
	            &USR_desc,
	            &USBD_CDC_cb,
	            &USR_cb);

	return;
}
void TIMER_1HZ_init(uint16_t a){

 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
 	TIM_TimeBaseInitTypeDef str;
 	str.TIM_Period=16799; //WCZESNIEJ 8399, optymalnie 2099,4199
 	str.TIM_Prescaler=a;
 	str.TIM_ClockDivision=TIM_CKD_DIV1;

 	str.TIM_CounterMode=TIM_CounterMode_Up;
 	TIM_TimeBaseInit(TIM3,&str);
 	TIM_Cmd(TIM3, ENABLE);
}
void TIMER_Interrupt_init(void)
 {
 	NVIC_InitTypeDef str;
 	str.NVIC_IRQChannel = TIM3_IRQn ;
 	str.NVIC_IRQChannelPreemptionPriority = 0x00;
 	str.NVIC_IRQChannelSubPriority = 0x00;
 	str.NVIC_IRQChannelCmd = ENABLE ;
 	NVIC_Init(&str);
 	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
 	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );
 }
void TIM3_IRQHandler(void)
 {
 	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
 	{
 		if(myIRData.command != 0){
			if(lastCommand == myIRData.command){
				possible = 0;
				myIRData.command = 0;
			}
			else if (lastCommand != myIRData.command)
				possible = 1;
 		}
 		else
 			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);

 		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
 	}
 }

void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}
void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}

