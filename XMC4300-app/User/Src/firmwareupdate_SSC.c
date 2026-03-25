/******************************************************************************
* Software License Agreement
*
* Copyright (c) 2016, Infineon Technologies AG
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* Neither the name of the copyright holders nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission

* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* To improve the quality of the software, users are encouraged to share
* modifications, enhancements or bug fixes with Infineon Technologies AG
* (dave@infineon.com).
*
*****************************************************************************/

/****************************************************************
* HEADER FILES
***************************************************************/
#include "ecatfoe.h"
#include "foeappl.h"
#include "shared_conf.h"
#include <xmc_scu.h>  // 必须包含此头文件以使用 SCU 复位功能
#include "objdef.h"
#include "ryhandObjects.h"
#include "firmwareupdate_SSC.h"

/****************************************************************
* MACROS AND DEFINES
***************************************************************/
#define BOOT_MAGIC_NUMBER  0xDEADBEEF
/****************************************************************
* LOCAL DATA
***************************************************************/
volatile int32_t g_nResetCountdown = -1; // -1 表示不动作

/****************************************************************
* API PROTOTYPES
***************************************************************/

void FWUPDATE_StartDownload(void)
{
	/* 1. 设置保留寄存器 (GPR0)
	 * 这个寄存器的值在 System Reset 后依然保留
	 */
	XMC_SCU_WriteGPR(0, BOOT_MAGIC_NUMBER);

	/* 2. 触发系统复位
	 * 复位后，芯片从 0x0C000000 (Bootloader) 开始运行。
	 * Bootloader 检查到 GPR0 == 0xDEADBEEF，就会停留在 Boot 模式并接收固件。
	 */
	NVIC_SystemReset();

	/* 代码不会运行到这里 */
	while(1);
}

#define CMD_START_UPDATE 0xF1  // 新增宏定义
volatile uint8_t g_bBootResetRequest = 0;
stu_upgrade upgrade_info;
uint8_t SDO_WriteCallback(uint16_t index, uint8_t subindex, uint32_t dataSize, uint16_t *pData, uint8_t bCompleteAccess)
{
    // 强转指针
    uint8_t *pByteData = (uint8_t *)pData;
    uint8_t Status = 0; // 默认为 0 (密码错误/失败)
    uint8_t pwdLow;
	uint8_t pwdHigh;
	uint8_t pwdChk;
    // ----------------------------------------------------------------
    // Sub 6: RxID (校验)
    // ----------------------------------------------------------------
    if (subindex == 0x06) {
        if (pByteData[0] != 254) return ABORTIDX_VALUE_EXCEEDED;
        upgrade_info.RxID = 254;
    }
    // ----------------------------------------------------------------
    // Sub 7: RxLen (长度)
    // ----------------------------------------------------------------
    else if (subindex == 0x07) {
        // 以前只允许 1，现在 PLC 发送 F1 命令包长度为 7
        if (pByteData[0] != 7)
        {
             // 如果您希望严格一点，可以只允许 7
              return ABORTIDX_VALUE_EXCEEDED;
        }
        upgrade_info.RxLen = pByteData[0];
    }
    // ----------------------------------------------------------------
    // Sub 5: RxIndex (触发器)
    // ----------------------------------------------------------------
    else if (subindex == 0x05) {
    	upgrade_info.RxIndex = pByteData[0];
    }
    // ----------------------------------------------------------------
    // Sub 8: RxData (命令解析 & 密码校验)
    // ----------------------------------------------------------------
    else if (subindex == 0x08)
    {
        uint8_t cmd = pByteData[0];

        // 清空发送缓冲区 (Sub 4)
        memset(SimulateCANFD0x9000.TxData, 0, 64);

        // -----------------------------------------------------
        // 处理 CMD_START (0xF1)
        // 协议: [Cmd:1] [Size:3] [PwdLow:1] [PwdHigh:1] [Check:1]
        // -----------------------------------------------------
        if (cmd == CMD_START_UPDATE && upgrade_info.RxID == 254 && upgrade_info.RxLen == dataSize ) // 0xF1
        {
        	uint32_t fileSize = pByteData[1] | (pByteData[2] << 8) | (pByteData[3] << 16);
            if (fileSize > APP_MAX_SIZE)
			{
				Status = 3; // 返回状态 3: 内存不足/文件过大
			}
			else
			{
	            // 1. 提取密码 (根据 PLC 代码: Byte4=0x34, Byte5=0x12, Byte6=64)
				pwdLow  = pByteData[4];
				pwdHigh = pByteData[5];
				pwdChk  = pByteData[6];
				// 2. 校验密码
				if (pwdLow == 0x34 && pwdHigh == 0x12 && pwdChk == 64)
				{
					// 密码正确！
					g_bBootResetRequest = 1; // 通知 Main 循环准备复位


					Status = 1; // 【状态码 1】: "目前在 App，验证成功，准备跳转"
				}
				else
				{
					// 密码错误
					Status = 0; // 【状态码 0】: "密码错误"
				}
			}
        }
        else
        {
            // 其他未知命令
            Status = 0;
        }

        // -----------------------------------------------------
        // 准备回复 (写入 TxData / Sub 4)
        // -----------------------------------------------------
        // PLC 会读取 Sub 4 来判断状态
        SimulateCANFD0x9000.TxData[0] = cmd;    // 回复命令字 (F1)
        SimulateCANFD0x9000.TxData[1] = Status; // 回复状态 (1=成功, 0=失败)
        SimulateCANFD0x9000.TxData[2] = 0x34;
        SimulateCANFD0x9000.TxData[3] = 0x12;
        SimulateCANFD0x9000.TxData[4] = pwdChk;
        // 填充其他辅助信息 (可选)
        SimulateCANFD0x9000.TxLen = 5;
        SimulateCANFD0x9000.TxIndex = upgrade_info.RxIndex;
        SimulateCANFD0x9000.TxID = 254 + 256;
//        memset(&upgrade_info, 0, sizeof(upgrade_info));
    }

    return 0; // SDO 写入成功
}

/**
 * @brief 处理异步复位逻辑（非阻塞倒计时）
 * @note  应在 MainLoop 所在的 while(1) 中调用。
 * 复位请求通常由 SDO 写入触发 g_bBootResetRequest。
 */
void FOE_HandleAsyncReset(void)
{
    /* 1. 检查并初始化倒计时 */
    if (g_bBootResetRequest)
    {
        // 如果当前没有正在进行的倒计时（假设初始化值为 -1）
        if (g_nResetCountdown == -1)
        {
            /* 装载倒计时数值：1000ms / 0.1ms 周期 = 10000 次 (原代码为 200000)
               请确保该数值与您主循环的实际执行频率匹配 */
            g_nResetCountdown = 40000;

            // 清除触发标志，防止重复装载计数器
            g_bBootResetRequest = 0;
        }
    }

    /* 2. 执行倒计时逻辑 */
    if (g_nResetCountdown > 0)
    {
        g_nResetCountdown--;

        /* 3. 倒计时结束，执行复位回调 */
        if (g_nResetCountdown == 0)
        {
            // 在此处调用实际的复位或进入 Bootstrap 模式的函数
            Handle_Bootstrap_Request();

            // 安全起见，执行后重置为闲置状态
            g_nResetCountdown = -1;
        }
    }
}














  
