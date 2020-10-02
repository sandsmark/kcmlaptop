/* pmlib.c -- Sample PM interface routines
 * Created: Mon Jan  8 10:28:16 1996 by r.faith@ieee.org
 * Revised: Thu Apr  4 21:59:01 1996 by r.faith@ieee.org
 * Copyright 1996 Rickard E. Faith (r.faith@ieee.org)
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: pmlib.c,v 1.6 1996/04/05 03:20:39 faith Exp $
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/sysmacros.h>
#include <systemd/sd-bus.h>
#include "pm.h"

/* Read information from /proc/pm.  Return 0 on success, 1 if PM not
   installed, 2 if PM installed, but old version. */

static int readFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("failed to open file '%s'\n", filename);
        return -1;
    }
    int value = -1;
    if (fscanf(file, "%d", &value) != 1) {
        printf("Failed to read file '%s'\n", filename);
        fclose(file);
        return -1;
    }
    fclose(file);
    return value;
}

int pm_read( pm_info *i )
{
    i->battery_percentage = readFile("/sys/class/power_supply/BAT0/capacity");

    if (i->battery_percentage < 0) {
        i->pm_flags = PM_NOT_AVAILABLE;
        return 1;
    }

    FILE *file = fopen("/sys/class/power_supply/BAT0/status", "rw");
    if (!file) {
        puts("Failed to open battery status file");
        i->pm_flags = PM_NOT_AVAILABLE;
        return 1;
    }

    char *line = NULL;
    size_t len;
    getline(&line, &len, file);
    fclose(file);
    if (strcmp(line, "Charging\n") == 0) {
        i->ac_line_status = 1;
    } else {
        i->ac_line_status = 0;
    }

    int rate = readFile("/sys/class/power_supply/BAT0/power_now") / 1000;
    int energyNow = readFile("/sys/class/power_supply/BAT0/energy_now") / 1000;
    if (energyNow <= 0 || rate <= 0) {
        i->pm_flags = PM_NOT_AVAILABLE;
        return 1;
    }

    if (i->ac_line_status) {
        int energyFull = readFile("/sys/class/power_supply/BAT0/energy_full");
        if (energyFull <= 0) {
            i->pm_flags = PM_NOT_AVAILABLE;
            return 1;
        }
        if (energyNow != energyFull) {
            i->battery_time = 3600 * (energyFull - energyNow) / rate;
        } else {
            i->battery_time = 0;
        }
    } else {
        i->battery_time = 3600 * energyNow / rate;
    }
    i->pm_flags = 0;

    return 0;
}

void invoke_login_manager(const char *method)
{
    sd_bus *bus = NULL;
    int ret = sd_bus_open_system(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to system bus: %s", strerror(-ret));
        return;
    }

    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *dbusRet = NULL;
    ret = sd_bus_call_method(bus,
            "org.freedesktop.login1",           /* service to contact */
            "/org/freedesktop/login1",          /* object path */
            "org.freedesktop.login1.Manager",   /* interface name */
            method,                          /* method name */
            &error,                               /* object to return error in */
            &dbusRet,                                   /* return message on success */
            "b",                                 /* input signature */
            "true");                       /* first argument */

    if (ret < 0) {
        fprintf(stderr, "Failed to issue method call: %s\n", error.message);
    }

    sd_bus_error_free(&error);
    sd_bus_message_unref(dbusRet);
    sd_bus_unref(bus);
}
