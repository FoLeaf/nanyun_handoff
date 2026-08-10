#include "stm32f10x.h"
#include "HAL_uart.h"
#include "usmart.h"
#include "string.h"
#include "radio.h"
#include "delay.h"
#include "sx1276.h"

/* ====================================================================
 * 硬件接线映射表
 * ====================================================================
 * STM32引脚          Ra-01 (SX1276)         USB转TTL模块
 * --------------------------------------------------------------------
 * GND                GND                    GND
 * 3V3                3V3                    -
 * PA_9 (USART1_TX)   -                      RXD
 * PA_10 (USART1_RX)  -                      TXD
 * PB_14              RST (复位引脚)         -
 * PA_4               CS  (片选引脚/NSS)     -
 * PA_5               SCK (SPI时钟)          -
 * PA_6               MISO(SPI主机输主从入)  -
 * PA_7               MOSI(SPI主机入从主机出)-
 * PB_0               DIO0(Lora中断引脚)     -
 * --------------------------------------------------------------------
 * LED 指示灯: PA_8
 * ==================================================================== */

#define LED_PIN GPIO_Pin_8
#define LED_PORT GPIOA

/* ================== 测试发包与载波配置 ================== */
#define FSK_SINGLE_CARRIER  0  // FSK 单载波连续发出测试模式 (要求配置为FSK模式)
#define LORA_CONTINUE       0  // LoRa 连续发送测试模式

/* ================== 通讯频段与模式选择 ================== */
#define USE_BAND_433           // 选用由宏映射的频段 (下方对应 470.5MHz)
#define USE_MODEM_LORA         // 启用 LoRa 强抗干扰调制模式
//#define USE_MODEM_FSK        // 启用常规 FSK 调制模式 (如需使用请取消注释并注释上方 LoRa 配置)

/* 匹配选定频段的具体运行频率 */
#if defined( USE_BAND_433 )
    #define RF_FREQUENCY      470500000 // 工作频率设定为 470.5 MHz
#elif defined( USE_BAND_780 )
    #define RF_FREQUENCY      780000000 // 780 MHz
#elif defined( USE_BAND_868 )
    #define RF_FREQUENCY      868000000 // 868 MHz
#elif defined( USE_BAND_915 )
    #define RF_FREQUENCY      915000000 // 915 MHz
#else
    #error "错误：请务必配置一个指定的射频工作频段!"
#endif

// 发射功率 (单位：dBm)
#define TX_OUTPUT_POWER       20        

/* ================== 调制模式具体核心参数配置 ================== */
#if defined( USE_MODEM_LORA )
    #define LORA_BANDWIDTH               0      // 带宽设置：[0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: 预留]
    #define LORA_SPREADING_FACTOR        9      // 扩频因子：[SF7 到 SF12] (数值越大，传输越远、抗干扰越强，但通信速度越慢)
    #define LORA_CODINGRATE              1      // 纠错编码率：[1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    #define LORA_PREAMBLE_LENGTH         8      // 前导码长度：用于接收端同步，收发双方必须一致
    #define LORA_SYMBOL_TIMEOUT          0      // 符号接收超时阈值
    #define LORA_FIX_LENGTH_PAYLOAD_ON   false  // 是否使用固定长度数据包模式 (false为开启变长包模式)
    #define LORA_IQ_INVERSION_ON         false  // 是否反转 IQ 极性 (通常用于基站/网关的上下行区分)
#elif defined( USE_MODEM_FSK )
    #define FSK_FDEV                     25e3   // FSK 频偏设置 (Hz)
    #define FSK_DATARATE                 50e3   // FSK 数据传输率 (bps)
    #define FSK_BANDWIDTH                50e3   // FSK 接收端检测带宽 (Hz)
    #define FSK_AFC_BANDWIDTH            83.333e3 // FSK 自动频率控制 (AFC) 搜寻带宽 (Hz)
    #define FSK_PREAMBLE_LENGTH          5      // FSK 前导码长度，要求收发双方一致
    #define FSK_FIX_LENGTH_PAYLOAD_ON    false  // 是否使用固定长度数据包模式
#else
    #error "错误：请务必定义并选择一种调制解调模式!"
#endif

/* 这个枚举定义了设备的主状态机，方便按不同工作状态切换逻辑 */
typedef enum
{
    LOWPOWER,     // 低功耗待命状态
    RX,           // 接收成功触发状态
    RX_TIMEOUT,   // 接收超时状态
    RX_ERROR,     // 接收校验错误状态 (如CRC损坏)
    TX,           // 发送成功触发状态
    TX_TIMEOUT,   // 发送超时状态
}States_t;

/* ======= 全局配置参数与射频缓存区 ======= */
#define RX_TIMEOUT_VALUE  5000          // 接收窗口开启的阻塞及超时时间
#define BUFFER_SIZE       64            // 射频数据包的最大处理缓存长度

const uint8_t PingMsg[] = "PING";       // 心跳或测试 Ping 包
const uint8_t PongMsg[] = "PONG";       // 心跳响应 Pong 包

