//
// Hack-the-book - PIW Cover CrackMe
// http://blog.malcom.pl/2020/hackowanie-ksiazki-piw-cover-crackme.html
//
// Copyright (C) 2020 by Marcin 'Malcom' Malich <me@malcom.pl>
// Released under the MIT License
//
// Password generator for CrackMe (code.bin/code.asm)
//

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <intrin.h>

#define Size 23

const uint32_t Checksum[Size] = {
	0x564D94CE, 0x56487985, 0xCD6966E6, 0x791B5538,
	0xBDC0BFB7, 0x8ECBF593, 0xC652C4A2, 0x94A3EBF7,
	0x2673B49E, 0x89F7731E, 0x32C1EB44, 0x7886F083,
	0x52E2BB44, 0xC7FDFFAA, 0x69F9005A, 0xE04743AC,
	0x44229524, 0xE8032961, 0x8CC30F1E, 0x084CDEA3,
	0xEB48D1AD, 0x90809740, 0xE4292D5A
};

int main() {

	char passwd[Size + 1] = { 0 };

	for (int i = Size - 1; i >= 0; i--) {
		for (char c = 0; c < UCHAR_MAX; c++) {
			passwd[i] = c;

			uint32_t sum = 0;
			for (int x = i; x < Size; x++)
				sum = _mm_crc32_u8(sum, passwd[x]);

			if (sum == Checksum[i])
				break;
		}
	}

	printf("passwd: %s\n", passwd);
	return 0;
}
