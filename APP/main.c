/**
  ******************************************************************************
  * @file    Project/Template/main.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    04/06/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usart.h"	
#include "DJI_Pro_Test.h" 

#include "BSP_Config.H"



unsigned char Key0_Value=1;

unsigned char Key1_Value=1;

unsigned char t;
unsigned char data[10] = {50,51,52,53,0,55,56,57,58,0};

void Key_Delay (void)
{
	unsigned int i;
	
	for(i=0;i<0xfff;i++);
	
	
}

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

 

/* Private function prototypes -----------------------------------------------*/

//void Delay(int nCount);


/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
int main(void)
{
  /* Setup STM32 system (clock, PLL and Flash configuration) */
  SystemInit();

  RCC_Configuration();

  NVIC_Configuration();

  GPIO_Configuration();

  SysTick_init();		     //延时初始化

  SPI2_Init(); 			     //初始化SPI硬件口
	uart_init(115200);	 	//串口初始化为230400，M100支持115200和230400，请结合N1调参软件修改

  TIM2_Config();			//定时器初始化 
	
	LED0_ON();//LED亮
	
  LED0_OFF();//LED灭
	
	LED1_ON();//LED亮
	
  LED1_OFF();//LED灭
 
  OLED_Init();			 //初始化OLED      
	DJI_Pro_Test_Setup(); // 配置DJI SDK序列号 密钥 初始化通信链路
	OLED_ShowString(0,0, "MICL_Piao"); 
	OLED_ShowString(0,16,"Drone Status:");
	OLED_ShowString(0,32,"Wating!");
	
	while(1)
	{	
		

		while(1)//激活API
			{	
				if(DJI_Onboard_API_Activation())
				  {
					   OLED_ShowString(0,32,"Activate Success");
							break;
          }
				else 
						 OLED_ShowString(0,32,"Activate erro   ");			
      }
		delay_ms(1);
		DJI_Onboard_API_Control(1);//获取控制权	
		delay_ms(1000);
	  if(i>3)
		{
			if(RecBuff[0]==0xAA&RecBuff[12]==0x02&RecBuff[13]==0x00)
			{
				OLED_ShowString(0,32,"Obtain Success       ");
			}
			else
				OLED_ShowString(0,32,"Obtain error         ");
			i=0;				
		}	
		else continue;	
		delay_ms(1);			
		DJI_Onboard_API_UAV_Control(4);//起飞
		delay_ms(1000);
		if(i>3)
		{
			if(RecBuff[0]==0xAA&RecBuff[12]==0x02&RecBuff[13]==0x00)
			{
				OLED_ShowString(0,32,"Begin to Take off   ");
			}
			i=0;				
		}	
		else continue;
    
		break; 
  }
  /* Infinite loop */
  while (1)
  {
		
		delay_ms(1000);
		//DJI_Onboard_API_DataTran("Hello");
		DJI_Onboard_API_DataTran(data,5);
		delay_ms(1000);
		DJI_Onboard_API_DataTran(data+5,5);
		//DJI_Onboard_API_DataTran("World");
  }

}




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {


   

  }
}
#endif



/**
  * @}
  */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
