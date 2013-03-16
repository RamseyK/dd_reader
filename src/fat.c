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

}

void fat_write_partition(byte_buffer *bb, fat_partition *part) {

}

void fat_print_partition(fat_partition *part, bool verbose) {
	printf("==================================================\n");
	print_ascii(part->boot_sector->oem_id, 8);
	printf("\n");
	printf("==================================================\n");
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
	bs->jmp[0] = bb_get(bb);
	bs->jmp[1] = bb_get(bb);
	bs->jmp[2] = bb_get(bb);

	// OEM ID string
	bb_get_bytes_in(bb, sizeof(bs->oem_id), bs->oem_id);

	// BPB
	bs->bpb.bytes_per_sector = bb_get_short(bb);
	bs->bpb.sectors_per_cluster = bb_get(bb);
	bs->bpb.reserved_sectors = bb_get_short(bb);
	bs->bpb.num_fats = bb_get(bb);
	bs->bpb.root_entries_f16 = bb_get_short(bb);
	bs->bpb.total_sectors_f16 = bb_get_short(bb);
	bs->bpb.media_descriptor = bb_get(bb);
	bs->bpb.sectors_per_fat_f16 = bb_get_short(bb);
	bs->bpb.sectors_per_track = bb_get_short(bb);
	bs->bpb.num_heads = bb_get_short(bb);
	bs->bpb.hidden_sectors = bb_get_int(bb);
	// FAT32 portion of the BPB
	if(part->type == PT_FAT32) {
		bs->bpb.total_sectors_f32 = bb_get_int(bb);
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
		printf("Warning: FAT VBR boot signature does not match 0x55 0xAA!\n");
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