#include "lora_link.h"
#include "radio.h"
#include "sx126x.h"
#include <string.h>

/**
 * @brief 声明外部底层的滴答更新函数
 * 用于更新射频收发芯片在无中断/延时操作下的软件超时计数
 */
extern void Radio_UpdateTick(void);

/**
 * @brief LoRa 链路上下文内部管理结构体
 */
typedef struct
{
    uint8_t inited;          /**< 模块初始化状态标志。0: 未初始化, 1: 已初始化 */
    LoraLinkConfig cfg;     /**< 链路配置参数副本 */
    LoraLinkCallbacks cbs;   /**< 链路事件回调函数副本 */
    RadioEvents_t events;    /**< 底层射频驱动回调事件结构体 */
} lora_link_ctx_t;

/**
 * @brief 全局私有的 LoRa 链路上下文状态实例
 */
static lora_link_ctx_t g_link = {0};

/**
 * @brief 底层射频驱动发送完成回调函数
 * 用于向应用层派发 tx_done 事件
 */
static void link_on_tx_done(void)
{
    if (g_link.cbs.tx_done != NULL)
    {
        g_link.cbs.tx_done(g_link.cbs.user_ctx);
    }
}

/**
 * @brief 底层射频驱动发送超时回调函数
 * 用于向应用层派发 tx_timeout 事件
 */
static void link_on_tx_timeout(void)
{
    if (g_link.cbs.tx_timeout != NULL)
    {
        g_link.cbs.tx_timeout(g_link.cbs.user_ctx);
    }
}

/**
 * @brief 底层射频驱动接收超时回调函数
 * 用于向应用层派发 rx_timeout 事件
 */
static void link_on_rx_timeout(void)
{
    if (g_link.cbs.rx_timeout != NULL)
    {
        g_link.cbs.rx_timeout(g_link.cbs.user_ctx);
    }
}

/**
 * @brief 底层射频驱动接收错误回调函数
 * 用于向应用层派发 rx_error 事件
 */
static void link_on_rx_error(void)
{
    if (g_link.cbs.rx_error != NULL)
    {
        g_link.cbs.rx_error(g_link.cbs.user_ctx);
    }
}

/**
 * @brief 底层射频驱动接收数据成功回调函数
 * @param[in] payload 接收到的有效载荷数据缓冲区指针
 * @param[in] size    有效载荷数据大小 (字节)
 * @param[in] rssi    接收信号强度指示 (dBm)
 * @param[in] snr     信噪比 (dB)
 * 用于向应用层派发 rx_done 事件，传递接收到的数据及信号质量指标
 */
static void link_on_rx_done(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    if (g_link.cbs.rx_done != NULL)
    {
        g_link.cbs.rx_done(payload, size, rssi, snr, g_link.cbs.user_ctx);
    }
}

/**
 * @brief 应用射频同步字 (Sync Word) 配置
 * SX126x 寄存器配置：将同步字拆分为 2 字节并写入同步字配置寄存器中
 * @param[in] cfg 指向配置参数结构体的指针
 */
static void link_apply_syncword(const LoraLinkConfig *cfg)
{
    uint8_t sync_bytes[2];
    // 设置是否为公共网络模式 (底层的公共网络设置通常会更改射频内部的默认同步字)
    Radio.SetPublicNetwork(cfg->public_network);
    
    // 提取同步字的高低字节
    sync_bytes[0] = (uint8_t)((cfg->syncword >> 8) & 0xFFU);
    sync_bytes[1] = (uint8_t)(cfg->syncword & 0xFFU);
    
    // 写入 SX126x 的 LoRa 同步字寄存器 (REG_LR_SYNCWORD)
    SX126xWriteRegisters(REG_LR_SYNCWORD, sync_bytes, 2);
}

