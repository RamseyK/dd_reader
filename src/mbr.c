#include "mbr.h"

mbr *mbr_new() {
	mbr *m = (mbr*)malloc(sizeof(mbr));
	memset(m, 0, sizeof(mbr));

	return m;
}

void mbr_free(mbr *m) {
	free(m);
}

char *mbr_get_partition_str(uint8_t type) {
	char *str = (char*)calloc(128, sizeof(char));

	switch(type) {
		case PT_FAT12:
			strcpy(str, "DOS 12-bit FAT");
		break;

		/*case PT_FAT16:
			strcpy(str, "DOS 16-bit FAT for partitions smaller than 32 MB");
		break;*/

		case PT_FAT16B:
			strcpy(str, "DOS 16-bit FAT for partitions larger than 32 MB");
		break;

		case PT_NTFS:
			strcpy(str, "NTFS");
		break;

		case PT_FAT32:
			strcpy(str, "DOS 32-bit FAT");
		break;

		default:
			strcpy(str, "Unknown");
		break;
	}

	return str;
}

void mbr_read(byte_buffer *bb, mbr *m) {
	// +446 boot loader
	bb_get_bytes_in(bb, sizeof(m->boot_ldr), m->boot_ldr);
	
	// Read 4 partition entries
	partition_entry *pe;
	for(int i = 0; i < 4; i++) {
		pe = &(m->pentry[i]);
		memset(pe, 0, sizeof(partition_entry));

		pe->boot_indicator = bb_get(bb);
		pe->head_start = bb_get(bb);
		pe->sector_start = bb_get(bb);
		pe->cylinder_start = bb_get(bb);
		pe->type = bb_get(bb);
		pe->head_end = bb_get(bb);
		pe->sector_end = bb_get(bb);
		pe->cylinder_end = bb_get(bb);
		pe->relative_sector = bb_get_int(bb);
		pe->num_sectors = bb_get_int(bb);
	}

	m->sig1 = bb_get(bb);
	m->sig2 = bb_get(bb);
	if(m->sig1 != 0x55 || m->sig2 != 0xAA) {
		printf("Warning: MBR boot signature does not match 0x55 0xAA!\n");
	}
}

void mbr_write(byte_buffer *bb, mbr *m) {

}

void mbr_print(mbr* m, bool verbose) {
	printf("==================================================\n");

	/*if(verbose) {
		printf("Boot Loader: ");
		for(int i = 0; i < sizeof(m->boot_ldr); i++) {
			printf("0x%02x ", m->boot_ldr[i]);
		}
		printf("\n");
	}*/

	// Print all partitions
	partition_entry *pe;
	char *part_str = NULL;
	for(int i = 0; i < 4; i++) {
		pe = &(m->pentry[i]);

		part_str = mbr_get_partition_str(pe->type);

		// Verbose
		if(verbose) {
			printf("Partiton %i\n", i+1);
			printf("Boot Indicator: 0x%02x\n", pe->boot_indicator);
			printf("Head Start: 0x%02x\n", pe->head_start);
			printf("Sector Start: 0x%02x\n", pe->sector_start);
			printf("Cylinder Start: 0x%02x\n", pe->cylinder_start);
			printf("Type: 0x%02x %s\n", pe->type, part_str);
			printf("Head end: 0x%02x\n", pe->head_end);
			printf("Sector end: 0x%02x\n", pe->sector_end);
			printf("Cylinder end: 0x%02x\n", pe->cylinder_end);
			printf("Relative Sector: %i\n", pe->relative_sector);
			printf("Num Sectors: %i\n", pe->num_sectors);
			printf("\n");
		} else { // Regular format
			printf("(%02x) %s, %i, %i\n", pe->type, part_str, pe->relative_sector, pe->num_sectors);
		}

		free(part_str);
	}
}
