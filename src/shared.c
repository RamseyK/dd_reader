#include "shared.h"

void print_ascii(unsigned char *buf, size_t len) {
	for(int i = 0; i < len; i++) {
		printf("%c", buf[i]);
	}
}

void print_hex(unsigned char *buf, size_t len) {
	for(int i = 0; i < len; i++) {
		printf("%02x ", buf[i]);
	}
}

void print_hex2(unsigned char *buf, size_t len) {
	for(int i = 0; i < len; i++) {
		printf("%02x", buf[i]);
	}
}

char *new_string(const char *str) {
	return (char*)malloc(strlen(str) + 1);
}