void LoraLink_GetDefaultConfig(LoraLinkConfig *cfg)
{
    if (cfg == NULL)
    {
        return;
    }
    // 默认配置初始化参数：
    cfg->rf_frequency = 470500000U;          // 频率 470.5 MHz (常用于中国微功率无线电频段)
    cfg->tx_power_dbm = 14;                  // 发射功率 14 dBm
    cfg->lora_bandwidth = 0;                 // 带宽: 0 代表 125 kHz
    cfg->lora_spreading_factor = 9;          // 扩频因子: SF9 (折中选择，速率与距离平衡)
    cfg->lora_coding_rate = 1;               // 编码率: 1 代表 CR 4/5
    cfg->lora_preamble_length = 8;           // 前导码长度: 8
    cfg->lora_symbol_timeout = 0;            // 符号超时: 0 (不超时)
    cfg->lora_fix_len_payload = false;       // 采用动态数据包长度负载 (变长 payload)
    cfg->lora_iq_invert = false;             // IQ 信号不反转
    cfg->tx_timeout_ms = 3000U;              // 发送超时时间为 3000 ms
    cfg->rx_continuous = true;               // 默认开启连续接收模式
    cfg->public_network = false;             // 默认工作在私有网络模式
    cfg->syncword = 0x19ABU;                 // 私有网络下的默认同步字
}

uint8_t LoraLink_Init(const LoraLinkConfig *cfg, const LoraLinkCallbacks *cbs)
{
    if (cfg == NULL || cbs == NULL)
    {
        return 1;
    }

    // 保存配置与回调结构体
    g_link.cfg = *cfg;
    g_link.cbs = *cbs;

    // 清零底层事件并注册对应的私有事件分发函数
    memset(&g_link.events, 0, sizeof(g_link.events));
    g_link.events.TxDone = link_on_tx_done;
    g_link.events.TxTimeout = link_on_tx_timeout;
    g_link.events.RxTimeout = link_on_rx_timeout;
    g_link.events.RxError = link_on_rx_error;
    g_link.events.RxDone = link_on_rx_done;

    // 初始化底层射频驱动并配置信道频率
    Radio.Init(&g_link.events);
    Radio.SetChannel(g_link.cfg.rf_frequency);

    // 配置发送参数：
    // modem (MODEM_LORA), 功率, fdev=0, 带宽, sf, cr, 前导码长度, 固定长度, crc=true, frequency hopping=0, hopPeriod=0, iqInverted, 发送超时
    Radio.SetTxConfig(MODEM_LORA, g_link.cfg.tx_power_dbm, 0, g_link.cfg.lora_bandwidth,
                      g_link.cfg.lora_spreading_factor, g_link.cfg.lora_coding_rate,
                      g_link.cfg.lora_preamble_length, g_link.cfg.lora_fix_len_payload,
                      true, 0, 0, g_link.cfg.lora_iq_invert, g_link.cfg.tx_timeout_ms);

    // 配置接收参数：
    // modem (MODEM_LORA), 带宽, sf, cr, bandwidthAfc=0, 前导码长度, 符号超时, 固定长度, payloadLen=0, crc=true, freqHopping=0, hopPeriod=0, iqInverted, 连续接收
    Radio.SetRxConfig(MODEM_LORA, g_link.cfg.lora_bandwidth, g_link.cfg.lora_spreading_factor,
                      g_link.cfg.lora_coding_rate, 0, g_link.cfg.lora_preamble_length,
                      g_link.cfg.lora_symbol_timeout, g_link.cfg.lora_fix_len_payload,
                      0, true, 0, 0, g_link.cfg.lora_iq_invert, g_link.cfg.rx_continuous);

    // 写入同步字配置
    link_apply_syncword(&g_link.cfg);
    
    // 初始化完成后将射频状态置为待机模式 (Standby)
    Radio.Standby();
    g_link.inited = 1;
    return 0;
}

void LoraLink_Process(void)
{
    if (!g_link.inited)
    {
        return;
    }
    // 更新底层射频时间片，驱动超时事件
    Radio_UpdateTick();
    // 轮询并处理射频芯片的中断状态寄存器和收发事件
    Radio.IrqProcess();
}

void LoraLink_StartRx(void)
{
    if (!g_link.inited)
    {
        return;
    }
    // 开启接收。0 表示立即开启并不设置特定的超时时限
    Radio.Rx(0);
}

void LoraLink_StopRx(void)
{
    if (!g_link.inited)
    {
        return;
    }
    // 将芯片拉回待机状态，以停止接收操作
    Radio.Standby();
}

uint8_t LoraLink_Send(const uint8_t *payload, uint8_t size)
{
    if (!g_link.inited || payload == NULL || size == 0)
    {
        return 1;
    }
    // 调用底层发送函数，触发数据包发送
    Radio.Send((uint8_t *)payload, size);
    return 0;
}
