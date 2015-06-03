/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*-
 *
 * Copyright (C) 2009 David Zeuthen <david@fubar.dk>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include <atasmart.h>

static void
usage (void)
{
  fprintf (stderr, "incorrect usage\n");
}

int
main (int argc,
      char *argv[])
{
  int ret;
  const char *device;
  SkDisk *d;
  SkBool smart_is_available;

  d = NULL;
  ret = 1;

  if (argc != 2)
    {
      usage ();
      goto out;
    }

  device = argv[1];

  /* Open disk */
  if (sk_disk_open (device, &d) != 0)
    {
      printf ("Failed to open disk %s: %m\n", device);
      goto out;
    }

  /* Check if smart is available on the disk */
  if (sk_disk_smart_is_available (d, &smart_is_available) != 0)
    {
      printf ("Failed to determine if smart is available for %s: %m\n", device);
      goto out;
    }

  /* main smart data */
  if (sk_disk_smart_read_data (d) != 0)
    {
      printf ("Failed to read smart data for %s: %m\n", device);
      goto out;
    }

  sk_disk_dump(d);

  ret = 0;

 out:

  if (d != NULL)
    sk_disk_free (d);
  return ret;
}