uint16_t BufferSize = BUFFER_SIZE;      // 实际使用的数据长度变量
uint8_t Buffer[BUFFER_SIZE];            // 发送与接收共用的数据缓存区

States_t State = LOWPOWER;              // 初始化设备的默认主运行状态为低功耗空闲

int8_t RssiValue = 0;                   // 保存每次有效接收到的 接收信号强度指示 (RSSI)
int8_t SnrValue = 0;                    // 保存每次有效接收到的 信号信噪比 (SNR)


/**
 * @brief  初始化调试指示用 的 LED 引脚 (PA8)
 */
void led_init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 配置中断优先级分组
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); // 关闭JTAG，仅释放SWD接口
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出驱动LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LED_PORT,LED_PIN); // 默认输出高电平拉灭LED
}

/**
 * @brief  控制 LED 指示灯的状态
 * @param  level 1 为输出高电平(熄灭)，0 为输出低电平(点亮)
 */
void led_set_level(uint8_t level){
	if(1 == level){
		GPIO_SetBits(LED_PORT,LED_PIN); 
	}else{
		GPIO_ResetBits(LED_PORT,LED_PIN); 
	}
}

/* 声明注册由射频底层触发的中断/事件回调函数 */
static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
void OnTxTimeout( void );
void OnRxTimeout( void );
void OnRxError( void );


/**
 * @brief  串口 1 接收中断回调函数
 * @note   用于解析以回车换行符 '\r\n' (0x0D 0x0A) 结尾的数据指令帧
 */
void uart1callBackTest(uint8_t data){
	u8 Res;
	Res = USART_ReceiveData(USART1); // 提取串口DR寄存器中接收到的一字节最新数据
		
	if((USART_RX_STA&0x8000)==0){    // bit15 标志位若为 0 ，代表先前尚未接收到一个完整的协议帧
		if(USART_RX_STA&0x4000){     // 此时若 bit14 标志位为 1 ，说明上一次处理恰好刚接收到了 '\r' (0x0D)
			if(Res!=0x0a)
				USART_RX_STA=0;        // 如果收到了 '\r'，但接着的不是换行符 '\n'，则判定数据包格式无效，清空重新开始匹配
			else
				USART_RX_STA|=0x8000;	 // 合法匹配到了结尾的 "\r\n"，将 bit15 置 1，宣布本条串口指令成功完成接收 
			
		}else{                       // 进入这里意味着还没捕获到回车符 '\r' 的初始搜集阶段
			if(Res==0x0d)
				USART_RX_STA|=0x4000;  // 一旦命中遇到回车符 0x0D，标注状态机准备迎接下一步的结束符
			else{
				USART_RX_BUF[USART_RX_STA&0X3FFF]=Res; // 正常的字符串内容将被存入串口缓存区数组
				USART_RX_STA++;                        // 有效计数值偏移递增
				if(USART_RX_STA>(USART_REC_LEN-1))
					USART_RX_STA=0;      // 异常保护：当传来的数据字节超出规定的限制边界，直接清盘重来，防止系统内存溢出崩溃	  
			}
		}
	}
}

/**
 * @brief 预留的常规测试与读取RSSI环境底噪示范函数
 */
uint8_t testFun(uint8_t num,char *str){
	printf("\r\ntestFun:%d-%s\r\n",num,str);
	printf("rssi:%d\r\n",SX1276ReadRssi(MODEM_LORA)); // 可单独读取当前环境 RSSI 值
	return 2;
}

/**
 * @brief  主程序执行入口
 */
