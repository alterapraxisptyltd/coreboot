/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "agesawrapper.h"
#include "amdlib.h"
#include "BiosCallOuts.h"
#include "heapManager.h"
#include "SB800.h"
#include <northbridge/amd/agesa/family14/dimmSpd.h>
#include <stdlib.h>

/* Should AGESA_GNB_PCIE_SLOT_RESET use agesa_NoopSuccess?
 *
 * Board is known to have some issues with integrated NIC and
 * might need implementation to drive some GPIOs.
 */

static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_ALLOCATE_BUFFER,			agesa_AllocateBuffer },
	{AGESA_DEALLOCATE_BUFFER,		agesa_DeallocateBuffer },
	{AGESA_LOCATE_BUFFER,			agesa_LocateBuffer },
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			BiosReadSpd },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DRAM_INIT,		board_BeforeDramInit },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
	{AGESA_GNB_PCIE_SLOT_RESET,		agesa_NoopUnsupported },
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/*  Call the host environment interface to provide a user hook opportunity. */
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
  AGESA_STATUS      Status;
  UINTN             FcnData;
  MEM_DATA_STRUCT   *MemData;
  UINT32            AcpiMmioAddr;
  UINT32            GpioMmioAddr;
  UINT8             Data8;
  UINT16            Data16;
  UINT8             TempData8;

  FcnData = Data;
  MemData = ConfigPtr;

  Status  = AGESA_SUCCESS;
  /* Get SB800 MMIO Base (AcpiMmioAddr) */
  WriteIo8 (0xCD6, 0x27);
  Data8   = ReadIo8(0xCD7);
  Data16  = Data8<<8;
  WriteIo8 (0xCD6, 0x26);
  Data8   = ReadIo8(0xCD7);
  Data16  |= Data8;
  AcpiMmioAddr = (UINT32)Data16 << 16;
  GpioMmioAddr = AcpiMmioAddr + GPIO_BASE;

  Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG178);
  Data8 &= ~BIT5;
  TempData8  = Read64Mem8 (GpioMmioAddr+SB_GPIO_REG178);
  TempData8 &= 0x03;
  TempData8 |= Data8;
  Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, TempData8);

  Data8 |= BIT2+BIT3;
  Data8 &= ~BIT4;
  TempData8  = Read64Mem8 (GpioMmioAddr+SB_GPIO_REG178);
  TempData8 &= 0x23;
  TempData8 |= Data8;
  Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, TempData8);
  Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG179);
  Data8 &= ~BIT5;
  TempData8  = Read64Mem8 (GpioMmioAddr+SB_GPIO_REG179);
  TempData8 &= 0x03;
  TempData8 |= Data8;
  Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, TempData8);
  Data8 |= BIT2+BIT3;
  Data8 &= ~BIT4;
  TempData8  = Read64Mem8 (GpioMmioAddr+SB_GPIO_REG179);
  TempData8 &= 0x23;
  TempData8 |= Data8;
  Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, TempData8);

  switch(MemData->ParameterListPtr->DDR3Voltage){
    case VOLT1_35:
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG178);
      Data8 &= ~(UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG179);
      Data8 |= (UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, Data8);
      break;
    case VOLT1_25:
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG178);
      Data8 &= ~(UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG179);
      Data8 &= ~(UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, Data8);
      break;
    case VOLT1_5:
    default:
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG178);
      Data8 |= (UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG179);
      Data8 &= ~(UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, Data8);
  }
  // disable memory clear for boot time reduction
  MemData->ParameterListPtr->EnableMemClr = FALSE;
  return Status;
}
