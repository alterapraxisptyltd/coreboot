/**
 * @file
 *
 * Install of build option: Preserve Mailbox
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 */
/*****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * 
 *
 ***************************************************************************/

#ifndef _OPTION_PRESERVE_MAILBOX_INSTALL_H_
#define _OPTION_PRESERVE_MAILBOX_INSTALL_H_

#include "PreserveMailbox.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_PRESERVE_MAILBOX_FEAT
#define F10_PRESERVE_MAILBOX_SUPPORT
#if ((AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE))
  #ifdef OPTION_FAMILY10H
    #if OPTION_FAMILY10H == TRUE
      extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeaturePreserveAroundMailbox;
      #undef OPTION_PRESERVE_MAILBOX_FEAT
      #define OPTION_PRESERVE_MAILBOX_FEAT &CpuFeaturePreserveAroundMailbox,
      CONST PCI_ADDR ROMDATA F10PreserveMailboxRegisters [] = {
        {MAKE_SBDFO (0, 0, 0, 3, 0x168)},
        {MAKE_SBDFO (0, 0, 0, 3, 0x170)},
        {ILLEGAL_SBDFO}
      };
      CONST PRESERVE_MAILBOX_FAMILY_SERVICES ROMDATA F10PreserveMailboxServices = {
        0,
        TRUE,
        (PCI_ADDR *)&F10PreserveMailboxRegisters
      };
      #undef F10_PRESERVE_MAILBOX_SUPPORT
      #define F10_PRESERVE_MAILBOX_SUPPORT {AMD_FAMILY_10, &F10PreserveMailboxServices},
    #endif
  #endif
  CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA PreserveMailboxFamilyServiceArray[] =
  {
    F10_PRESERVE_MAILBOX_SUPPORT
    {0, NULL}
  };
  CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA PreserveMailboxFamilyServiceTable =
  {
    (sizeof (PreserveMailboxFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
    &PreserveMailboxFamilyServiceArray[0]
  };
#endif

#endif  // _OPTION_PRESERVE_MAILBOX_INSTALL_H_
