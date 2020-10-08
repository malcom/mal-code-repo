//
// Hack-the-book - PIW Cover CrackMe
// http://blog.malcom.pl/2020/hackowanie-ksiazki-piw-cover-crackme.html
//
// Copyright (C) 2020 by Marcin 'Malcom' Malich <me@malcom.pl>
// Released under the MIT License
//
// Code decoder/unpacker (equivalent to decode.asm)
//

#include <stdio.h>
#include <stdint.h>

int main() {

	FILE* fs = fopen("./data.bin", "rb");
	FILE* fd = fopen("./code.bin", "wb");

	uint16_t word;
	while (fread(&word, 1, sizeof(word), fs) == sizeof(word)) {
		word ^= 0x5245;
		fwrite(&word, 1, sizeof(word), fd);
	}

	fclose(fd);
	fclose(fs);

	return 0;
}
