/**
   dd_reader
   main.c
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

#include <stdio.h>
#include <unistd.h>

#include "disk.h"
#include "mbr.h"

void print_help() {
	printf("Usage: dd_reader [OPTIONS] -f FILE\n");
	printf("-f\tFile path (required). Full path to the raw image.\n");
	printf("OPTIONS:\n");
	printf("-h\tHelp. Display this message\n");
	/*printf("-p TYPE\tFile is a single partition dump, do not attempt to read an MBR/GPT\n");
	printf("\tValid Types: FAT, NTFS\n");*/
	printf("-v\tVerbose. Print out all fields for all data structures\n");
	printf("\n");
}

int main(int argc, char **argv) {
	int opt;
	bool verbose = false, img_is_partition = false;
	char *file_path = NULL, *partition_type = false;

	// Parse command line options
	while((opt = getopt(argc, argv, "f:hp:v")) != -1) {
		switch(opt) {
			case 'f':
				file_path = new_string(optarg);
				break;

			case 'h':
				print_help();
				return 0;
				break;

			case 'p':
				img_is_partition = true;
				partition_type = new_string(optarg);
				break;

			case 'v':
				verbose = true;
				break;

			default:
				printf("Unknown argument: %c\n", (char)opt);
				print_help();
				return -1;
				break;
		}
	}

	if(file_path == NULL) {
		printf("Path to disk image must be set (-f)\n");
		print_help();
		return 0;
	}

	if(!img_is_partition) {
		disk_img *disk = disk_init(file_path);
		disk_parse(disk);
		disk_print(disk, verbose);
		disk_destroy(disk);
	} else {
		if(strcmp(partition_type, "FAT")) {
			byte_buffer *fat_bb = bb_new_from_file(file_path, "rb", true);
			fat_partition *fat_par = fat_new_partition();

			fat_read_partition(fat_bb, fat_par);
			fat_print_partition(fat_par, verbose);
			
			fat_free_partition(fat_par);
			bb_free(fat_bb);
		} else {
			printf("Unsupported partition type\n");
		}
	}

	if(file_path != NULL)
		free(file_path);
	if(partition_type != NULL)
		free(partition_type);

	return 0;
}

