/* firmwareupdate_SSC.h (Bootloader 版本) */
#ifndef _FIRMWAREUPDATE_SSC_H_
#define _FIRMWAREUPDATE_SSC_H_

#include <xmc_common.h> // 确保包含标准类型定义

typedef struct STRUCT_UPGRADE_DATA {
	uint16_t RxIndex; /* Subindex1 - TxIndex */
	uint16_t RxID; /* Subindex2 - TxID */
	uint16_t RxLen; /* Subindex3 - TxLen */
	char RxData[64]; /* Subindex4 - TxData */
} stu_upgrade;


#define ECAT_FOE_ERRCODE_CRCERROR  0x800B
#define APP_HEADER_SIZE  512
/* API PROTOTYPES */
void FWUPDATE_StartDownload(void);
uint16_t FWUPDATE_Data(uint16_t *pdata, uint16_t size);

/* 保持兼容性的空函数声明 */
void FWUPDATE_StateTransitionInit(void);
uint8_t FWUPDATE_GetDownloadFinished(void);
uint8_t FWUPDATE_SDO_DownloadFinished(void);
uint8_t FWUPDATE_SDO_Finish_New(uint32_t masterCRC);
uint32_t ExtractUint32(uint8_t *pData, uint8_t len);
void PackUint32(uint8_t *pData, uint32_t val, uint8_t len);
uint8_t SDO_WriteCallback(uint16_t index, uint8_t subindex, uint32_t dataSize, uint16_t *pData, uint8_t bCompleteAccess);
void FWUPDATE_FIFO_Init(void);
uint8_t FWUPDATE_FIFO_Write(uint8_t *pData, uint32_t len);
uint8_t FWUPDATE_FIFO_Flush(void);
void FOE_CheckAndPerformReset(void);
void FOE_ProcessEraseState(void);
static void FOE_SendStatusFeedback(uint8_t cmd, uint8_t status);
extern uint32_t g_sdo_total_received;
extern volatile uint8_t g_bFirmwareEraseChip;
extern volatile uint8_t g_UpdateStatus;

#endif
