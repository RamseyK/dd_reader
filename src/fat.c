/**
   dd_reader
   fat.c
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

#include "fat.h"

fat_partition *fat_new_partition(uint8_t type) {
	fat_partition *part = (fat_partition*)malloc(sizeof(fat_partition));
	memset(part, 0, sizeof(fat_partition));

	part->type = type;

	return part;
}

void fat_free_partition(fat_partition *part) {
	if(part->boot_sector != NULL)
		fat_free_boot_sector(part->boot_sector);

	if(part->fsinfo != NULL)
		fat_free_fsinfo(part->fsinfo);

	free(part);
}

void fat_read_partition(byte_buffer *bb, fat_partition *part) {
	fat_read_boot_sector(bb, part);
}

void fat_write_partition(byte_buffer *bb, fat_partition *part) {

}

void fat_print_partition(fat_partition *part, bool verbose) {
	if(verbose) {
		printf("Boot Sector:\n");
		printf("OEM ID: ");
		print_ascii(part->boot_sector->oem_id, sizeof(part->boot_sector->oem_id));
		printf("\n");
		printf("BIOS Parameter Block:\n");
		printf("Bytes per Sector: %u\n", part->boot_sector->bpb.bytes_per_sector);
		printf("Sectors per Cluster: %u\n", part->boot_sector->bpb.sectors_per_cluster);
		printf("Reserved Sectors: %u\n", part->boot_sector->bpb.reserved_sectors);
		printf("Number of FATs: %u\n", part->boot_sector->bpb.num_fats);
		printf("Root Entries (F16): %u\n", part->boot_sector->bpb.root_entries_f16);
		printf("Total Sectors (16 bit): %u\n", part->boot_sector->bpb.total_sectors_16bit);
		printf("Media: 0x%02x\n", part->boot_sector->bpb.media_descriptor);
		printf("Sectors per FAT (F16): %u\n", part->boot_sector->bpb.sectors_per_fat_f16);
		printf("Sectors per Track: %u\n", part->boot_sector->bpb.sectors_per_track);
		printf("Number of Heads: %u\n", part->boot_sector->bpb.num_heads);
		printf("Hidden Sectors: %u\n", part->boot_sector->bpb.hidden_sectors);
		printf("Total Sectors (32 bit): %u\n", part->boot_sector->bpb.total_sectors_32bit);
		// FAT32 portion of the BPB
		if(part->type == PT_FAT32) {
			printf("Sectors per FAT (F32): %u\n", part->boot_sector->bpb.sectors_per_fat_f32);
			printf("Flags (F32): %u\n", part->boot_sector->bpb.eflags_f32);
			printf("Version (F32): %u\n", part->boot_sector->bpb.version_f32);
			printf("Root Cluster (F32): %u\n", part->boot_sector->bpb.root_cluster_f32);
			printf("FSINFO Sector (F32): %u\n", part->boot_sector->bpb.fsinfo_sector_f32);
			printf("Backup Sector (F32): %u\n", part->boot_sector->bpb.backup_sector_f32);
			printf("Reserved (should all be 0): ");
			print_hex2(part->boot_sector->bpb.reserved_f32, sizeof(part->boot_sector->bpb.reserved_f32));
		}

		printf("\nExtended BIOS Parameter Block:\n");
		printf("Physical Drive Num: %u\n", part->boot_sector->ebpb.physical_drive_num);
		printf("Reserved (should be 0): %u\n", part->boot_sector->ebpb.reserved);
		printf("Signature: 0x%X\n", part->boot_sector->ebpb.eb_sig);
		printf("Volume Serial: 0x%X\n", part->boot_sector->ebpb.volume_serial);
		printf("Volume Label: ");
		print_ascii(part->boot_sector->ebpb.volume_label, sizeof(part->boot_sector->ebpb.volume_label));
		printf("\n");
		printf("System ID: ");
		print_ascii(part->boot_sector->ebpb.system_id, sizeof(part->boot_sector->ebpb.system_id));
		printf("\n");
	} else {

	}
}

fat_bs *fat_new_boot_sector() {
	fat_bs *bs = (fat_bs*)malloc(sizeof(fat_bs));
	memset(bs, 0, sizeof(fat_bs));

	return bs;
}

void fat_free_boot_sector(fat_bs *bs) {
	if(bs->bootstrap_code != NULL)
		free(bs->bootstrap_code);

	free(bs);
}

/*
 * Read's the FAT volume boot record in the byte buffer and sets the relevant information in the partition structure's boot_sector
 */
