/*
 * E18-MS1-PCB ZigBee 3.0 Host Tool
 * Windows native UI (Win32 API) - covers all module functions
 * Build: cl /O2 main.c /link user32.lib gdi32.lib comctl32.lib
 *        gcc -O2 main.c -lcomctl32 -mwindows
 */

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#ifdef _MSC_VER
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='[REDACTED_IP]' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

/* ========== Constants ========== */
#define MAX_LOG_LINES    5000
#define MAX_RX_BUF       65536
#define HEX_FRAME_MAX    256
#define MAX_NODES        64
#define MAX_BINDINGS     32

/* ========== Control IDs ========== */
/* Serial bar */
#define IDC_COMBO_PORT      1001
#define IDC_COMBO_BAUD      1002
#define IDC_BTN_OPEN        1003
#define IDC_BTN_REFRESH     1004
#define IDC_STATIC_STATUS   1005

/* Tab control */
#define IDC_TAB             1010

/* Tab 0: Basic Config */
#define IDC_GRP_STATUS      1100
#define IDC_TXT_STATUS      1101
#define IDC_BTN_QRY_STATUS  1102
#define IDC_GRP_NETWORK     1110
#define IDC_COMBO_NODETYPE  1111
#define IDC_BTN_SET_NODE    1112
#define IDC_COMBO_CHANNEL   1113
#define IDC_BTN_SET_CHAN    1114
#define IDC_EDIT_PANID      1115
#define IDC_BTN_SET_PANID   1116
#define IDC_COMBO_TXPOWER   1117
#define IDC_BTN_SET_TXPWR   1118
#define IDC_BTN_START_NWK   1120
#define IDC_BTN_STOP_NWK    1121
#define IDC_BTN_RESET       1122
#define IDC_BTN_RESTORE     1123
#define IDC_BTN_BIND        1124
#define IDC_GRP_TIME        1130
#define IDC_BTN_GET_UTC     1131
#define IDC_BTN_SET_UTC     1132
#define IDC_GRP_NWKSEC      1140
#define IDC_BTN_GET_KEY     1141
#define IDC_BTN_GET_NODES   1142
#define IDC_BTN_RETX_INFO   1143
#define IDC_BTN_GET_NODE    1150
#define IDC_BTN_GET_CHAN    1151
#define IDC_BTN_GET_PANID   1152
#define IDC_BTN_GET_TXPWR   1153

/* Tab 1: HEX Commands */
#define IDC_COMBO_HEX_TYPE  1201
#define IDC_COMBO_HEX_CMD   1202
#define IDC_EDIT_HEX_DATA   1203
#define IDC_BTN_HEX_SEND    1204
#define IDC_EDIT_HEX_RAW    1205
#define IDC_BTN_HEX_RAW_SEND 1206
#define IDC_EDIT_HEX_RESP   1207
#define IDC_BTN_HEX_CLEAR   1208
#define IDC_STATIC_HEX_FRAME 1209

/* Tab 2: AT Commands */
#define IDC_EDIT_AT_CMD     1301
#define IDC_BTN_AT_SEND     1302
#define IDC_EDIT_AT_RESP    1303
#define IDC_BTN_AT_CLEAR    1304
/* AT quick buttons */
#define IDC_AT_JOIN         1310
#define IDC_AT_LEAVE        1311
#define IDC_AT_RESET        1312
#define IDC_AT_RESTORE      1313
#define IDC_AT_MAC          1314
#define IDC_AT_SHORT        1315
#define IDC_AT_PANID_GET    1316
#define IDC_AT_CHAN_GET     1317
#define IDC_AT_BAUD_GET     1318
#define IDC_AT_POWER_GET    1319
#define IDC_AT_ROLE_GET     1320
#define IDC_AT_SOFT_ID      1321
#define IDC_AT_FIND         1322

/* Tab 3: Data Transfer */
#define IDC_EDIT_TX_DATA    1401
#define IDC_EDIT_RX_DATA    1402
#define IDC_BTN_SEND_DATA   1403
#define IDC_CHK_HEX_TX      1404
#define IDC_CHK_HEX_RX      1405
#define IDC_BTN_CLEAR_RX    1406
#define IDC_BTN_CLEAR_TX    1407
#define IDC_CHK_REPEAT      1408
#define IDC_EDIT_INTERVAL   1409
#define IDC_CHK_NEWLINE     1410
#define IDC_COMBO_NEWLINE   1411
#define IDC_STATIC_TX_COUNT 1412
#define IDC_STATIC_RX_COUNT 1413

/* Tab 4: Peripheral Control */
#define IDC_GRP_GPIO        1500
#define IDC_COMBO_GPIO_PIN  1501
#define IDC_COMBO_GPIO_DIR  1502
#define IDC_COMBO_GPIO_LVL  1503
#define IDC_BTN_GPIO_SET    1504
#define IDC_BTN_GPIO_READ   1505
#define IDC_GRP_ADC         1510
#define IDC_COMBO_ADC_CH    1511
#define IDC_BTN_ADC_READ    1512
#define IDC_EDIT_ADC_VAL    1513
#define IDC_GRP_PWM         1520
#define IDC_EDIT_PWM_PERIOD 1521
#define IDC_EDIT_PWM_D1     1522
#define IDC_EDIT_PWM_D2     1523
#define IDC_EDIT_PWM_D3     1524
#define IDC_EDIT_PWM_D4     1525
#define IDC_EDIT_PWM_D5     1526
#define IDC_BTN_PWM_SET     1527

/* Tab 5: Network Management */
#define IDC_BTN_DISCOVER    1601
#define IDC_LIST_NODES      1602
#define IDC_LIST_BINDINGS   1603
#define IDC_EDIT_BIND_SRC   1604
#define IDC_EDIT_BIND_DST   1605
#define IDC_BTN_BIND_SET    1606
#define IDC_BTN_BIND_QRY    1607
#define IDC_BTN_BIND_DEL    1608
#define IDC_STATIC_BIND_CL  1609
#define IDC_EDIT_BIND_CLUST 1610
#define IDC_BTN_JOIN_GROUP  1611
#define IDC_EDIT_GROUP_ID   1612
#define IDC_BTN_LEAVE_GROUP 1613
#define IDC_GRP_BINDING     1620
#define IDC_GRP_GROUP       1621

/* Log window */
#define IDC_LOG             2001
#define IDC_BTN_CLEAR_LOG   2002

/* Status bar */
#define IDC_STATUSBAR       3001

/* Timer IDs */
#define IDT_READ_TIMEOUT    1
#define IDT_RX_POLL         2
#define IDT_REPEAT_SEND     3

/* ========== Types ========== */
typedef struct {
    WCHAR name[16];       /* e.g. "COM3" */
    WCHAR path[16];       /* e.g. "\\.\COM3" */
} ComPortInfo;

typedef struct {
    unsigned short addr;
    unsigned char  type;      /* 1=Coordinator, 2=Router, 3=EndDevice, 4=SleepEndDevice */
    unsigned char  mac[8];
} NodeInfo;

typedef struct {
    unsigned short srcAddr;
    unsigned short dstAddr;
    unsigned short clusterId;
    unsigned char  srcEp;
    unsigned char  dstEp;
} BindingInfo;

/* ========== Global State ========== */
typedef struct {
    /* Serial */
    HANDLE        hCom;
    int           connected;
    WCHAR         portName[16];
    int           baudRate;
    OVERLAPPED    olRead, olWrite;
    unsigned char rxBuf[MAX_RX_BUF];
    int           rxHead, rxTail;
    CRITICAL_SECTION rxLock;

    /* HEX frame parser */
    unsigned char hexFrame[HEX_FRAME_MAX];
    int           hexFrameLen;
    int           hexFrameState;  /* 0=idle, 1=got_head, 2=reading */

    /* Node/Binding tables */
    NodeInfo      nodes[MAX_NODES];
    int           nodeCount;
    BindingInfo   bindings[MAX_BINDINGS];
    int           bindingCount;

    /* Data transfer */
    int           txCount;
    int           rxCount;
    int           repeatActive;
    UINT_PTR      repeatTimerId;

    /* UI */
    HWND          hMainWnd;
    HWND          hTab;
    HWND          hLog;
    HWND          hStatus;
    HWND          hComboPort, hComboBaud;
    HWND          hBtnOpen, hBtnRefresh;
    HWND          hStaticStatus;

    /* Tab 0 controls */
    HWND          hTxtStatus;
    HWND          hComboNodeType, hComboChannel, hEditPanId, hComboTxPower;
    HWND          hBtnStartNwk, hBtnStopNwk;

    /* Tab 1 controls */
    HWND          hComboHexType, hComboHexCmd, hEditHexData, hEditHexRaw;
    HWND          hEditHexResp, hStaticHexFrame;

    /* Tab 2 controls */
    HWND          hEditAtCmd, hEditAtResp;

    /* Tab 3 controls */
    HWND          hEditTxData, hEditRxData;
    HWND          hChkHexTx, hChkHexRx, hChkRepeat, hChkNewline;
    HWND          hEditInterval, hComboNewline;
    HWND          hStaticRxCount;

    /* Tab 4 controls */
    HWND          hComboGpioPin, hComboGpioDir, hComboGpioLvl;
    HWND          hComboAdcCh, hEditAdcVal;
    HWND          hEditPwmPeriod, hEditPwmD[5];

    /* Tab 5 controls */
    HWND          hListNodes, hListBindings;
    HWND          hEditBindSrc, hEditBindDst, hEditBindClust;
    HWND          hEditGroupId;

    /* Fonts */
    HFONT         hMonoFont;
    HFONT         hGuiFont;

    /* Baud rate for AT mode switching */
    int           currentBaud;
} AppState;

static AppState g;

/* ========== Forward Declarations ========== */
/* Serial */
int  serialEnum(ComPortInfo *ports, int maxPorts);
int  serialOpen(const WCHAR *path, int baud);
void serialClose(void);
int  serialSend(const unsigned char *data, int len);
int  serialRecv(unsigned char *buf, int maxLen);
void serialFlush(void);
void serialSetBaud(int baud);

/* HEX protocol */
int  hexCalcXor(const unsigned char *data, int len);
int  hexBuildFrame(unsigned char *out, int maxLen,
                   unsigned char cmdType, unsigned char cmdCode,
                   const unsigned char *data, int dataLen);
int  hexValidateFrame(const unsigned char *frame, int len);
void hexParseAsync(const unsigned char *frame, int len);
const WCHAR* hexGetCmdName(unsigned char cmdType, unsigned char cmdCode);
const WCHAR* hexGetNodeTypeName(unsigned char type);
const WCHAR* hexGetStatusName(unsigned char status);

/* AT protocol */
void atBuildCmd(WCHAR *out, int maxLen, const WCHAR *fmt, ...);
void atParseResponse(const char *resp);

/* Logging */
void logAdd(const WCHAR *fmt, ...);
void logAddHex(const WCHAR *prefix, const unsigned char *data, int len);

/* UI helpers */
void uiUpdateStatus(void);
void uiEnableControls(int enable);
void uiSetControlText(HWND hwnd, const WCHAR *text);
void uiGetControlText(HWND hwnd, WCHAR *buf, int maxLen);
int  uiGetControlInt(HWND hwnd);
void uiAppendEdit(HWND hEdit, const WCHAR *text);
void uiAppendEditHex(HWND hEdit, const unsigned char *data, int len);
void uiUpdateDataCounters(void);

/* Tab panels */
void tabCreateBasicConfig(HWND parent);
void tabCreateHexCmd(HWND parent);
void tabCreateAtCmd(HWND parent);
void tabCreateDataTransfer(HWND parent);
void tabCreatePeripheral(HWND parent);
void tabCreateNetworkMgmt(HWND parent);

/* Tab show/hide */
void tabShowPanel(int index);
HWND g_hPanels[6];

/* ========== Serial Port Functions ========== */

int serialEnum(ComPortInfo *ports, int maxPorts) {
    int count = 0;
    WCHAR path[16];
    for (int i = 1; i <= 32 && count < maxPorts; i++) {
        wsprintfW(path, L"\\\\.\\COM%d", i);
        HANDLE h = CreateFileW(path, 0, 0, NULL, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            CloseHandle(h);
            wsprintfW(ports[count].name, L"COM%d", i);
            wcscpy(ports[count].path, path);
            count++;
        }
    }
    return count;
}

int serialOpen(const WCHAR *path, int baud) {
    g.hCom = CreateFileW(path, GENERIC_READ | GENERIC_WRITE,
                         0, NULL, OPEN_EXISTING,
                         FILE_FLAG_OVERLAPPED, NULL);
    if (g.hCom == INVALID_HANDLE_VALUE) return 0;

    SetupComm(g.hCom, 65536, 65536);

    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);
    GetCommState(g.hCom, &dcb);
    dcb.BaudRate = baud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    SetCommState(g.hCom, &dcb);

    COMMTIMEOUTS to = {0};
    to.ReadIntervalTimeout = MAXDWORD;
    to.ReadTotalTimeoutMultiplier = 0;
    to.ReadTotalTimeoutConstant = 0;
    SetCommTimeouts(g.hCom, &to);

    memset(&g.olRead, 0, sizeof(g.olRead));
    g.olRead.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    memset(&g.olWrite, 0, sizeof(g.olWrite));
    g.olWrite.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    g.rxHead = g.rxTail = 0;
    g.hexFrameState = 0;
    g.hexFrameLen = 0;
    g.connected = 1;
    g.currentBaud = baud;
    return 1;
}

