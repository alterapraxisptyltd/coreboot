/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU SW C1e feature support code.
 *
 * Contains code that declares the AGESA CPU C1e related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "Topology.h"
#include "cpuFeatures.h"
#include "cpuSwC1e.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUSWC1E_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE SwC1eFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should software C1e be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               SW C1e is supported.
 * @retval       FALSE              SW C1e not supported.
 *
 */
BOOLEAN
STATIC
IsSwC1eFeatureEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN                IsEnabled;
  BOOLEAN                IsOtherC1eEnabled;
  AP_MAILBOXES           ApMailboxes;
  SW_C1E_FAMILY_SERVICES *SwFamilyServices;

  ASSERT (PlatformConfig->C1eMode < MaxC1eMode);
  IsEnabled = FALSE;

  // Check whether software C1e is enabled only if other C1e methods is/are not supported
  // or if the platform specifically uses C1eModeSoftwareDeprecated.
  IsOtherC1eEnabled = (IsFeatureEnabled (HardwareC1e, PlatformConfig, StdHeader) ||
                       IsFeatureEnabled (MsgBasedC1e, PlatformConfig, StdHeader));
  if ((PlatformConfig->C1eMode == C1eModeSoftwareDeprecated) ||
     ((!IsOtherC1eEnabled) && ((PlatformConfig->C1eMode == C1eModeHardwareSoftwareDeprecated) || (PlatformConfig->C1eMode == C1eModeAuto)))) {
    ASSERT ((PlatformConfig->C1ePlatformData1 < 0x10000) && (PlatformConfig->C1ePlatformData1 != 0));
    ASSERT (PlatformConfig->C1ePlatformData2 < 0x100);
    if ((PlatformConfig->C1ePlatformData1 != 0) && (PlatformConfig->C1ePlatformData1 < 0xFFFE) && (PlatformConfig->C1ePlatformData2 < 0xFF)) {
      if (!IsNonCoherentHt1 (StdHeader)) {
        if (GetNumberOfProcessors (StdHeader) == 1) {
          GetApMailbox (&ApMailboxes.ApMailInfo.Info, StdHeader);
          if (ApMailboxes.ApMailInfo.Fields.ModuleType == 0) {
            GetFeatureServicesOfCurrentCore (&SwC1eFamilyServiceTable, (CONST VOID **)&SwFamilyServices, StdHeader);
            if (SwFamilyServices != NULL) {
              IsEnabled = SwFamilyServices->IsSwC1eSupported (SwFamilyServices, StdHeader);
            }
          }
        }
      }
    }
  }
  return IsEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable Software C1e
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       The most severe status of any family specific service.
 *
 */
AGESA_STATUS
STATIC
InitializeSwC1eFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AGESA_STATUS AgesaStatus;
  SW_C1E_FAMILY_SERVICES *FamilyServices;

  AgesaStatus = AGESA_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "    SW C1e is enabled\n");

  if (IsWarmReset (StdHeader)) {
    GetFeatureServicesOfCurrentCore (&SwC1eFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
    AgesaStatus = FamilyServices->InitializeSwC1e (FamilyServices, EntryPoint, PlatformConfig, StdHeader);
  }

  return AgesaStatus;
}

CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureSwC1e =
{
  SoftwareC1e,
  CPU_FEAT_AFTER_PM_INIT,
  IsSwC1eFeatureEnabled,
  InitializeSwC1eFeature
};