void fat_read_boot_sector(byte_buffer *bb, fat_partition *part) {
	part->boot_sector = fat_new_boot_sector();
	fat_bs *bs = part->boot_sector;

	// Jump instruction
	bb_get_bytes_in(bb, sizeof(bs->jmp), bs->jmp);

	// OEM ID string
	bb_get_bytes_in(bb, sizeof(bs->oem_id), bs->oem_id);

	// BPB
	bs->bpb.bytes_per_sector = bb_get_short(bb);
	bs->bpb.sectors_per_cluster = bb_get(bb);
	bs->bpb.reserved_sectors = bb_get_short(bb);
	bs->bpb.num_fats = bb_get(bb);
	bs->bpb.root_entries_f16 = bb_get_short(bb);
	bs->bpb.total_sectors_16bit = bb_get_short(bb);
	bs->bpb.media_descriptor = bb_get(bb);
	bs->bpb.sectors_per_fat_f16 = bb_get_short(bb);
	bs->bpb.sectors_per_track = bb_get_short(bb);
	bs->bpb.num_heads = bb_get_short(bb);
	bs->bpb.hidden_sectors = bb_get_int(bb);
	bs->bpb.total_sectors_32bit = bb_get_int(bb);
	// FAT32 portion of the BPB
	if(part->type == PT_FAT32) {
		bs->bpb.sectors_per_fat_f32 = bb_get_int(bb);
		bs->bpb.eflags_f32 = bb_get_short(bb);
		bs->bpb.version_f32 = bb_get_short(bb);
		bs->bpb.root_cluster_f32 = bb_get_int(bb);
		bs->bpb.fsinfo_sector_f32 = bb_get_short(bb);
		bs->bpb.backup_sector_f32 = bb_get_short(bb);
		bb_skip(bb, sizeof(bs->bpb.reserved_f32)); // Skip 12 byte reserved
	}

	// EBPB
	bs->ebpb.physical_drive_num = bb_get(bb);
	bs->ebpb.reserved = bb_get(bb);
	bs->ebpb.eb_sig = bb_get(bb);
	bs->ebpb.volume_serial = bb_get_int(bb);
	bb_get_bytes_in(bb, sizeof(bs->ebpb.volume_label), bs->ebpb.volume_label);
	bb_get_bytes_in(bb, sizeof(bs->ebpb.system_id), bs->ebpb.system_id);

	// Bootstrap code
	if(part->type == PT_FAT12) {
		bs->bootstrap_code = bb_get_bytes(bb, FAT16_BOOTSTRAP_SIZE);
	} else if(part->type == PT_FAT16B) {
		bs->bootstrap_code = bb_get_bytes(bb, FAT16_BOOTSTRAP_SIZE);
	} else if(part->type == PT_FAT32) {
		bs->bootstrap_code = bb_get_bytes(bb, FAT32_BOOTSTRAP_SIZE);
	} else {
	}

	// End signature
	bs->sig_end1 = bb_get(bb);
	bs->sig_end2 = bb_get(bb);
	if(bs->sig_end1 != 0x55 || bs->sig_end2 != 0xAA) {
		printf("Warning: FAT VBR boot signature does not match 0x55 0xAA!. sig1: %X, sig2: %X\n", bs->sig_end1, bs->sig_end2);
	}
}

void fat_write_boot_sector(byte_buffer *bb, fat_partition *part) {

}

fat_fsinfo *fat_new_fsinfo() {
	fat_fsinfo *fsi = (fat_fsinfo*)malloc(sizeof(fat_fsinfo));
	memset(fsi, 0, sizeof(fat_fsinfo));

	return fsi;
}

void fat_free_fsinfo(fat_fsinfo *fsi) {
	free(fsi);
}