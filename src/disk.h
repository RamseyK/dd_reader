#ifndef _DISK_H_
#define _DISK_H_

#include <sys/stat.h>

#include "bytebuffer.h"
#include "fat.h"
#include "mbr.h"
#include "md5.h"
#include "sha1.h"
#include "shared.h"

/*
 * Disk state structure
 */
typedef struct disk_img_t {
	uint8_t *file_buf;
	size_t file_size;
	byte_buffer *buffer;

	// Disk Data structures
	mbr *master_boot_record;
	void *partition[4];
} disk_img;

/*
 * Disk functions
 */

disk_img *disk_init(const char *path);
void disk_output_sha1(disk_img *disk, const char *out_path);
void disk_output_md5(disk_img *disk, const char *out_path);
void disk_parse(disk_img *disk);
void disk_print(disk_img *disk, bool verbose);
void disk_destroy(disk_img *disk);

#endif