#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

#define G "\033[0;32m"
#define R "\033[0m"
#define E "\033[1;31m"

void fe(const char *filename) {
	FILE *file;
	unsigned char e_ident[16];
	file = fopen(filename, "rb");
	if (!file) {
		perror("error opening ELF file");
		return;
	}
	if (fread(e_ident, 1, sizeof(e_ident), file) < sizeof(e_ident)) {
		perror("error reading ELF header");
		fclose(file);
		return;
	}
	fclose(file);
	printf("architecture: ");
	switch (e_ident[4]) {
	case 1:
		printf(G"[32-bit]\n"R);
		break;
	case 2:
		printf(G"[64-bit]\n"R);
		break;
	default:
		printf(E"[unknown]\n"R);
		break;
	}
	if (e_ident[5] == 1) {
		printf("linked:"G"[dynamically]\n"R);
	} else {
		printf("linked:"G"[statically]\n"R);
	}
	printf("CPU type: ");
	switch (e_ident[16]) {
	case 0x28:
		printf(G"[ARM]\n"R);
		break;
	case 0x3E:
		printf(G"[x86_64]\n"R);
		break;
	case 0x03:
		printf(G"[x86]\n"R);
		break;
	default:
		printf(E"[unknown]\n"R);
		break;
	}
	printf("data encoding: ");
	switch (e_ident[EI_DATA]) {
	case ELFDATA2LSB:
		printf(G"[little endian]\n"R);
		break;
	case ELFDATA2MSB:
		printf(G"[big endian]\n"R);
		break;
	default:
		printf(E"[unknown]\n"R);
		break;
	}
	printf("version: "G"[%d]\n"R, e_ident[EI_VERSION]);
	printf("OS/ABI: "G"[%d]\n"R, e_ident[EI_OSABI]);
	printf("ABI version: "G"[%d]\n"R, e_ident[EI_ABIVERSION]);
}

void ff(const char *filename) {
	FILE *file;
	unsigned char header[4];
	file = fopen(filename, "rb");
	if (!file) {
		perror("error opening file");
		return;
	}
	if (fread(header, 1, sizeof(header), file) < sizeof(header)) {
		perror("error reading file");
		fclose(file);
		return;
	}
	fclose(file);
	if (header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' && header[3] == 'F') {
		printf("%s: "G"[ELF executable linkable format]\n"R, filename);
		fe(filename);
	} else if (header[0] == '#' && header[1] == '!') {
		printf("%s: "G"[POSIX shell script]\n"R, filename);
	} else if (header[0] == 0x42 && header[1] == 0x5A) {
		printf("%s: "G"[BZIP2 compressed format]\n"R, filename);
	} else if (header[0] == 0x50 && header[1] == 0x4B) {
		printf("%s: "G"[ZIP file format]\n"R, filename);
	} else if (header[0] == 0x37 && header[1] == 0x7A) {
		printf("%s: "G"[7-Zip compressed format]\n"R, filename);
	} else if (header[0] == 0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
		printf("%s: "G"[PNG portable network graphics]\n"R, filename);
	} else if (header[0] == 'B' && header[1] == 'M') {
		printf("%s: "G"[BMP bitmap image file]\n"R, filename);
	} else if (header[0] == 'G' && header[1] == 'I' && header[2] == 'F') {
		printf("%s: "G"[GIF graphics interchange format]\n"R, filename);
	} else if (header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46) {
		printf("%s: "G"[PDF document format]\n"R, filename);
	} else if (header[0] == 0x4D && header[1] == 0x5A) {
		printf("%s: "G"[PE executable format]\n"R, filename);
	} else if (header[0] == 0x7F && header[1] == 'S' && header[2] == 'N' && header[3] == 'A') {
		printf("%s: "G"[SNAX file format]\n"R, filename);
	} else if (header[0] == 0x1F && header[1] == 0x8B) {
		printf("%s: "G"[GZIP compressed format]\n"R, filename);
	} else if (header[0] == 0x52 && header[1] == 0x61 && header[2] == 0x72 && header[3] == 0x21) {
		printf("%s: "G"[RAR archive format]\n"R, filename);
	} else if (header[0] == 0x52 && header[1] == 0x49) {
		printf("%s: "G"[RIFF file format]\n"R, filename);
	} else if (header[0] == 0x4D && header[1] == 0x54) {
		printf("%s: "G"[MIDI file format]\n"R, filename);
	} else if (header[0] == 0x1A && header[1] == 0x45) {
		printf("%s: "G"[matroska file format]\n"R, filename);
	} else if (header[0] == 0x66 && header[1] == 0x74) {
		printf("%s: "G"[FLAC audio format]\n"R, filename);
	} else if (header[0] == 0x4F && header[1] == 0x67) {
		printf("%s: "G"[OGG audio format]\n"R, filename);
	} else {
		printf("%s: "E"[unknown file type]\n"R, filename);
	}
}

int main(int z, char *x[]) {
	if (z < 2) {
		fprintf(stderr, "usage: %s <file>\n", x[0]);
		return EXIT_FAILURE;
	}
	ff(x[1]);
	return EXIT_SUCCESS;
}
