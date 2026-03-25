#ifndef SHARED_CONF_H_
#define SHARED_CONF_H_

#include <stdint.h>

/* ==============================================================
 * 1. Flash 内存地图定义 (Total 256KB Flash)
 * 
 * XMC4300 Flash 扇区布局 (每扇区 16KB):
 *   - S0-S5 (96KB):  Bootloader 代码
 *   - S6-S7 (32KB):  EEPROM 仿真区域 (SharedConfig 等)
 *   - S8    (128KB): Application 代码
 *
 * 地址映射:
 *   - Cached (0x08...): 用于代码执行和数据读取
 *   - Uncached (0x0C...): 用于 Flash 编程/擦除操作
 * ============================================================== */

/* --- Bootloader 区域 (96KB, S0-S5) --- */
#define BOOT_START_ADDR         0x08000000UL
#define BOOT_START_ADDR_UC      0x0C000000UL
#define BOOT_SIZE               0x00018000UL  // 96 KB (6 sectors * 16KB)
#define BOOT_END_ADDR           (BOOT_START_ADDR + BOOT_SIZE - 1)

/* --- EEPROM 仿真区域 (32KB, S6-S7) --- */
#define EEPROM_START_ADDR       0x0C018000UL  // Uncached 地址（用于写入/擦除）
#define EEPROM_START_ADDR_RD    0x08018000UL  // Cached 地址（用于读取）
#define EEPROM_SIZE             0x00008000UL  // 32 KB (2 sectors * 16KB)
#define EEPROM_END_ADDR         (EEPROM_START_ADDR + EEPROM_SIZE - 1)

/* --- App 区域 (128KB, S8) --- */
#define APP_START_ADDR          0x0C020000UL  // Uncached 地址（用于写入）
#define APP_START_ADDR_RD       0x08020000UL  // Cached 地址（用于读取/执行）
#define APP_MAX_SIZE            0x00020000UL  // 128 KB
#define APP_END_ADDR            (APP_START_ADDR + APP_MAX_SIZE - 1)

/* ==============================================================
 * 1.5. RAM 标志位 (用于区分软件复位和断电重启)
 * 
 * 原理：
 *   - 软件复位 (NVIC_SystemReset): RAM 数据保留
 *   - 断电重启: RAM 数据丢失 (变成随机值)
 * 
 * 使用 .noinit 段：
 *   - 编译器自动分配地址
 *   - 启动代码不会清零这个变量
 *   - Boot 和 App 都必须定义这个变量（同名同段）
 * ============================================================== */

#define RAM_MAGIC_VAL  0xB007B007UL  // "BOOTBOOT" 的数字化表示

/* 
 * 注意：这个变量在 Boot 和 App 中都需要定义一次（不能只在头文件声明）
 * 在各自的 main.c 或 app_init.c / boot_init.c 中添加：
 * uint32_t g_RamMagicFlag __attribute__((section(".noinit")));
 */

/* ==============================================================
 * 2. SharedConfig 在 EEPROM 中的存储布局
 * 
 * E_EEPROM_XMC4 使用块循环计数 + CRC 方式管理数据，
 * 我们的 SharedConfig_t (13 字节) 存储在 EEPROM 数据区的开头。
 * 
 * 数据块布局 (假设 DATA_BLOCK_SIZE = 2048):
 *   - Offset 0x00:  SharedConfig_t (13 字节)
 *   - Offset 0x10+: 其他用户数据 (可扩展)
 * ============================================================== */

/* SharedConfig_t 在 E_EEPROM 数据区中的偏移地址 */
/* EtherCAT SII 数据通常占用前 0x800 字节，因此我们将 SharedConfig 移到 0x1000 (4KB) 处 */
#define SHARED_CONFIG_EEPROM_OFFSET    0x1000U

/* ==============================================================
 * 3. 共享配置结构体定义
 * ============================================================== */

#define SHARED_MAGIC_VALID      0x5AA5A55AUL  // App 有效标志

