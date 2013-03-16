#include <stdio.h>

#include "disk.h"
#include "mbr.h"

int main(int argc, char **argv) {
	disk_img *disk = disk_init("/Users/ramseykant/Desktop/TestImage1.img");
	disk_parse(disk);
	disk_print(disk, true);
	disk_destroy(disk);
	return 0;
}