void serialClose(void) {
    g.connected = 0;
    if (g.hCom != INVALID_HANDLE_VALUE) {
        CloseHandle(g.hCom);
        g.hCom = INVALID_HANDLE_VALUE;
    }
    if (g.olRead.hEvent) { CloseHandle(g.olRead.hEvent); g.olRead.hEvent = NULL; }
    if (g.olWrite.hEvent) { CloseHandle(g.olWrite.hEvent); g.olWrite.hEvent = NULL; }
}

void serialSetBaud(int baud) {
    if (!g.connected) return;
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);
    GetCommState(g.hCom, &dcb);
    dcb.BaudRate = baud;
    SetCommState(g.hCom, &dcb);
    g.currentBaud = baud;
}

int serialSend(const unsigned char *data, int len) {
    if (!g.connected) return 0;
    DWORD written;
    ResetEvent(g.olWrite.hEvent);
    if (!WriteFile(g.hCom, data, len, &written, &g.olWrite)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            GetOverlappedResult(g.hCom, &g.olWrite, &written, TRUE);
        } else {
            return 0;
        }
    }
    return written;
}

int serialRecv(unsigned char *buf, int maxLen) {
    if (!g.connected) return 0;
    EnterCriticalSection(&g.rxLock);
    int avail = g.rxTail - g.rxHead;
    if (avail < 0) avail += MAX_RX_BUF;
    if (avail > maxLen) avail = maxLen;
    for (int i = 0; i < avail; i++) {
        buf[i] = g.rxBuf[g.rxHead];
        g.rxHead = (g.rxHead + 1) % MAX_RX_BUF;
    }
    LeaveCriticalSection(&g.rxLock);
    return avail;
}

void serialFlush(void) {
    EnterCriticalSection(&g.rxLock);
    g.rxHead = g.rxTail = 0;
    LeaveCriticalSection(&g.rxLock);
}

static DWORD WINAPI serialReadThread(LPVOID param) {
    unsigned char tmp[4096];
    while (g.connected) {
        DWORD read = 0;
        if (ReadFile(g.hCom, tmp, sizeof(tmp), &read, &g.olRead)) {
            if (read > 0) {
                EnterCriticalSection(&g.rxLock);
                for (DWORD i = 0; i < read; i++) {
                    g.rxBuf[g.rxTail] = tmp[i];
                    g.rxTail = (g.rxTail + 1) % MAX_RX_BUF;
                }
                LeaveCriticalSection(&g.rxLock);
                PostMessageW(g.hMainWnd, WM_APP, 0, read);
            }
        } else if (GetLastError() == ERROR_IO_PENDING) {
            DWORD transferred;
            if (GetOverlappedResult(g.hCom, &g.olRead, &transferred, TRUE)) {
                if (transferred > 0) {
                    EnterCriticalSection(&g.rxLock);
                    for (DWORD i = 0; i < transferred; i++) {
                        g.rxBuf[g.rxTail] = tmp[i];
                        g.rxTail = (g.rxTail + 1) % MAX_RX_BUF;
                    }
                    LeaveCriticalSection(&g.rxLock);
                    PostMessageW(g.hMainWnd, WM_APP, 0, transferred);
                }
            }
        }
    }
    return 0;
}

/* ========== HEX Protocol Functions ========== */

int hexCalcXor(const unsigned char *data, int len) {
    unsigned char xor = 0;
    for (int i = 0; i < len; i++) xor ^= data[i];
    return xor;
}

int hexBuildFrame(unsigned char *out, int maxLen,
                  unsigned char cmdType, unsigned char cmdCode,
                  const unsigned char *data, int dataLen) {
    int payloadLen = 2 + dataLen; /* cmdType + cmdCode + data */
    int totalLen = 1 + 1 + payloadLen + 1; /* head + len + payload + xor */
    if (totalLen > maxLen) return -1;

    int idx = 0;
    out[idx++] = 0x55;                           /* frame head */
    out[idx++] = (unsigned char)(payloadLen + 1); /* frame len = payload + xor */
    out[idx++] = cmdType;
    out[idx++] = cmdCode;
    if (dataLen > 0) {
        memcpy(out + idx, data, dataLen);
        idx += dataLen;
    }
    /* XOR over cmdType + cmdCode + data */
    unsigned char xorVal = cmdType ^ cmdCode;
    for (int i = 0; i < dataLen; i++) xorVal ^= data[i];
    out[idx++] = xorVal;
    return idx;
}

int hexValidateFrame(const unsigned char *frame, int len) {
    if (len < 4) return 0; /* minimum: head + len + type + code + xor */
    if (frame[0] != 0x55) return 0;
    int payloadLen = frame[1];
    if (payloadLen < 3) return 0; /* at least type+code+xor */
    if (payloadLen + 2 != len) return 0;
    /* Verify XOR */
    unsigned char expected = 0;
    for (int i = 0; i < payloadLen; i++)
        expected ^= frame[2 + i];
    if (expected != 0) return 0; /* XOR of all payload bytes should be 0 */
    return 1;
}

const WCHAR* hexGetCmdName(unsigned char cmdType, unsigned char cmdCode) {
    if (cmdType == 0x00) {
        switch (cmdCode) {
            case 0x00: return L"查询模组状态";
            case 0x01: return L"模组开机/软启动";
            case 0x02: return L"开始配网";
            case 0x03: return L"停止配网";
            case 0x04: return L"复位/恢复出厂";
            case 0x05: return L"设置节点类型";
            case 0x06: return L"查询信道";
            case 0x07: return L"设置信道";
            case 0x08: return L"设置PANID";
            case 0x09: return L"查询网络状态";
            case 0x0A: return L"加入组播组";
            case 0x0B: return L"退出组播组";
            case 0x0C: return L"信道扫描";
            case 0x0D: return L"设置发射功率";
            case 0x10: return L"读取本地属性";
            case 0x11: return L"设置本地属性";
            case 0x14: return L"自动绑定目标";
            case 0x16: return L"进入AT模式";
            case 0x20: return L"获取UTC时间";
            case 0x21: return L"设置UTC时间";
            case 0x22: return L"读取节点地址表";
            case 0x23: return L"读取网络密钥";
            case 0x28: return L"重传设备信息";
            default: return L"未知命令";
        }
    } else if (cmdType == 0x01) {
        switch (cmdCode) {
            case 0x00: return L"ZDO网络地址请求";
            case 0x01: return L"ZDO IEEE地址请求";
            case 0x02: return L"ZDO节点描述请求";
            case 0x03: return L"ZDO简单描述请求";
            case 0x04: return L"ZDO活动端点请求";
            case 0x21: return L"ZDO绑定请求";
            case 0x22: return L"ZDO解绑请求";
            case 0x33: return L"ZDO绑定表请求";
            case 0x34: return L"ZDO离网请求";
            default: return L"未知ZDO命令";
        }
    } else if (cmdType == 0x02) {
        switch (cmdCode) {
            case 0x00: return L"ZCL读属性";
            case 0x01: return L"ZCL写属性";
            case 0x02: return L"ZCL读上报配置";
            case 0x03: return L"ZCL写上报配置";
            case 0x04: return L"ZCL发现属性";
            case 0x05: return L"ZCL发现扩展属性";
            case 0x06: return L"ZCL发现接收命令";
            case 0x07: return L"ZCL发现生成命令";
            case 0x0F: return L"ZCL命令/透传";
            default: return L"未知ZCL命令";
        }
    } else if (cmdType == 0x80) return L"系统通知";
    else if (cmdType == 0x81) return L"网络管理返回";
    else if (cmdType == 0x82) return L"ZCL数据接收";
    else if (cmdType == 0x8F) return L"发送确认";
    return L"未知类型";
}

const WCHAR* hexGetNodeTypeName(unsigned char type) {
    switch (type) {
        case 0: return L"协调器";
        case 1: return L"路由器";
        case 2: return L"终端设备";
        case 3: return L"休眠终端";
        default: return L"未知";
    }
}

const WCHAR* hexGetStatusName(unsigned char status) {
    switch (status) {
        case 0x00: return L"成功";
        case 0x01: return L"参数错误";
        case 0x02: return L"命令不支持";
        case 0x03: return L"执行超时";
        case 0x04: return L"网络未就绪";
        case 0x05: return L"安全密钥缺失";
        case 0x06: return L"内存不足";
        case 0x07: return L"校验失败";
        case 0x08: return L"帧长越界";
        case 0x09: return L"权限拒绝";
        default: return L"未知错误";
    }
}

/* Format HEX frame for display */
void hexFormatFrame(WCHAR *out, int maxLen, const unsigned char *frame, int len) {
    int pos = 0;
    for (int i = 0; i < len && pos < maxLen - 4; i++) {
        pos += wsprintfW(out + pos, L"%02X ", frame[i]);
    }
    if (pos > 0) out[pos - 1] = 0;
}

/* Parse a response (feedback) frame and update UI */
void hexParseResponse(const unsigned char *frame, int len) {
    if (len < 6) return; /* head+len+type+code+status+xor at minimum */
    unsigned char cmdType = frame[2];
    unsigned char cmdCode = frame[3];
    if (cmdType >= 0x80) return; /* not a response, it's async */

    unsigned char status = frame[4];
    const unsigned char *data = frame + 5;
    int dataLen = len - 6; /* minus head, len, type, code, status, xor */

    if (status != 0x00) {
        logAdd(L"[响应] %s (0x%02X/0x%02X) 失败: %s",
               hexGetCmdName(cmdType, cmdCode), cmdType, cmdCode, hexGetStatusName(status));
        return;
    }

    if (cmdType == 0x00 && cmdCode == 0x00 && dataLen >= 15) {
        /* Query status response */
        unsigned char nwkState = data[0];
        unsigned char nodeType = data[1];
        /* MAC: 8 bytes little-endian */
        WCHAR macStr[32];
        wsprintfW(macStr, L"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
            data[9], data[8], data[7], data[6], data[5], data[4], data[3], data[2]);
        unsigned char channel = data[10];
        unsigned short panId = data[11] | (data[12] << 8);
        unsigned short shortAddr = data[13] | (data[14] << 8);

        WCHAR statusText[512];
        int pos = 0;
        pos += wsprintfW(statusText + pos, L"状态: %s\r\n", hexGetStatusName(status));
        pos += wsprintfW(statusText + pos, L"网络状态: %s\r\n", (nwkState == 0) ? L"已组网" : L"未组网");
        pos += wsprintfW(statusText + pos, L"节点类型: %s\r\n", hexGetNodeTypeName(nodeType));
        pos += wsprintfW(statusText + pos, L"IEEE MAC: %s\r\n", macStr);
        pos += wsprintfW(statusText + pos, L"短地址: 0x%04X\r\n", shortAddr);
        pos += wsprintfW(statusText + pos, L"PANID: 0x%04X\r\n", panId);
        pos += wsprintfW(statusText + pos, L"信道: %d\r\n", channel);

        /* Show additional raw data */
        if (dataLen > 14) {
            pos += wsprintfW(statusText + pos, L"\r\n附加数据(%d字节):\r\n", dataLen - 14);
            for (int i = 14; i < dataLen && pos < 480; i++) {
                pos += wsprintfW(statusText + pos, L"%02X ", data[i]);
                if ((i - 13) % 16 == 0) pos += wsprintfW(statusText + pos, L"\r\n");
            }
        }

        SetWindowTextW(g.hTxtStatus, statusText);
        logAdd(L"[状态] 已更新 - 节点:%s, 短地址:0x%04X, PANID:0x%04X",
               hexGetNodeTypeName(nodeType), shortAddr, panId);
               
        /* Sync UI combos/edits */
        if (nodeType <= 3) SendMessageW(g.hComboNodeType, CB_SETCURSEL, nodeType, 0);
        if (channel >= 11 && channel <= 26) SendMessageW(g.hComboChannel, CB_SETCURSEL, channel - 11, 0);
        WCHAR hexPan[16];
        wsprintfW(hexPan, L"%04X", panId);
        SetWindowTextW(g.hEditPanId, hexPan);
    } else if (cmdType == 0x00 && cmdCode == 0x0D && dataLen >= 2) {
        unsigned char pwr = data[1];
        if (pwr < 17) {
            SendMessageW(g.hComboTxPower, CB_SETCURSEL, pwr, 0);
            logAdd(L"[配置] 发射功率同步成功");
        }
    } else if (cmdType == 0x00 && cmdCode == 0x09 && dataLen >= 1) {
        /* Query network joined status */
        unsigned char joined = data[0];
        logAdd(L"[网络] 已加入网络: %s", joined ? L"是" : L"否");
        WCHAR buf[256];
        wsprintfW(buf, L"入网状态: %s\r\n", joined ? L"已加入网络" : L"未加入网络");
        SetWindowTextW(g.hTxtStatus, buf);
    } else if (cmdType == 0x00 && cmdCode == 0x06 && dataLen >= 1) {
        /* Query channel response */
        unsigned char chan = data[0];
        logAdd(L"[信道] 当前信道: %d", chan);
        WCHAR buf[64];
        wsprintfW(buf, L"当前信道: %d", chan);
        SetWindowTextW(g.hTxtStatus, buf);
    } else if (cmdType == 0x00 && cmdCode == 0x22 && dataLen >= 2) {
        /* Read node address table */
        int nodeCount = data[0];
        logAdd(L"[节点表] 节点数: %d", nodeCount);
        /* Clear and populate node list */
        SendMessageW(g.hListNodes, LVM_DELETEALLITEMS, 0, 0);
        int offset = 1;
        for (int i = 0; i < nodeCount && offset + 4 <= dataLen; i++) {
            unsigned short addr = data[offset] | (data[offset + 1] << 8);
            unsigned char type = (offset + 2 < dataLen) ? data[offset + 2] : 0;
            WCHAR addrStr[16], typeStr[32];
            wsprintfW(addrStr, L"0x%04X", addr);
            wcscpy(typeStr, hexGetNodeTypeName(type));

            LVITEMW item = {0};
            item.mask = LVIF_TEXT;
            item.iItem = i;
            item.pszText = addrStr;
            SendMessageW(g.hListNodes, LVM_INSERTITEMW, 0, (LPARAM)&item);

            item.iSubItem = 1;
            item.pszText = L"";
            SendMessageW(g.hListNodes, LVM_SETITEMW, 0, (LPARAM)&item);

            item.iSubItem = 2;
            item.pszText = typeStr;
            SendMessageW(g.hListNodes, LVM_SETITEMW, 0, (LPARAM)&item);

            offset += 3;
        }
    } else {
        /* Generic response - show raw data */
        WCHAR buf[512];
        int pos = 0;
        pos += wsprintfW(buf + pos, L"命令: %s\r\n状态: %s\r\n数据: ",
            hexGetCmdName(cmdType, cmdCode), hexGetStatusName(status));
        for (int i = 0; i < dataLen && pos < 480; i++)
            pos += wsprintfW(buf + pos, L"%02X ", data[i]);
        SetWindowTextW(g.hTxtStatus, buf);
        logAdd(L"[响应] %s 成功, 返回数据: %d 字节", hexGetCmdName(cmdType, cmdCode), dataLen);
    }
}

