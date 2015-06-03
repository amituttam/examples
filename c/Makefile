targets = carrays fork smart sizeof tempname alloc find_bay sg_get_size \
sg_mode_sense mount ntfs_identify fs_ntfs fs_ntfs_tsk3 fs_fat \
fs_fat_tsk3 fs_hfs_tsk3 sha1 openssl_sha sha1_threads mount_new \
mount_status mount_cifs resolve_host port_scan valgrind read \
topology run_command fallocate foo-extentmap parse ftw

all: $(targets)

carrays: carrays.c
	gcc -Wall -g -o carrays carrays.c

ev: ev.c
	gcc -Wall -g -o ev ev.c -lev

fork: fork.c
	gcc -Wall -g -o fork fork.c

smart: smart.c
	gcc -Wall -g -o smart smart.c -latasmart

sizeof: sizeof.c
	gcc -Wall -g -o sizeof sizeof.c

tempname: tempname.c
	gcc -Wall -g -o tempname tempname.c

alloc: alloc.c
	gcc -Wall -g -o alloc alloc.c

find_bay: find_bay.c
	gcc -Wall -g -o find_bay find_bay.c -ludev

sg_get_size: sg_get_size.c
	gcc -Wall -g -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -o sg_get_size sg_get_size.c -lsgutils2

sg_mode_sense: sg_mode_sense.c
	gcc -Wall -g -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -o sg_mode_sense sg_mode_sense.c -lsgutils2

mount:
	gcc -Wall -g -o mount mount.c

ntfs_identify:
	colorgcc -Wall -g -o ntfs_identify ntfs_identify.c -ltsk3

fs_ntfs:
	colorgcc -Wall -g -o fs_ntfs fs_ntfs.c -lntfs

fs_ntfs_tsk3:
	colorgcc -Wall -g -o fs_ntfs_tsk3 fs_ntfs_tsk3.c -ltsk3

fs_fat:
	colorgcc -Wall -g -o fs_fat fs_fat.c

fs_fat_tsk3:
	colorgcc -Wall -g -o fs_fat_tsk3 fs_fat_tsk3.c -ltsk3

fs_hfs_tsk3:
	colorgcc -Wall -g -o fs_hfs_tsk3 fs_hfs_tsk3.c -ltsk3

sha1:
	colorgcc -Wall -g -o sha1 sha1.c -lcrypto

openssl_sha:
	colorgcc -Wall -g -o openssl_sha openssl_sha.c -lcrypto -fno-omit-framepointer

sha1_threads:
	colorgcc -Wall -g -o sha1_threads sha1_threads.c -lcrypto -pthread

mount_new:
	colorgcc -Wall -g -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/uuid -lmount -o mount_new mount_new.c

mount_status:
	colorgcc -Wall -g -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/uuid -lmount -o mount_status mount_status.c

mount_cifs:
	colorgcc -Wall -g -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/uuid -lmount -o mount_cifs mount_cifs.c

resolve_host:
	colorgcc -Wall -g -o resolve_host resolve_host.c

port_scan:
	colorgcc -Wall -g -o port_scan port_scan.c

valgrind:
	colorgcc -Wall -g -o valgrind valgrind.c

read:
	colorgcc -Wall -g -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -o read read.c

topology:
	colorgcc -Wall -g -I/usr/include/blkid -I/usr/include/uuid -lblkid -o topology topology.c

run_command:
	colorgcc -Wall -g -D_GNU_SOURCE -o run_command run_command.c

fallocate:
	colorgcc -Wall -g -D_GNU_SOURCE -o fallocate fallocate.c

foo-extentmap:
	colorgcc -Wall -g -D_GNU_SOURCE -o foo-extentmap foo-extentmap.c

parse:
	colorgcc -Wall -g -o parse parse.c

ftw:
	colorgcc -Wall -g -D_XOPEN_SOURCE=500 -o ftw ftw.c

clean:
	rm -f *.o
	rm -f $(targets)
