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
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include "pm.h"

#define BACKWARD_COMPAT 1

/* If PM support of the right version exists in kernel, return zero.
   Otherwise, return 1 if no support exists, or 2 if it is the wrong
   version.  *NOTE* The sense of the return value is not intuitive. */

int pm_exists( void )
{
   pm_info i;
   
   if (access( PM_PROC, R_OK )) return 1;
   return pm_read( &i );
}

/* Read information from /proc/pm.  Return 0 on success, 1 if PM not
   installed, 2 if PM installed, but old version. */

int pm_read( pm_info *i )
{
   FILE *str;
   char units[10];
   char buffer[100];
   int  retcode = 0;

   if (!(str = fopen( PM_PROC, "r" ))) return 1;
   fgets( buffer, sizeof( buffer ) - 1, str );
   buffer[ sizeof( buffer ) - 1 ] = '\0';
   sscanf( buffer, "%s %d.%d %x %x %x %x %d%% %d %s\n",
	   (char *)i->driver_version,
	   &i->pm_version_major,
	   &i->pm_version_minor,
	   &i->pm_flags,
	   &i->ac_line_status,
	   &i->battery_status,
	   &i->battery_flags,
	   &i->battery_percentage,
	   &i->battery_time,
	   units );
   i->using_minutes = !strncmp( units, "min", 3 ) ? 1 : 0;
   if (i->driver_version[0] == 'B') { /* old style.  argh. */
#if !BACKWARD_COMPAT
      retcode = 2;
#else
      strcpy( (char *)i->driver_version, "pre-0.7" );
      i->pm_version_major  = 0;
      i->pm_version_minor  = 0;
      i->pm_flags          = 0;
      i->ac_line_status     = 0xff;
      i->battery_status     = 0xff;
      i->battery_flags      = 0xff;
      i->battery_percentage = -1;
      i->battery_time       = -1;
      i->using_minutes      = 1;

      sscanf( buffer, "BIOS version: %d.%d",
	      &i->pm_version_major, &i->pm_version_minor );
      fgets( buffer, sizeof( buffer ) - 1, str );
      sscanf( buffer, "Flags: 0x%02x", &i->pm_flags );
      if (i->pm_flags & PM_32_BIT_SUPPORT) {
	 fgets( buffer, sizeof( buffer ) - 1, str );
	 fgets( buffer, sizeof( buffer ) - 1, str );
	 if (buffer[0] != 'P') {
	    if (!strncmp( buffer+4, "off line", 8 )) i->ac_line_status = 0;
	    else if (!strncmp( buffer+4, "on line", 7 )) i->ac_line_status = 1;
	    else if (!strncmp( buffer+4, "on back", 7 )) i->ac_line_status = 2;

	    fgets( buffer, sizeof( buffer ) - 1, str );
	    if (!strncmp( buffer+16, "high", 4 )) i->battery_status = 0;
	    else if (!strncmp( buffer+16, "low", 3 )) i->battery_status = 1;
	    else if (!strncmp( buffer+16, "crit", 4 )) i->battery_status = 2;
	    else if (!strncmp( buffer+16, "charg", 5 )) i->battery_status = 3;
	    
	    fgets( buffer, sizeof( buffer ) - 1, str );
	    if (strncmp( buffer+14, "unknown", 7 ))
	       i->battery_percentage = atoi( buffer + 14 );
	    if (i->pm_version_major >= 1 && i->pm_version_minor >= 1) {
	       fgets( buffer, sizeof( buffer ) - 1, str );
	       sscanf( buffer, "Battery flag: 0x%02x", &i->battery_flags );
	       
	       fgets( buffer, sizeof( buffer ) - 1, str );
	       if (strncmp( buffer+14, "unknown", 7 ))
		  i->battery_time = atoi( buffer + 14 );
	    }
	 }
      }
#endif
   }

				/* Fix possible kernel bug -- percentage
                                   set to 0xff (==255) instead of -1. */
   if (i->battery_percentage > 100) i->battery_percentage = -1;
   
   fclose( str );
   return retcode;
}


/* Lookup the device number for the pm_bios device. */

dev_t pm_dev( void )
{
   FILE       *str;
   static int cached = -1;
   char       buf[80];
   char       *pt;
   pm_info  i;

   if (cached >= 0) return cached;

   if (access( PM_PROC, R_OK ) || pm_read( &i ) == 1) return cached = -1;
   if (i.driver_version[0] == '1') return cached = makedev( 10, 134 );

   if (!(str = fopen( PM_DEV, "r" ))) return -1;
   while (fgets( buf, sizeof( buf ) - 1, str )) {
      buf[ sizeof( buf ) - 1 ] = '\0';
      for (pt = buf; *pt && isspace( *pt ); ++pt); /* skip leading spaces */
      for (; *pt && !isspace( *pt ); ++pt); /* find next space */
      if (isspace( *pt )) {
	 *pt++ = '\0';
	 pt[ strlen( pt ) - 1 ] = '\0';	/* get rid of newline */
	 if (!strcmp( pt, PM_NAME )) {
	    fclose( str );
	    return cached = makedev( atoi( buf ), 0 );
	 }
      }
   }
   fclose( str );
   return cached = -1;
}


