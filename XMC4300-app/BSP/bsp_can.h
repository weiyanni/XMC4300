/* ============================================================================
 * 文件名: bsp_can.h
 * 描述: CAN 通信控制接口
 * ============================================================================ */
#ifndef BSP_CAN_H_
#define BSP_CAN_H_

#include <stdint.h>
#include "ecat_def.h"
/* ==============================================================
 * CAN 配置常量 (统一管理)
 * ============================================================== */
/**
 * @brief 定义函数执行状态的枚举
 */
typedef enum {
    ECAT_CAN_FWD_SUCCESS,             /**< 成功转发所有数据 */
    ECAT_CAN_FWD_ERROR_INVALID_PARAMS,/**< 传入了无效的参数 (如NULL指针) */
    ECAT_CAN_FWD_ERROR_CAN_TX         /**< CAN发送过程中发生错误 */
} EcatCanFwdStatus_t;

// 辅助结构体，用于通过指针访问 TOBJ7000/TOBJ7010/TOBJ7020 中的每一组
typedef struct __attribute__((packed)) {
    UINT16 len;       // 数据长度
    CHAR   data[24];  // 数据内容
} EcatChannel_t;

typedef struct {
    // --- 寻址参数 ---
    uint32_t baseCanId;       //  CAN ID
    void*    pEcatObject;     // 目标 EtherCAT 对象地址
    uint8_t  channelCount;    // 通道数量

    // --- 过滤参数 ---
    const uint8_t* pCmdList;     // 期望的命令字 (pData[0])
    uint8_t  cmdListSize;    // 1: 必须匹配命令字才转发; 0: 无条件转发
} EcatRxMap_t;

//EcatRxMap_t 对象最大个数
#define RX_MAP_COUNT 3

//自动计算长度
#define CMD_LIST(...) \
    (const uint8_t[]){ __VA_ARGS__ }, \
    sizeof((const uint8_t[]){ __VA_ARGS__ })

/** CAN 接收中断优先级 (0=最高, 63=最低) */
#define CAN_IRQ_PRIORITY        63

/** 默认采样点 (单位: 0.01%, 如 8000=80.00%) */
#define CAN_DEFAULT_SAMPLE_POINT  8000

/** 默认同步跳转宽度 */
#define CAN_DEFAULT_SJW         1

/** 默认设备 ID */
#define CAN_DEFAULT_DEV_ID      254

/** 支持的波特率列表 */
extern const uint32_t CAN_SUPPORTED_BAUDRATES[];
extern const uint8_t  CAN_BAUDRATE_COUNT;

/* ==============================================================
 * CAN 配置结构体
 * ============================================================== */

/** CAN 运行时配置 */
typedef struct {
    uint32_t baudrate;      /**< 波特率 (bps) */
    uint16_t dev_id;        /**< 设备 ID (发送 0-512) */
    uint8_t  use_120r;      /**< 终端电阻: 1=开启, 0=关闭 */
} CAN_Config_t;

/* ==============================================================
 * 初始化与控制接口
 * ============================================================== */

/**
 * @brief 初始化 CAN (切换到CAN模式 + 终端电阻开启)
 */
void BSP_CAN_Init(void);

/**
 * @brief 切换到 CAN 模式 (CTRLB_H_A=HIGH, CTRLA_L_B=HIGH)
 */
void BSP_CAN_ModeSelect(void);

/**
 * @brief CAN 收发器正常模式 (STB=Low)
 */
void BSP_CAN_Normal(void);

/**
 * @brief CAN 收发器待机模式 (STB=High)
 */
void BSP_CAN_Standby(void);

/**
 * @brief 开启 120Ω 终端电阻 (CTRL_120R=LOW)
 */
void BSP_120R_Enable(void);

/**
 * @brief 关闭 120Ω 终端电阻 (CTRL_120R=HIGH)
 */
void BSP_120R_Disable(void);

void can_set_can_baud(uint32_t BR_canfd_ctrl,uint32_t BR_canfd_data);
void can_set_rs485_baud(uint32_t BR_485);
/* ==============================================================
 * CAN 波特率切换 (仅 App 项目)
 * ============================================================== */

/** CAN 操作状态 */
typedef enum {
    BSP_CAN_OK = 0,
    BSP_CAN_ERROR_INVALID_BAUDRATE,
    BSP_CAN_ERROR_HW_FAILED
} BSP_CAN_STATUS_t;

/**
 * @brief 检查 CAN 波特率是否有效
 * @param baudrate 波特率 (bps)
 * @return 1=有效, 0=无效
 */
uint8_t BSP_CAN_IsBaudrateValid(uint32_t baudrate);

/**
 * @brief 设置 CAN 波特率
 * @param baudrate 波特率 (bps)
 * @param save_to_eeprom 1=保存到EEPROM, 0=仅切换硬件
 * @return BSP_CAN_OK=成功, 其他=失败
 */
BSP_CAN_STATUS_t BSP_CAN_SetBaudrate(uint32_t baudrate, uint8_t save_to_eeprom);

/**
 * @brief 设置 CAN 波特率
 * @param baudrate 波特率 (bps)
 * @return BSP_CAN_OK=成功, 其他=失败
 */
BSP_CAN_STATUS_t BSP_CANFD_UpdateBaudrate(uint32_t baudrate_ctrl,uint32_t baudrate_data);
/* ==============================================================
 * CAN ID 和接收过滤器配置
 * ============================================================== */

/**
 * @brief 从 DevID 字符串解析设备 ID
 * @param dev_id_str 设备 ID 字符串 (如 "254")
 * @return 解析后的设备 ID (发送 0-512), 失败返回 254
 */
uint16_t BSP_CAN_ParseDevID(const char *dev_id_str);

/**
 * @brief 配置 CAN 本机 ID 和接收过滤器
 *        - 发送 ID = 设备 ID (DevID)
 *        - 接收过滤器只接收 0-254
 * @param dev_id 设备 ID (发送 0-512)
 */
void BSP_CAN_ConfigID(uint16_t dev_id);


/**
 * @brief 设置 CAN 发送 ID
 * @param tx_id 发送 ID (11位标准ID)
 */
void BSP_CAN_SetTxID(uint32_t tx_id);

/**
 * @brief 配置 CAN 接收过滤器
 *        接收 ID 范围: 0 - max_id
 * @param max_id 最大接收 ID (建议 254)
 */
void BSP_CAN_SetRxFilter(uint8_t max_id);

/**
 * @brief 应用 CAN 完整配置 (波特率 + ID + 中断 + 终端电阻)
 *        统一入口，供 DevConfig 模块调用
 * @param cfg CAN 配置结构体指针
 */
void BSP_CAN_ApplyConfig(const CAN_Config_t *cfg);

/* ==============================================================
 * CAN 收发功能
 * ============================================================== */

/**
 * @brief CAN 接收处理函数 (在中断回调或轮询中调用)
 */
void BSP_CAN_RxHandler(void);

/**
 * @brief CAN 发送数据
 * @param data 数据指针
 * @param len 数据长度 (1-8)
 * @return 0=成功
 */
uint8_t BSP_CAN_Transmit(const uint8_t *data, uint8_t len);

EcatCanFwdStatus_t EcatCan_ForwardPdo_Modified(uint32_t baseCanId, const void* pData, uint16_t dataSize);

void Forward_MultiChannel_Obj_Auto(void* pCurrentObj, void* pLastObj, uint32_t baseCanId);

#endif /* BSP_CAN_H_ */



