/**
   dd_reader
   disk.h
   Copyright 2013 Ramsey Kant

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef _DISK_H_
#define _DISK_H_

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
   char *file_path;
   char *image_name;

	byte_buffer *buffer;

	// Disk Data structures
	mbr *master_boot_record;
   //gpt *guid_table;
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