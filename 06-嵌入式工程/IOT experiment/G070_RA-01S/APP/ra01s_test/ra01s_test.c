#include "ra01s_test.h"
#include "lora_link.h"
#include "main.h"
#include "aht30.h"
#include "aht30_port.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
    int16_t temp_x10;
    int16_t humi_x10;
    uint32_t seq;
} lora_sensor_packet_t;

typedef struct
{
    char tx_status[20];
    char rx_status[20];
} lora_test_status_t;

#define LORA_SENSOR_INVALID_VAL -9999   /**< 传感器失效时的魔术值（哨兵值） */

typedef struct
{
    float temp;
    float humi;
    int16_t rssi;
    int8_t snr;
    uint8_t parse_ok;
    uint8_t sensor_ok;   /**< 传感器状态标志。1: 正常, 0: 损坏/掉线 */
    uint8_t has_new_data;
    char raw_msg[24];
} lora_rx_view_t;

static lora_test_status_t g_status = {"Tx: None", "Rx: None"};
static LoraLinkConfig g_link_cfg;

#if LORA_TEST_IS_TX_NODE
static AHT30_HandleTypeDef g_aht30;
static uint8_t g_aht30_ready = 0;
static uint8_t g_tx_buf[64];
static lora_sensor_packet_t g_last_sample = {0};
#else
static uint8_t g_oled_inited = 0;
static lora_rx_view_t g_rx_view = {0};
#endif

static int lora_encode_payload(char *out, uint32_t out_len, const lora_sensor_packet_t *pkt)
{
    if (out == NULL || pkt == NULL || out_len == 0)
    {
        return -1;
    }
    return snprintf(out, out_len, "T=%d,H=%d,S=%lu",
                    (int)pkt->temp_x10, (int)pkt->humi_x10, (unsigned long)pkt->seq);
}

#if LORA_TEST_IS_RX_NODE
static uint8_t lora_decode_payload(const char *msg, lora_sensor_packet_t *pkt)
{
    long temp_x10 = 0;
    long humi_x10 = 0;
    unsigned long seq = 0;

    if (msg == NULL || pkt == NULL)
    {
        return 0;
    }
    if (sscanf(msg, "T=%ld,H=%ld,S=%lu", &temp_x10, &humi_x10, &seq) != 3)
    {
        return 0;
    }

    pkt->temp_x10 = (int16_t)temp_x10;
    pkt->humi_x10 = (int16_t)humi_x10;
    pkt->seq = (uint32_t)seq;
    return 1;
}
#endif

#if LORA_TEST_IS_TX_NODE
static void tx_node_sensor_init(void)
{
    AHT30_Port_Init();
    memset(&g_aht30, 0, sizeof(g_aht30));
    AHT30_Port_Bind(&g_aht30);

    if (AHT30_Init(&g_aht30) == 0)
    {
        g_aht30_ready = 1;
        snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Tx:AHT30 OK");
    }
    else
    {
        g_aht30_ready = 0;
        snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Tx:AHT30 ERR");
    }
}

static uint8_t tx_node_sample_sensor(lora_sensor_packet_t *sample)
{
    if (sample == NULL)
    {
        return 0;
    }
    // 若初始化未成功，尝试重新初始化
    if (!g_aht30_ready)
    {
        tx_node_sensor_init();
        if (!g_aht30_ready)
        {
            // 传感器未就绪，填充失效魔术值并返回失败
            sample->temp_x10 = LORA_SENSOR_INVALID_VAL;
            sample->humi_x10 = LORA_SENSOR_INVALID_VAL;
            return 0;
        }
    }
    // 读取温湿度数据
    if (AHT30_ReadMeasure(&g_aht30) != 0)
    {
        snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Tx:ReadErr");
        // 读取失败，填充失效魔术值并返回失败
        sample->temp_x10 = LORA_SENSOR_INVALID_VAL;
        sample->humi_x10 = LORA_SENSOR_INVALID_VAL;
        return 0;
    }

    // 读取成功，转换数据（放大10倍以传输整数）
    sample->temp_x10 = (int16_t)(g_aht30.temperature * 10.0f);
    sample->humi_x10 = (int16_t)(g_aht30.humidity * 10.0f);
    return 1;
}
#else
static void rx_node_oled_init(void)
{
    OLED_Init();
    g_oled_inited = 1;
    OLED_ClearBuffer();
    OLED_Print(0, 0, 6, "LoRa RX Node");
    OLED_Print(0, 16, 6, "Wait packet...");
    OLED_Refresh();
}

static void rx_node_oled_update(const lora_rx_view_t *view)
{
    if (!g_oled_inited || view == NULL)
    {
        return;
    }

    OLED_ClearBuffer();
    OLED_Print(0, 0, 6, "LoRa RX Node");
    if (!view->parse_ok)
    {
        // 协议数据包格式解析失败
        OLED_Print(0, 16, 6, "RAW:");
        OLED_Print(24, 16, 6, "%s", view->raw_msg);
        OLED_Print(0, 28, 6, "Parse error");
    }
    else if (!view->sensor_ok)
    {
        // 协议包解析正常，但发送端的传感器处于损坏或离线状态
        OLED_Print(0, 16, 6, "Sensor Error");
        OLED_Print(0, 28, 6, "Check AHT30");
    }
    else
    {
        // 一切正常，展示实时的温湿度数据
        OLED_Print(0, 16, 6, "T: %.1f C", view->temp);
        OLED_Print(0, 28, 6, "H: %.1f %%", view->humi);
    }
    OLED_Print(0, 40, 6, "RSSI:%ddBm", view->rssi);
    OLED_Print(0, 52, 6, "SNR:%ddB", (int)view->snr);
    OLED_Refresh();
}
#endif