void hexParseAsync(const unsigned char *frame, int len) {
    if (len < 4) return;
    unsigned char cmdType = frame[2];
    unsigned char cmdCode = frame[3];

    (void)cmdCode;
    if (cmdType == 0x80) {
        /* System notification */
        if (len >= 5) {
            unsigned char event = frame[4];
            switch (event) {
                case 0x00: logAdd(L"[通知] 模组已复位"); break;
                case 0x01: logAdd(L"[通知] 已加入网络"); break;
                case 0x02: logAdd(L"[通知] 已离开网络"); break;
                case 0x03: logAdd(L"[通知] 网络开启配网"); break;
                case 0x04: logAdd(L"[通知] 网络停止配网"); break;
                case 0x05: logAdd(L"[通知] 收到绑定请求"); break;
                case 0x06: logAdd(L"[通知] 绑定完成"); break;
                default: {
                    WCHAR buf[128];
                    wsprintfW(buf, L"[通知] 事件码=0x%02X", event);
                    logAdd(buf);
                }
            }
        }
    } else if (cmdType == 0x81) {
        logAdd(L"[网络返回] 收到网络管理返回");
    } else if (cmdType == 0x82) {
        /* ZCL data received - transparent data */
        if (len >= 12) {
            int dataOffset = 9;
            int dataLen = len - dataOffset;
            if (dataLen > 3) {
                int zclDataOffset = dataOffset + 3;
                int zclDataLen = len - zclDataOffset;
                if (zclDataLen > 0) {
                    g.rxCount += zclDataLen;
                    logAddHex(L"[ZCL数据] 数据:", frame + zclDataOffset, zclDataLen);
                    if (g.hStaticRxCount) uiUpdateDataCounters();
                    if (g.hEditRxData) {
                        if (SendMessageW(g.hChkHexRx, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                            uiAppendEditHex(g.hEditRxData, frame + zclDataOffset, zclDataLen);
                            uiAppendEdit(g.hEditRxData, L" ");
                        } else {
                            WCHAR utf16[1024];
                            int wlen = MultiByteToWideChar(CP_UTF8, 0, (char*)(frame + zclDataOffset), zclDataLen, utf16, 1023);
                            utf16[wlen] = 0;
                            uiAppendEdit(g.hEditRxData, utf16);
                        }
                    }
                }
            }
        }
    } else if (cmdType == 0x8F) {
        if (len >= 5) {
            unsigned char st = frame[4];
            logAdd(st == 0x00 ? L"[发送确认] 成功" : L"[发送确认] 失败");
        }
    }
}

/* ========== AT Protocol Functions ========== */

void atBuildCmd(WCHAR *out, int maxLen, const WCHAR *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vswprintf(out, maxLen, fmt, args);
    va_end(args);
}

void atParseResponse(const char *resp) {
    if (strncmp(resp, "+OK", 3) == 0) {
        logAdd(L"[AT响应] 成功");
    } else if (strncmp(resp, "+ERR=", 5) == 0) {
        WCHAR buf[128];
        wsprintfW(buf, L"[AT响应] 错误: %S", resp + 5);
        logAdd(buf);
    } else {
        WCHAR buf[512];
        wsprintfW(buf, L"[AT响应] %S", resp);
        logAdd(buf);
    }
}

/* ========== Logging Functions ========== */

void logAdd(const WCHAR *fmt, ...) {
    WCHAR buf[1024];
    va_list args;
    va_start(args, fmt);
    vswprintf(buf, 1024, fmt, args);
    va_end(args);

    /* Add timestamp */
    SYSTEMTIME st;
    GetLocalTime(&st);
    WCHAR line[1152];
    wsprintfW(line, L"[%02d:%02d:%02d.%03d] %s\r\n",
              st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buf);

    /* Append to log */
    int len = GetWindowTextLengthW(g.hLog);
    SendMessageW(g.hLog, EM_SETSEL, len, len);
    SendMessageW(g.hLog, EM_REPLACESEL, FALSE, (LPARAM)line);

    /* Trim old content */
    int totalLen = GetWindowTextLengthW(g.hLog);
    if (totalLen > 50000) {
        SendMessageW(g.hLog, EM_SETSEL, 0, totalLen / 2);
        SendMessageW(g.hLog, EM_REPLACESEL, FALSE, (LPARAM)L"");
    }
}

void logAddHex(const WCHAR *prefix, const unsigned char *data, int len) {
    WCHAR hex[1024];
    int pos = 0;
    for (int i = 0; i < len && pos < 1000; i++) {
        pos += wsprintfW(hex + pos, L"%02X ", data[i]);
    }
    if (pos > 0) hex[pos - 1] = 0;

    WCHAR line[2048];
    wsprintfW(line, L"%s %s", prefix, hex);
    logAdd(line);
}

/* ========== UI Helper Functions ========== */

void uiUpdateStatus(void) {
    WCHAR status[256];
    if (g.connected) {
        wsprintfW(status, L"已连接 %s | 波特率: %d | 数据位:8 停止位:1 校验:无",
                  g.portName, g.currentBaud);
    } else {
        wcscpy(status, L"未连接");
    }
    SetWindowTextW(g.hStaticStatus, status);
}

void uiUpdateDataCounters(void) {
    if (g.hStaticRxCount) {
        WCHAR buf[64];
        wsprintfW(buf, L"TX:%d RX:%d", g.txCount, g.rxCount);
        SetWindowTextW(g.hStaticRxCount, buf);
    }
}

void uiEnableControls(int enable) {
    /* Enable/disable all feature controls based on connection state */
    for (int i = 0; i < 6; i++) {
        if (g_hPanels[i]) EnableWindow(g_hPanels[i], enable);
    }
    EnableWindow(g.hBtnOpen, TRUE);
    EnableWindow(g.hBtnRefresh, TRUE);
}

void uiSetControlText(HWND hwnd, const WCHAR *text) {
    SetWindowTextW(hwnd, text);
}

void uiGetControlText(HWND hwnd, WCHAR *buf, int maxLen) {
    GetWindowTextW(hwnd, buf, maxLen);
}

int uiGetControlInt(HWND hwnd) {
    WCHAR buf[32];
    GetWindowTextW(hwnd, buf, 32);
    return _wtoi(buf);
}

void uiAppendEdit(HWND hEdit, const WCHAR *text) {
    int len = GetWindowTextLengthW(hEdit);
    SendMessageW(hEdit, EM_SETSEL, len, len);
    SendMessageW(hEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
}

void uiAppendEditHex(HWND hEdit, const unsigned char *data, int len) {
    WCHAR hex[4096];
    int pos = 0;
    for (int i = 0; i < len && pos < 4000; i++) {
        pos += wsprintfW(hex + pos, L"%02X ", data[i]);
    }
    hex[pos] = 0;
    uiAppendEdit(hEdit, hex);
}

/* Parse hex string to bytes, returns byte count */
static int parseHexString(const WCHAR *hexStr, unsigned char *out, int maxLen) {
    int count = 0;
    WCHAR byteStr[3] = {0};
    const WCHAR *p = hexStr;
    while (*p && count < maxLen) {
        /* Skip spaces */
        while (*p == L' ' || *p == L'\t' || *p == L'\r' || *p == L'\n') p++;
        if (!*p) break;
        byteStr[0] = *p++;
        byteStr[1] = (*p && *p != L' ') ? *p++ : 0;
        if (byteStr[0] && byteStr[1]) {
            unsigned int val;
            if (swscanf(byteStr, L"%2x", &val) == 1) {
                out[count++] = (unsigned char)val;
            }
        }
    }
    return count;
}

/* ========== Tab Panel Creation ========== */

void tabCreateBasicConfig(HWND parent) {
    int y = 10, w = 550;

    /* Status group */
    CreateWindowW(L"BUTTON", L"模组状态",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, y, w, 120, parent, (HMENU)IDC_GRP_STATUS, NULL, NULL);
    g.hTxtStatus = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_BORDER,
        20, y + 18, w - 30, 70, parent, (HMENU)IDC_TXT_STATUS, NULL, NULL);
    SendMessageW(g.hTxtStatus, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
    CreateWindowW(L"BUTTON", L"查询状态",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, y + 95, 80, 22, parent, (HMENU)IDC_BTN_QRY_STATUS, NULL, NULL);
    y += 125;

    /* Network config group */
    CreateWindowW(L"BUTTON", L"网络配置",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, y, w, 160, parent, (HMENU)IDC_GRP_NETWORK, NULL, NULL);

    /* Row 1 */
    CreateWindowW(L"STATIC", L"节点类型:", WS_CHILD | WS_VISIBLE, 20, y + 20, 60, 18, parent, NULL, NULL, NULL);
    g.hComboNodeType = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        85, y + 18, 100, 100, parent, (HMENU)IDC_COMBO_NODETYPE, NULL, NULL);
    {
        const WCHAR *types[] = {L"协调器 (Coordinator)", L"路由器 (Router)",
            L"终端设备 (EndDevice)", L"休眠终端 (SleepEndDevice)"};
        for (int i = 0; i < 4; i++)
            SendMessageW(g.hComboNodeType, CB_ADDSTRING, 0, (LPARAM)types[i]);
        SendMessageW(g.hComboNodeType, CB_SETCURSEL, 0, 0);
    }
    CreateWindowW(L"BUTTON", L"读取", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 195, y + 17, 40, 20, parent, (HMENU)IDC_BTN_GET_NODE, NULL, NULL);
    CreateWindowW(L"BUTTON", L"设置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, y + 17, 40, 20, parent, (HMENU)IDC_BTN_SET_NODE, NULL, NULL);

    CreateWindowW(L"STATIC", L"信道:", WS_CHILD | WS_VISIBLE, 300, y + 20, 40, 18, parent, NULL, NULL, NULL);
    g.hComboChannel = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        340, y + 18, 60, 100, parent, (HMENU)IDC_COMBO_CHANNEL, NULL, NULL);
    for (int i = 11; i <= 26; i++) {
        WCHAR s[8]; wsprintfW(s, L"%d", i);
        SendMessageW(g.hComboChannel, CB_ADDSTRING, 0, (LPARAM)s);
    }
    SendMessageW(g.hComboChannel, CB_SETCURSEL, 0, 0);
    CreateWindowW(L"BUTTON", L"读取", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 410, y + 17, 40, 20, parent, (HMENU)IDC_BTN_GET_CHAN, NULL, NULL);
    CreateWindowW(L"BUTTON", L"设置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 455, y + 17, 40, 20, parent, (HMENU)IDC_BTN_SET_CHAN, NULL, NULL);

    /* Row 2 */
    CreateWindowW(L"STATIC", L"PANID:", WS_CHILD | WS_VISIBLE, 20, y + 48, 45, 18, parent, NULL, NULL, NULL);
    g.hEditPanId = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        70, y + 46, 60, 20, parent, (HMENU)IDC_EDIT_PANID, NULL, NULL);
    CreateWindowW(L"BUTTON", L"读取", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 140, y + 45, 40, 20, parent, (HMENU)IDC_BTN_GET_PANID, NULL, NULL);
    CreateWindowW(L"BUTTON", L"设置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 185, y + 45, 40, 20, parent, (HMENU)IDC_BTN_SET_PANID, NULL, NULL);

    CreateWindowW(L"STATIC", L"发射功率:", WS_CHILD | WS_VISIBLE, 240, y + 48, 60, 18, parent, NULL, NULL, NULL);
    g.hComboTxPower = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        305, y + 46, 80, 100, parent, (HMENU)IDC_COMBO_TXPOWER, NULL, NULL);
    {
        const WCHAR *pwrs[] = {L"4 dBm", L"3 dBm", L"2 dBm", L"1 dBm", L"0 dBm",
            L"-1 dBm", L"-2 dBm", L"-3 dBm", L"-4 dBm", L"-6 dBm", L"-8 dBm",
            L"-10 dBm", L"-12 dBm", L"-14 dBm", L"-16 dBm", L"-18 dBm", L"-20 dBm"};
        for (int i = 0; i < 17; i++)
            SendMessageW(g.hComboTxPower, CB_ADDSTRING, 0, (LPARAM) pwrs[i]);
        SendMessageW(g.hComboTxPower, CB_SETCURSEL, 0, 0);
    }
    CreateWindowW(L"BUTTON", L"读取", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 395, y + 45, 40, 20, parent, (HMENU)IDC_BTN_GET_TXPWR, NULL, NULL);
    CreateWindowW(L"BUTTON", L"设置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 440, y + 45, 40, 20, parent, (HMENU)IDC_BTN_SET_TXPWR, NULL, NULL);

    /* Network operation buttons */
    int btnY = y + 75;
    g.hBtnStartNwk = CreateWindowW(L"BUTTON", L"开始配网",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, btnY, 80, 25, parent, (HMENU)IDC_BTN_START_NWK, NULL, NULL);
    g.hBtnStopNwk = CreateWindowW(L"BUTTON", L"停止配网",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        110, btnY, 80, 25, parent, (HMENU)IDC_BTN_STOP_NWK, NULL, NULL);
    CreateWindowW(L"BUTTON", L"软复位",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        200, btnY, 70, 25, parent, (HMENU)IDC_BTN_RESET, NULL, NULL);
    CreateWindowW(L"BUTTON", L"恢复出厂",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        280, btnY, 70, 25, parent, (HMENU)IDC_BTN_RESTORE, NULL, NULL);
    CreateWindowW(L"BUTTON", L"一键绑定",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        360, btnY, 70, 25, parent, (HMENU)IDC_BTN_BIND, NULL, NULL);

    btnY += 30;
    CreateWindowW(L"BUTTON", L"获取UTC时间",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, btnY, 90, 25, parent, (HMENU)IDC_BTN_GET_UTC, NULL, NULL);
    CreateWindowW(L"BUTTON", L"设置UTC时间",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, btnY, 90, 25, parent, (HMENU)IDC_BTN_SET_UTC, NULL, NULL);
    CreateWindowW(L"BUTTON", L"读取密钥",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        220, btnY, 80, 25, parent, (HMENU)IDC_BTN_GET_KEY, NULL, NULL);
    CreateWindowW(L"BUTTON", L"读取节点表",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        310, btnY, 90, 25, parent, (HMENU)IDC_BTN_GET_NODES, NULL, NULL);
    CreateWindowW(L"BUTTON", L"重传设备信息",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        410, btnY, 100, 25, parent, (HMENU)IDC_BTN_RETX_INFO, NULL, NULL);
}

void tabCreateHexCmd(HWND parent) {
    int y = 10, w = 560;

    /* Hex command type */
    CreateWindowW(L"STATIC", L"命令类型:", WS_CHILD | WS_VISIBLE, 10, y + 3, 60, 18, parent, NULL, NULL, NULL);
    g.hComboHexType = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        75, y, 160, 200, parent, (HMENU)IDC_COMBO_HEX_TYPE, NULL, NULL);
    {
        const WCHAR *types[] = {L"本地配置 (0x00)", L"ZDO请求 (0x01)", L"ZCL发送 (0x02)"};
        for (int i = 0; i < 3; i++)
            SendMessageW(g.hComboHexType, CB_ADDSTRING, 0, (LPARAM)types[i]);
        SendMessageW(g.hComboHexType, CB_SETCURSEL, 0, 0);
    }

    CreateWindowW(L"STATIC", L"命令码:", WS_CHILD | WS_VISIBLE, 250, y + 3, 55, 18, parent, NULL, NULL, NULL);
    g.hComboHexCmd = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        310, y, 200, 300, parent, (HMENU)IDC_COMBO_HEX_CMD, NULL, NULL);
    /* Populate with type 0x00 commands initially */
    const WCHAR *cmds00[] = {
        L"0x00 查询模组状态", L"0x01 模组开机", L"0x02 开始配网", L"0x03 停止配网",
        L"0x04 复位/恢复出厂", L"0x05 设置节点类型", L"0x06 查询信道", L"0x07 设置信道",
        L"0x08 设置PANID", L"0x09 查询网络状态", L"0x0A 加入组播组", L"0x0B 退出组播组",
        L"0x0C 信道扫描", L"0x0D 设置发射功率", L"0x10 读取本地属性", L"0x11 设置本地属性",
        L"0x14 自动绑定", L"0x16 进入AT模式", L"0x20 获取UTC", L"0x21 设置UTC",
        L"0x22 读取节点表", L"0x23 读取密钥", L"0x28 重传设备信息"
    };
    for (int i = 0; i < 23; i++)
        SendMessageW(g.hComboHexCmd, CB_ADDSTRING, 0, (LPARAM)cmds00[i]);
    SendMessageW(g.hComboHexCmd, CB_SETCURSEL, 0, 0);

    y += 28;

    /* Data input */
    CreateWindowW(L"STATIC", L"数据(HEX):", WS_CHILD | WS_VISIBLE, 10, y + 3, 65, 18, parent, NULL, NULL, NULL);
    g.hEditHexData = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        80, y, 370, 22, parent, (HMENU)IDC_EDIT_HEX_DATA, NULL, NULL);
    SendMessageW(g.hEditHexData, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
    CreateWindowW(L"BUTTON", L"发送",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        460, y, 60, 22, parent, (HMENU)IDC_BTN_HEX_SEND, NULL, NULL);

    y += 28;

    /* Frame preview */
    g.hStaticHexFrame = CreateWindowW(L"STATIC", L"帧预览: ",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        10, y, w - 20, 18, parent, (HMENU)IDC_STATIC_HEX_FRAME, NULL, NULL);
    SendMessageW(g.hStaticHexFrame, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);

    y += 24;

    /* Raw hex input */
    CreateWindowW(L"STATIC", L"原始帧(HEX):", WS_CHILD | WS_VISIBLE, 10, y + 3, 80, 18, parent, NULL, NULL, NULL);
    g.hEditHexRaw = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        95, y, 350, 22, parent, (HMENU)IDC_EDIT_HEX_RAW, NULL, NULL);
    SendMessageW(g.hEditHexRaw, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
    CreateWindowW(L"BUTTON", L"原始发送",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        455, y, 60, 22, parent, (HMENU)IDC_BTN_HEX_RAW_SEND, NULL, NULL);

    y += 30;

    /* Response display */
    CreateWindowW(L"STATIC", L"响应/返回:", WS_CHILD | WS_VISIBLE, 10, y, 80, 18, parent, NULL, NULL, NULL);
    CreateWindowW(L"BUTTON", L"清空",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        w - 60, y, 50, 18, parent, (HMENU)IDC_BTN_HEX_CLEAR, NULL, NULL);
    y += 18;
    g.hEditHexResp = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_BORDER | WS_HSCROLL,
        10, y, w - 20, 200, parent, (HMENU)IDC_EDIT_HEX_RESP, NULL, NULL);
    SendMessageW(g.hEditHexResp, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
}

