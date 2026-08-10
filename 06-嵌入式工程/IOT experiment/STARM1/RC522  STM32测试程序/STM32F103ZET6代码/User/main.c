#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include <stdbool.h>



void IC_test ( void )
{
	char cStr [ 30 ];
  u8 ucArray_ID [ 4 ];                                                                                             //先后存放IC卡的类型和UID(IC卡序列号)
	u8 ucStatusReturn;                                                                                               //返回状态
  static u8 ucLineCount = 0; 
	
  while ( 1 )
  { 
		if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK )                                    //寻卡
		{
				ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID );		                                                 //若失败再次寻卡
		}
		if ( ucStatusReturn == MI_OK  )
		{
			if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   //防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）
			{
				sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X", ucArray_ID [ 0 ], ucArray_ID [ 1 ], ucArray_ID [ 2 ], ucArray_ID [ 3 ] );
				
				
				printf ( "%s\r\n",cStr ); 
				

				if ( ucLineCount == 0 )
				
				ucLineCount ++;
				
				if ( ucLineCount == 17 ) ucLineCount = 0;
				
				
			}
			
		}
		
  }
	
	
}




/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{
	SysTick_Init ();   //滴答时钟初始化
	
	USART1_Config ();  //USART1 配置模式为 115200 8-N-1，中断接收 
	
	RC522_Init ();     //RC522模块所需外设的初始化配置
	
	
	
	printf ( "WF-RC522 Test\n" );
	
	
	
	
	PcdReset ();
	M500PcdConfigISOType ( 'A' );//设置工作方式
	
	
  while ( 1 )
  {
    IC_test ();//IC卡检测	

  }
	
	
}


/****************************END OF FILE**********************/

