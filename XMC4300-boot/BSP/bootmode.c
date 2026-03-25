/* ============================================================================
 * BSP/bootmode.c - Boot 的 Bootmode 实现
 * 排除 SSC/Src/bootmode.c，使用此文件
 * ============================================================================ */
#include "ecat_def.h"
#include "bootmode.h"
#include "ecatfoe.h"

/* Boot 进入 Bootstrap 时调用 (无需处理) */
void BL_Start(UINT8 State)
{
}

/* 
 * BOOT → INIT 转换时调用
 * 不需要在此处跳转，因为：
 * 1. 升级完成后 User_FoE_Impl.c 会设置 SHARED_MAGIC_VALID 并复位
 * 2. 断电重启后 main.c 的 Check_Can_Jump_To_App() 会自动跳转
 */
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