void tabCreateAtCmd(HWND parent) {
    int y = 10, w = 560;

    /* AT command input */
    CreateWindowW(L"STATIC", L"AT指令:", WS_CHILD | WS_VISIBLE, 10, y + 3, 55, 18, parent, NULL, NULL, NULL);
    g.hEditAtCmd = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        70, y, 370, 22, parent, (HMENU)IDC_EDIT_AT_CMD, NULL, NULL);
    SendMessageW(g.hEditAtCmd, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
    SetWindowTextW(g.hEditAtCmd, L"AT+");
    CreateWindowW(L"BUTTON", L"发送",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        450, y, 50, 22, parent, (HMENU)IDC_BTN_AT_SEND, NULL, NULL);

    y += 28;

    /* Quick command buttons */
    CreateWindowW(L"STATIC", L"快捷指令:", WS_CHILD | WS_VISIBLE, 10, y + 3, 60, 18, parent, NULL, NULL, NULL);

    struct { int id; const WCHAR *text; } btns[] = {
        {IDC_AT_JOIN, L"JOIN"}, {IDC_AT_LEAVE, L"LEAVE"},
        {IDC_AT_RESET, L"RESET"}, {IDC_AT_RESTORE, L"RESTORE"},
        {IDC_AT_MAC, L"MAC?"}, {IDC_AT_SHORT, L"SHORT?"},
        {IDC_AT_PANID_GET, L"PANID?"}, {IDC_AT_CHAN_GET, L"CHANNEL?"},
        {IDC_AT_BAUD_GET, L"BAUD?"}, {IDC_AT_POWER_GET, L"POWER?"},
        {IDC_AT_ROLE_GET, L"ROLE?"}, {IDC_AT_SOFT_ID, L"SOFT_ID?"},
        {IDC_AT_FIND, L"FIND"}
    };
    int btnX = 75, btnW = 55;
    for (int i = 0; i < 13; i++) {
        if (i == 7) { y += 24; btnX = 75; }
        CreateWindowW(L"BUTTON", btns[i].text,
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            btnX, y, btnW, 20, parent, (HMENU)(INT_PTR)btns[i].id, NULL, NULL);
        btnX += btnW + 5;
    }

    y += 30;

    /* Response display */
    CreateWindowW(L"STATIC", L"响应:", WS_CHILD | WS_VISIBLE, 10, y, 80, 18, parent, NULL, NULL, NULL);
    CreateWindowW(L"BUTTON", L"清空",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        w - 60, y, 50, 18, parent, (HMENU)IDC_BTN_AT_CLEAR, NULL, NULL);
    y += 18;
    g.hEditAtResp = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_BORDER,
        10, y, w - 20, 250, parent, (HMENU)IDC_EDIT_AT_RESP, NULL, NULL);
    SendMessageW(g.hEditAtResp, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
}