typedef struct {
    uint32_t MagicHeader;       // 0x00: VALID, REQUEST, 或 0xFFFFFFFF
    uint32_t AppActualSize;     // 0x04: App 实际大小
    uint32_t AppCRC32;          // 0x08: CRC32 校验值
    uint8_t  Checksum;          // 0x0C: 前面所有字节的 8 位累加和
} __attribute__((packed)) SharedConfig_t;

/* 计算校验和 (不包含 Checksum 字段本身) */
static inline uint8_t SharedConfig_CalcChecksum(const SharedConfig_t *cfg)
{
    const uint8_t *p = (const uint8_t *)cfg;
    uint8_t sum = 0;
    for (uint16_t i = 0; i < sizeof(SharedConfig_t) - 1; i++) {
        sum += p[i];
    }
    return sum;
}

/* 校验结构体是否有效 */
static inline uint8_t SharedConfig_IsValid(const SharedConfig_t *cfg)
{
    return (cfg->Checksum == SharedConfig_CalcChecksum(cfg));
}

/* ==============================================================
 * 4. 设备配置结构体 (0x8000 对象)
 * 
 * 与 SSC 的 TOBJ8000 结构体对应，存储在 EEPROM 中。
 * 通过 TwinCAT CoE 读写时，同步到 EEPROM。
 * ============================================================== */

#define DEVICE_CONFIG_EEPROM_OFFSET    0x1100U  // SharedConfig 之后 (0x1000 + 256)
#define DEVICE_CONFIG_MAGIC            0xDE01CF6UL // 魔数标识 "DEV1CFG"

typedef struct {
    uint32_t Magic;             // 魔数标识 (DEVICE_CONFIG_MAGIC)
    char     Name[32];          // SubIndex 1: 设备名称
    char     Hv[32];            // SubIndex 2: 硬件版本
    char     Sv[32];            // SubIndex 3: 软件版本
    char     Sn[32];            // SubIndex 4: 序列号
    uint32_t BuildTime;         // SubIndex 5: 编译时间
    uint32_t BR_canfd_ctrl;     // SubIndex 6: CANFD 控制域波特率
    uint32_t BR_canfd_data;     // SubIndex 7: CANFD 数据域波特率
    uint32_t BR_485;            // SubIndex 8: RS485 波特率
    uint8_t  Use120;            // SubIndex 9: 120Ω终端电阻
    uint8_t  Use485;            // SubIndex 10: 485端口选择
    uint8_t  UseModbus;         // SubIndex 11: Modbus使能
    uint8_t  Reserved;          // SubIndex 12: 保留
    char     DevID[8];          // SubIndex 13: 设备ID (6字节+2对齐)
    uint8_t  Checksum;          // 校验和
} __attribute__((packed)) DeviceConfig_t;

/* 计算 DeviceConfig 校验和 */
static inline uint8_t DeviceConfig_CalcChecksum(const DeviceConfig_t *cfg)
{
    const uint8_t *p = (const uint8_t *)cfg;
    uint8_t sum = 0;
    for (uint16_t i = 0; i < sizeof(DeviceConfig_t) - 1; i++) {
        sum += p[i];
    }
    return sum;
}

/* 校验 DeviceConfig 是否有效 */
static inline uint8_t DeviceConfig_IsValid(const DeviceConfig_t *cfg)
{
    return (cfg->Magic == DEVICE_CONFIG_MAGIC) && 
           (cfg->Checksum == DeviceConfig_CalcChecksum(cfg));
}

/* ==============================================================
 * 5. 辅助宏和函数
 * ============================================================== */

/* 
 * 注意: 由于 SharedConfig 现在存储在 E_EEPROM 中，
 * 不能直接通过指针访问 Flash，需要使用 E_EEPROM_XMC4_ReadArray() 读取。
 * 
 * 使用示例:
 *   SharedConfig_t cfg;
 *   E_EEPROM_XMC4_ReadArray(SHARED_CONFIG_EEPROM_OFFSET, (uint8_t*)&cfg, sizeof(SharedConfig_t));
 *   if (SharedConfig_IsValid(&cfg) && cfg.MagicHeader == SHARED_MAGIC_VALID) {
 *       // App 有效
 *   }
 */

#endif /* SHARED_CONF_H_ */