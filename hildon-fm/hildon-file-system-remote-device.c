/*
 * This file is part of hildon-fm package
 *
 * Copyright (C) 2006 Nokia Corporation.  All rights reserved.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <glib.h>
#include "hildon-file-common-private.h"

#include "hildon-file-system-remote-device.h"
#include "hildon-file-system-settings.h"

static void
hildon_file_system_remote_device_class_init (HildonFileSystemRemoteDeviceClass
                                             *klass);
static void
hildon_file_system_remote_device_init (HildonFileSystemRemoteDevice *device);
static void
hildon_file_system_remote_device_finalize (GObject *obj);
static void
flightmode_changed (GObject *settings, GParamSpec *param, gpointer data);
static gboolean
hildon_file_system_remote_device_requires_access (HildonFileSystemSpecialLocation
                                                  *location);
static gboolean
hildon_file_system_remote_device_is_available (HildonFileSystemSpecialLocation
                                               *location);
static gchar*
hildon_file_system_remote_device_get_unavailable_reason (
                HildonFileSystemSpecialLocation *location);


static gpointer parent_class = NULL;

G_DEFINE_TYPE (HildonFileSystemRemoteDevice,
               hildon_file_system_remote_device,
               HILDON_TYPE_FILE_SYSTEM_SPECIAL_LOCATION);

static void
hildon_file_system_remote_device_class_init (HildonFileSystemRemoteDeviceClass
                                             *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    HildonFileSystemSpecialLocationClass *location =
            HILDON_FILE_SYSTEM_SPECIAL_LOCATION_CLASS (klass);

    parent_class = g_type_class_peek_parent (klass);
    gobject_class->finalize = hildon_file_system_remote_device_finalize;
    location->requires_access = hildon_file_system_remote_device_requires_access;
    location->is_available = hildon_file_system_remote_device_is_available;
    location->get_unavailable_reason =
            hildon_file_system_remote_device_get_unavailable_reason;
}

static void
hildon_file_system_remote_device_init (HildonFileSystemRemoteDevice *device)
{
    HildonFileSystemSettings *fs_settings;
    HildonFileSystemSpecialLocation *location;

    fs_settings = _hildon_file_system_settings_get_instance ();

    location = HILDON_FILE_SYSTEM_SPECIAL_LOCATION (device);
    location->sort_weight = SORT_WEIGHT_REMOTE_DEVICE;

    g_object_get (fs_settings, "flight-mode", &device->accessible, NULL);
    device->accessible = !device->accessible;

    device->signal_handler_id = g_signal_connect (fs_settings,
                                                 "notify::flight-mode",
                                                 G_CALLBACK(flightmode_changed),
                                                 device);
    HILDON_FILE_SYSTEM_SPECIAL_LOCATION (device)->compatibility_type =
        HILDON_FILE_SYSTEM_MODEL_GATEWAY;
}

static void
hildon_file_system_remote_device_finalize (GObject *obj)
{
    HildonFileSystemSpecialLocation *location;
    HildonFileSystemRemoteDevice *device;
    HildonFileSystemSettings *fs_settings;

    location = HILDON_FILE_SYSTEM_SPECIAL_LOCATION (obj);
    device = HILDON_FILE_SYSTEM_REMOTE_DEVICE (obj);
    fs_settings = _hildon_file_system_settings_get_instance ();

    if (g_signal_handler_is_connected (fs_settings, device->signal_handler_id))
    {
        g_signal_handler_disconnect (fs_settings, device->signal_handler_id);
    }

    G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
flightmode_changed (GObject *settings, GParamSpec *param, gpointer data)
{
    HildonFileSystemRemoteDevice *device;
    gboolean was_accessible;

    device = HILDON_FILE_SYSTEM_REMOTE_DEVICE (data);
    was_accessible = device->accessible;

    g_object_get (settings, "flight-mode", &device->accessible, NULL);
    device->accessible = !device->accessible;
    if (was_accessible && !device->accessible && !g_ascii_strcasecmp(HILDON_FILE_SYSTEM_SPECIAL_LOCATION(device)->basepath, "obex://")) {
            //we want a Bluetooth folder be accessible in flight mode but not any of its children
            device->accessible = TRUE;
    }

    g_signal_emit_by_name (device, "connection-state");
}

static gboolean
hildon_file_system_remote_device_requires_access (HildonFileSystemSpecialLocation
                                                  *location)
{
    return TRUE;
}

static gboolean
hildon_file_system_remote_device_is_available (HildonFileSystemSpecialLocation
                                               *location)
{
    HildonFileSystemRemoteDevice *device;
    device = HILDON_FILE_SYSTEM_REMOTE_DEVICE (location);

    return device->accessible;
}

static gchar*
hildon_file_system_remote_device_get_unavailable_reason (
                HildonFileSystemSpecialLocation *location)
{
    HildonFileSystemRemoteDevice *device;

    device = HILDON_FILE_SYSTEM_REMOTE_DEVICE (location);

    if (!device->accessible)
    {
        return g_strdup (_("sfil_ib_no_connections_flightmode"));
    }

    return NULL;
}

