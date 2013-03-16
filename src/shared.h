#ifndef _SHARED_H_
#define _SHARED_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*
 * Partition Types
 */
#define PT_EMPTY 0x00
#define PT_FAT12 0x01
//#define PT_FAT16 0x04 // Under 32MB
#define PT_FAT16B 0x06 // Over 32MB
#define PT_NTFS 0x07
#define PT_FAT32 0x0B

void print_ascii(unsigned char *buf, size_t len);
void print_hex(unsigned char *buf, size_t len);
void print_hex2(unsigned char *buf, size_t len);
char *new_string(const char *str);

#endif
