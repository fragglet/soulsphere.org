#!/usr/bin/env python
#
# Copyright(C) Simon Howard
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
# IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
# Bluetooth snoozer daemon.
#
# Monitors xscreensaver and disconnects Bluetooth keyboards and mice
# when the screen blanks, so that the battery is saved.
#

import os
import dbus

BT_KEYBOARD_DEVICE=9536
BT_MOUSE_DEVICE=9600

def get_object(path, iface):
	obj = bus.get_object("org.bluez", path)
	return dbus.Interface(obj, dbus_interface=iface)

def get_default_adapter():
	manager = get_object("/", "org.bluez.Manager")
	return manager.DefaultAdapter()

def list_devices(adapter_path):
	adapter = get_object(adapter_path, "org.bluez.Adapter")
	return adapter.ListDevices()

def disconnect_device(device_path):
	device = get_object(device_path, "org.bluez.Device")
	device.Disconnect()

def get_device_properties(device_path):
	device = get_object(device_path, "org.bluez.Device")
	return device.GetProperties()

def send_devices_to_sleep():
	adapter = get_default_adapter()

	for device in list_devices(adapter):
		props = get_device_properties(device)

		if props["Connected"]:
			dev_class = props["Class"]

			# Bluetooth keyboard

			if dev_class == BT_KEYBOARD_DEVICE \
			 or dev_class == BT_MOUSE_DEVICE:
				disconnect_device(device)

bus = dbus.SystemBus()

# Monitor xscreensaver, and disconnect bluetooth devices when
# the screensaver is activated.

watcher = os.popen("xscreensaver-command -watch")

while True:
	line = watcher.readline()

	if line.startswith("BLANK") or line.startswith("LOCK"):
		send_devices_to_sleep()