void tabCreateDataTransfer(HWND parent) {
    int y = 10, w = 560;

    /* Send area */
    CreateWindowW(L"STATIC", L"发送数据:", WS_CHILD | WS_VISIBLE, 10, y, 60, 18, parent, NULL, NULL, NULL);
    g.hChkHexTx = CreateWindowW(L"BUTTON", L"HEX发送",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        75, y, 70, 18, parent, (HMENU)IDC_CHK_HEX_TX, NULL, NULL);
    g.hChkNewline = CreateWindowW(L"BUTTON", L"加换行",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        150, y, 60, 18, parent, (HMENU)IDC_CHK_NEWLINE, NULL, NULL);
    g.hComboNewline = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        215, y - 2, 80, 60, parent, (HMENU)IDC_COMBO_NEWLINE, NULL, NULL);
    {
        const WCHAR *nls[] = {L"\\r\\n", L"\\n", L"\\r"};
        for (int i = 0; i < 3; i++)
            SendMessageW(g.hComboNewline, CB_ADDSTRING, 0, (LPARAM)nls[i]);
        SendMessageW(g.hComboNewline, CB_SETCURSEL, 0, 0);
    }
    g.hChkRepeat = CreateWindowW(L"BUTTON", L"定时发送(ms):",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        305, y, 100, 18, parent, (HMENU)IDC_CHK_REPEAT, NULL, NULL);
    g.hEditInterval = CreateWindowW(L"EDIT", L"1000",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        410, y - 2, 50, 20, parent, (HMENU)IDC_EDIT_INTERVAL, NULL, NULL);

    CreateWindowW(L"BUTTON", L"发送",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        470, y, 50, 22, parent, (HMENU)IDC_BTN_SEND_DATA, NULL, NULL);
    CreateWindowW(L"BUTTON", L"清空",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        525, y, 35, 22, parent, (HMENU)IDC_BTN_CLEAR_TX, NULL, NULL);

    y += 24;
    g.hEditTxData = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_BORDER | WS_HSCROLL,
        10, y, w - 20, 100, parent, (HMENU)IDC_EDIT_TX_DATA, NULL, NULL);
    SendMessageW(g.hEditTxData, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);

    y += 105;

    /* Receive area */
    CreateWindowW(L"STATIC", L"接收数据:", WS_CHILD | WS_VISIBLE, 10, y, 60, 18, parent, NULL, NULL, NULL);
    g.hChkHexRx = CreateWindowW(L"BUTTON", L"HEX显示",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        75, y, 70, 18, parent, (HMENU)IDC_CHK_HEX_RX, NULL, NULL);
    SendMessageW(g.hChkHexRx, BM_SETCHECK, BST_CHECKED, 0);
    g.hStaticRxCount = CreateWindowW(L"STATIC", L"TX:0 RX:0",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        320, y, 150, 18, parent, (HMENU)IDC_STATIC_RX_COUNT, NULL, NULL);
    CreateWindowW(L"BUTTON", L"统计重置",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        475, y, 55, 18, parent, (HMENU)IDC_STATIC_TX_COUNT, NULL, NULL);
    CreateWindowW(L"BUTTON", L"清空",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        535, y, 35, 18, parent, (HMENU)IDC_BTN_CLEAR_RX, NULL, NULL);

    y += 20;
    g.hEditRxData = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_BORDER | WS_HSCROLL,
        10, y, w - 20, 200, parent, (HMENU)IDC_EDIT_RX_DATA, NULL, NULL);
    SendMessageW(g.hEditRxData, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
}

void tabCreatePeripheral(HWND parent) {
    int y = 10, w = 560;

    /* GPIO group */
    CreateWindowW(L"BUTTON", L"GPIO 控制",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, y, w - 20, 90, parent, (HMENU)IDC_GRP_GPIO, NULL, NULL);
    CreateWindowW(L"STATIC", L"引脚:", WS_CHILD | WS_VISIBLE, 20, y + 20, 35, 18, parent, NULL, NULL, NULL);
    g.hComboGpioPin = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        55, y + 18, 55, 100, parent, (HMENU)IDC_COMBO_GPIO_PIN, NULL, NULL);
    for (int i = 0; i <= 7; i++) {
        WCHAR s[8]; wsprintfW(s, L"P0.%d", i);
        SendMessageW(g.hComboGpioPin, CB_ADDSTRING, 0, (LPARAM)s);
    }
    for (int i = 0; i <= 7; i++) {
        WCHAR s[8]; wsprintfW(s, L"P1.%d", i);
        SendMessageW(g.hComboGpioPin, CB_ADDSTRING, 0, (LPARAM)s);
    }
    for (int i = 0; i <= 2; i++) {
        WCHAR s[8]; wsprintfW(s, L"P2.%d", i);
        SendMessageW(g.hComboGpioPin, CB_ADDSTRING, 0, (LPARAM)s);
    }
    SendMessageW(g.hComboGpioPin, CB_SETCURSEL, 0, 0);

    CreateWindowW(L"STATIC", L"方向:", WS_CHILD | WS_VISIBLE, 120, y + 20, 35, 18, parent, NULL, NULL, NULL);
    g.hComboGpioDir = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        155, y + 18, 60, 60, parent, (HMENU)IDC_COMBO_GPIO_DIR, NULL, NULL);
    SendMessageW(g.hComboGpioDir, CB_ADDSTRING, 0, (LPARAM)L"输入");
    SendMessageW(g.hComboGpioDir, CB_ADDSTRING, 0, (LPARAM)L"输出");
    SendMessageW(g.hComboGpioDir, CB_SETCURSEL, 1, 0);

    CreateWindowW(L"STATIC", L"电平:", WS_CHILD | WS_VISIBLE, 225, y + 20, 35, 18, parent, NULL, NULL, NULL);
    g.hComboGpioLvl = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        260, y + 18, 55, 60, parent, (HMENU)IDC_COMBO_GPIO_LVL, NULL, NULL);
    SendMessageW(g.hComboGpioLvl, CB_ADDSTRING, 0, (LPARAM)L"低");
    SendMessageW(g.hComboGpioLvl, CB_ADDSTRING, 0, (LPARAM)L"高");
    SendMessageW(g.hComboGpioLvl, CB_SETCURSEL, 0, 0);

    CreateWindowW(L"BUTTON", L"设置",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        325, y + 17, 50, 20, parent, (HMENU)IDC_BTN_GPIO_SET, NULL, NULL);
    CreateWindowW(L"BUTTON", L"读取",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        380, y + 17, 50, 20, parent, (HMENU)IDC_BTN_GPIO_READ, NULL, NULL);

    y += 95;

    /* ADC group */
    CreateWindowW(L"BUTTON", L"ADC 读取",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, y, w - 20, 55, parent, (HMENU)IDC_GRP_ADC, NULL, NULL);
    CreateWindowW(L"STATIC", L"通道:", WS_CHILD | WS_VISIBLE, 20, y + 22, 35, 18, parent, NULL, NULL, NULL);
    g.hComboAdcCh = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        55, y + 20, 55, 100, parent, (HMENU)IDC_COMBO_ADC_CH, NULL, NULL);
    for (int i = 0; i <= 6; i++) {
        WCHAR s[8]; wsprintfW(s, L"CH%d", i);
        SendMessageW(g.hComboAdcCh, CB_ADDSTRING, 0, (LPARAM)s);
    }
    SendMessageW(g.hComboAdcCh, CB_SETCURSEL, 0, 0);
    CreateWindowW(L"BUTTON", L"读取ADC",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, y + 18, 70, 22, parent, (HMENU)IDC_BTN_ADC_READ, NULL, NULL);
    CreateWindowW(L"STATIC", L"值(mV):", WS_CHILD | WS_VISIBLE, 200, y + 22, 45, 18, parent, NULL, NULL, NULL);
    g.hEditAdcVal = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
        245, y + 20, 70, 20, parent, (HMENU)IDC_EDIT_ADC_VAL, NULL, NULL);

    y += 60;

    /* PWM group */
    CreateWindowW(L"BUTTON", L"PWM 输出 (仅终端设备)",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, y, w - 20, 80, parent, (HMENU)IDC_GRP_PWM, NULL, NULL);
    CreateWindowW(L"STATIC", L"周期(us):", WS_CHILD | WS_VISIBLE, 20, y + 22, 55, 18, parent, NULL, NULL, NULL);
    g.hEditPwmPeriod = CreateWindowW(L"EDIT", L"1000",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        80, y + 20, 60, 20, parent, (HMENU)IDC_EDIT_PWM_PERIOD, NULL, NULL);

    WCHAR *labels[] = {L"D1:", L"D2:", L"D3:", L"D4:", L"D5:"};
    for (int i = 0; i < 5; i++) {
        CreateWindowW(L"STATIC", labels[i], WS_CHILD | WS_VISIBLE,
            150 + i * 75, y + 22, 25, 18, parent, NULL, NULL, NULL);
        g.hEditPwmD[i] = CreateWindowW(L"EDIT", L"0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            175 + i * 75, y + 20, 45, 20, parent, (HMENU)(INT_PTR)(IDC_EDIT_PWM_D1 + i), NULL, NULL);
    }
    CreateWindowW(L"BUTTON", L"设置PWM",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, y + 48, 80, 22, parent, (HMENU)IDC_BTN_PWM_SET, NULL, NULL);
}

void tabCreateNetworkMgmt(HWND parent) {
    int y = 10, w = 560;

    /* Node list */
    CreateWindowW(L"STATIC", L"已入网节点:", WS_CHILD | WS_VISIBLE, 10, y, 80, 18, parent, NULL, NULL, NULL);
    CreateWindowW(L"BUTTON", L"设备发现",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        w - 80, y - 2, 70, 22, parent, (HMENU)IDC_BTN_DISCOVER, NULL, NULL);
    y += 18;
    g.hListNodes = CreateWindowW(L"SysListView32", L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER | LVS_SINGLESEL,
        10, y, w - 20, 120, parent, (HMENU)IDC_LIST_NODES, NULL, NULL);
    {
        LVCOLUMNW col = {0};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        WCHAR *headers[] = {L"短地址", L"MAC地址", L"设备类型", L"端点"};
        int widths[] = {80, 140, 80, 50};
        for (int i = 0; i < 4; i++) {
            col.pszText = headers[i];
            col.cx = widths[i];
            SendMessageW(g.hListNodes, LVM_INSERTCOLUMNW, i, (LPARAM)&col);
        }
        DWORD style = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
        SendMessageW(g.hListNodes, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, style);
    }

    y += 125;

    /* Binding group */
    CreateWindowW(L"BUTTON", L"绑定管理",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, y, w - 20, 120, parent, (HMENU)IDC_GRP_BINDING, NULL, NULL);

    CreateWindowW(L"STATIC", L"源短地址:", WS_CHILD | WS_VISIBLE, 20, y + 22, 60, 18, parent, NULL, NULL, NULL);
    g.hEditBindSrc = CreateWindowW(L"EDIT", L"0000",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        85, y + 20, 55, 20, parent, (HMENU)IDC_EDIT_BIND_SRC, NULL, NULL);
    SendMessageW(g.hEditBindSrc, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);

    CreateWindowW(L"STATIC", L"目标地址:", WS_CHILD | WS_VISIBLE, 150, y + 22, 60, 18, parent, NULL, NULL, NULL);
    g.hEditBindDst = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        215, y + 20, 55, 20, parent, (HMENU)IDC_EDIT_BIND_DST, NULL, NULL);
    SendMessageW(g.hEditBindDst, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);

    CreateWindowW(L"STATIC", L"Cluster:", WS_CHILD | WS_VISIBLE, 280, y + 22, 45, 18, parent, NULL, NULL, NULL);
    g.hEditBindClust = CreateWindowW(L"EDIT", L"FC08",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        330, y + 20, 55, 20, parent, (HMENU)IDC_EDIT_BIND_CLUST, NULL, NULL);
    SendMessageW(g.hEditBindClust, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);

    CreateWindowW(L"BUTTON", L"绑定",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        395, y + 18, 50, 22, parent, (HMENU)IDC_BTN_BIND_SET, NULL, NULL);
    CreateWindowW(L"BUTTON", L"查询",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        450, y + 18, 50, 22, parent, (HMENU)IDC_BTN_BIND_QRY, NULL, NULL);
    CreateWindowW(L"BUTTON", L"解绑",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        505, y + 18, 50, 22, parent, (HMENU)IDC_BTN_BIND_DEL, NULL, NULL);

    /* Binding list */
    g.hListBindings = CreateWindowW(L"SysListView32", L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER | LVS_SINGLESEL,
        20, y + 45, w - 40, 65, parent, (HMENU)IDC_LIST_BINDINGS, NULL, NULL);
    {
        LVCOLUMNW col = {0};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        WCHAR *headers[] = {L"源地址", L"目标地址", L"Cluster", L"源EP", L"目标EP"};
        int widths[] = {70, 70, 70, 50, 50};
        for (int i = 0; i < 5; i++) {
            col.pszText = headers[i];
            col.cx = widths[i];
            SendMessageW(g.hListBindings, LVM_INSERTCOLUMNW, i, (LPARAM)&col);
        }
        DWORD style = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
        SendMessageW(g.hListBindings, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, style);
    }

    y += 130;

    /* Group management group */
    CreateWindowW(L"BUTTON", L"组播管理",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, y, w - 20, 55, parent, (HMENU)IDC_GRP_GROUP, NULL, NULL);
    CreateWindowW(L"STATIC", L"组ID:", WS_CHILD | WS_VISIBLE, 20, y + 22, 35, 18, parent, NULL, NULL, NULL);
    g.hEditGroupId = CreateWindowW(L"EDIT", L"0001",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        55, y + 20, 55, 20, parent, (HMENU)IDC_EDIT_GROUP_ID, NULL, NULL);
    SendMessageW(g.hEditGroupId, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
    CreateWindowW(L"BUTTON", L"加入组",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, y + 18, 60, 22, parent, (HMENU)IDC_BTN_JOIN_GROUP, NULL, NULL);
    CreateWindowW(L"BUTTON", L"退出组",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        185, y + 18, 60, 22, parent, (HMENU)IDC_BTN_LEAVE_GROUP, NULL, NULL);
}

void tabShowPanel(int index) {
    for (int i = 0; i < 6; i++) {
        if (g_hPanels[i]) {
            ShowWindow(g_hPanels[i], (i == index) ? SW_SHOW : SW_HIDE);
        }
    }
    if (g_hPanels[index]) {
        InvalidateRect(g_hPanels[index], NULL, TRUE);
        UpdateWindow(g_hPanels[index]);
    }
}

/* ========== HEX Command Execution ========== */

