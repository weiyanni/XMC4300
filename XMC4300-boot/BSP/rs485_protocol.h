/* ============================================================================
 * 文件名: can_protocol.h
 * 描述: CAN 协议处理 - 固件升级命令定义
 * 
 * 全局约定：所有多字节数据均按小端传输（低字节在前）
 * ============================================================================ */
#ifndef RS485_PROTOCOL_H_
#define RS485_PROTOCOL_H_

#include <stdint.h>

/* ==============================================================
 * CAN 协议命令字定义
 * ============================================================== */

#define RS485_CMD_GET_SW_VERSION      0xF0    /* 设备软件版本读取 */
#define RS485_CMD_START_UPGRADE       0xF1    /* 开始升级（帧长协商） */
#define RS485_CMD_UPGRADE_DATA        0xF2    /* 升级数据 */
#define RS485_CMD_UPGRADE_END         0xF3    /* 升级结束 */

/* ==============================================================
 * 帧信息字节定义 (0xF0 应答)
 * ============================================================== */

#define RS485_FRAME_INFO_TAIL_FLAG    0x80    /* bit7=1 表示尾帧 */
#define RS485_FRAME_INFO_NUM_MASK     0x7F    /* bit0~6 帧号 */
#define RS485_FRAME_PAYLOAD_SIZE      6       /* 每帧携带 6 字节数据 */

/* ==============================================================
 * 0xF1 设备状态定义
 * ============================================================== */

#define UPGRADE_STATUS_PASSWORD_FAIL    0   /* 密码不通过 */
#define UPGRADE_STATUS_APP_TO_BOOT      1   /* 从 App 进入升级模式 */
#define UPGRADE_STATUS_BOOT_READY       2   /* Boot 进入升级模式 */
#define UPGRADE_STATUS_NO_MEMORY        3   /* 内存不够，进入失败 */
#define UPGRADE_STATUS_ERASE_START      4   /* 数据擦除开始 */
#define UPGRADE_STATUS_ERASE_OK         5   /* 擦除完成，可传输 */

/* ==============================================================
 * 0xF3 升级结果定义
 * ============================================================== */

#define UPGRADE_RESULT_SUCCESS          0   /* 升级成功，节点重启 */
#define UPGRADE_RESULT_CRC_FAIL         1   /* CRC32 校验失败 */
#define UPGRADE_RESULT_HW_MISMATCH      2   /* 硬件版本不匹配 */

/* ==============================================================
 * 帧长度 (标准 CAN = 8 字节)
 * ============================================================== */

#define RS485_MAX_FRAME_LENGTH            8

/* ==============================================================
 * 密码计算 (基于设备ID)
 * ============================================================== */

uint16_t RS485_Protocol_CalcPassword(uint16_t dev_id);

/* ==============================================================
 * 协议处理函数
 * ============================================================== */

void RS485_Protocol_HandleCommand(const uint8_t *rx_data, uint8_t rx_len);
void RS485_Protocol_HandleGetSwVersion(void);
void RS485_Protocol_HandleStartUpgrade(const uint8_t *rx_data, uint8_t rx_len);
void RS485_Protocol_HandleUpgradeData(const uint8_t *rx_data, uint8_t rx_len);
void RS485_Protocol_HandleUpgradeEnd(const uint8_t *rx_data, uint8_t rx_len);

/* ==============================================================
 * CRC32 计算 (多项式 0x04C11DB7)
 * ============================================================== */

uint32_t RS485_Protocol_CalcCRC32(const uint8_t *data, uint32_t len);

/* ==============================================================
 * 升级状态管理 (Boot 专用)
 * ============================================================== */

/** 升级状态机 */
typedef enum {
    UPGRADE_STATE_IDLE = 0,         /* 空闲 */
    UPGRADE_STATE_ERASING,          /* 正在擦除 */
    UPGRADE_STATE_READY,            /* 准备接收数据 */
    UPGRADE_STATE_RECEIVING,        /* 正在接收数据 */
    UPGRADE_STATE_VERIFYING,        /* 正在校验 */
    UPGRADE_STATE_DONE              /* 完成 */
} UpgradeState_t;

/** 升级上下文 */
typedef struct {
    UpgradeState_t state;           /* 当前状态 */
    uint32_t total_size;            /* 固件总大小 */
    uint32_t received_size;         /* 已接收大小 */
    uint32_t next_addr;             /* 下一个写入地址 (相对地址) */
    uint32_t crc32;                 /* 累计 CRC32 */
} UpgradeContext_t;

//extern UpgradeContext_t g_UpgradeCtx;

#endif /* CAN_PROTOCOL_H_ */
