/**
 * @file
 *
 * AMD Family_15 Richland Logical ID Table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * --@e sub-project:  CPU/Family/0x15/TN
 * --@e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc. All rights reserved.
 * Copyright (c) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "AGESA.h"
#include "cpuRegisters.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_RL_F15RLLOGICALIDTABLES_FILECODE

/* FIXME: Rename GetF15TnLogicalIdAndRev() func */

VOID
GetF15TnLogicalIdAndRev (
     OUT   CONST CPU_LOGICAL_ID_XLAT **TnIdPtr,
     OUT   UINT8 *NumberOfElements,
     OUT   UINT64 *LogicalFamily,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

STATIC CONST CPU_LOGICAL_ID_XLAT ROMDATA CpuF15RlLogicalIdAndRevArray[] =
{
  {
    0x6131,
    AMD_F15_RL_A1
  },
  {
    0x6100,
    0x0000000000000100ull
  }
};

VOID
GetF15TnLogicalIdAndRev (
     OUT   CONST CPU_LOGICAL_ID_XLAT **TnIdPtr,
     OUT   UINT8 *NumberOfElements,
     OUT   UINT64 *LogicalFamily,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NumberOfElements = (sizeof (CpuF15RlLogicalIdAndRevArray) / sizeof (CPU_LOGICAL_ID_XLAT));
  *TnIdPtr = CpuF15RlLogicalIdAndRevArray;
  *LogicalFamily = AMD_FAMILY_15_RL;
}
