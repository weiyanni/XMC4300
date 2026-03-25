/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
* \addtogroup ryhand ryhand
* @{
*/

/**
\file ryhandObjects
\author ET9300Utilities.ApplicationHandler (Version 1.6.4.0) | EthercatSSC@beckhoff.com

\brief ryhand specific objects<br>
\brief NOTE : This file will be overwritten if a new object dictionary is generated!<br>
*/

#if defined(_RYHAND_) && (_RYHAND_ == 1)
#define PROTO
#else
#define PROTO extern
#endif
/******************************************************************************
*                    Object 0x1600 : Rxpdo16 Mapping
******************************************************************************/
/**
* \addtogroup 0x1600 0x1600 | Rxpdo16 Mapping
* @{
* \brief Object 0x1600 (Rxpdo16 Mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1<br>
* SubIndex 2<br>
* SubIndex 3<br>
* SubIndex 4<br>
* SubIndex 5<br>
* SubIndex 6<br>
* SubIndex 7<br>
* SubIndex 8<br>
* SubIndex 9<br>
* SubIndex 10<br>
* SubIndex 11<br>
* SubIndex 12<br>
* SubIndex 13<br>
* SubIndex 14<br>
* SubIndex 15<br>
* SubIndex 16<br>
* SubIndex 17<br>
* SubIndex 18<br>
* SubIndex 19<br>
* SubIndex 20<br>
* SubIndex 21<br>
* SubIndex 22<br>
* SubIndex 23<br>
* SubIndex 24<br>
* SubIndex 25<br>
* SubIndex 26<br>
* SubIndex 27<br>
* SubIndex 28<br>
* SubIndex 29<br>
* SubIndex 30<br>
* SubIndex 31<br>
* SubIndex 32<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1600[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex7 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex10 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex11 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex12 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex13 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex15 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex16 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex17 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex18 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex19 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex20 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex21 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex22 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex23 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex24 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex25 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex26 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex27 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex28 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex29 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex30 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex31 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }}; /* Subindex32 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1600[] = "Rxpdo16 Mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000"
"SubIndex 013\000"
"SubIndex 014\000"
"SubIndex 015\000"
"SubIndex 016\000"
"SubIndex 017\000"
"SubIndex 018\000"
"SubIndex 019\000"
"SubIndex 020\000"
"SubIndex 021\000"
"SubIndex 022\000"
"SubIndex 023\000"
"SubIndex 024\000"
"SubIndex 025\000"
"SubIndex 026\000"
"SubIndex 027\000"
"SubIndex 028\000"
"SubIndex 029\000"
"SubIndex 030\000"
"SubIndex 031\000"
"SubIndex 032\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 -  */
UINT32 SI2; /* Subindex2 -  */
UINT32 SI3; /* Subindex3 -  */
UINT32 SI4; /* Subindex4 -  */
UINT32 SI5; /* Subindex5 -  */
UINT32 SI6; /* Subindex6 -  */
UINT32 SI7; /* Subindex7 -  */
UINT32 SI8; /* Subindex8 -  */
UINT32 SI9; /* Subindex9 -  */
UINT32 SI10; /* Subindex10 -  */
UINT32 SI11; /* Subindex11 -  */
UINT32 SI12; /* Subindex12 -  */
UINT32 SI13; /* Subindex13 -  */
UINT32 SI14; /* Subindex14 -  */
UINT32 SI15; /* Subindex15 -  */
UINT32 SI16; /* Subindex16 -  */
UINT32 SI17; /* Subindex17 -  */
UINT32 SI18; /* Subindex18 -  */
UINT32 SI19; /* Subindex19 -  */
UINT32 SI20; /* Subindex20 -  */
UINT32 SI21; /* Subindex21 -  */
UINT32 SI22; /* Subindex22 -  */
UINT32 SI23; /* Subindex23 -  */
UINT32 SI24; /* Subindex24 -  */
UINT32 SI25; /* Subindex25 -  */
UINT32 SI26; /* Subindex26 -  */
UINT32 SI27; /* Subindex27 -  */
UINT32 SI28; /* Subindex28 -  */
UINT32 SI29; /* Subindex29 -  */
UINT32 SI30; /* Subindex30 -  */
UINT32 SI31; /* Subindex31 -  */
UINT32 SI32; /* Subindex32 -  */
} OBJ_STRUCT_PACKED_END
TOBJ1600;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1600 Rxpdo16Mapping0x1600
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={32,0x70000110,0x700002C0,0x70000310,0x700004C0,0x70000510,0x700006C0,0x70000710,0x700008C0,0x70000910,0x70000AC0,0x70000B10,0x70000CC0,0x70000D10,0x70000EC0,0x70000F10,0x700010C0,0x70001110,0x700012C0,0x70001310,0x700014C0,0x70001510,0x700016C0,0x70001710,0x700018C0,0x70001910,0x70001AC0,0x70001B10,0x70001CC0,0x70001D10,0x70001EC0,0x70001F10,0x700020C0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1601 : Rxpdo6 Mapping
******************************************************************************/
/**
* \addtogroup 0x1601 0x1601 | Rxpdo6 Mapping
* @{
* \brief Object 0x1601 (Rxpdo6 Mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1<br>
* SubIndex 2<br>
* SubIndex 3<br>
* SubIndex 4<br>
* SubIndex 5<br>
* SubIndex 6<br>
* SubIndex 7<br>
* SubIndex 8<br>
* SubIndex 9<br>
* SubIndex 10<br>
* SubIndex 11<br>
* SubIndex 12<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1601[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex7 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex10 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex11 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }}; /* Subindex12 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1601[] = "Rxpdo6 Mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 -  */
UINT32 SI2; /* Subindex2 -  */
UINT32 SI3; /* Subindex3 -  */
UINT32 SI4; /* Subindex4 -  */
UINT32 SI5; /* Subindex5 -  */
UINT32 SI6; /* Subindex6 -  */
UINT32 SI7; /* Subindex7 -  */
UINT32 SI8; /* Subindex8 -  */
UINT32 SI9; /* Subindex9 -  */
UINT32 SI10; /* Subindex10 -  */
UINT32 SI11; /* Subindex11 -  */
UINT32 SI12; /* Subindex12 -  */
} OBJ_STRUCT_PACKED_END
TOBJ1601;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1601 Rxpdo6Mapping0x1601
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={12,0x70100110,0x701002C0,0x70100310,0x701004C0,0x70100510,0x701006C0,0x70100710,0x701008C0,0x70100910,0x70100AC0,0x70100B10,0x70100CC0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1602 : RxpdoTS Mapping
******************************************************************************/
/**
* \addtogroup 0x1602 0x1602 | RxpdoTS Mapping
* @{
* \brief Object 0x1602 (RxpdoTS Mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1<br>
* SubIndex 2<br>
* SubIndex 3<br>
* SubIndex 4<br>
* SubIndex 5<br>
* SubIndex 6<br>
* SubIndex 7<br>
* SubIndex 8<br>
* SubIndex 9<br>
* SubIndex 10<br>
* SubIndex 11<br>
* SubIndex 12<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1602[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex7 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex10 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex11 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }}; /* Subindex12 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1602[] = "RxpdoTS Mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 -  */
UINT32 SI2; /* Subindex2 -  */
UINT32 SI3; /* Subindex3 -  */
UINT32 SI4; /* Subindex4 -  */
UINT32 SI5; /* Subindex5 -  */
UINT32 SI6; /* Subindex6 -  */
UINT32 SI7; /* Subindex7 -  */
UINT32 SI8; /* Subindex8 -  */
UINT32 SI9; /* Subindex9 -  */
UINT32 SI10; /* Subindex10 -  */
UINT32 SI11; /* Subindex11 -  */
UINT32 SI12; /* Subindex12 -  */
} OBJ_STRUCT_PACKED_END
TOBJ1602;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1602 RxpdoTSMapping0x1602
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={12,0x70200110,0x702002C0,0x70200310,0x702004C0,0x70200510,0x702006C0,0x70200710,0x702008C0,0x70200910,0x70200AC0,0x70200B10,0x70200CC0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1A00 : Txpdo16 Mapping
******************************************************************************/
/**
* \addtogroup 0x1A00 0x1A00 | Txpdo16 Mapping
* @{
* \brief Object 0x1A00 (Txpdo16 Mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Rxlen1 (16bit)<br>
* SubIndex 2 - RxData1 (192bit)<br>
* SubIndex 3 - Rxlen2<br>
* SubIndex 4 - RxData2<br>
* SubIndex 5 - Rxlen3<br>
* SubIndex 6 - RxData3<br>
* SubIndex 7 - Rxlen4<br>
* SubIndex 8 - RxData4<br>
* SubIndex 9 - Rxlen5<br>
* SubIndex 10 - RxData5<br>
* SubIndex 11 - Rxlen6<br>
* SubIndex 12 - RxData6<br>
* SubIndex 13 - Rxlen7<br>
* SubIndex 14 - RxData7<br>
* SubIndex 15 - Rxlen8<br>
* SubIndex 16 - RxData8<br>
* SubIndex 17 - Rxlen9<br>
* SubIndex 18 - RxData9<br>
* SubIndex 19 - Rxlen10<br>
* SubIndex 20 - RxData10<br>
* SubIndex 21 - Rxlen11<br>
* SubIndex 22 - RxData11<br>
* SubIndex 23 - Rxlen12<br>
* SubIndex 24 - RxData12<br>
* SubIndex 25 - Rxlen13<br>
* SubIndex 26 - RxData13<br>
* SubIndex 27 - Rxlen14<br>
* SubIndex 28 - RxData14<br>
* SubIndex 29 - Rxlen15<br>
* SubIndex 30 - RxData15<br>
* SubIndex 31 - Rxlen16<br>
* SubIndex 32 - RxData16<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A00[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex1 - Rxlen1 (16bit) */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex2 - RxData1 (192bit) */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex3 - Rxlen2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex4 - RxData2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex5 - Rxlen3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex6 - RxData3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex7 - Rxlen4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex8 - RxData4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex9 - Rxlen5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex10 - RxData5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex11 - Rxlen6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex12 - RxData6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex13 - Rxlen7 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex14 - RxData7 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex15 - Rxlen8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex16 - RxData8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex17 - Rxlen9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex18 - RxData9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex19 - Rxlen10 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex20 - RxData10 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex21 - Rxlen11 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex22 - RxData11 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex23 - Rxlen12 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex24 - RxData12 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex25 - Rxlen13 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex26 - RxData13 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex27 - Rxlen14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex28 - RxData14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex29 - Rxlen15 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex30 - RxData15 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex31 - Rxlen16 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }}; /* Subindex32 - RxData16 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1A00[] = "Txpdo16 Mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000"
"SubIndex 013\000"
"SubIndex 014\000"
"SubIndex 015\000"
"SubIndex 016\000"
"SubIndex 017\000"
"SubIndex 018\000"
"SubIndex 019\000"
"SubIndex 020\000"
"SubIndex 021\000"
"SubIndex 022\000"
"SubIndex 023\000"
"SubIndex 024\000"
"SubIndex 025\000"
"SubIndex 026\000"
"SubIndex 027\000"
"SubIndex 028\000"
"SubIndex 029\000"
"SubIndex 030\000"
"SubIndex 031\000"
"SubIndex 032\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Rxlen1 (16bit) */
UINT32 SI2; /* Subindex2 - RxData1 (192bit) */
UINT32 SI3; /* Subindex3 - Rxlen2 */
UINT32 SI4; /* Subindex4 - RxData2 */
UINT32 SI5; /* Subindex5 - Rxlen3 */
UINT32 SI6; /* Subindex6 - RxData3 */
UINT32 SI7; /* Subindex7 - Rxlen4 */
UINT32 SI8; /* Subindex8 - RxData4 */
UINT32 SI9; /* Subindex9 - Rxlen5 */
UINT32 SI10; /* Subindex10 - RxData5 */
UINT32 SI11; /* Subindex11 - Rxlen6 */
UINT32 SI12; /* Subindex12 - RxData6 */
UINT32 SI13; /* Subindex13 - Rxlen7 */
UINT32 SI14; /* Subindex14 - RxData7 */
UINT32 SI15; /* Subindex15 - Rxlen8 */
UINT32 SI16; /* Subindex16 - RxData8 */
UINT32 SI17; /* Subindex17 - Rxlen9 */
UINT32 SI18; /* Subindex18 - RxData9 */
UINT32 SI19; /* Subindex19 - Rxlen10 */
UINT32 SI20; /* Subindex20 - RxData10 */
UINT32 SI21; /* Subindex21 - Rxlen11 */
UINT32 SI22; /* Subindex22 - RxData11 */
UINT32 SI23; /* Subindex23 - Rxlen12 */
UINT32 SI24; /* Subindex24 - RxData12 */
UINT32 SI25; /* Subindex25 - Rxlen13 */
UINT32 SI26; /* Subindex26 - RxData13 */
UINT32 SI27; /* Subindex27 - Rxlen14 */
UINT32 SI28; /* Subindex28 - RxData14 */
UINT32 SI29; /* Subindex29 - Rxlen15 */
UINT32 SI30; /* Subindex30 - RxData15 */
UINT32 SI31; /* Subindex31 - Rxlen16 */
UINT32 SI32; /* Subindex32 - RxData16 */
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 Txpdo16Mapping0x1A00
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={32,0x60000110,0x600002C0,0x60000310,0x600004C0,0x60000510,0x600006C0,0x60000710,0x600008C0,0x60000910,0x60000AC0,0x60000B10,0x60000CC0,0x60000D10,0x60000EC0,0x60000F10,0x600010C0,0x60001110,0x600012C0,0x60001310,0x600014C0,0x60001510,0x600016C0,0x60001710,0x600018C0,0x60001910,0x60001AC0,0x60001B10,0x60001CC0,0x60001D10,0x60001EC0,0x60001F10,0x600020C0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1A01 : Txpdo6 Mapping
******************************************************************************/
/**
* \addtogroup 0x1A01 0x1A01 | Txpdo6 Mapping
* @{
* \brief Object 0x1A01 (Txpdo6 Mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Rxlen1<br>
* SubIndex 2 - RxData1<br>
* SubIndex 3 - Rxlen2<br>
* SubIndex 4 - RxData2<br>
* SubIndex 5 - Rxlen3<br>
* SubIndex 6 - RxData3<br>
* SubIndex 7 - Rxlen4<br>
* SubIndex 8 - RxData4<br>
* SubIndex 9 - Rxlen5<br>
* SubIndex 10 - RxData5<br>
* SubIndex 11 - Rxlen6<br>
* SubIndex 12 - RxData6<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A01[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex1 - Rxlen1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex2 - RxData1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex3 - Rxlen2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex4 - RxData2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex5 - Rxlen3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex6 - RxData3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex7 - Rxlen4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex8 - RxData4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex9 - Rxlen5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex10 - RxData5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex11 - Rxlen6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }}; /* Subindex12 - RxData6 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1A01[] = "Txpdo6 Mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Rxlen1 */
UINT32 SI2; /* Subindex2 - RxData1 */
UINT32 SI3; /* Subindex3 - Rxlen2 */
UINT32 SI4; /* Subindex4 - RxData2 */
UINT32 SI5; /* Subindex5 - Rxlen3 */
UINT32 SI6; /* Subindex6 - RxData3 */
UINT32 SI7; /* Subindex7 - Rxlen4 */
UINT32 SI8; /* Subindex8 - RxData4 */
UINT32 SI9; /* Subindex9 - Rxlen5 */
UINT32 SI10; /* Subindex10 - RxData5 */
UINT32 SI11; /* Subindex11 - Rxlen6 */
UINT32 SI12; /* Subindex12 - RxData6 */
} OBJ_STRUCT_PACKED_END
TOBJ1A01;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A01 Txpdo6Mapping0x1A01
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={12,0x60100110,0x601002C0,0x60100310,0x601004C0,0x60100510,0x601006C0,0x60100710,0x601008C0,0x60100910,0x60100AC0,0x60100B10,0x60100CC0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1A02 : TxpdoTS Mapping
******************************************************************************/
/**
* \addtogroup 0x1A02 0x1A02 | TxpdoTS Mapping
* @{
* \brief Object 0x1A02 (TxpdoTS Mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Rxlen1<br>
* SubIndex 2 - RxData1<br>
* SubIndex 3 - Rxlen2<br>
* SubIndex 4 - RxData2<br>
* SubIndex 5 - Rxlen3<br>
* SubIndex 6 - RxData3<br>
* SubIndex 7 - Rxlen4<br>
* SubIndex 8 - RxData4<br>
* SubIndex 9 - Rxlen5<br>
* SubIndex 10 - RxData5<br>
* SubIndex 11 - Rxlen6<br>
* SubIndex 12 - RxData6<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A02[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex1 - Rxlen1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex2 - RxData1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex3 - Rxlen2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex4 - RxData2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex5 - Rxlen3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex6 - RxData3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex7 - Rxlen4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex8 - RxData4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex9 - Rxlen5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex10 - RxData5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex11 - Rxlen6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }}; /* Subindex12 - RxData6 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1A02[] = "TxpdoTS Mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Rxlen1 */
UINT32 SI2; /* Subindex2 - RxData1 */
UINT32 SI3; /* Subindex3 - Rxlen2 */
UINT32 SI4; /* Subindex4 - RxData2 */
UINT32 SI5; /* Subindex5 - Rxlen3 */
UINT32 SI6; /* Subindex6 - RxData3 */
UINT32 SI7; /* Subindex7 - Rxlen4 */
UINT32 SI8; /* Subindex8 - RxData4 */
UINT32 SI9; /* Subindex9 - Rxlen5 */
UINT32 SI10; /* Subindex10 - RxData5 */
UINT32 SI11; /* Subindex11 - Rxlen6 */
UINT32 SI12; /* Subindex12 - RxData6 */
} OBJ_STRUCT_PACKED_END
TOBJ1A02;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A02 TxpdoTSMapping0x1A02
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={12,0x60200110,0x602002C0,0x60200310,0x602004C0,0x60200510,0x602006C0,0x60200710,0x602008C0,0x60200910,0x60200AC0,0x60200B10,0x60200CC0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1C12 : SM2 RxPDO Assign
******************************************************************************/
/**
* \addtogroup 0x1C12 0x1C12 | SM2 RxPDO Assign
* @{
* \brief Object 0x1C12 (SM2 RxPDO Assign) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
* 
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C12[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0x1C12[] = "SM2 RxPDO Assign\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[3];  /**< \brief Subindex 1 - 3 */
} OBJ_STRUCT_PACKED_END
TOBJ1C12;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C12 sRxPDOassign
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={3,{0x1600,0x1601,0x1602}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1C13 : SM3 TxPDO Assign
******************************************************************************/
/**
* \addtogroup 0x1C13 0x1C13 | SM3 TxPDO Assign
* @{
* \brief Object 0x1C13 (SM3 TxPDO Assign) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
* 
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C13[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0x1C13[] = "SM3 TxPDO Assign\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[3];  /**< \brief Subindex 1 - 3 */
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C13 sTxPDOassign
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={3,{0x1A00,0x1A01,0x1A02}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x6000 : Txpdo16
******************************************************************************/
/**
* \addtogroup 0x6000 0x6000 | Txpdo16
* @{
* \brief Object 0x6000 (Txpdo16) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Rxlen_1<br>
* SubIndex 2 - RxData_1<br>
* SubIndex 3 - Rxlen_2<br>
* SubIndex 4 - RxData_2<br>
* SubIndex 5 - Rxlen_3<br>
* SubIndex 6 - RxData_3<br>
* SubIndex 7 - Rxlen_4<br>
* SubIndex 8 - RxData_4<br>
* SubIndex 9 - Rxlen_5<br>
* SubIndex 10 - RxData_5<br>
* SubIndex 11 - Rxlen_6<br>
* SubIndex 12 - RxData_6<br>
* SubIndex 13 - Rxlen_7<br>
* SubIndex 14 - RxData_7<br>
* SubIndex 15 - Rxlen_8<br>
* SubIndex 16 - RxData_8<br>
* SubIndex 17 - Rxlen_9<br>
* SubIndex 18 - RxData_9<br>
* SubIndex 19 - Rxlen_10<br>
* SubIndex 20 - RxData_10<br>
* SubIndex 21 - Rxlen_11<br>
* SubIndex 22 - RxData_11<br>
* SubIndex 23 - Rxlen_12<br>
* SubIndex 24 - RxData_12<br>
* SubIndex 25 - Rxlen_13<br>
* SubIndex 26 - RxData_13<br>
* SubIndex 27 - Rxlen_14<br>
* SubIndex 28 - RxData_14<br>
* SubIndex 29 - Rxlen_15<br>
* SubIndex 30 - RxData_15<br>
* SubIndex 31 - Rxlen_16<br>
* SubIndex 32 - RxData_16<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex1 - Rxlen_1 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex2 - RxData_1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex3 - Rxlen_2 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex4 - RxData_2 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex5 - Rxlen_3 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex6 - RxData_3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex7 - Rxlen_4 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex8 - RxData_4 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex9 - Rxlen_5 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex10 - RxData_5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex11 - Rxlen_6 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex12 - RxData_6 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex13 - Rxlen_7 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex14 - RxData_7 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex15 - Rxlen_8 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex16 - RxData_8 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex17 - Rxlen_9 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex18 - RxData_9 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex19 - Rxlen_10 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex20 - RxData_10 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex21 - Rxlen_11 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex22 - RxData_11 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex23 - Rxlen_12 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex24 - RxData_12 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex25 - Rxlen_13 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex26 - RxData_13 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex27 - Rxlen_14 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex28 - RxData_14 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex29 - Rxlen_15 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex30 - RxData_15 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex31 - Rxlen_16 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }}; /* Subindex32 - RxData_16 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "Txpdo16\000"
"Rxlen_1\000"
"RxData_1\000"
"Rxlen_2\000"
"RxData_2\000"
"Rxlen_3\000"
"RxData_3\000"
"Rxlen_4\000"
"RxData_4\000"
"Rxlen_5\000"
"RxData_5\000"
"Rxlen_6\000"
"RxData_6\000"
"Rxlen_7\000"
"RxData_7\000"
"Rxlen_8\000"
"RxData_8\000"
"Rxlen_9\000"
"RxData_9\000"
"Rxlen_10\000"
"RxData_10\000"
"Rxlen_11\000"
"RxData_11\000"
"Rxlen_12\000"
"RxData_12\000"
"Rxlen_13\000"
"RxData_13\000"
"Rxlen_14\000"
"RxData_14\000"
"Rxlen_15\000"
"RxData_15\000"
"Rxlen_16\000"
"RxData_16\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 Rxlen_1; /* Subindex1 - Rxlen_1 */
CHAR RxData_1[24]; /* Subindex2 - RxData_1 */
UINT16 Rxlen_2; /* Subindex3 - Rxlen_2 */
CHAR RxData_2[24]; /* Subindex4 - RxData_2 */
UINT16 Rxlen_3; /* Subindex5 - Rxlen_3 */
CHAR RxData_3[24]; /* Subindex6 - RxData_3 */
UINT16 Rxlen_4; /* Subindex7 - Rxlen_4 */
CHAR RxData_4[24]; /* Subindex8 - RxData_4 */
UINT16 Rxlen_5; /* Subindex9 - Rxlen_5 */
CHAR RxData_5[24]; /* Subindex10 - RxData_5 */
UINT16 Rxlen_6; /* Subindex11 - Rxlen_6 */
CHAR RxData_6[24]; /* Subindex12 - RxData_6 */
UINT16 Rxlen_7; /* Subindex13 - Rxlen_7 */
CHAR RxData_7[24]; /* Subindex14 - RxData_7 */
UINT16 Rxlen_8; /* Subindex15 - Rxlen_8 */
CHAR RxData_8[24]; /* Subindex16 - RxData_8 */
UINT16 Rxlen_9; /* Subindex17 - Rxlen_9 */
CHAR RxData_9[24]; /* Subindex18 - RxData_9 */
UINT16 Rxlen_10; /* Subindex19 - Rxlen_10 */
CHAR RxData_10[24]; /* Subindex20 - RxData_10 */
UINT16 Rxlen_11; /* Subindex21 - Rxlen_11 */
CHAR RxData_11[24]; /* Subindex22 - RxData_11 */
UINT16 Rxlen_12; /* Subindex23 - Rxlen_12 */
CHAR RxData_12[24]; /* Subindex24 - RxData_12 */
UINT16 Rxlen_13; /* Subindex25 - Rxlen_13 */
CHAR RxData_13[24]; /* Subindex26 - RxData_13 */
UINT16 Rxlen_14; /* Subindex27 - Rxlen_14 */
CHAR RxData_14[24]; /* Subindex28 - RxData_14 */
UINT16 Rxlen_15; /* Subindex29 - Rxlen_15 */
CHAR RxData_15[24]; /* Subindex30 - RxData_15 */
UINT16 Rxlen_16; /* Subindex31 - Rxlen_16 */
CHAR RxData_16[24]; /* Subindex32 - RxData_16 */
} OBJ_STRUCT_PACKED_END
TOBJ6000;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 Txpdo160x6000
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={32,0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        "}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x6010 : Txpdo6
******************************************************************************/
/**
* \addtogroup 0x6010 0x6010 | Txpdo6
* @{
* \brief Object 0x6010 (Txpdo6) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Rxlen_17<br>
* SubIndex 2 - RxData_17<br>
* SubIndex 3 - Rxlen_18<br>
* SubIndex 4 - RxData_18<br>
* SubIndex 5 - Rxlen_19<br>
* SubIndex 6 - RxData_19<br>
* SubIndex 7 - Rxlen_20<br>
* SubIndex 8 - RxData_20<br>
* SubIndex 9 - Rxlen_21<br>
* SubIndex 10 - RxData_21<br>
* SubIndex 11 - Rxlen_22<br>
* SubIndex 12 - RxData_22<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex1 - Rxlen_17 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex2 - RxData_17 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex3 - Rxlen_18 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex4 - RxData_18 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex5 - Rxlen_19 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex6 - RxData_19 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex7 - Rxlen_20 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex8 - RxData_20 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex9 - Rxlen_21 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }, /* Subindex10 - RxData_21 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex11 - Rxlen_22 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READ }}; /* Subindex12 - RxData_22 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6010[] = "Txpdo6\000"
"Rxlen_17\000"
"RxData_17\000"
"Rxlen_18\000"
"RxData_18\000"
"Rxlen_19\000"
"RxData_19\000"
"Rxlen_20\000"
"RxData_20\000"
"Rxlen_21\000"
"RxData_21\000"
"Rxlen_22\000"
"RxData_22\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 Rxlen_17; /* Subindex1 - Rxlen_17 */
CHAR RxData_17[24]; /* Subindex2 - RxData_17 */
UINT16 Rxlen_18; /* Subindex3 - Rxlen_18 */
CHAR RxData_18[24]; /* Subindex4 - RxData_18 */
UINT16 Rxlen_19; /* Subindex5 - Rxlen_19 */
CHAR RxData_19[24]; /* Subindex6 - RxData_19 */
UINT16 Rxlen_20; /* Subindex7 - Rxlen_20 */
CHAR RxData_20[24]; /* Subindex8 - RxData_20 */
UINT16 Rxlen_21; /* Subindex9 - Rxlen_21 */
CHAR RxData_21[24]; /* Subindex10 - RxData_21 */
UINT16 Rxlen_22; /* Subindex11 - Rxlen_22 */
CHAR RxData_22[24]; /* Subindex12 - RxData_22 */
} OBJ_STRUCT_PACKED_END
TOBJ6010;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6010 Txpdo60x6010
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={12,0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        "}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x6020 : Txpdo_TS
******************************************************************************/
/**
* \addtogroup 0x6020 0x6020 | Txpdo_TS
* @{
* \brief Object 0x6020 (Txpdo_TS) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Rxlen_1<br>
* SubIndex 2 - RxData_1<br>
* SubIndex 3 - Rxlen_2<br>
* SubIndex 4 - RxData_2<br>
* SubIndex 5 - Rxlen_3<br>
* SubIndex 6 - RxData_3<br>
* SubIndex 7 - Rxlen_4<br>
* SubIndex 8 - RxData_4<br>
* SubIndex 9 - Rxlen_5<br>
* SubIndex 10 - RxData_5<br>
* SubIndex 11 - Rxlen_6<br>
* SubIndex 12 - RxData_6<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6020[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex1 - Rxlen_1 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }, /* Subindex2 - RxData_1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex3 - Rxlen_2 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }, /* Subindex4 - RxData_2 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex5 - Rxlen_3 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }, /* Subindex6 - RxData_3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex7 - Rxlen_4 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }, /* Subindex8 - RxData_4 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex9 - Rxlen_5 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }, /* Subindex10 - RxData_5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex11 - Rxlen_6 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }}; /* Subindex12 - RxData_6 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6020[] = "Txpdo_TS\000"
"Rxlen_1\000"
"RxData_1\000"
"Rxlen_2\000"
"RxData_2\000"
"Rxlen_3\000"
"RxData_3\000"
"Rxlen_4\000"
"RxData_4\000"
"Rxlen_5\000"
"RxData_5\000"
"Rxlen_6\000"
"RxData_6\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 Rxlen_1; /* Subindex1 - Rxlen_1 */
CHAR RxData_1[32]; /* Subindex2 - RxData_1 */
UINT16 Rxlen_2; /* Subindex3 - Rxlen_2 */
CHAR RxData_2[32]; /* Subindex4 - RxData_2 */
UINT16 Rxlen_3; /* Subindex5 - Rxlen_3 */
CHAR RxData_3[32]; /* Subindex6 - RxData_3 */
UINT16 Rxlen_4; /* Subindex7 - Rxlen_4 */
CHAR RxData_4[32]; /* Subindex8 - RxData_4 */
UINT16 Rxlen_5; /* Subindex9 - Rxlen_5 */
CHAR RxData_5[32]; /* Subindex10 - RxData_5 */
UINT16 Rxlen_6; /* Subindex11 - Rxlen_6 */
CHAR RxData_6[32]; /* Subindex12 - RxData_6 */
} OBJ_STRUCT_PACKED_END
TOBJ6020;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6020 Txpdo_TS0x6020
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={12,0,"                                ",0,"                                ",0,"                                ",0,"                                ",0,"                                ",0,"                                "}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x7000 : Rxpdo16
******************************************************************************/
/**
* \addtogroup 0x7000 0x7000 | Rxpdo16
* @{
* \brief Object 0x7000 (Rxpdo16) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Txlen_1<br>
* SubIndex 2 - TxData_1<br>
* SubIndex 3 - Txlen_2<br>
* SubIndex 4 - TxData_2<br>
* SubIndex 5 - Txlen_3<br>
* SubIndex 6 - TxData_3<br>
* SubIndex 7 - Txlen_4<br>
* SubIndex 8 - TxData_4<br>
* SubIndex 9 - Txlen_5<br>
* SubIndex 10 - TxData_5<br>
* SubIndex 11 - Txlen_6<br>
* SubIndex 12 - TxData_6<br>
* SubIndex 13 - Txlen_7<br>
* SubIndex 14 - TxData_7<br>
* SubIndex 15 - Txlen_8<br>
* SubIndex 16 - TxData_8<br>
* SubIndex 17 - Txlen_9<br>
* SubIndex 18 - TxData_9<br>
* SubIndex 19 - Txlen_10<br>
* SubIndex 20 - TxData_10<br>
* SubIndex 21 - Txlen_11<br>
* SubIndex 22 - TxData_11<br>
* SubIndex 23 - Txlen_12<br>
* SubIndex 24 - TxData_12<br>
* SubIndex 25 - Txlen_13<br>
* SubIndex 26 - TxData_13<br>
* SubIndex 27 - Txlen_14<br>
* SubIndex 28 - TxData_14<br>
* SubIndex 29 - Txlen_15<br>
* SubIndex 30 - TxData_15<br>
* SubIndex 31 - Txlen_16<br>
* SubIndex 32 - TxData_16<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex1 - Txlen_1 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex2 - TxData_1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex3 - Txlen_2 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex4 - TxData_2 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex5 - Txlen_3 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex6 - TxData_3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex7 - Txlen_4 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex8 - TxData_4 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex9 - Txlen_5 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex10 - TxData_5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex11 - Txlen_6 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex12 - TxData_6 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex13 - Txlen_7 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex14 - TxData_7 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex15 - Txlen_8 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex16 - TxData_8 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex17 - Txlen_9 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex18 - TxData_9 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex19 - Txlen_10 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex20 - TxData_10 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex21 - Txlen_11 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex22 - TxData_11 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex23 - Txlen_12 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex24 - TxData_12 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex25 - Txlen_13 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex26 - TxData_13 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex27 - Txlen_14 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex28 - TxData_14 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex29 - Txlen_15 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex30 - TxData_15 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex31 - Txlen_16 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }}; /* Subindex32 - TxData_16 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7000[] = "Rxpdo16\000"
"Txlen_1\000"
"TxData_1\000"
"Txlen_2\000"
"TxData_2\000"
"Txlen_3\000"
"TxData_3\000"
"Txlen_4\000"
"TxData_4\000"
"Txlen_5\000"
"TxData_5\000"
"Txlen_6\000"
"TxData_6\000"
"Txlen_7\000"
"TxData_7\000"
"Txlen_8\000"
"TxData_8\000"
"Txlen_9\000"
"TxData_9\000"
"Txlen_10\000"
"TxData_10\000"
"Txlen_11\000"
"TxData_11\000"
"Txlen_12\000"
"TxData_12\000"
"Txlen_13\000"
"TxData_13\000"
"Txlen_14\000"
"TxData_14\000"
"Txlen_15\000"
"TxData_15\000"
"Txlen_16\000"
"TxData_16\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 Txlen_1; /* Subindex1 - Txlen_1 */
CHAR TxData_1[24]; /* Subindex2 - TxData_1 */
UINT16 Txlen_2; /* Subindex3 - Txlen_2 */
CHAR TxData_2[24]; /* Subindex4 - TxData_2 */
UINT16 Txlen_3; /* Subindex5 - Txlen_3 */
CHAR TxData_3[24]; /* Subindex6 - TxData_3 */
UINT16 Txlen_4; /* Subindex7 - Txlen_4 */
CHAR TxData_4[24]; /* Subindex8 - TxData_4 */
UINT16 Txlen_5; /* Subindex9 - Txlen_5 */
CHAR TxData_5[24]; /* Subindex10 - TxData_5 */
UINT16 Txlen_6; /* Subindex11 - Txlen_6 */
CHAR TxData_6[24]; /* Subindex12 - TxData_6 */
UINT16 Txlen_7; /* Subindex13 - Txlen_7 */
CHAR TxData_7[24]; /* Subindex14 - TxData_7 */
UINT16 Txlen_8; /* Subindex15 - Txlen_8 */
CHAR TxData_8[24]; /* Subindex16 - TxData_8 */
UINT16 Txlen_9; /* Subindex17 - Txlen_9 */
CHAR TxData_9[24]; /* Subindex18 - TxData_9 */
UINT16 Txlen_10; /* Subindex19 - Txlen_10 */
CHAR TxData_10[24]; /* Subindex20 - TxData_10 */
UINT16 Txlen_11; /* Subindex21 - Txlen_11 */
CHAR TxData_11[24]; /* Subindex22 - TxData_11 */
UINT16 Txlen_12; /* Subindex23 - Txlen_12 */
CHAR TxData_12[24]; /* Subindex24 - TxData_12 */
UINT16 Txlen_13; /* Subindex25 - Txlen_13 */
CHAR TxData_13[24]; /* Subindex26 - TxData_13 */
UINT16 Txlen_14; /* Subindex27 - Txlen_14 */
CHAR TxData_14[24]; /* Subindex28 - TxData_14 */
UINT16 Txlen_15; /* Subindex29 - Txlen_15 */
CHAR TxData_15[24]; /* Subindex30 - TxData_15 */
UINT16 Txlen_16; /* Subindex31 - Txlen_16 */
CHAR TxData_16[24]; /* Subindex32 - TxData_16 */
} OBJ_STRUCT_PACKED_END
TOBJ7000;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7000 Rxpdo160x7000
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={32,0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        "}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x7010 : Rxpdo6
******************************************************************************/
/**
* \addtogroup 0x7010 0x7010 | Rxpdo6
* @{
* \brief Object 0x7010 (Rxpdo6) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Txlen_17<br>
* SubIndex 2 - TxData_17<br>
* SubIndex 3 - Txlen_18<br>
* SubIndex 4 - TxData_18<br>
* SubIndex 5 - Txlen_19<br>
* SubIndex 6 - TxData_19<br>
* SubIndex 7 - Txlen_20<br>
* SubIndex 8 - TxData_20<br>
* SubIndex 9 - Txlen_21<br>
* SubIndex 10 - TxData_21<br>
* SubIndex 11 - Txlen_22<br>
* SubIndex 12 - TxData_22<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex1 - Txlen_17 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex2 - TxData_17 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex3 - Txlen_18 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex4 - TxData_18 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex5 - Txlen_19 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex6 - TxData_19 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex7 - Txlen_20 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex8 - TxData_20 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex9 - Txlen_21 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }, /* Subindex10 - TxData_21 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex11 - Txlen_22 */
{ DEFTYPE_VISIBLESTRING , 0xC0 , ACCESS_READWRITE }}; /* Subindex12 - TxData_22 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "Rxpdo6\000"
"Txlen_17\000"
"TxData_17\000"
"Txlen_18\000"
"TxData_18\000"
"Txlen_19\000"
"TxData_19\000"
"Txlen_20\000"
"TxData_20\000"
"Txlen_21\000"
"TxData_21\000"
"Txlen_22\000"
"TxData_22\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 Txlen_17; /* Subindex1 - Txlen_17 */
CHAR TxData_17[24]; /* Subindex2 - TxData_17 */
UINT16 Txlen_18; /* Subindex3 - Txlen_18 */
CHAR TxData_18[24]; /* Subindex4 - TxData_18 */
UINT16 Txlen_19; /* Subindex5 - Txlen_19 */
CHAR TxData_19[24]; /* Subindex6 - TxData_19 */
UINT16 Txlen_20; /* Subindex7 - Txlen_20 */
CHAR TxData_20[24]; /* Subindex8 - TxData_20 */
UINT16 Txlen_21; /* Subindex9 - Txlen_21 */
CHAR TxData_21[24]; /* Subindex10 - TxData_21 */
UINT16 Txlen_22; /* Subindex11 - Txlen_22 */
CHAR TxData_22[24]; /* Subindex12 - TxData_22 */
} OBJ_STRUCT_PACKED_END
TOBJ7010;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7010 Rxpdo60x7010
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={12,0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        ",0,"                        "}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x7020 : Rxpdo6_TS
******************************************************************************/
/**
* \addtogroup 0x7020 0x7020 | Rxpdo6_TS
* @{
* \brief Object 0x7020 (Rxpdo6_TS) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Txlen_1<br>
* SubIndex 2 - TxData_1<br>
* SubIndex 3 - Txlen_2<br>
* SubIndex 4 - TxData_2<br>
* SubIndex 5 - Txlen_3<br>
* SubIndex 6 - TxData_3<br>
* SubIndex 7 - Txlen_4<br>
* SubIndex 8 - TxData_4<br>
* SubIndex 9 - Txlen_5<br>
* SubIndex 10 - TxData_5<br>
* SubIndex 11 - Txlen_6<br>
* SubIndex 12 - TxData_6<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7020[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex1 - Txlen_1 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READWRITE }, /* Subindex2 - TxData_1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex3 - Txlen_2 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READWRITE }, /* Subindex4 - TxData_2 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex5 - Txlen_3 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READWRITE }, /* Subindex6 - TxData_3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex7 - Txlen_4 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READWRITE }, /* Subindex8 - TxData_4 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex9 - Txlen_5 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READWRITE }, /* Subindex10 - TxData_5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex11 - Txlen_6 */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READWRITE }}; /* Subindex12 - TxData_6 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7020[] = "Rxpdo6_TS\000"
"Txlen_1\000"
"TxData_1\000"
"Txlen_2\000"
"TxData_2\000"
"Txlen_3\000"
"TxData_3\000"
"Txlen_4\000"
"TxData_4\000"
"Txlen_5\000"
"TxData_5\000"
"Txlen_6\000"
"TxData_6\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 Txlen_1; /* Subindex1 - Txlen_1 */
CHAR TxData_1[32]; /* Subindex2 - TxData_1 */
UINT16 Txlen_2; /* Subindex3 - Txlen_2 */
CHAR TxData_2[32]; /* Subindex4 - TxData_2 */
UINT16 Txlen_3; /* Subindex5 - Txlen_3 */
CHAR TxData_3[32]; /* Subindex6 - TxData_3 */
UINT16 Txlen_4; /* Subindex7 - Txlen_4 */
CHAR TxData_4[32]; /* Subindex8 - TxData_4 */
UINT16 Txlen_5; /* Subindex9 - Txlen_5 */
CHAR TxData_5[32]; /* Subindex10 - TxData_5 */
UINT16 Txlen_6; /* Subindex11 - Txlen_6 */
CHAR TxData_6[32]; /* Subindex12 - TxData_6 */
} OBJ_STRUCT_PACKED_END
TOBJ7020;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7020 Rxpdo6_TS0x7020
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={12,0,"                                ",0,"                                ",0,"                                ",0,"                                ",0,"                                ",0,"                                "}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x8000 : DevInfo
******************************************************************************/
/**
* \addtogroup 0x8000 0x8000 | DevInfo
* @{
* \brief Object 0x8000 (DevInfo) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Name<br>
* SubIndex 2 - Hv<br>
* SubIndex 3 - Sv<br>
* SubIndex 4 - Sn<br>
* SubIndex 5 - BuildTime<br>
* SubIndex 6 - BR_canfd_ctrl<br>
* SubIndex 7 - BR_canfd_data<br>
* SubIndex 8 - BR_485<br>
* SubIndex 9 - Use120<br>
* SubIndex 10 - Use485<br>
* SubIndex 11 - UseModbus<br>
* SubIndex 12 - 保留<br>
* SubIndex 13 - DevID<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x8000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }, /* Subindex1 - Name */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }, /* Subindex2 - Hv */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READ }, /* Subindex3 - Sv */
{ DEFTYPE_VISIBLESTRING , 0x100 , ACCESS_READWRITE }, /* Subindex4 - Sn */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - BuildTime */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex6 - BR_canfd_ctrl */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex7 - BR_canfd_data */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }, /* Subindex8 - BR_485 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READWRITE }, /* Subindex9 - Use120 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READWRITE }, /* Subindex10 - Use485 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READWRITE }, /* Subindex11 - UseModbus */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READWRITE }, /* Subindex12 - 保留 */
{ DEFTYPE_VISIBLESTRING , 0x30 , ACCESS_READWRITE }}; /* Subindex13 - DevID */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x8000[] = "DevInfo\000"
"Name\000"
"Hv\000"
"Sv\000"
"Sn\000"
"BuildTime\000"
"BR_canfd_ctrl\000"
"BR_canfd_data\000"
"BR_485\000"
"Use120\000"
"Use485\000"
"UseModbus\000"
"SubIndex 012\000"
"DevID\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
CHAR Name[32]; /* Subindex1 - Name */
CHAR Hv[32]; /* Subindex2 - Hv */
CHAR Sv[32]; /* Subindex3 - Sv */
CHAR Sn[32]; /* Subindex4 - Sn */
UINT32 BuildTime; /* Subindex5 - BuildTime */
UINT32 BR_canfd_ctrl; /* Subindex6 - BR_canfd_ctrl */
UINT32 BR_canfd_data; /* Subindex7 - BR_canfd_data */
UINT32 BR_485; /* Subindex8 - BR_485 */
UINT8 Use120; /* Subindex9 - Use120 */
UINT8 Use485; /* Subindex10 - Use485 */
UINT8 UseModbus; /* Subindex11 - UseModbus */
UINT8 SubIndex012; /* Subindex12 - SubIndex 012 */
CHAR DevID[6]; /* Subindex13 - DevID */
} OBJ_STRUCT_PACKED_END
TOBJ8000;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ8000 DevInfo0x8000
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={13,"                                ","                                ","                                ","                                ",0x20240910,0x000F4240,0x004C4B40,0x004C4B40,0,0,0,0,"      "}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x9000 : SimulateCANFD
******************************************************************************/
/**
* \addtogroup 0x9000 0x9000 | SimulateCANFD
* @{
* \brief Object 0x9000 (SimulateCANFD) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - TxIndex<br>
* SubIndex 2 - TxID<br>
* SubIndex 3 - TxLen<br>
* SubIndex 4 - TxData<br>
* SubIndex 5 - RxIndex<br>
* SubIndex 6 - RxID<br>
* SubIndex 7 - RxLen<br>
* SubIndex 8 - RxData<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x9000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex1 - TxIndex */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex2 - TxID */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex3 - TxLen */
{ DEFTYPE_VISIBLESTRING , 0x200 , ACCESS_READWRITE }, /* Subindex4 - TxData */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex5 - RxIndex */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex6 - RxID */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE }, /* Subindex7 - RxLen */
{ DEFTYPE_VISIBLESTRING , 0x200 , ACCESS_READWRITE }}; /* Subindex8 - RxData */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x9000[] = "SimulateCANFD\000"
"TxIndex\000"
"TxID\000"
"TxLen\000"
"TxData\000"
"RxIndex\000"
"RxID\000"
"RxLen\000"
"RxData\000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 TxIndex; /* Subindex1 - TxIndex */
UINT16 TxID; /* Subindex2 - TxID */
UINT16 TxLen; /* Subindex3 - TxLen */
CHAR TxData[64]; /* Subindex4 - TxData */
UINT16 RxIndex; /* Subindex5 - RxIndex */
UINT16 RxID; /* Subindex6 - RxID */
UINT16 RxLen; /* Subindex7 - RxLen */
CHAR RxData[64]; /* Subindex8 - RxData */
} OBJ_STRUCT_PACKED_END
TOBJ9000;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ9000 SimulateCANFD0x9000
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={8,0,0,0,"                                                                ",0,0,0,"                                                                "}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x9001 : Param Action
******************************************************************************/
/**
* \addtogroup 0x9001 0x9001 | Param Action
* @{
* \brief Object 0x9001 (Param Action) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x9001 = { DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0x9001[] = "Param Action\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO UINT16 ParamAction0x9001
#if defined(_RYHAND_) && (_RYHAND_ == 1)
= 0x0000
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF000 : Modular Device Profile
******************************************************************************/
/**
* \addtogroup 0xF000 0xF000 | Modular Device Profile
* @{
* \brief Object 0xF000 (Modular Device Profile) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Index distance <br>
* SubIndex 2 - Maximum number of modules <br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex1 - Index distance  */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }}; /* Subindex2 - Maximum number of modules  */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF000[] = "Modular Device Profile\000"
"Index distance \000"
"Maximum number of modules \000\377";
#endif //#ifdef _OBJD_

