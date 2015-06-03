#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libudev.h>

int main(int argc, char *argv[])
{
	int rc = 1;
	int bay;
	char *endptr = NULL;
	char *target = NULL;
	char *target_id = NULL;
	char sys_path[1024] = {0};
	struct udev *udev = NULL;
	struct udev_device *dev = NULL;
	struct udev_list_entry *devs, *devs_list_entry;

	/* devpath */
	char *devpath = argv[2];

	if (strcmp(argv[1], "pata_via") == 0)
	{
		/* Get target */
		target = strstr(devpath, "/target");
		if (target == NULL)
			return rc;

		/* Get id */
		target_id = strchr(target, ':');
		if (target_id == NULL)
			return rc;

		/* Translate to bay */
		bay = atoi(target_id + 3);
		switch(bay)
		{
			case 0:
				printf("m1");
				break;
			case 1:
				printf("m2");
				break;
		}
		rc = 0;
	}
	else
	{
		/* Create a new udev library context */
		udev = udev_new();
		if (!udev)
			return rc;

		/* Create a udev device object using the device path */
		snprintf(sys_path, 1024, "/sys%s", devpath);
		endptr = strstr(sys_path, "/end_device");
		if (endptr != NULL)
			*endptr = '\0';

		dev = udev_device_new_from_syspath(udev, sys_path);
		if (dev == NULL)
			goto cleanup;

		/* find the attribute list for the device */
		devs = udev_device_get_sysattr_list_entry( dev );
		udev_list_entry_foreach( devs_list_entry, devs )
		{
			const char *attr;

			/* get the phy-* attribute for the device; since it
			 * contains the physical bay id */
			attr = udev_list_entry_get_name( devs_list_entry );
			if(strstr( attr, "phy-" ) != NULL)
			{
				char *bayid = strrchr( attr, ':' );

				if ( ( NULL != bayid++ ) && ( NULL != bayid ) )
				{
					bay = atoi( bayid );
					switch (bay)
					{
						case 0:
							printf("a1");
							break;
						case 1:
							printf("a2");
							break;
						case 2:
							printf("a3");
							break;
						case 3:
							printf("a4");
							break;
						case 4:
							printf("b1");
							break;
						case 5:
							printf("b2");
							break;
						case 6:
							printf("b3");
							break;
						case 7:
							printf("b4");
							break;
					}
					rc = 0;
				}
				break;
			}
		}
	}

cleanup:
	if (dev)
		udev_device_unref(dev);

	if (udev)
		udev_unref(udev);

	return rc;
}
