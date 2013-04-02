/**
   dd_reader
   mbr.h
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
void mbr_read(byte_buffer *bb, mbr *m);
void mbr_write(byte_buffer *bb, mbr *m);
void mbr_print(mbr *m, bool verbose);

#endif