int main(void)
{
	bool isMaster = true;  // 设置设备当前的角色： true = 广播/主动发出端
//	bool isMaster = false; //                    false = 纯接收/从机回包端
	
	delay_init();	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	HALUart1Init(115200,uart1callBackTest); // 初始化串口配置波特率以进行 Log 打印与交互
	led_init();
	
	myPrintf(LEVEL_DEBUG,"init ok\r\n");
	if(isMaster){
		myPrintf(LEVEL_DEBUG,"this is master\r\n");
	}else{
		myPrintf(LEVEL_DEBUG,"this is slave\r\n");
	}
	
	usmart_dev.init(SystemCoreClock/1000000); // 串口调试组件 USMART 注册初始化
	
	/* 将事件回调函数挂载并提交到底层射频结构中 */
	RadioEvents.TxDone = OnTxDone;
	RadioEvents.RxDone = OnRxDone;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = OnRxTimeout;
	RadioEvents.RxError = OnRxError;

	Radio.Init( &RadioEvents ); // 执行软硬件复位及射频芯片底层硬件基础初始化
	Radio.SetChannel( RF_FREQUENCY ); // 设置已指定的物理通讯频道

#if defined( USE_MODEM_LORA )
	/* 下发 LoRa 调制参数 (输出功率、带宽、扩频因子等) 给底层发送和接收引擎 */
	Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
    
	Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

#elif defined( USE_MODEM_FSK )
	Radio.SetTxConfig( MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0,
                                  FSK_DATARATE, 0,
                                  FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                                  true, 0, 0, 0, 3000 );
    
	Radio.SetRxConfig( MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                                  0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                                  0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, true,
                                  0, 0,false, true );
#else
    #error "错误：请务必配置一个指定的射频工作频段"
#endif

/* ================= 专门用于物理层调试验证逻辑 ================= */
#if LORA_CONTINUE
	myPrintf(LEVEL_DEBUG,"enter lora continue mode\r\n");
	SX1276Write( REG_FEILSB, ( SX1276Read( REG_FEILSB ) | (1<<3) ) ); // 操作底层寄存器置位 TxContinuousMode
	Radio.Send( Buffer, BufferSize ); // 触发发射动作从而激活连续发包测试行为。警告：仅作频谱验证用，无软跳出机制，需重启芯片
	while(1){
	}
#endif

#if FSK_SINGLE_CARRIER
	myPrintf(LEVEL_DEBUG,"enter fsk mode\r\n");
	SX1276Write( REG_FDEVMSB, 0x00 ); // 强制清除频偏(Dev)相关的设置
	SX1276Write( REG_FDEVLSB, 0x00 ); 
	Radio.Send( Buffer, BufferSize ); // 触发一次发包从而释放常开载波信号以作检漏测试。无软跳出机制，必须重启。
	while(1){
	}
#endif
/* ============================================================= */

	Radio.Rx( RX_TIMEOUT_VALUE );     // 上电首选进入一定周期的射频接收等待状态
	
	if(true == isMaster){
		memcpy(Buffer,"hello",sizeof("hello"));
		Radio.Send( Buffer, BufferSize ); // 若设备为主机，在预接收配置后主动触发第一包发送请求
	}									
	
	/* ====== 设备核心主循环 ====== */
	while( 1 ){
		switch( State ){
			case TX:
				/* 若为广播透传系统，可在发出后继续调用Send；此范例展示持续将数据投递 */
				Radio.Send( Buffer, BufferSize ); 
				State = LOWPOWER; // 操作后置为等待状以备中断回调再次切换主状态机
				
				led_set_level(0); // 频闪 LED 以指示发送动作执行正常
				delay_ms( 250 );
				led_set_level(1);
				delay_ms( 250 );			
				break;
				
			case TX_TIMEOUT:
				Radio.Send( Buffer, BufferSize ); // 若上次发生了发送超时，重新请求发送操作
				State = LOWPOWER;
				break;
				
			case RX:
				led_set_level(0); // 频闪 LED 以指明收到有效射频信号以及处理就绪
				delay_ms( 250 );
				led_set_level(1);
				delay_ms( 250 );	
				
				Radio.Rx( RX_TIMEOUT_VALUE ); // 解析提取结束后，一定要重置并开启下一次超时接听窗口
				break;
				
			case RX_TIMEOUT:
				myPrintf(LEVEL_DEBUG,"RxTIMEOUT\r\n");
				delay_ms( 100 );
				Radio.Rx( RX_TIMEOUT_VALUE ); // 在规定的等候时间内未发生通信，重新打底刷新接收监听窗口
				break;
				
			case RX_ERROR:
				myPrintf(LEVEL_DEBUG,"RX_ERROR\r\n");
				delay_ms( 100 );
				Radio.Rx( RX_TIMEOUT_VALUE ); // 若接收中途中断或解析校验失败，也要重置并继续接听后继信号
				break;
				
			default:
				// 非活动执行情况时仅在此维持休眠或者释放单片机负载，等待底层射频中断跳入上方的Case项
				delay_ms( 50 );
				break;
		}
	}
}

/* =========================== 底层中断回调触发任务实现 =========================== */

void OnTxDone( void )
{
	Radio.Sleep( );            // 先进入射频休眠切断发射功耗
	State = TX;                // 切换至主程序的 TX 任务树状态以进行下步逻辑
	myPrintf(LEVEL_DEBUG,"TxDone\r\n");
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	Radio.Sleep( );            // 先休眠射频进入省电
	BufferSize = size;
	memcpy( Buffer, payload, BufferSize ); // 拷贝截获成功的数据到共用缓存内
	RssiValue = rssi;          // 保留当前环境强度和信噪比以备其他读取请求
	SnrValue = snr;
	State = RX;                // 提交触发 RX 成功状态机制
	
	// 在调试端打出具体数据报文和链路质量信息
	myPrintf(LEVEL_DEBUG,"RxDone\r\nrssi:%d\r\nsnr:%d\r\nsize:%d\r\ndata:payload:%s\r\n",rssi,snr,size,payload);
}

void OnTxTimeout( void )
{
	Radio.Sleep( );            // 收发异常时都要执行休眠以释放硬件锁死并降低功耗
	State = TX_TIMEOUT;
	myPrintf(LEVEL_DEBUG,"TxTIMEOUT\r\n");
}

void OnRxTimeout( void )
{
	Radio.Sleep( );
	State = RX_TIMEOUT;
}

void OnRxError( void )
{
	Radio.Sleep( );
	State = RX_ERROR;
	myPrintf(LEVEL_DEBUG,"RxError\r\n");
}
