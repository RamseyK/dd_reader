/**
   dd_reader
   shared.c
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
