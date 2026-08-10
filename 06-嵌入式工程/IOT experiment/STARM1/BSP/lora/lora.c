#include "lora.h"
#include "usart.h"
#include "MCP23017.h"
#include "delay.h"

#define LORA_RX_MAX 256
uint8_t lora_rx_buf[LORA_RX_MAX];
volatile uint8_t lora_rx_flag = 0;
volatile uint16_t lora_rx_len = 0;

void lora_set_mode(uint8_t mode)
{
    // 读取当前GPIOB输出寄存器
    uint8_t pb_val = mcp23017_read_reg(MCP23017_GPIOB);
    
    // 清除B0(M0)和B1(M1)位
    pb_val &= ~0x03;
    
    // 根据模式设置对应位
    /*模式 0	0	0	传输模式

    串口和无线均打开，支持透明传输。
    模式 1	0	1	WOR 模式

    支持空中唤醒，可定义为发送方或接收方。
    模式 2	1	0	配置模式

    串口波特率固定为 9600 8N1，用于读写寄存器及配置参数。
    模式 3	1	1	深度休眠*/
    if (mode & 0x01) pb_val |= 0x01; // M0
    if (mode & 0x02) pb_val |= 0x02; // M1
    
    mcp23017_write_reg(MCP23017_GPIOB, pb_val);
}

void lora_get_default_config(lora_config_t *cfg)
{
    cfg->addh = 0x00;
    cfg->addl = 0x00;
    // 默认：9600波特率，8N1，空中速率 2.4k
    cfg->reg0 = LORA_REG0_BAUD_9600_8N1 | LORA_REG0_AIR_RATE_2K4; 
    // 默认：240字节子包，不启用环境噪音计算，发射功率最大(22dBm)
    cfg->reg1 = LORA_REG1_SUBPACKET_240 | LORA_REG1_POWER_22DBM;
    // 默认：信道23 (根据模块基准频率比如410MHz, 信道23即为433MHz)
    cfg->reg2 = LORA_REG2_CHANNEL_433MHZ;
    // 默认：透传模式，无RSSI等附加
    cfg->reg3 = LORA_REG3_DEFAULT;
}

void lora_set_config(lora_config_t *cfg)
{
    uint8_t cmd[9];
    cmd[0] = 0xC0; // C0指令代表保存配置掉电不丢失
    cmd[1] = 0x00; // 寄存器起始地址
    cmd[2] = 0x06; // 写入长度
    cmd[3] = cfg->addh;
    cmd[4] = cfg->addl;
    cmd[5] = cfg->reg0;
    cmd[6] = cfg->reg1;
    cmd[7] = cfg->reg2;
    cmd[8] = cfg->reg3;

    // 暂停可能正在接收的中断，避免冲突
    HAL_UART_AbortReceive(&huart3);

    // 切换到配置模式 (M0=0, M1=1) 并等待稳定
    lora_set_mode(2);
    delay_ms(50);
    
    // 发送配置指令
    HAL_UART_Transmit(&huart3, cmd, 9, HAL_MAX_DELAY);
    
    // 等待模块将参数落盘保存（非常重要，因没接AUX）
    delay_ms(100);
    
    // 恢复透传模式并重启接收
    lora_set_mode(0);
    delay_ms(50);
    
    lora_rx_flag = 0;
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, lora_rx_buf, LORA_RX_MAX);
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
}

void lora_init(void)
{
    // 配置默认参数 (9600 8N1, 2.4k air rate, 22dBm, 433MHz)
    lora_config_t cfg;
    lora_get_default_config(&cfg);
    
    // 如果您发现开机初始化过慢(卡顿200ms)，可以注释掉下面这行，前提是模块已被正确配过一次。
    lora_set_config(&cfg);

    // 模式0：正常/透传模式 (lora_set_config 内部已经切回了模式0并开启接收)
    // 但为了纯粹防范意外，再次断言状态并确保接收正常开启
    lora_set_mode(0);
    delay_ms(10);
    
    // 使用空闲线检测启动USART3 DMA接收
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, lora_rx_buf, LORA_RX_MAX);
    // 禁用半传输中断以避免不必要的双重触发
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
}

void lora_send(uint8_t *data, uint16_t len)
{
    // 在透传模式下，发送到USART3的数据将通过无线方式广播
    HAL_UART_Transmit(&huart3, data, len, HAL_MAX_DELAY);
}

// 当USART接收到数据（空闲线或缓冲区满）时调用的回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART3)
    {
        // 仅标记标志位和大小，不要在中断内进行任何可能阻塞的操作（如发送）
        // 在 main 的 while() 循环中交由 lora_process() 处理
        lora_rx_len = Size;
        lora_rx_flag = 1;
    }
}

// 放在 main() 的 while(1) 或 scheduler 中不断被调用
void lora_process(void)
{
    if (lora_rx_flag)
    {
        // 将接收到的LoRa数据转发到USART1（PC端）
        HAL_UART_Transmit(&huart1, lora_rx_buf, lora_rx_len, HAL_MAX_DELAY);
        
        // 清除标志位
        lora_rx_flag = 0;

        // 重新启动接收，以便接收下一包
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, lora_rx_buf, LORA_RX_MAX);
        __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
    }
}
