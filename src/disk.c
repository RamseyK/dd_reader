/**
   dd_reader
   disk.c
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

#include "disk.h"

/*
 * Initialize structures for analyzing a disk image.
 * Opens the specified image at path as read only and reads the data into the disk file buffer
 *
 * @param path Path to the disk image
 * @return disk_img structure that represents the state of the opened disk image. NULL if unsuccessful
 */
disk_img *disk_init(const char *path) {
	struct stat sb;
	FILE *fp;
	size_t bytes_read = 0;
	disk_img *disk = (disk_img*)malloc(sizeof(disk_img));
	memset(disk, 0, sizeof(disk_img));

	// Get the size of the disk image
	if(stat(path, &sb) != 0) {
		printf("Could not get the size of the disk image file %s\n", path);
		return NULL;
	}

	// Open disk image as read only
	fp = fopen(path, "rb");
	if(fp == NULL) {
		printf("Could not open disk image file %s\n", path);
		return NULL;
	}

	// Read image data into the global buffer
	disk->file_size = sb.st_size;
	disk->file_buf = (uint8_t*)malloc(disk->file_size);
	bytes_read = fread(disk->file_buf, sizeof(uint8_t), disk->file_size, fp);

	fclose(fp);

	if(bytes_read != sb.st_size) {
		printf("Incomplete read. Read %i out of %i bytes.\n", (int)bytes_read, (int)sb.st_size);
		return NULL;
	}

	// Wrap a byte buffer around the disk image buffer
	disk->buffer = bb_new_wrap(disk->file_buf, disk->file_size);

	return disk;
}

/*
 * Generate a SHA1 hash of the contents of the open disk image and output the hash to a file
 *
 * @param disk Disk Image state structure
 * @param out_path File Path to output the SHA1 hash as plain text
 */
void disk_output_sha1(disk_img *disk, const char *out_path) {
	SHA1Context ctx;
	SHA1Reset(&ctx);
	SHA1Input(&ctx, disk->file_buf, disk->file_size);
	if(SHA1Result(&ctx) != 1) {
		printf("Failed to generate SHA1 hash\n");
		return;
	}

	// Output to screen
	printf("SHA1: ");
	print_hex2((uint8_t*)ctx.Message_Digest, 5);
	printf("\n");

	// Output to file
	FILE *fp = fopen(out_path, "w+");
	if(fp == NULL) {
		printf("Could not open file %s to write sha1 hash\n", out_path);
		return;
	}

	fprintf(fp, "SHA1: ");
	for(int i = 0; i < 5; i++) {
		fprintf(fp, "%02x", ctx.Message_Digest[i]);
	}

	fclose(fp);
}

/*
 * Generate a MD5 hash of the contents of the open disk image and output the hash to a file
 *
 * @param disk Disk Image state structure
 * @param out_path File Path to output the MD5 hash as plain text
 */
void disk_output_md5(disk_img *disk, const char *out_path) {
	unsigned char digest[16];
	memset(digest, 0 , 16);

	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, disk->file_buf, disk->file_size);
	MD5_Final(digest, &ctx);

	// Output to screen
	printf("MD5: ");
	print_hex2(digest, sizeof(digest));
	printf("\n");

	// Output to file
	FILE *fp = fopen(out_path, "w+");
	if(fp == NULL) {
		printf("Could not open file %s to write md5 hash\n", out_path);
		return;
	}

	fprintf(fp, "MD5: ");
	for(int i = 0; i < sizeof(digest); i++) {
		fprintf(fp, "%02x", digest[i]);
	}

	fclose(fp);
}

/*
 * Reads the entire disk image buffer and populates corresponding structures (MBR, File Systems)
 *
 * @param disk Disk Image state structure
 */
void disk_parse(disk_img *disk) {
	// MBR
	disk->master_boot_record = mbr_new();
	mbr_read(disk->buffer, disk->master_boot_record);

	// Create and parse partitions based on their locations in the MBR
	uint8_t part_type = 0;
	for(int i = 0; i < 4; i++) {
		part_type = disk->master_boot_record->pentry[i].type;

		if(part_type == PT_FAT12 || part_type == PT_FAT16B || part_type == PT_FAT32) {
			disk->partition[i] = fat_new_partition(part_type);

			// Move byte buffer position to the starting posititon of the partition
			// Calculate this by multiplying the relative sector by 512 (default bytes per sector)
			disk->buffer->pos = disk->master_boot_record->pentry[i].relative_sector * 512;

			fat_read_partition(disk->buffer, (fat_partition*)(disk->partition[i]));
		} else {
			printf("disk_parse: Could not read partition of type %i\n", part_type);
		}
	}
}

/*
 * Outputs a human readable representation of the major data structures in the disk image
 *
 * @param disk Disk Image state structure
 * @param verbose If true, display every field in every data structure. If false, only display major elements
 */
void disk_print(disk_img *disk, bool verbose) {
	/*printf("CHECKSUMS\n");
	printf("==================================================\n");
	disk_output_sha1("MD5.txt");
	disk_output_md5("SHA1.txt");
	printf("\n");*/

	printf("MBR ANALYSIS\n");
	mbr_print(disk->master_boot_record, verbose);
	printf("\n");

	printf("VBR ANALYSIS\n");
	uint8_t part_type = 0;
	for(int i = 0; i < 4; i++) {
		part_type = disk->master_boot_record->pentry[i].type;

		printf("==================================================\n");
		char *type_str = mbr_get_partition_str(part_type);
		printf("Partition %i (%s):\n", i, type_str);
		free(type_str);

		if(part_type == PT_FAT12 || part_type == PT_FAT16B || part_type == PT_FAT32) {
			fat_print_partition((fat_partition*)(disk->partition[i]), verbose);
		} else {
			printf("Printing for this volume type not yet supported");
		}

		printf("==================================================\n\n");
	}
}

/*
 * Release all resources related to the currently open disk image
 *
 * @param disk Disk Image state structure
 */
void disk_destroy(disk_img *disk) {
	if(disk->master_boot_record != NULL)
		mbr_free(disk->master_boot_record);

	uint8_t part_type = 0;
	for(int i = 0; i < 4; i++) {
		part_type = disk->master_boot_record->pentry[i].type;

		if(part_type == PT_FAT12 || part_type == PT_FAT16B || part_type == PT_FAT32) {
			fat_free_partition((fat_partition*)(disk->partition[i]));
		}
	}

	disk->file_size = 0;
	free(disk->file_buf);
}
