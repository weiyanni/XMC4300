/* ============================================================================
 * 文件名: can_protocol.h
 * 描述: CAN 协议处理 - 固件升级命令定义
 * 
 * 全局约定：所有多字节数据均按小端传输（低字节在前）
 * ============================================================================ */
#ifndef CAN_PROTOCOL_H_
#define CAN_PROTOCOL_H_

#include <stdint.h>

/* ==============================================================
 * CAN 协议命令字定义
 * ============================================================== */

#define CAN_CMD_GET_SW_VERSION      0xF0    /* 设备软件版本读取 */
#define CAN_CMD_START_UPGRADE       0xF1    /* 开始升级（帧长协商） */
#define CAN_CMD_UPGRADE_DATA        0xF2    /* 升级数据 */
#define CAN_CMD_UPGRADE_END         0xF3    /* 升级结束 */
#define CAN_CMD_SET_CAN_BAUD        0xEC    /* 设置can波特率 */
#define CAN_CMD_SET_RS485_BAUD      0xED    /* 设置rs485波特率 */

/* ==============================================================
 * 帧信息字节定义 (0xF0 应答)
 * ============================================================== */

#define CAN_FRAME_INFO_TAIL_FLAG    0x80    /* bit7=1 表示尾帧 */
#define CAN_FRAME_INFO_NUM_MASK     0x7F    /* bit0~6 帧号 */
#define CAN_FRAME_PAYLOAD_SIZE      6       /* 每帧携带 6 字节数据 */

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

#define CAN_MAX_FRAME_LENGTH            8

/* ==============================================================
 * 密码计算 (基于设备ID)
 * ============================================================== */

/**
 * @brief 根据设备ID计算正确的升级密码
 * @param dev_id 设备ID (0-254)
 * @return 16位升级密码
 */
uint16_t CAN_Protocol_CalcPassword(uint8_t dev_id);

/* ==============================================================
 * 协议处理函数
 * ============================================================== */

/**
 * @brief 处理 CAN 协议命令
 * @param rx_data 接收到的数据
 * @param rx_len 数据长度
 */
void CAN_Protocol_HandleCommand(const uint8_t *rx_data, uint8_t rx_len);

/**
 * @brief 处理 0xF0 命令 - 读取软件版本
 */
void CAN_Protocol_HandleGetSwVersion(void);

/**
 * @brief 处理 0xF1 命令 - 开始升级
 * @param rx_data 接收数据
 * @param rx_len 数据长度
 */
void CAN_Protocol_HandleStartUpgrade(const uint8_t *rx_data, uint8_t rx_len);

/**
 * @brief 处理 0xF2 命令 - 升级数据
 * @param rx_data 接收数据
 * @param rx_len 数据长度
 */
void CAN_Protocol_HandleUpgradeData(const uint8_t *rx_data, uint8_t rx_len);

/**
 * @brief 处理 0xF3 命令 - 升级结束
 * @param rx_data 接收数据
 * @param rx_len 数据长度
 */
void CAN_Protocol_HandleUpgradeEnd(const uint8_t *rx_data, uint8_t rx_len);

/* ==============================================================
 * CRC32 计算 (多项式 0x04C11DB7)
 * ============================================================== */

/**
 * @brief 计算数据块的 CRC32
 * @param data 数据指针
 * @param len 数据长度（字节）
 * @return CRC32 值
 */
uint32_t CAN_Protocol_CalcCRC32(const uint8_t *data, uint32_t len);

#endif /* CAN_PROTOCOL_H_ */