static void hexSendCommand(unsigned char cmdType, unsigned char cmdCode,
                           const unsigned char *data, int dataLen) {
    unsigned char frame[HEX_FRAME_MAX];
    int frmLen = hexBuildFrame(frame, sizeof(frame), cmdType, cmdCode, data, dataLen);
    if (frmLen <= 0) {
        logAdd(L"[错误] 帧构建失败");
        return;
    }
    logAddHex(L"[HEX发送]", frame, frmLen);
    int sent = serialSend(frame, frmLen);
    if (sent != frmLen) {
        logAdd(L"[错误] 发送失败, 期望=%d, 实际=%d", frmLen, sent);
    }
}

static void hexSendLocalCmd(unsigned char cmdCode, const unsigned char *data, int dataLen) {
    hexSendCommand(0x00, cmdCode, data, dataLen);
}

static void hexSendNetworkCmd(unsigned char cmdCode, const unsigned char *data, int dataLen) {
    hexSendCommand(0x01, cmdCode, data, dataLen);
}

static void hexSendZclCmd(unsigned char cmdCode, const unsigned char *data, int dataLen) {
    hexSendCommand(0x02, cmdCode, data, dataLen);
}

/* ========== AT Command Execution ========== */

static void atSendCommand(const WCHAR *cmd) {
    if (!g.connected) {
        logAdd(L"[错误] 串口未连接");
        return;
    }
    char buf[512];
    int len = WideCharToMultiByte(CP_UTF8, 0, cmd, -1, buf, sizeof(buf) - 2, NULL, NULL);
    if (len <= 0) return;
    buf[len] = '\r';
    buf[len + 1] = '\n';
    logAdd(L"[AT发送] %s", cmd);
    serialSend((unsigned char*)buf, len + 2);
}

