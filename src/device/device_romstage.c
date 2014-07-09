/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003-2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2003 Greg Watson <jarrah@users.sourceforge.net>
 * Copyright (C) 2004 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005-2006 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
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

#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/resource.h>

/** Linked list of ALL devices */
ROMSTAGE_CONST struct device * ROMSTAGE_CONST all_devices = &dev_root;

/**
 * Given a PCI bus and a devfn number, find the device structure.
 *
 * @param bus The bus number.
 * @param devfn A device/function number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
ROMSTAGE_CONST struct device *dev_find_slot(unsigned int bus,
						unsigned int devfn)
{
	ROMSTAGE_CONST struct device *dev, *result;

	result = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_PCI) &&
		    (dev->bus->secondary == bus) &&
		    (dev->path.pci.devfn == devfn)) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Given a device pointer, find the next PCI device.
 *
 * @param previous_dev A pointer to a PCI device structure.
 * @return Pointer to the next device structure (if found), 0 otherwise.
 */
ROMSTAGE_CONST struct device *dev_find_next_pci_device(
		ROMSTAGE_CONST struct device *previous_dev)
{
	ROMSTAGE_CONST struct device *dev, *result;

	if (previous_dev == NULL)
		previous_dev = all_devices;

	result = 0;
	for (dev = previous_dev->next; dev; dev = dev->next) {
		if (dev->path.type == DEVICE_PATH_PCI) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Given an SMBus bus and a device number, find the device structure.
 *
 * @param bus The bus number.
 * @param addr A device number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
ROMSTAGE_CONST struct device *dev_find_slot_on_smbus(unsigned int bus,
							unsigned int addr)
{
	ROMSTAGE_CONST struct device *dev, *result;

	result = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_I2C) &&
		    (dev->bus->secondary == bus) &&
		    (dev->path.i2c.device == addr)) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Given a PnP port and a device number, find the device structure.
 *
 * @param port The I/O port.
 * @param device Logical device number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
ROMSTAGE_CONST struct device *dev_find_slot_pnp(u16 port, u16 device)
{
	ROMSTAGE_CONST struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_PNP) &&
		    (dev->path.pnp.port == port) &&
		    (dev->path.pnp.device == device)) {
			return dev;
		}
	}
	return 0;
}

/*
 * Warning: This function uses a static buffer. Don't call it more than once
 * from the same print statement!
 */
ROMSTAGE_CONST char *dev_path(device_t dev)
{
	static char buffer[DEVICE_PATH_MAX];

	buffer[0] = '\0';
#if 0
	if (!dev) {
		memcpy(buffer, "<null>", 7);
	} else {
		switch(dev->path.type) {
		case DEVICE_PATH_PNP:
			snprintf(buffer, sizeof (buffer), "PNP: %04x.%01x",
				 dev->path.pnp.port, dev->path.pnp.device);
			break;
		default:
			printk(BIOS_ERR, "Unknown device path type: %d\n",
			       dev->path.type);
			break;
		}
	}
#endif
	return buffer;
}

/**
 * See if a device structure already exists and if not allocate it.
 *
 * @param parent The bus to find the device on.
 * @param path The relative path from the bus to the appropriate device.
 * @return Pointer to a device structure for the device on bus at path.
 */
device_t alloc_find_dev(struct bus *parent, struct device_path *path)
{
	device_t child = NULL;
#if 0
	spin_lock(&dev_lock);
	child = find_dev_path(parent, path);
	if (!child)
		child = __alloc_dev(parent, path);
	spin_unlock(&dev_lock);
#endif
	return child;
}

#if 0
/**
 * See if a resource structure already exists for a given index and if not
 * allocate one.
 *
 * Then initialize the resource to default values.
 *
 * @param dev The device to find the resource on.
 * @param index The index of the resource on the device.
 * @return TODO.
 */
ROMSTAGE_CONST struct resource *new_resource(device_t dev, unsigned index)
{
	struct resource *resource = NULL; //, *tail;

	/* First move all of the free resources to the end. */
	compact_resources(dev);

	/* See if there is a resource with the appropriate index. */
	resource = probe_resource(dev, index);
	if (!resource) {
		if (free_resources == NULL && !allocate_more_resources())
			die("Couldn't allocate more resources.");

		resource = free_resources;
		free_resources = free_resources->next;
		memset(resource, 0, sizeof(*resource));
		resource->next = NULL;
		tail = dev->resource_list;
		if (tail) {
			while (tail->next) tail = tail->next;
			tail->next = resource;
		} else {
			dev->resource_list = resource;
		}
	}

	/* Initialize the resource values. */
	if (!(resource->flags & IORESOURCE_FIXED)) {
		resource->flags = 0;
		resource->base = 0;
	}
	resource->size  = 0;
	resource->limit = 0;
	resource->index = index;
	resource->align = 0;
	resource->gran  = 0;

	return resource;
}
#endif
