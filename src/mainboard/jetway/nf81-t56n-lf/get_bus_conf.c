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

#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/amd/amdfam14.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <southbridge/amd/cimx/sb800/sb_cimx.h>


/**
 * Global variables for mainboard layouts. These are shared by irqtable,
 * mptable and acpi_tables where busnum is default.
 */
u8 bus_isa;
u8 bus_sb800[6];
u32 apicid_sb800;
u32 apicver_sb800;

/**
 * Here you only need to set value in pci1234 for HT-IO that could be
 * installed or not. You may need to preset pci1234 for HTIO board,
 * please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
 */
u32 pci1234x[] = {
	0x0000ff0,
};

u32 bus_type[256];
u32 sbdn_sb800;


void get_bus_conf(void)
{
	u32 apicid_base;

	device_t dev;
	int i, j;

	sbdn_sb800 = 0;

	memset(bus_sb800, 0, sizeof(bus_sb800));

	for (i = 0; i < 256; i++) {
		bus_type[i] = 0; /* default ISA bus. */
	}

	bus_type[0] = 1;	/* pci */

//	bus_sb800[0] = (sysconf.pci1234[0] >> 16) & 0xff;
	bus_sb800[0] = (pci1234x[0] >> 16) & 0xff;

	/* sb800 */
	dev = dev_find_slot(bus_sb800[0], PCI_DEVFN(sbdn_sb800 + 0x14, 4));

	if (dev) {
		bus_sb800[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);

		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
		for (j = bus_sb800[1]; j < bus_isa; j++)
			bus_type[j] = 1;
	}

	for (i = 0; i < 4; i++) {
		dev = dev_find_slot(bus_sb800[0], PCI_DEVFN(sbdn_sb800 + 0x14, i));
		if (dev) {
			bus_sb800[2 + i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_isa++;
		}
	}

	for (j = bus_sb800[2]; j < bus_isa; j++)
		bus_type[j] = 1;


	/* I/O APICs:    APIC ID Version State   Address */
	bus_isa = 10;
	apicid_base = CONFIG_MAX_CPUS;

	/*
	 * By the time this function gets called, the IOAPIC registers
	 * have been written so they can be read to get the correct
	 * APIC ID and Version
	 */
	apicid_sb800 = (io_apic_read(IO_APIC_ADDR, 0x00) >> 24);
	apicver_sb800 = (io_apic_read(IO_APIC_ADDR, 0x01) & 0xFF);

#if CONFIG_AMD_SB_CIMX
	sb_Late_Post();
#endif /* CONFIG_AMD_SB_CIMX */
}
