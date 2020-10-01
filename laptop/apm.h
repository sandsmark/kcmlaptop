/* pm.h -- Header file for sample PM interface routines
 * Created: Mon Jan  8 11:40:50 1996 by r.faith@ieee.org
 * Revised: Thu Apr  4 21:57:31 1996 by r.faith@ieee.org
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
 * $Id: pm.h,v 1.4 1996/04/05 03:20:38 faith Exp $
 * 
 */

#include <linux/pm_bios.h>
#include <sys/types.h>

#define PM_PROC   "/proc/pm"
#define PM_DEVICE "/dev/pm_bios"

#define PM_DEV  "/proc/devices"
#define PM_NAME "pm_bios"

#ifndef PM_32_BIT_SUPPORT
#define PM_32_BIT_SUPPORT      0x0002
#endif
#ifdef __cplusplus
extern "C" {
#endif   

typedef struct pm_info {
   const char driver_version[10];
   int        pm_version_major;
   int        pm_version_minor;
   int        pm_flags;
   int        ac_line_status;
   int        battery_status;
   int        battery_flags;
   int        battery_percentage;
   int        battery_time;
   int        using_minutes;
} pm_info;

extern int   pm_exists( void );
extern int   pm_read( pm_info *i );
extern dev_t pm_dev( void );
extern int   pm_open( void );
extern int   pm_close( int fd );
extern int   pm_get_events( int fd, int timeout, pm_event_t *events, int n );
extern int   pm_suspend( int fd );
extern int   pm_standby( int fd );
extern const char *pm_event_name( pm_event_t event );
extern const char *pm_time( time_t t );
extern const char *pm_delta_time( time_t then, time_t now );
extern const char *pm_time_nosec( time_t t );
#ifdef __cplusplus
}
#endif  