static void app_on_tx_done(void *user_ctx)
{
    (void)user_ctx;
    snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Tx: Done");
}

static void app_on_tx_timeout(void *user_ctx)
{
    (void)user_ctx;
    snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Tx: Timeout");
}

static void app_on_rx_timeout(void *user_ctx)
{
    (void)user_ctx;
    snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Rx: Timeout");
#if LORA_TEST_IS_RX_NODE
    LoraLink_StartRx();
#endif
}

static void app_on_rx_error(void *user_ctx)
{
    (void)user_ctx;
    snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Rx: Error");
#if LORA_TEST_IS_RX_NODE
    LoraLink_StartRx();
#endif
}

static void app_on_rx_done(const uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr, void *user_ctx)
{
    char msg[64];
    uint16_t copy_len = size;
    (void)user_ctx;

    if (copy_len >= sizeof(msg))
    {
        copy_len = sizeof(msg) - 1U;
    }
    memcpy(msg, payload, copy_len);
    msg[copy_len] = '\0';

    snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Rx:%ddBm", rssi);

#if LORA_TEST_IS_RX_NODE
    lora_sensor_packet_t pkt;
    snprintf(g_rx_view.raw_msg, sizeof(g_rx_view.raw_msg), "%s", msg);
    g_rx_view.rssi = rssi;
    g_rx_view.snr = snr;

    if (lora_decode_payload(msg, &pkt))
    {
        g_rx_view.parse_ok = 1;
        // 检查数据包内是否包含传感器失效魔术值
        if (pkt.temp_x10 == LORA_SENSOR_INVALID_VAL || pkt.humi_x10 == LORA_SENSOR_INVALID_VAL)
        {
            g_rx_view.sensor_ok = 0;
            snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Rx:SensErr %d", rssi);
        }
        else
        {
            g_rx_view.temp = (float)pkt.temp_x10 / 10.0f;
            g_rx_view.humi = (float)pkt.humi_x10 / 10.0f;
            g_rx_view.sensor_ok = 1;
            snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Rx:OK %ddBm", rssi);
        }
    }
    else
    {
        g_rx_view.parse_ok = 0;
        g_rx_view.sensor_ok = 0;
        snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Rx:ParseErr");
    }
    g_rx_view.has_new_data = 1;
#else
    (void)snr;
#endif
}

void RA01S_Test_Init(void)
{
    LoraLinkCallbacks cbs;

    LoraLink_GetDefaultConfig(&g_link_cfg);
    g_link_cfg.syncword = 0x19ABU;
    g_link_cfg.public_network = false;
    g_link_cfg.tx_timeout_ms = 3000U;
    g_link_cfg.rx_continuous = true;

    cbs.tx_done = app_on_tx_done;
    cbs.tx_timeout = app_on_tx_timeout;
    cbs.rx_timeout = app_on_rx_timeout;
    cbs.rx_error = app_on_rx_error;
    cbs.rx_done = app_on_rx_done;
    cbs.user_ctx = NULL;

    if (LoraLink_Init(&g_link_cfg, &cbs) != 0)
    {
        snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Lora Init Err");
        snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Lora Init Err");
        return;
    }

#if LORA_TEST_IS_TX_NODE
    tx_node_sensor_init();
    snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Role:A TX");
#else
    rx_node_oled_init();
    snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Role:B RX");
    snprintf(g_status.rx_status, sizeof(g_status.rx_status), "Rx: Waiting");
    LoraLink_StartRx();
#endif
}

void RA01S_Test_Loop(void)
{
#if LORA_TEST_IS_TX_NODE
    static uint32_t last_tx_tick = 0;
    uint32_t now = HAL_GetTick();
    int enc_len = 0;

    if ((now - last_tx_tick) < 3000U)
    {
        return;
    }
    last_tx_tick = now;

    // 尝试采样温湿度数据。若读取失败，底层函数会自动填充失效魔术值
    tx_node_sample_sensor(&g_last_sample);
    // 无论是新数据还是无效数据，包序列号均递增，保证心跳报文发送
    g_last_sample.seq++;

    // 编码数据载荷为 ASCII 文本字符串格式
    enc_len = lora_encode_payload((char *)g_tx_buf, sizeof(g_tx_buf), &g_last_sample);
    if (enc_len <= 0)
    {
        snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Tx:EncErr");
        return;
    }

    // 在本地状态中更新屏幕/调试信息
    if (g_last_sample.temp_x10 == LORA_SENSOR_INVALID_VAL)
    {
        snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Tx:SensErr");
    }
    else
    {
        snprintf(g_status.tx_status, sizeof(g_status.tx_status), "Tx:%d.%dC",
                 g_last_sample.temp_x10 / 10, (g_last_sample.temp_x10 < 0 ? -g_last_sample.temp_x10 : g_last_sample.temp_x10) % 10);
    }
    
    // 通过 LoRa 发送链路层数据包
    (void)LoraLink_Send(g_tx_buf, (uint8_t)enc_len);
#else
    if (g_rx_view.has_new_data)
    {
        g_rx_view.has_new_data = 0;
        rx_node_oled_update(&g_rx_view);
    }
#endif
}

void RA01S_Test_Process(void)
{
    LoraLink_Process();
}

const char *RA01S_Test_GetTxStatus(void)
{
    return g_status.tx_status;
}

const char *RA01S_Test_GetRxStatus(void)
{
    return g_status.rx_status;
}
