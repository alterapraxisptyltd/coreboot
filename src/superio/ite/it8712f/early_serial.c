/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <arch/io.h>
#include <device/pnp.h>
#include "it8712f.h"

/* Global configuration registers. */
#define IT8712F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8712F_CONFIG_REG_CONFIGSEL 0x22 /* Configuration Select. */
#define IT8712F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8712F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. */
#define IT8712F_CONFIG_REG_MFC       0x2a /* Multi-function control */
#define IT8712F_CONFIG_REG_WATCHDOG  0x72 /* Watchdog control. */


/*
 * Enable configuration: pass entry key '0x87 ??'
 */
static void pnp_enter_conf_state(device_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

/*
 * Disable configuration: pass exit key '0x02'
 */
static void pnp_exit_conf_state(device_t dev)
{
	pnp_write_config(dev, IT8712F_CONFIG_REG_CC, 0x02);
}


/* Select 24MHz CLKIN (48MHz is the default). */
void it8712f_24mhz_clkin(device_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_write_config(dev, IT8712F_CONFIG_REG_CLOCKSEL, 0x1);
	pnp_exit_conf_state(dev);
}

/*
 * We need to set enable 3VSBSW#, this was documented only in IT8712F_V0.9.2!
 *
 * LDN 7, reg 0x2a - needed for S3, or memory power will be cut off.
 *
 * Enable 3VSBSW#. (For System Suspend-to-RAM)
 * 0: 3VSBSW# will be always inactive.
 * 1: 3VSBSW# enabled. It will be (NOT SUSB#) NAND SUSC#.
 */
void it8712f_enable_3vsbsw(device_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_write_config(dev, IT8712F_CONFIG_REG_MFC, 0x80);
	pnp_exit_conf_state(dev);
}

void it8712f_kill_watchdog(device_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_write_config(dev, IT8712F_CONFIG_REG_WATCHDOG, 0x00);
	pnp_exit_conf_state(dev);
}

/*
 * Bring up early serial debugging output before the RAM is initialized.
 */
void it8712f_enable_serial(device_t dev, u16 iobase)
{
	/* (1) Enter the configuration state (MB PnP mode). */
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);

	/* (2) Modify the data of configuration registers. */

	/*
	 * Select the chip to configure (if there's more than one).
	 * Set bit 7 to select JP3=1, clear bit 7 to select JP3=0.
	 * If this register is not written, both chips are configured.
	 */

	/* it8712f_sio_write(0x00, IT8712F_CONFIG_REG_CONFIGSEL, 0x00); */

	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);

	/* Clear software suspend mode (clear bit 0). TODO: Needed? */
	/* it8712f_sio_write(0x00, IT8712F_CONFIG_REG_SWSUSP, 0x00); */

	/* (3) Exit the configuration state (MB PnP mode). */
	pnp_exit_conf_state(dev);
}
