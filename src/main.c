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

int main(int argc, char **argv) {
	disk_img *disk = disk_init("/Users/ramseykant/TestImage1.img");
	disk_parse(disk);
	disk_print(disk, false);
	disk_destroy(disk);
	return 0;
}