/* ========== Main Window Procedure ========== */

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        return 0;

    case WM_APP: {
        /* Serial data received - process it */
        unsigned char buf[4096];
        int count = serialRecv(buf, sizeof(buf));
        if (count > 0) {
            unsigned char rawBytes[4096];
            int rawLen = 0;
            /* Try to parse HEX frames */
            for (int i = 0; i < count; i++) {
                unsigned char b = buf[i];
                if (g.hexFrameState == 0) {
                    if (b == 0x55) {
                        g.hexFrame[0] = b;
                        g.hexFrameLen = 1;
                        g.hexFrameState = 1;
                    } else {
                        rawBytes[rawLen++] = b;
                    }
                } else if (g.hexFrameState == 1) {
                    /* Got head, this is frame length */
                    g.hexFrame[1] = b;
                    g.hexFrameLen = 2;
                    if (b < 3 || b > 254) {
                        /* Invalid length, reset */
                        rawBytes[rawLen++] = 0x55;
                        rawBytes[rawLen++] = b;
                        g.hexFrameState = 0;
                    } else {
                        g.hexFrameState = 2;
                    }
                } else {
                    g.hexFrame[g.hexFrameLen++] = b;
                    int expectedLen = g.hexFrame[1] + 2; /* head + len + payload */
                    if (g.hexFrameLen >= expectedLen) {
                        if (hexValidateFrame(g.hexFrame, g.hexFrameLen)) {
                            logAddHex(L"[HEX接收]", g.hexFrame, g.hexFrameLen);
                            hexParseResponse(g.hexFrame, g.hexFrameLen);
                            hexParseAsync(g.hexFrame, g.hexFrameLen);
                            /* Echo response to HEX tab */
                            WCHAR frameStr[1024];
                            hexFormatFrame(frameStr, 1024, g.hexFrame, g.hexFrameLen);
                            uiAppendEdit(g.hEditHexResp, frameStr);
                            uiAppendEdit(g.hEditHexResp, L"\r\n");
                        } else {
                            for (int j = 0; j < g.hexFrameLen; j++) {
                                rawBytes[rawLen++] = g.hexFrame[j];
                            }
                            logAddHex(L"[HEX校验失败,转为裸数据]", g.hexFrame, g.hexFrameLen);
                        }
                        g.hexFrameState = 0;
                        g.hexFrameLen = 0;
                    }
                }
            }
            if (rawLen > 0) {
                g.rxCount += rawLen;
                uiUpdateDataCounters();
                if (g.hEditRxData) {
                    if (SendMessageW(g.hChkHexRx, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        uiAppendEditHex(g.hEditRxData, rawBytes, rawLen);
                        uiAppendEdit(g.hEditRxData, L" ");
                    } else {
                        WCHAR utf16[4096];
                        int wlen = MultiByteToWideChar(CP_UTF8, 0, (char*)rawBytes, rawLen, utf16, 4095);
                        utf16[wlen] = 0;
                        uiAppendEdit(g.hEditRxData, utf16);
                    }
                }
                if (g.hEditAtResp) {
                    int isAt = 0;
                    for (int j = 0; j < rawLen; j++) {
                        if (rawBytes[j] == '+' || rawBytes[j] == 'O' || rawBytes[j] == 'E' || rawBytes[j] == '\r' || rawBytes[j] == '\n') { isAt = 1; break; }
                    }
                    if (isAt) {
                        WCHAR utf16[4096];
                        int wlen = MultiByteToWideChar(CP_UTF8, 0, (char*)rawBytes, rawLen, utf16, 4095);
                        utf16[wlen] = 0;
                        uiAppendEdit(g.hEditAtResp, utf16);
                    }
                }
            }
        }
        return 0;
    }

    case WM_NOTIFY: {
        NMHDR *nm = (NMHDR*)lp;
        if (nm->idFrom == IDC_TAB && nm->code == TCN_SELCHANGE) {
            int idx = SendMessageW(g.hTab, TCM_GETCURSEL, 0, 0);
            tabShowPanel(idx);
        }
        return 0;
    }

    case WM_COMMAND: {
        WORD id = LOWORD(wp);
        WORD code = HIWORD(wp);

        /* Serial bar */
        if (id == IDC_BTN_OPEN) {
            if (!g.connected) {
                WCHAR path[16];
                int idx = SendMessageW(g.hComboPort, CB_GETCURSEL, 0, 0);
                if (idx < 0) { logAdd(L"[错误] 请选择串口"); break; }
                SendMessageW(g.hComboPort, CB_GETLBTEXT, idx, (LPARAM)g.portName);
                wsprintfW(path, L"\\\\.\\%s", g.portName);
                int baudIdx = SendMessageW(g.hComboBaud, CB_GETCURSEL, 0, 0);
                if (baudIdx < 0) baudIdx = 3; /* default 115200 */
                WCHAR baudStr[16];
                SendMessageW(g.hComboBaud, CB_GETLBTEXT, baudIdx, (LPARAM)baudStr);
                int baud = _wtoi(baudStr);
                if (serialOpen(path, baud)) {
                    uiSetControlText(g.hBtnOpen, L"关闭");
                    uiUpdateStatus();
                    uiEnableControls(TRUE);
                    logAdd(L"[串口] 已打开 %s, 波特率 %d", g.portName, baud);
                    /* Start read thread */
                    CreateThread(NULL, 0, serialReadThread, NULL, 0, NULL);
                } else {
                    logAdd(L"[错误] 无法打开 %s", g.portName);
                }
            } else {
                serialClose();
                uiSetControlText(g.hBtnOpen, L"打开");
                uiUpdateStatus();
                uiEnableControls(FALSE);
                logAdd(L"[串口] 已关闭");
            }
        } else if (id == IDC_BTN_REFRESH) {
            ComPortInfo ports[32];
            int count = serialEnum(ports, 32);
            SendMessageW(g.hComboPort, CB_RESETCONTENT, 0, 0);
            for (int i = 0; i < count; i++)
                SendMessageW(g.hComboPort, CB_ADDSTRING, 0, (LPARAM)ports[i].name);
            if (count > 0) SendMessageW(g.hComboPort, CB_SETCURSEL, 0, 0);
            logAdd(L"[串口] 找到 %d 个串口", count);
        }
        /* ===== Tab 0: Basic Config ===== */
        else if (id == IDC_BTN_QRY_STATUS) {
            hexSendLocalCmd(0x00, NULL, 0);
        } else if (id == IDC_BTN_SET_NODE) {
            int idx = SendMessageW(g.hComboNodeType, CB_GETCURSEL, 0, 0);
            unsigned char type = (unsigned char)(idx + 1);
            hexSendLocalCmd(0x05, &type, 1);
            logAdd(L"[配置] 设置节点类型: %s", hexGetNodeTypeName(type));
        } else if (id == IDC_BTN_SET_CHAN) {
            int idx = SendMessageW(g.hComboChannel, CB_GETCURSEL, 0, 0);
            unsigned char chan = (unsigned char)(idx + 11);
            hexSendLocalCmd(0x07, &chan, 1);
            logAdd(L"[配置] 设置信道: %d", chan);
        } else if (id == IDC_BTN_SET_PANID) {
            WCHAR buf[16];
            GetWindowTextW(g.hEditPanId, buf, 16);
            unsigned int panid;
            if (swscanf(buf, L"%x", &panid) != 1) panid = _wtoi(buf);
            unsigned char data[2] = {(unsigned char)(panid & 0xFF), (unsigned char)((panid >> 8) & 0xFF)};
            hexSendLocalCmd(0x08, data, 2);
            logAdd(L"[配置] 设置PANID: 0x%04X", panid);
        } else if (id == IDC_BTN_SET_TXPWR) {
            int idx = SendMessageW(g.hComboTxPower, CB_GETCURSEL, 0, 0);
            /* Power values: 4,3,2,1,0,-1,-2,-3,-4,-6,-8,-10,-12,-14,-16,-18,-20 dBm */
            int pwrs[] = {4,3,2,1,0,-1,-2,-3,-4,-6,-8,-10,-12,-14,-16,-18,-20};
            unsigned char pwr = (unsigned char)idx;
            hexSendLocalCmd(0x0D, &pwr, 1);
            logAdd(L"[配置] 设置发射功率: %d dBm", pwrs[idx]);
        } else if (id == IDC_BTN_START_NWK) {
            hexSendLocalCmd(0x02, NULL, 0);
            logAdd(L"[网络] 开始配网");
        } else if (id == IDC_BTN_STOP_NWK) {
            hexSendLocalCmd(0x03, NULL, 0);
            logAdd(L"[网络] 停止配网");
        } else if (id == IDC_BTN_RESET) {
            hexSendLocalCmd(0x04, NULL, 0);
            logAdd(L"[系统] 软复位");
        } else if (id == IDC_BTN_RESTORE) {
            unsigned char param = 0x02; /* 0x02 = factory reset */
            hexSendLocalCmd(0x04, &param, 1);
            logAdd(L"[系统] 恢复出厂设置");
        } else if (id == IDC_BTN_BIND) {
            /* Auto bind: no parameters */
            hexSendLocalCmd(0x14, NULL, 0);
            logAdd(L"[绑定] 一键绑定");
        } else if (id == IDC_BTN_GET_UTC) {
            hexSendLocalCmd(0x20, NULL, 0);
            logAdd(L"[时间] 请求UTC时间");
        } else if (id == IDC_BTN_SET_UTC) {
            /* Send current system time as UTC timestamp */
            time_t now = time(NULL);
            unsigned char data[4];
            data[0] = (unsigned char)(now & 0xFF);
            data[1] = (unsigned char)((now >> 8) & 0xFF);
            data[2] = (unsigned char)((now >> 16) & 0xFF);
            data[3] = (unsigned char)((now >> 24) & 0xFF);
            hexSendLocalCmd(0x21, data, 4);
            logAdd(L"[时间] 设置UTC: %lld", (long long)now);
        } else if (id == IDC_BTN_GET_KEY) {
            hexSendLocalCmd(0x23, NULL, 0);
            logAdd(L"[配置] 读取密钥");
        } else if (id == IDC_BTN_GET_NODES) {
            hexSendLocalCmd(0x22, NULL, 0);
            logAdd(L"[配置] 读取节点表");
        } else if (id == IDC_BTN_RETX_INFO) {
            hexSendLocalCmd(0x28, NULL, 0);
            logAdd(L"[配置] 重传设备信息");
        } else if (id == IDC_BTN_GET_NODE || id == IDC_BTN_GET_CHAN || id == IDC_BTN_GET_PANID) {
            hexSendLocalCmd(0x00, NULL, 0);
            logAdd(L"[配置] 请求读取设备参数");
        } else if (id == IDC_BTN_GET_TXPWR) {
            unsigned char mode = 0x00; /* Query */
            hexSendLocalCmd(0x0D, &mode, 1);
            logAdd(L"[配置] 请求读取发射功率");
        }
        /* ===== Tab 1: HEX Commands ===== */
        else if (id == IDC_BTN_HEX_SEND) {
            int typeIdx = SendMessageW(g.hComboHexType, CB_GETCURSEL, 0, 0);
            int cmdIdx = SendMessageW(g.hComboHexCmd, CB_GETCURSEL, 0, 0);
            if (typeIdx < 0 || cmdIdx < 0) { logAdd(L"[错误] 请选择命令类型和命令码"); break; }
            unsigned char cmdType = (unsigned char)typeIdx;
            unsigned char cmdCode;
            /* Map combobox index to actual command code */
            if (typeIdx == 0) { /* Local config */
                unsigned char codes[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                    0x0A,0x0B,0x0C,0x0D,0x10,0x11,0x14,0x16,0x20,0x21,0x22,0x23,0x28};
                cmdCode = (cmdIdx < 23) ? codes[cmdIdx] : 0x00;
            } else if (typeIdx == 1) {
                unsigned char codes01[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x21, 0x22, 0x33, 0x34};
                cmdCode = (cmdIdx < 9) ? codes01[cmdIdx] : 0x00;
            } else {
                unsigned char codes02[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x0F};
                cmdCode = (cmdIdx < 9) ? codes02[cmdIdx] : 0x00;
            }

            WCHAR hexStr[512];
            GetWindowTextW(g.hEditHexData, hexStr, 512);
            unsigned char data[256];
            int dataLen = parseHexString(hexStr, data, sizeof(data));
            if (cmdType == 0)
                hexSendLocalCmd(cmdCode, data, dataLen);
            else if (cmdType == 1)
                hexSendNetworkCmd(cmdCode, data, dataLen);
            else
                hexSendZclCmd(cmdCode, data, dataLen);
        } else if (id == IDC_BTN_HEX_RAW_SEND) {
            WCHAR hexStr[1024];
            GetWindowTextW(g.hEditHexRaw, hexStr, 1024);
            unsigned char data[256];
            int dataLen = parseHexString(hexStr, data, sizeof(data));
            if (dataLen > 0) {
                logAddHex(L"[原始发送]", data, dataLen);
                serialSend(data, dataLen);
            }
        } else if (id == IDC_BTN_HEX_CLEAR) {
            SetWindowTextW(g.hEditHexResp, L"");
        } else if (id == IDC_COMBO_HEX_TYPE && code == CBN_SELCHANGE) {
            /* Update command combobox based on type selection */
            int typeIdx = SendMessageW(g.hComboHexType, CB_GETCURSEL, 0, 0);
            SendMessageW(g.hComboHexCmd, CB_RESETCONTENT, 0, 0);
            if (typeIdx == 0) {
                const WCHAR *cmds[] = {
                    L"0x00 查询状态", L"0x01 开机/软启动", L"0x02 开始配网", L"0x03 停止配网",
                    L"0x04 复位/恢复出厂", L"0x05 设置节点类型", L"0x06 查询信道", L"0x07 设置信道",
                    L"0x08 设置PANID", L"0x09 查询网络状态", L"0x0A 加入组播", L"0x0B 退出组播",
                    L"0x0C 信道扫描", L"0x0D 设置发射功率", L"0x10 读本地属性", L"0x11 写本地属性",
                    L"0x14 自动绑定", L"0x16 进入AT模式", L"0x20 获取UTC", L"0x21 设置UTC",
                    L"0x22 读节点表", L"0x23 读密钥", L"0x28 重传设备信息"
                };
                for (int i = 0; i < 23; i++)
                    SendMessageW(g.hComboHexCmd, CB_ADDSTRING, 0, (LPARAM)cmds[i]);
            } else if (typeIdx == 1) {
                const WCHAR *cmds[] = {
                    L"0x00 ZDO网络地址", L"0x01 ZDO IEEE地址", L"0x02 ZDO节点描述",
                    L"0x03 ZDO简单描述", L"0x04 ZDO活动端点", L"0x21 ZDO绑定",
                    L"0x22 ZDO解绑", L"0x33 ZDO绑定表", L"0x34 ZDO离网"
                };
                for (int i = 0; i < 9; i++)
                    SendMessageW(g.hComboHexCmd, CB_ADDSTRING, 0, (LPARAM)cmds[i]);
            } else {
                const WCHAR *cmds[] = {
                    L"0x00 ZCL读属性", L"0x01 ZCL写属性", L"0x02 ZCL读上报配置",
                    L"0x03 ZCL写上报配置", L"0x04 ZCL发现属性", L"0x05 ZCL发现扩展属性",
                    L"0x06 ZCL发现接收命令", L"0x07 ZCL发现生成命令", L"0x0F ZCL命令/透传"
                };
                for (int i = 0; i < 9; i++)
                    SendMessageW(g.hComboHexCmd, CB_ADDSTRING, 0, (LPARAM)cmds[i]);
            }
            SendMessageW(g.hComboHexCmd, CB_SETCURSEL, 0, 0);
        }
        /* ===== Tab 2: AT Commands ===== */
        else if (id == IDC_BTN_AT_SEND) {
            WCHAR cmd[256];
            GetWindowTextW(g.hEditAtCmd, cmd, 256);
            if (wcslen(cmd) > 0) {
                /* Check if switching to AT mode needed */
                atSendCommand(cmd);
            }
        } else if (id == IDC_AT_JOIN) atSendCommand(L"AT+JOIN");
        else if (id == IDC_AT_LEAVE) atSendCommand(L"AT+LEAVE");
        else if (id == IDC_AT_RESET) atSendCommand(L"AT+RESET");
        else if (id == IDC_AT_RESTORE) atSendCommand(L"AT+RESTORE");
        else if (id == IDC_AT_MAC) atSendCommand(L"AT+MAC=?");
        else if (id == IDC_AT_SHORT) atSendCommand(L"AT+SHORT=?");
        else if (id == IDC_AT_PANID_GET) atSendCommand(L"AT+PANID=?");
        else if (id == IDC_AT_CHAN_GET) atSendCommand(L"AT+CHANNEL=?");
        else if (id == IDC_AT_BAUD_GET) atSendCommand(L"AT+BAUD=?");
        else if (id == IDC_AT_POWER_GET) atSendCommand(L"AT+POWER=?");
        else if (id == IDC_AT_ROLE_GET) atSendCommand(L"AT+ROLE=?");
        else if (id == IDC_AT_SOFT_ID) atSendCommand(L"AT+SOFT_ID=?");
        else if (id == IDC_AT_FIND) atSendCommand(L"AT+FIND");
        else if (id == IDC_BTN_AT_CLEAR) {
            SetWindowTextW(g.hEditAtResp, L"");
        }
        /* ===== Tab 3: Data Transfer ===== */
        else if (id == IDC_BTN_SEND_DATA) {
            WCHAR text[4096];
            GetWindowTextW(g.hEditTxData, text, 4096);
            int textLen = wcslen(text);
            if (textLen == 0) break;

            int hexMode = (SendMessageW(g.hChkHexTx, BM_GETCHECK, 0, 0) == BST_CHECKED);
            int addNewline = (SendMessageW(g.hChkNewline, BM_GETCHECK, 0, 0) == BST_CHECKED);

            if (hexMode) {
                unsigned char data[2048];
                int len = parseHexString(text, data, sizeof(data));
                if (len > 0) {
                    serialSend(data, len);
                    g.txCount += len;
                    uiUpdateDataCounters();
                    logAddHex(L"[数据发送 HEX]", data, len);
                }
            } else {
                char utf8[4096];
                int len = WideCharToMultiByte(CP_UTF8, 0, text, textLen, utf8, sizeof(utf8) - 4, NULL, NULL);
                if (addNewline) {
                    int nlIdx = SendMessageW(g.hComboNewline, CB_GETCURSEL, 0, 0);
                    if (nlIdx == 0) { utf8[len] = '\r'; utf8[len+1] = '\n'; len += 2; }
                    else if (nlIdx == 1) { utf8[len] = '\n'; len += 1; }
                    else { utf8[len] = '\r'; len += 1; }
                }
                serialSend((unsigned char*)utf8, len);
                g.txCount += len;
                uiUpdateDataCounters();
                logAddHex(L"[数据发送 TXT]", (unsigned char*)utf8, len);
            }

            /* Handle repeat send */
            int repeat = (SendMessageW(g.hChkRepeat, BM_GETCHECK, 0, 0) == BST_CHECKED);
            if (repeat && !g.repeatActive) {
                int interval = uiGetControlInt(g.hEditInterval);
                if (interval < 50) interval = 50;
                g.repeatActive = 1;
                SetTimer(g.hMainWnd, IDT_REPEAT_SEND, interval, NULL);
            } else if (!repeat && g.repeatActive) {
                g.repeatActive = 0;
                KillTimer(g.hMainWnd, IDT_REPEAT_SEND);
            }
        } else if (id == IDC_BTN_CLEAR_TX) {
            SetWindowTextW(g.hEditTxData, L"");
        } else if (id == IDC_BTN_CLEAR_RX) {
            SetWindowTextW(g.hEditRxData, L"");
            g.rxCount = 0;
        } else if (id == IDC_STATIC_TX_COUNT) {
            g.txCount = g.rxCount = 0;
            uiUpdateDataCounters();
            logAdd(L"[统计] 计数器已重置");
        }
        /* ===== Tab 4: Peripheral ===== */
        else if (id == IDC_BTN_GPIO_SET) {
            int pinIdx = SendMessageW(g.hComboGpioPin, CB_GETCURSEL, 0, 0);
            int dirIdx = SendMessageW(g.hComboGpioDir, CB_GETCURSEL, 0, 0);
            int lvlIdx = SendMessageW(g.hComboGpioLvl, CB_GETCURSEL, 0, 0);
            /* Extract port and pin number */
            int port = pinIdx / 8;
            int pin = pinIdx % 8;
            WCHAR atCmd[64];
            logAdd(L"[提示] 外设控制依赖AT指令，如果无响应请确保模块已通过 0x16 命令进入AT模式");
            wsprintfW(atCmd, L"AT+GPIO_PUT=%d,%d,%d", port, pin, (dirIdx == 1) ? 1 : 0);
            atSendCommand(atCmd);
            if (dirIdx == 1) { /* output */
                WCHAR atLvl[64];
                wsprintfW(atLvl, L"AT+GPIO_LEVEL=%d,%d,%d", port, pin, lvlIdx);
                atSendCommand(atLvl);
            }
        } else if (id == IDC_BTN_GPIO_READ) {
            /* Use AT+GPIO_LEVEL to read */
            int pinIdx = SendMessageW(g.hComboGpioPin, CB_GETCURSEL, 0, 0);
            int port = pinIdx / 8;
            int pin = pinIdx % 8;
            WCHAR atCmd[64];
            logAdd(L"[提示] 外设控制依赖AT指令，如果无响应请确保模块已通过 0x16 命令进入AT模式");
            wsprintfW(atCmd, L"AT+GPIO_LEVEL=%d,%d,?", port, pin);
            atSendCommand(atCmd);
        } else if (id == IDC_BTN_ADC_READ) {
            int chIdx = SendMessageW(g.hComboAdcCh, CB_GETCURSEL, 0, 0);
            WCHAR atCmd[64];
            logAdd(L"[提示] 外设控制依赖AT指令，如果无响应请确保模块已通过 0x16 命令进入AT模式");
            wsprintfW(atCmd, L"AT+ADC=0,%d", chIdx);
            atSendCommand(atCmd);
        } else if (id == IDC_BTN_PWM_SET) {
            int period = uiGetControlInt(g.hEditPwmPeriod);
            int d1 = uiGetControlInt(g.hEditPwmD[0]);
            int d2 = uiGetControlInt(g.hEditPwmD[1]);
            int d3 = uiGetControlInt(g.hEditPwmD[2]);
            int d4 = uiGetControlInt(g.hEditPwmD[3]);
            int d5 = uiGetControlInt(g.hEditPwmD[4]);
            WCHAR atCmd[128];
            logAdd(L"[提示] 外设控制依赖AT指令，如果无响应请确保模块已通过 0x16 命令进入AT模式");
            wsprintfW(atCmd, L"AT+PWM=0,%d,%d,%d,%d,%d,%d", period, d1, d2, d3, d4, d5);
            atSendCommand(atCmd);
        }
        /* ===== Tab 5: Network Management ===== */
        else if (id == IDC_BTN_DISCOVER) {
            /* Send device discovery via network management */
            unsigned char data[2] = {0xFF, 0xFF}; /* Broadcast */
            hexSendNetworkCmd(0x00, data, 2);
            logAdd(L"[网络] 设备发现 (广播)");
        } else if (id == IDC_BTN_BIND_SET) {
            WCHAR src[16], dst[16], clust[16];
            GetWindowTextW(g.hEditBindSrc, src, 16);
            GetWindowTextW(g.hEditBindDst, dst, 16);
            GetWindowTextW(g.hEditBindClust, clust, 16);
            unsigned int uSrc = 0, uDst = 0, uClust = 0xFC08;
            swscanf(src, L"%x", &uSrc);
            swscanf(dst, L"%x", &uDst);
            swscanf(clust, L"%x", &uClust);
            unsigned char data[8] = {
                (unsigned char)(uSrc & 0xFF), (unsigned char)(uSrc >> 8),
                (unsigned char)(uDst & 0xFF), (unsigned char)(uDst >> 8),
                0x01, 0x01, /* srcEP=1, dstEP=1 */
                (unsigned char)(uClust & 0xFF), (unsigned char)(uClust >> 8)
            };
            hexSendNetworkCmd(0x21, data, 8);
            logAdd(L"[绑定] 设置绑定 0x%04X -> 0x%04X, Cluster=0x%04X", uSrc, uDst, uClust);
        } else if (id == IDC_BTN_BIND_QRY) {
            WCHAR src[16];
            GetWindowTextW(g.hEditBindSrc, src, 16);
            unsigned int uSrc = 0;
            swscanf(src, L"%x", &uSrc);
            unsigned char data[2] = {(unsigned char)(uSrc & 0xFF), (unsigned char)(uSrc >> 8)};
            hexSendNetworkCmd(0x33, data, 2);
            logAdd(L"[绑定] 查询绑定: 0x%04X", uSrc);
        } else if (id == IDC_BTN_BIND_DEL) {
            WCHAR src[16], dst[16], clust[16];
            GetWindowTextW(g.hEditBindSrc, src, 16);
            GetWindowTextW(g.hEditBindDst, dst, 16);
            GetWindowTextW(g.hEditBindClust, clust, 16);
            unsigned int uSrc = 0, uDst = 0, uClust = 0xFC08;
            swscanf(src, L"%x", &uSrc);
            swscanf(dst, L"%x", &uDst);
            swscanf(clust, L"%x", &uClust);
            unsigned char data[8] = {
                (unsigned char)(uSrc & 0xFF), (unsigned char)(uSrc >> 8),
                (unsigned char)(uDst & 0xFF), (unsigned char)(uDst >> 8),
                0x01, 0x01,
                (unsigned char)(uClust & 0xFF), (unsigned char)(uClust >> 8)
            };
            hexSendNetworkCmd(0x22, data, 8);
            logAdd(L"[绑定] 解除绑定 0x%04X -> 0x%04X", uSrc, uDst);
        } else if (id == IDC_BTN_JOIN_GROUP) {
            WCHAR gid[16];
            GetWindowTextW(g.hEditGroupId, gid, 16);
            unsigned int ugid = 0;
            swscanf(gid, L"%x", &ugid);
            unsigned char data[2] = {(unsigned char)(ugid & 0xFF), (unsigned char)(ugid >> 8)};
            hexSendLocalCmd(0x0A, data, 2);
            logAdd(L"[组播] 加入组 0x%04X", ugid);
        } else if (id == IDC_BTN_LEAVE_GROUP) {
            WCHAR gid[16];
            GetWindowTextW(g.hEditGroupId, gid, 16);
            unsigned int ugid = 0;
            swscanf(gid, L"%x", &ugid);
            unsigned char data[2] = {(unsigned char)(ugid & 0xFF), (unsigned char)(ugid >> 8)};
            hexSendLocalCmd(0x0B, data, 2);
            logAdd(L"[组播] 退出组 0x%04X", ugid);
        }
        /* ===== Log ===== */
        else if (id == IDC_BTN_CLEAR_LOG) {
            SetWindowTextW(g.hLog, L"");
        }

        return 0;
    }

    case WM_TIMER: {
        if (wp == IDT_REPEAT_SEND) {
            /* Re-send data */
            WCHAR text[4096];
            GetWindowTextW(g.hEditTxData, text, 4096);
            if (wcslen(text) > 0) {
                int hexMode = (SendMessageW(g.hChkHexTx, BM_GETCHECK, 0, 0) == BST_CHECKED);
                int addNewline = (SendMessageW(g.hChkNewline, BM_GETCHECK, 0, 0) == BST_CHECKED);
                if (hexMode) {
                    unsigned char data[2048];
                    int len = parseHexString(text, data, sizeof(data));
                    if (len > 0) { serialSend(data, len); g.txCount += len; uiUpdateDataCounters(); }
                } else {
                    char utf8[4096];
                    int len = WideCharToMultiByte(CP_UTF8, 0, text, wcslen(text), utf8, sizeof(utf8) - 4, NULL, NULL);
                    if (addNewline) {
                        int nlIdx = SendMessageW(g.hComboNewline, CB_GETCURSEL, 0, 0);
                        if (nlIdx == 0) { utf8[len] = '\r'; utf8[len+1] = '\n'; len += 2; }
                        else if (nlIdx == 1) { utf8[len] = '\n'; len += 1; }
                        else { utf8[len] = '\r'; len += 1; }
                    }
                    serialSend((unsigned char*)utf8, len); g.txCount += len; uiUpdateDataCounters();
                }
            }
        }
        return 0;
    }

    case WM_SIZE: {
        int w = LOWORD(lp), h = HIWORD(lp);

        /* Serial bar */
        SetWindowPos(g.hComboPort, NULL, 5, 5, 80, 22, SWP_NOZORDER);
        SetWindowPos(g.hComboBaud, NULL, 90, 5, 90, 22, SWP_NOZORDER);
        SetWindowPos(g.hBtnOpen, NULL, 185, 4, 60, 24, SWP_NOZORDER);
        SetWindowPos(g.hBtnRefresh, NULL, 250, 4, 50, 24, SWP_NOZORDER);
        SetWindowPos(g.hStaticStatus, NULL, 310, 7, w - 320, 20, SWP_NOZORDER);

        /* Tab control */
        int tabTop = 35;
        SetWindowPos(g.hTab, NULL, 0, tabTop, w, h - tabTop - 160, SWP_NOZORDER);

        /* Tab panels */
        RECT rc;
        GetClientRect(g.hTab, &rc);
        SendMessageW(g.hTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);
        int panelW = rc.right - rc.left;
        int panelH = rc.bottom - rc.top;
        int panelX = rc.left;
        int panelY = tabTop + rc.top;

        for (int i = 0; i < 6; i++) {
            if (g_hPanels[i])
                SetWindowPos(g_hPanels[i], NULL, panelX, panelY, panelW, panelH, SWP_NOZORDER);
        }

        /* Log area */
        int logTop = h - 155;
        SetWindowPos(g.hLog, NULL, 0, logTop, w - 5, 155, SWP_NOZORDER);

        /* Status bar at bottom */
        /* Not using a real status bar for simplicity, status text in serial bar */
        return 0;
    }

    case WM_DESTROY:
        if (g.repeatActive) KillTimer(hwnd, IDT_REPEAT_SEND);
        serialClose();
        if (g.hMonoFont) DeleteObject(g.hMonoFont);
        if (g.hGuiFont) DeleteObject(g.hGuiFont);
        DeleteCriticalSection(&g.rxLock);
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    return 0;
}

