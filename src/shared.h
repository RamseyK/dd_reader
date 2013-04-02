/**
   dd_reader
   shared.h
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

#ifndef _SHARED_H_
#define _SHARED_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*
 * Partition Types
 * See: http://www.win.tue.nl/~aeb/partitions/partition_types-1.html / https://en.wikipedia.org/wiki/Partition_type
 */
#define PT_EMPTY 0x00
#define PT_FAT12 0x01
//#define PT_FAT16 0x04 // Under 32MB
#define PT_FAT16B 0x06 // Over 32MB
#define PT_NTFS 0x07
#define PT_FAT32 0x0B

/*
 * Shared functions
 */

void print_ascii(unsigned char *buf, size_t len);
void print_hex(unsigned char *buf, size_t len);
void print_hex2(unsigned char *buf, size_t len);
char *new_string(const char *str);
char *get_partition_str(uint8_t type);

#endif
