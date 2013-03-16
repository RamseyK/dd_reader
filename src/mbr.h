#ifndef _MBR_H_
#define _MBR_H_

#include "bytebuffer.h"
#include "shared.h"

/*
 * Structure for Partition entry within MBR
 */
 typedef struct partition_entry_t {
	uint8_t boot_indicator; // 0x80 = Bootable. 0x00 = Off
	uint8_t head_start;
	uint8_t sector_start;
	uint8_t cylinder_start;
	uint8_t type;
	uint8_t head_end;
	uint8_t sector_end;
	uint8_t cylinder_end;
	uint32_t relative_sector; // Starting sector relative to the start of the disk
	uint32_t num_sectors; // Number of sectors in the partition
} partition_entry;

/*
 * MBR structure
 */
typedef struct mbr_t {
	uint8_t boot_ldr[446];
	partition_entry pentry[4];
	// End of sector marker
	uint8_t sig1;
	uint8_t sig2;
} mbr;

/*
 * MBR functions
 */
mbr *mbr_new();
void mbr_free(mbr *m);
char *mbr_get_partition_str(uint8_t type);
void mbr_read(byte_buffer *bb, mbr *m);
void mbr_write(byte_buffer *bb, mbr *m);
void mbr_print(mbr *m, bool verbose);

#endif