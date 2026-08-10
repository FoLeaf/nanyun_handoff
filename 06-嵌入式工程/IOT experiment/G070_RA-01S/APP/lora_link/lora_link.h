#ifndef __LORA_LINK_H__
#define __LORA_LINK_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief LoRa 链路配置结构体
 * 包含射频频率、发射功率、扩频因子、带宽、编码率等通信核心参数
 */
typedef struct
{
    uint32_t rf_frequency;          /**< 射频工作频率，单位：Hz。例如：470500000U (470.5MHz) */
    int8_t tx_power_dbm;            /**< 发射功率，单位：dBm。例如：14 或 22 */
    uint8_t lora_bandwidth;         /**< 信号带宽，[0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: 保留] */
    uint8_t lora_spreading_factor;  /**< 扩频因子 (SF)，可选范围：5 ~ 12。值越大传输距离越远但速率越低 */
    uint8_t lora_coding_rate;       /**< 编码率 (CR)，[1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]。值越大纠错能力越强 */
    uint16_t lora_preamble_length;  /**< 前导码长度 (Preamble Length)，通常设为 8 */
    uint16_t lora_symbol_timeout;   /**< 接收超时符号数 (Symbol Timeout)，0 表示不使用符号超时 */
    bool lora_fix_len_payload;      /**< 是否使用固定长度数据包负载。false: 变长, true: 固定长度 */
    bool lora_iq_invert;            /**< IQ 信号是否反转。false: 正常, true: 反转 (通常用于区分网关与节点，防止同频干扰) */
    uint32_t tx_timeout_ms;         /**< 发送超时时间，单位：毫秒 (ms)。防止发送过程挂死 */
    bool rx_continuous;             /**< 是否开启连续接收模式。false: 单次接收, true: 连续接收 */
    bool public_network;            /**< 是否为公共网络 (如 LoRaWAN)。影响同步字和底层网络配置 */
    uint16_t syncword;              /**< LoRa 同步字 (Sync Word)，用于网络物理层隔离过滤。例如：0x19AB */
} LoraLinkConfig;

/**
 * @brief LoRa 链路事件回调函数结构体
 * 用于将接收、发送完成、超时等底层事件上报给应用层
 */
typedef struct
{
    void (*tx_done)(void *user_ctx);                                                                    /**< 发送完成回调函数指针 */
    void (*tx_timeout)(void *user_ctx);                                                                 /**< 发送超时回调函数指针 */
    void (*rx_timeout)(void *user_ctx);                                                                 /**< 接收超时回调函数指针 */
    void (*rx_error)(void *user_ctx);                                                                   /**< 接收错误回调函数指针 */
    void (*rx_done)(const uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr, void *user_ctx);   /**< 接收完成回调函数指针，包含接收数据、长度、RSSI、SNR等信息 */
    void *user_ctx;                                                                                     /**< 用户自定义上下文指针，在触发回调时会作为参数传入 */
} LoraLinkCallbacks;

/**
 * @brief 获取默认的 LoRa 链路配置
 * @param[out] cfg 接收配置参数的结构体指针
 */
void LoraLink_GetDefaultConfig(LoraLinkConfig *cfg);

/**
 * @brief 初始化 LoRa 链路
 * @param[in] cfg  指向配置结构体的指针
 * @param[in] cbs  指向回调结构体的指针
 * @retval 0 初始化成功
 * @retval 1 初始化失败 (如参数为空指针)
 */
uint8_t LoraLink_Init(const LoraLinkConfig *cfg, const LoraLinkCallbacks *cbs);

/**
 * @brief LoRa 链路任务轮询处理函数
 * 需在系统主循环 (如 while(1)) 中持续调用，处理射频时间片及中断事件
 */
void LoraLink_Process(void);

/**
 * @brief 开启 LoRa 接收模式
 */
void LoraLink_StartRx(void);

/**
 * @brief 停止 LoRa 接收，使芯片进入待机模式 (Standby)
 */
void LoraLink_StopRx(void);

/**
 * @brief 发送 LoRa 数据包
 * @param[in] payload 发送的数据包数据缓冲指针
 * @param[in] size    发送的数据包长度
 * @retval 0 发送命令提交成功
 * @retval 1 发送失败 (未初始化或参数无效)
 */
uint8_t LoraLink_Send(const uint8_t *payload, uint8_t size);

#endif // __LORA_LINK_H__
