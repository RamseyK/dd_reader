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

#include "disk.h"
#include "mbr.h"

void print_help() {
	printf("Usage: dd_reader [OPTIONS] FILE\n");
	printf("FILE: Path to raw disk image dump\n");
	printf("OPTIONS:\n");
	printf("-h\tHelp. Display this message\n");
	printf("-v\tVerbose. Print out all fields for all data structures\n");
	printf("\n");
}

int main(int argc, char **argv) {
	if(argc != 2) {
		print_help();
		return 0;
	}

	disk_img *disk = disk_init(argv[1]);
	if(disk == NULL)
		return -1;
   
	disk_parse(disk);
	disk_print(disk, true);
	disk_destroy(disk);
	return 0;
}

