/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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

#ifndef SUPERIO_ITE_COMMON_ROMSTAGE_H
#define SUPERIO_ITE_COMMON_ROMSTAGE_H

#include <arch/io.h>
#include <stdint.h>

#define ITE_UART_CLK_PREDIVIDE_48 0x00 /* default */
#define ITE_UART_CLK_PREDIVIDE_24 0x01

void ite_conf_clkin(device_t dev, u8 predivide);
void ite_enable_serial(pnp_devfn_t dev, u16 iobase);

/* Some boards need to init wdt+gpio's very early */
void ite_reg_write(device_t dev, u8 reg, u8 value);
void ite_enable_3vsbsw(device_t dev);
void ite_kill_watchdog(device_t dev);

#endif /* SUPERIO_ITE_COMMON_ROMSTAGE_H */
