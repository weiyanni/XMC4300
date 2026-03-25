/* ============================================================================
 * BSP/bootmode.c - App 的 Bootmode 实现
 * 排除 SSC/Src/bootmode.c，使用此文件
 * ============================================================================ */
#include "ecat_def.h"
#include "bootmode.h"
#include "ecatfoe.h"
#include "app_init.h"

/* SSC 协议栈在 INIT → BOOT 转换时调用 */
void BL_Start(UINT8 State)
{
    /* 跳转到 Bootloader */
    Handle_Bootstrap_Request();
}

/* BOOT → INIT 转换时调用 (App 不会进入此状态) */
void BL_Stop(void)
{
}

/* FoE 相关占位函数 */
void BL_StartDownload(UINT32 password)
{
}

UINT16 BL_Data(UINT16 *pData, UINT16 Size)
{
    return 0;
}
