#include "sx126x_example_recive.h"
#include "radio.h"
#include "project_config.h"
#include "stdio.h"
#include "stm32f10x_it.h"
#include "delay.h"
#include "string.h"

/*!
 * Radio events function pointer
 * 这个是传参进入其他函数中了，所以用全局变量(局部变量使用完了内存释放可能导致异常)
 */
static RadioEvents_t SX126xRadioEvents;

static void SX126xOnTxDone( void );
static void SX126xOnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
static void SX126xOnTxTimeout( void );
static void SX126xOnRxTimeout( void );
static void SX126xOnRxError( void );

//开启一个定时发送任务，每隔1S发送一条数据
void ExampleSX126xReciveDemo(void){
	uint8_t OCP_Value = 0;
	printf("start %s() example\r\n",__func__);
	
	SX126xRadioEvents.TxDone = SX126xOnTxDone;
	SX126xRadioEvents.RxDone = SX126xOnRxDone;
	SX126xRadioEvents.TxTimeout = SX126xOnTxTimeout;
	SX126xRadioEvents.RxTimeout = SX126xOnRxTimeout;
	SX126xRadioEvents.RxError = SX126xOnRxError;

	Radio.Init( &SX126xRadioEvents );
	Radio.SetChannel(LORA_FRE);
	Radio.SetTxConfig( MODEM_LORA, LORA_TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                     LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                     LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                     true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );//参数：lora模式,发射功率,fsk用的lora设置为0就可以，带宽，纠错编码率，前导码长度，固定长度数据包(一般是不固定的所以选false)，crc校验，0表示关闭跳频，跳频之间的符号数(关闭跳频这个参数没有意义)，这个应该是表示是否要翻转中断电平的，超时时间

	OCP_Value = Radio.Read(REG_OCP);
	printf("[%s()-%d]read OCP register value:0x%04X\r\n",__func__,__LINE__,OCP_Value);
	
	Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                     LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                     LORA_SX126X_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                     0, true, 0, 0, LORA_IQ_INVERSION_ON, false );

	printf("start enter rx mode\r\n");
	Radio.Rx( LORA_RX_TIMEOUT_VALUE );	//进入接收模式
	printf("all setting\r\n");
	printf("freq: %d\r\n Tx power: %d\r\n band width: %d\r\n FS: %d\r\n CODINGRATE: %d\r\n PREAMBLE_LENGTH: %d\r\n",LORA_FRE,LORA_TX_OUTPUT_POWER,LORA_BANDWIDTH,LORA_SPREADING_FACTOR,LORA_CODINGRATE,LORA_PREAMBLE_LENGTH);
	while(1){
		Radio.IrqProcess( ); // Process Radio IRQ
		delay_ms(1);
	}
}

static void SX126xOnTxDone( void )
{
	printf("TxDone\r\n");
	Radio.Standby();
	//发送完成闪烁一下led提示
	GPIO_SetBits(GREEN_LED_PORT,GREEN_LED_PIN);
	delay_ms(100);
	GPIO_ResetBits(GREEN_LED_PORT,GREEN_LED_PIN);
}

static void SX126xOnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	printf("RxDone\r\n");
	Radio.Standby();
	printf("size:%d\r\nrssi:%d\r\nsnr:%d\r\npayload:%s\r\n",size,rssi,snr,payload);
	Radio.Rx( LORA_RX_TIMEOUT_VALUE );
	GPIO_SetBits(GREEN_LED_PORT,GREEN_LED_PIN);
	delay_ms(100);
	GPIO_ResetBits(GREEN_LED_PORT,GREEN_LED_PIN);
}

static void SX126xOnTxTimeout( void )
{
	printf("TxTimeout\r\n");
}

static void SX126xOnRxTimeout( void )
{
	Radio.Standby();
	printf("RxTimeout retry recive\r\n");
	Radio.Rx( LORA_RX_TIMEOUT_VALUE ); 
}

static void SX126xOnRxError( void )
{
	Radio.Standby();
	printf("RxError retry recive\r\n");
	Radio.Rx(LORA_RX_TIMEOUT_VALUE); 
}
