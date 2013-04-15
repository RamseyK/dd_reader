/**
   dd_reader
   fat.h
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

#ifndef _FAT_H_
#define _FAT_H_

#include <math.h>

#include "bytebuffer.h"
#include "shared.h"
#include "mbr.h"

#define FAT16_BOOTSTRAP_SIZE 448
#define FAT32_BOOTSTRAP_SIZE 420

/*
 * BIOS Parameter Block as part of the Boot Sector
 * Typical values are indicated. Postfix of _f16 indicates FAT12/16 values, _f32 for FAT32
 */
typedef struct fat_bpb_t {
	// 512 (default), 1024, 2048, 4096
	uint16_t bytes_per_sector;

	// Must not take on a value s.t. bytes_per_sector * sectors_per_cluster > 32K. 1,2,4,8,16,32,64,128
	uint8_t sectors_per_cluster;

	// Number of sectors to contain BS, FSINO, additional reserved sectors. Always starts at sector 0
	uint16_t reserved_sectors;

	// # of FATs (1 primary, 1 backup). Always 2
	uint8_t num_fats;

	// FAT12/16: # of 32 byte directory entries (512 default). FAT32: 0, Unused
	uint16_t root_entries_f16;

	// FAT12/16: Total # of sectors on volume. If 0, total_sectors_32 is used. FAT32: 0, Unused
	uint16_t total_sectors_16bit;

	// Media type. 0xF8 (non removable)
	uint8_t media_descriptor;

	// FAT12/16: Sectors occupied by one FAT. FAT32: 0, Unused
	uint16_t sectors_per_fat_f16;

	uint16_t sectors_per_track;
	uint16_t num_heads;

	// # of sectors on the disk preceding the start of the volume. (before the boot sector itself) Uused in order to calculate the absolute offset to the root directory and data areas.
	uint32_t hidden_sectors;

	// FAT12/16: If total_sectors_16 is 0, this must be set. FAT32: Must be set. Total # of sectors on volume.
	uint32_t total_sectors_32bit;

	///
	// FAT32 specific
	///

	// Number of sectors occupied by one FAT
	uint32_t sectors_per_fat_f32;

	// Bits 0 - 3: Zero based number of active FAT. Bit 7: 0 = FAT is mirrored. 1 = One FAT is active
	uint16_t eflags_f32;

	// High = Major. Low = Minor. Default: 0 0
	uint16_t version_f32;

	// Cluster # of the first cluster of the root directory (usually 2)
	uint32_t root_cluster_f32;

	// Sector number of FSINFO structure (usually 1)
	uint16_t fsinfo_sector_f32;

	// Non zero indicates sector number in the reserved area is a copy of the boot record. Default 6
	uint16_t backup_sector_f32;

	// Unused. Set to 0
	uint8_t reserved_f32[12];
} fat_bpb;

/*
 * Extended BIOS Parameter Block as part of the Boot Sector
 */
typedef struct fat_ebpb_t {
	uint8_t physical_drive_num;
	uint8_t reserved; // 0
	uint8_t eb_sig; // 0x29
	uint32_t volume_serial; // Random serial made when creating the disk
	uint8_t volume_label[11]; // Should match label file in root directory. Default: "NO NAME    "
	uint8_t system_id[8]; // "FAT32   ", "FAT16   ", "FAT12   ", "FAT     "
} fat_ebpb;

/*
 * Boot sector portion of a FAT formatted disk
 */
typedef struct fat_boot_sector_t {
	uint8_t jmp[3]; // x86 jump instruction
	uint8_t oem_id[8]; // OS that formatted the disk
	fat_bpb bpb; // BIOS parameter block
	fat_ebpb ebpb; // Extended BIOS parameter block
	uint8_t *bootstrap_code; // See FATXX_BOOTSTRAP_SIZE
	// End of sector marker
	uint8_t sig_end1;
	uint8_t sig_end2;
} fat_boot_sector, fat_bs;

/*
 * FAT File System Information Structure (FAT32 only)
 * Located at fsinfo_sector_f32 from BPB (usually sector 1)
 */
typedef struct fat_fsinfo_t {
	uint32_t sig_begin; // 0x41615252 (little). (0x52 0x52 0x61 0x41 = "RRaA")
	uint8_t reserved1[480]; // 0
	uint32_t sig_data_begin; // 0x61417272 (little). (0x72 0x72 0x41 0x61 = "rrAa")
	uint32_t free_cluster_count; // last known free cluster count. 0xFFFFFFFF = unknown
	uint32_t next_free_cluster; // Hint for the driver to where it should start looking for free clusters. 0xFFFFFFFF = No hint, start at cluster 2
	uint8_t reserved2[12]; // 0
	uint32_t sig_end; // 0xAA550000 (little). 00 00 55 AA (big). Last 2 bytes match end of sector marker
} fat_fsinfo;

/*
 * FAT partition structure
 */
typedef struct fat_partition_t {
	// Not part of the actual layout
	uint8_t type; // Used for identifying the type of FAT. See Partition Types in shared.h
	uint32_t start_pos; // byte_buffer position that points to the beginning of the partition

	// Reserved section. Size = Number of reserved sectors
	fat_bs *boot_sector;
	fat_fsinfo *fsinfo; // FAT32 only

	// FATs. Size = Num of FATs * Sectors per FAT

	// Root Directory (FAT12/16 only). Size = (Num of root entries * 32) / bytes per sector

	// Data Region. Size = Num of Clusters * Sectors per cluster

} fat_partition;

/*
 * FAT functions
 */

// Overall partition
fat_partition *fat_new_partition();
void fat_free_partition(fat_partition *part);
void fat_read_partition(byte_buffer *bb, fat_partition *part);
void fat_write_partition(byte_buffer *bb, fat_partition *part);
void fat_print_partition(fat_partition *part, bool verbose);

// Location/Size calculation helper functions
uint32_t fat_sectors_per_fat(fat_partition *part);
uint32_t fat_rootdir_size(fat_partition *part);
uint32_t fat_rootdir_start_rel(fat_partition *part);
uint32_t fat_rootdir_start_abs(fat_partition *part);
uint32_t fat_data_size(fat_partition *part);
uint32_t fat_data_start_rel(fat_partition *part);
uint32_t fat_data_start_abs(fat_partition *part);
uint32_t fat_count_clusters(fat_partition *part);
uint32_t fat_cluster_to_sector_rel(fat_partition *part, uint32_t cluster);

// Reserved Sectors
fat_bs *fat_new_boot_sector();
void fat_free_boot_sector(fat_bs *bs);
void fat_read_boot_sector(byte_buffer *bb, fat_partition *part);
void fat_read_backup_boot_secotr(byte_buffer *bb, fat_partition *part);
void fat_write_boot_sector(byte_buffer *bb, fat_partition *part);

fat_fsinfo *fat_new_fsinfo();
void fat_free_fsinfo(fat_fsinfo *fsi);
void fat_read_fsinfo(byte_buffer *bb, fat_partition *part);
void fat_write_fsinfo(byte_buffer *bb, fat_partition *part);


#endif