/* Return a file descriptor for the pm_bios device, or -1 if there is an
   error.  Is this method secure?  Should we make the device in /dev
   instead of /tmp? */

int pm_open( void )
{
   char     *tmp;
   int      fd;
   pm_info i;

   if (access( PM_PROC, R_OK ) || pm_read( &i ) == 1) return -1;
   if (i.driver_version[0] == '1') {
      if ((fd = open( PM_DEVICE, O_RDWR )) < 0) {
				/* Try to create it.  This is reasonable
                                   for backward compatibility. */
	 if (mknod( PM_DEVICE, S_IFCHR | S_IRUSR | S_IWUSR, pm_dev() )) {
	    unlink( PM_DEVICE );
	    return -1;
	 }
	 fd = open( PM_DEVICE, O_RDWR );
      }
   } else {
      if (!(tmp = tmpnam( NULL ))) return -1;
      if (mknod( tmp, S_IFCHR | S_IRUSR | S_IWUSR, pm_dev() )) {
	 unlink( tmp );
	 return -1;
      }
      fd = open( tmp, O_RDWR );
      unlink( tmp );
   }
   
   return fd;
}


/* Given a file descriptor for the pm_bios device, close it. */

int pm_close( int fd )
{
   return close( fd );
}

/* Given a file descriptor for the pm_bios device, this routine will wait
    timeout seconds for PM events.  Up to n events will be placed in the
    events queue.  The return code will indicate the number of events
    stored.  Since this routine uses select(2), it will return if an
    unblocked signal is caught.  A timeout < 0 means to block indefinately. */

int pm_get_events( int fd, int timeout, pm_event_t *events, int n )
{
   int            retcode;
   fd_set         fds;
   struct timeval t;
   
   t.tv_sec  = timeout;
   t.tv_usec = 0;
   
   FD_ZERO( &fds );
   FD_SET( fd, &fds );
   retcode = select( fd + 1, &fds, NULL, NULL, timeout < 0 ? NULL : &t );
   if (retcode < 0) return 0;
   return read( fd, events, n * sizeof(pm_event_t) ) / sizeof(pm_event_t);
}


/* Try to set the Power State to Suspend. */

int pm_suspend( int fd )
{
   sync();
   sleep( 2 );
   return ioctl( fd, PM_IOC_SUSPEND, NULL );
}


/* Try to set the Power State to Standby. */

int pm_standby( int fd )
{
   sync();
   sleep( 2 );
   return ioctl( fd, PM_IOC_STANDBY, NULL );
}


/* Return a string describing the event. From p. 16 of the Intel/Microsoft
   Advanded Power Management (PM) BIOS Interface Specification, Revision
   1.1 (September 1993). Intel Order Number: 241704-001.  Microsoft Part
   Number: 781-110-X01. */

const char *pm_event_name( pm_event_t event )
{
   switch (event) {
   case PM_SYS_STANDBY:         return "System Standby Request";
   case PM_SYS_SUSPEND:         return "System Suspend Request";
   case PM_NORMAL_RESUME:       return "Normal Resume System";
   case PM_CRITICAL_RESUME:     return "Critical Resume System";
   case PM_LOW_BATTERY:         return "Battery Low";
   case PM_POWER_STATUS_CHANGE: return "Power Status Change";
   case PM_UPDATE_TIME:         return "Update Time";
   case PM_CRITICAL_SUSPEND:    return "Critical Suspend";
   case PM_USER_STANDBY:        return "User System Standby Request";
   case PM_USER_SUSPEND:        return "User System Suspend Request";
   case PM_STANDBY_RESUME:      return "System Standby Resume";
   }
   return "Unknown";
}


/* This is a convenience function that has nothing to do with PM.  It just
   formats a time nicely.  If you don't like this format, then write your
   own. */

#define SEC_PER_DAY  (60*60*24)
#define SEC_PER_HOUR (60*60)
#define SEC_PER_MIN  (60)

const char *pm_delta_time( time_t then, time_t now )
{
   return pm_time( now - then );
}

const char *pm_time( time_t t )
{
   static char   buffer[128];
   unsigned long s,m,h,d;

   d = t / SEC_PER_DAY;  t -= d * SEC_PER_DAY;
   h = t / SEC_PER_HOUR; t -= h * SEC_PER_HOUR;
   m = t / SEC_PER_MIN;  t -= m * SEC_PER_MIN;
   s = t;

   if (d) sprintf( buffer, "%lu day%s, %02lu:%02lu:%02lu",
                   d, d > 1 ? "s" : "", h, m, s );
   else   sprintf( buffer, "%02lu:%02lu:%02lu", h, m, s );

   return buffer;
}

const char *pm_time_nosec( time_t t )
{
   static char   buffer[128];
   unsigned long s,m,h,d;

   d = t / SEC_PER_DAY;  t -= d * SEC_PER_DAY;
   h = t / SEC_PER_HOUR; t -= h * SEC_PER_HOUR;
   m = t / SEC_PER_MIN;  t -= m * SEC_PER_MIN;
   s = t;

   if (s > 30) ++m;

   if (d) sprintf( buffer, "%lu day%s, %lu:%02lu",
                   d, d > 1 ? "s" : "", h, m );
   else   sprintf( buffer, "%lu:%02lu", h, m );

   return buffer;
}