#ifndef _RYHAND_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 IndexDistance; /* Subindex1 - Index distance  */
UINT16 MaximumNumberOfModules; /* Subindex2 - Maximum number of modules  */
} OBJ_STRUCT_PACKED_END
TOBJF000;
#endif //#ifndef _RYHAND_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJF000 ModularDeviceProfile0xF000
#if defined(_RYHAND_) && (_RYHAND_ == 1)
={2,0x0010,0x0008}
#endif
;
/** @}*/





PROTO UINT8 WriteSimulateCANFD(UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess);


#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1600 */
{NULL , NULL ,  0x1600 , {DEFTYPE_PDOMAPPING , 32 | (OBJCODE_REC << 8)} , asEntryDesc0x1600 , aName0x1600 , &Rxpdo16Mapping0x1600 , NULL , NULL , 0x0000 },
/* Object 0x1601 */
{NULL , NULL ,  0x1601 , {DEFTYPE_PDOMAPPING , 12 | (OBJCODE_REC << 8)} , asEntryDesc0x1601 , aName0x1601 , &Rxpdo6Mapping0x1601 , NULL , NULL , 0x0000 },
/* Object 0x1602 */
{NULL , NULL ,  0x1602 , {DEFTYPE_PDOMAPPING , 12 | (OBJCODE_REC << 8)} , asEntryDesc0x1602 , aName0x1602 , &RxpdoTSMapping0x1602 , NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 32 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &Txpdo16Mapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1A01 */
{NULL , NULL ,  0x1A01 , {DEFTYPE_PDOMAPPING , 12 | (OBJCODE_REC << 8)} , asEntryDesc0x1A01 , aName0x1A01 , &Txpdo6Mapping0x1A01 , NULL , NULL , 0x0000 },
/* Object 0x1A02 */
{NULL , NULL ,  0x1A02 , {DEFTYPE_PDOMAPPING , 12 | (OBJCODE_REC << 8)} , asEntryDesc0x1A02 , aName0x1A02 , &TxpdoTSMapping0x1A02 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 3 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 3 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 32 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &Txpdo160x6000 , NULL , NULL , 0x0000 },
/* Object 0x6010 */
{NULL , NULL ,  0x6010 , {DEFTYPE_RECORD , 12 | (OBJCODE_REC << 8)} , asEntryDesc0x6010 , aName0x6010 , &Txpdo60x6010 , NULL , NULL , 0x0000 },
/* Object 0x6020 */
{NULL , NULL ,  0x6020 , {DEFTYPE_RECORD , 12 | (OBJCODE_REC << 8)} , asEntryDesc0x6020 , aName0x6020 , &Txpdo_TS0x6020 , NULL , NULL , 0x0000 },
/* Object 0x7000 */
{NULL , NULL ,  0x7000 , {DEFTYPE_RECORD , 32 | (OBJCODE_REC << 8)} , asEntryDesc0x7000 , aName0x7000 , &Rxpdo160x7000 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 12 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &Rxpdo60x7010 , NULL , NULL , 0x0000 },
/* Object 0x7020 */
{NULL , NULL ,  0x7020 , {DEFTYPE_RECORD , 12 | (OBJCODE_REC << 8)} , asEntryDesc0x7020 , aName0x7020 , &Rxpdo6_TS0x7020 , NULL , NULL , 0x0000 },
/* Object 0x8000 */
{NULL , NULL ,  0x8000 , {DEFTYPE_RECORD , 13 | (OBJCODE_REC << 8)} , asEntryDesc0x8000 , aName0x8000 , &DevInfo0x8000 , NULL , NULL , 0x0000 },
/* Object 0x9000 */
{NULL , NULL ,  0x9000 , {DEFTYPE_RECORD , 8 | (OBJCODE_REC << 8)} , asEntryDesc0x9000 , aName0x9000 , &SimulateCANFD0x9000 , NULL , WriteSimulateCANFD , 0x0000 },
/* Object 0x9001 */
{NULL , NULL ,  0x9001 , {DEFTYPE_UNSIGNED16 , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0x9001 , aName0x9001 , &ParamAction0x9001 , NULL , NULL , 0x0000 },
/* Object 0xF000 */
{NULL , NULL ,  0xF000 , {DEFTYPE_RECORD , 2 | (OBJCODE_REC << 8)} , asEntryDesc0xF000 , aName0xF000 , &ModularDeviceProfile0xF000 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _RYHAND_OBJECTS_H_