/* ========== Panel Window Procedure (forwards WM_COMMAND to main) ========== */

static LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_COMMAND) {
        /* Forward to main window so tab buttons work */
        return SendMessageW(g.hMainWnd, msg, wp, lp);
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

/* ========== WinMain Entry Point ========== */

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmd, int nShow) {
    /* Initialize common controls */
    INITCOMMONCONTROLSEX icex = {sizeof(icex), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icex);

    /* Initialize global state */
    memset(&g, 0, sizeof(g));
    g.hCom = INVALID_HANDLE_VALUE;
    g.currentBaud = 115200;
    InitializeCriticalSection(&g.rxLock);

    /* Create mono font */
    g.hMonoFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas");
    g.hGuiFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, L"Microsoft YaHei UI");

    /* Register window class */
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"E18HostToolWnd";
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) return 1;

    /* Register a simple panel container class that forwards WM_COMMAND */
    WNDCLASSEXW wcPanel = {0};
    wcPanel.cbSize = sizeof(wcPanel);
    wcPanel.style = CS_HREDRAW | CS_VREDRAW;
    wcPanel.lpfnWndProc = PanelWndProc;
    wcPanel.hInstance = hInst;
    wcPanel.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wcPanel.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcPanel.lpszClassName = L"E18Panel";
    RegisterClassExW(&wcPanel);

    /* Create main window */
    g.hMainWnd = CreateWindowExW(0, L"E18HostToolWnd",
        L"E18-MS1-PCB ZigBee 3.0 上位机 v1.0",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 720,
        NULL, NULL, hInst, NULL);

    if (!g.hMainWnd) return 1;

    /* Create serial bar controls */
    g.hComboPort = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        5, 5, 100, 200, g.hMainWnd, (HMENU)IDC_COMBO_PORT, NULL, NULL);
    {
        ComPortInfo ports[32];
        int count = serialEnum(ports, 32);
        for (int i = 0; i < count; i++)
            SendMessageW(g.hComboPort, CB_ADDSTRING, 0, (LPARAM)ports[i].name);
        if (count > 0) SendMessageW(g.hComboPort, CB_SETCURSEL, 0, 0);
    }

    g.hComboBaud = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        110, 5, 90, 200, g.hMainWnd, (HMENU)IDC_COMBO_BAUD, NULL, NULL);
    {
        const WCHAR *bauds[] = {L"9600", L"19200", L"38400", L"57600", L"115200"};
        for (int i = 0; i < 5; i++)
            SendMessageW(g.hComboBaud, CB_ADDSTRING, 0, (LPARAM)bauds[i]);
        SendMessageW(g.hComboBaud, CB_SETCURSEL, 4, 0); /* default 115200 */
    }

    g.hBtnOpen = CreateWindowW(L"BUTTON", L"打开",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        205, 4, 60, 24, g.hMainWnd, (HMENU)IDC_BTN_OPEN, NULL, NULL);
    SendMessageW(g.hBtnOpen, WM_SETFONT, (WPARAM)g.hGuiFont, TRUE);

    g.hBtnRefresh = CreateWindowW(L"BUTTON", L"刷新",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        270, 4, 50, 24, g.hMainWnd, (HMENU)IDC_BTN_REFRESH, NULL, NULL);

    g.hStaticStatus = CreateWindowW(L"STATIC", L"未连接",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        330, 7, 400, 20, g.hMainWnd, (HMENU)IDC_STATIC_STATUS, NULL, NULL);

    /* Create tab control */
    g.hTab = CreateWindowW(WC_TABCONTROLW, L"",
        WS_CHILD | WS_VISIBLE,
        0, 35, 900, 500, g.hMainWnd, (HMENU)IDC_TAB, NULL, NULL);

    TCITEMW ti = {0};
    ti.mask = TCIF_TEXT;
    const WCHAR *tabNames[] = {
        L"基本配置", L"HEX指令", L"AT指令", L"数据传输", L"外设控制", L"网络管理"
    };
    for (int i = 0; i < 6; i++) {
        ti.pszText = (LPWSTR)tabNames[i];
        SendMessageW(g.hTab, TCM_INSERTITEMW, i, (LPARAM)&ti);
    }

    /* Create panel containers (child dialogs) */
    HINSTANCE hi = (HINSTANCE)GetWindowLongPtrW(g.hMainWnd, GWLP_HINSTANCE);
    for (int i = 0; i < 6; i++) {
        g_hPanels[i] = CreateWindowExW(0, L"E18Panel", L"",
            WS_CHILD | WS_CLIPCHILDREN /* hidden initially */,
            0, 0, 100, 100, g.hMainWnd, NULL, hi, NULL);
    }

    /* Populate each tab panel */
    tabCreateBasicConfig(g_hPanels[0]);
    tabCreateHexCmd(g_hPanels[1]);
    tabCreateAtCmd(g_hPanels[2]);
    tabCreateDataTransfer(g_hPanels[3]);
    tabCreatePeripheral(g_hPanels[4]);
    tabCreateNetworkMgmt(g_hPanels[5]);

    /* Show first panel, hide others */
    tabShowPanel(0);

    /* Initial panel positioning - will be refined by first WM_SIZE */
    {
        RECT rc;
        GetClientRect(g.hMainWnd, &rc);
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        int tabTop = 35;
        SetWindowPos(g.hTab, NULL, 0, tabTop, w, h - tabTop - 160, SWP_NOZORDER);
        GetClientRect(g.hTab, &rc);
        SendMessageW(g.hTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);
        for (int i = 0; i < 6; i++) {
            if (g_hPanels[i])
                SetWindowPos(g_hPanels[i], NULL, rc.left, tabTop + rc.top,
                    rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
        }
    }

    /* Create log window */
    g.hLog = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_BORDER | WS_HSCROLL,
        0, 530, 900, 160, g.hMainWnd, (HMENU)IDC_LOG, NULL, NULL);
    SendMessageW(g.hLog, WM_SETFONT, (WPARAM)g.hMonoFont, TRUE);
    /* Set log background to dark */
    SendMessageW(g.hLog, EM_SETLIMITTEXT, 0, 0); /* no limit */

    /* Clear log button next to it - actually in the tab area, let me put it on the right */

    /* Initially disable feature controls */
    uiEnableControls(FALSE);

    /* Show window */
    ShowWindow(g.hMainWnd, nShow);
    UpdateWindow(g.hMainWnd);

    logAdd(L"[系统] E18-MS1-PCB 上位机启动");
    logAdd(L"[系统] 请选择串口并点击'打开'按钮连接模块");

    /* Message loop */
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}
