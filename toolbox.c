#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <elf.h>

#define G "\x1b[92m"
#define R "\x1b[0m"
#define E "\x1b[91m"
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

// --- ls ---
int fls(int argc, char **argv) {
	const char *dir = argc > 1 ? argv[1] : ".";
	DIR *dp = opendir(dir);
	if (!dp) {
		perror("opendir");
		return 1;
	}
	struct dirent *e;
	while ((e = readdir(dp))) printf("%s\n", e->d_name);
	closedir(dp);
	return 0;
}

// --- cp ---
int fcp(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: %s <src> <dst>\n", argv[0]);
		return 1;
	}
	FILE *fs = fopen(argv[1], "rb");
	if (!fs) {
		perror("fopen src");
		return 1;
	}
	FILE *fd = fopen(argv[2], "wb");
	if (!fd) {
		perror("fopen dst");
		fclose(fs);
		return 1;
	}
	char buf[4096];
	size_t n;
	while ((n = fread(buf, 1, sizeof(buf), fs)))
		if (fwrite(buf, 1, n, fd) != n) {
			perror("fwrite");
			break;
		}
	fclose(fs);
	fclose(fd);
	return 0;
}

// --- mkdir ---
int fmkdir(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <dir>\n", argv[0]);
		return 1;
	}
	if (mkdir(argv[1], 0777) == -1) {
		perror("mkdir");
		return 1;
	}
	return 0;
}

// --- cat ---
int fcat(int argc, char **argv) {
	if (argc == 1) {
		char buf[4096];
		while (fgets(buf, sizeof(buf), stdin)) fputs(buf, stdout);
		return 0;
	}
	FILE *f;
	for (int i = 1; i < argc; i++) {
		if (!(f = fopen(argv[i], "r"))) {
			perror("fopen");
			continue;
		}
		char buf[4096];
		while (fgets(buf, sizeof(buf), f)) fputs(buf, stdout);
		fclose(f);
	}
	return 0;
}

// --- echo ---
int fecho(int argc, char **argv) {
	for (int i = 1; i < argc; i++) printf("%s%c", argv[i], i < argc-1 ? ' ' : '\n');
	return 0;
}

// --- kill ---
int fkill(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: %s <SIGNAL> <PID>\n", argv[0]);
		return 1;
	}
	int sig = SIGTERM;
	int pid_pos = 2;
	if (strcmp(argv[1], "-s") == 0) {
		if (argc < 4) {
			fprintf(stderr, "missing SIGNAL or PID\n");
			return 1;
		}
		sig = atoi(argv[2]);
		pid_pos = 3;
	}
	pid_t pid = atoi(argv[pid_pos]);
	if (kill(pid, sig) == -1) {
		perror("kill");
		return 1;
	}
	return 0;
}

// --- clear ---
int fclear(int argc, char **argv) {
	printf("\033c");
	return 0;
}

// --- rm ---
int frm(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		return 1;
	}
	int r = 0;
	for (int i = 1; i < argc; i++) {
		if (unlink(argv[i]) == -1) {
			perror("unlink");
			r = 1;
		}
	}
	return r;
}

// --- head ---
int fhead(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file> <lines>\n", argv[0]);
		return 1;
	}
	int lines = argc > 2 ? atoi(argv[2]) : 10;
	FILE *f = fopen(argv[1], "r");
	if (!f) {
		perror("fopen");
		return 1;
	}
	char buf[4096];
	while (lines-- > 0 && fgets(buf, sizeof(buf), f)) printf("%s", buf);
	fclose(f);
	return 0;
}

// --- diff ---
int fdiff(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: %s <file1> <file2>\n", argv[0]);
		return 1;
	}
	FILE *f1 = fopen(argv[1], "r");
	if (!f1) {
		perror("fopen file1");
		return 1;
	}
	FILE *f2 = fopen(argv[2], "r");
	if (!f2) {
		perror("fopen file2");
		fclose(f1);
		return 1;
	}
	char line1[65536];
	char line2[65536];
	int line_num = 1;
	int diff_found = 0;
	while (1) {
		char *res1 = fgets(line1, sizeof(line1), f1);
		char *res2 = fgets(line2, sizeof(line2), f2);
		if (!res1 && !res2) {
			break;
		}
		if (!res1 || !res2) {
			diff_found = 1;
			if (res1) {
				printf("only in %s at line %d:\n> %s", argv[1], line_num, line1);
				while (fgets(line1, sizeof(line1), f1)) {
					printf("> %s", line1);
					line_num++;
				}
			} else {
				printf("only in %s at line %d:\n< %s", argv[2], line_num, line2);
				while (fgets(line2, sizeof(line2), f2)) {
					printf("< %s", line2);
					line_num++;
				}
			}
			break;
		}
		if (strcmp(line1, line2) != 0) {
			diff_found = 1;
			printf("difference at line %d:\n", line_num);
			printf("< %s", line1);
			printf("> %s", line2);
		}
		line_num++;
	}
	fclose(f1);
	fclose(f2);
	return diff_found;
}


// --- file ---
void felf(const char *filename) {
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
		printf(G "[32-bit]\n" R);
		break;
	case 2:
		printf(G "[64-bit]\n" R);
		break;
	default:
		printf(E "[unknown]\n" R);
		break;
	}
	if (e_ident[5] == 1) {
		printf("linked:" G "[dynamically]\n" R);
	} else {
		printf("linked:" G "[statically]\n" R);
	}
	printf("CPU type: ");
	unsigned char e_machine[2] = {0, 0};
	file = fopen(filename, "rb");
	if (!file) {
		perror("error reopening ELF file");
		return;
	}
	if (fseek(file, 18, SEEK_SET) != 0 || fread(e_machine, 1, 2, file) < 2) {
		perror("error reading ELF machine type");
		fclose(file);
		return;
	}
	fclose(file);
	unsigned short machine = e_machine[0] | (e_machine[1] << 8);
	switch (machine) {
	case 0x28:
		printf(G "[ARM]\n" R);
		break;
	case 0x3E:
		printf(G "[x86_64]\n" R);
		break;
	case 0x03:
		printf(G "[x86]\n" R);
		break;
	default:
		printf(E "[unknown]\n" R);
		break;
	}
	printf("data encoding: ");
	switch (e_ident[EI_DATA]) {
	case ELFDATA2LSB:
		printf(G "[little endian]\n" R);
		break;
	case ELFDATA2MSB:
		printf(G "[big endian]\n" R);
		break;
	default:
		printf(E "[unknown]\n" R);
		break;
	}
	printf("version: " G "[%d]\n" R, e_ident[EI_VERSION]);
	printf("OS/ABI: " G "[%d]\n" R, e_ident[EI_OSABI]);
	printf("ABI version: " G "[%d]\n" R, e_ident[EI_ABIVERSION]);
}

void ftype(const char *filename) {
	FILE *file;
	unsigned char header[16];
	file = fopen(filename, "rb");
	if (!file) {
		perror("error opening file");
		return;
	}
	if (fread(header, 1, sizeof(header), file) < 4) {
		perror("error reading file");
		fclose(file);
		return;
	}
	fclose(file);
	if (header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' && header[3] == 'F') {
		printf("%s: " G "[ELF executable linkable format]\n" R, filename);
		felf(filename);
	} else if (header[0] == '#' && header[1] == '!') {
		printf("%s: " G "[POSIX shell script]\n" R, filename);
	} else if (header[0] == 0x42 && header[1] == 0x5A) {
		printf("%s: " G "[BZIP2 compressed format]\n" R, filename);
	} else if (header[0] == 0x50 && header[1] == 0x4B) {
		printf("%s: " G "[ZIP file format]\n" R, filename);
	} else if (header[0] == 0x37 && header[1] == 0x7A) {
		printf("%s: " G "[7-Zip compressed format]\n" R, filename);
	} else if (header[0] == 0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
		printf("%s: " G "[PNG portable network graphics]\n" R, filename);
	} else if (header[0] == 'B' && header[1] == 'M') {
		printf("%s: " G "[BMP bitmap image file]\n" R, filename);
	} else if (header[0] == 'G' && header[1] == 'I' && header[2] == 'F') {
		printf("%s: " G "[GIF graphics interchange format]\n" R, filename);
	} else if (header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46) {
		printf("%s: " G "[PDF document format]\n" R, filename);
	} else if (header[0] == 0x4D && header[1] == 0x5A) {
		printf("%s: " G "[PE executable format]\n" R, filename);
	} else if (header[0] == 0x7F && header[1] == 'S' && header[2] == 'N' && header[3] == 'A') {
		printf("%s: " G "[SNAX file format]\n" R, filename);
	} else if (header[0] == 0x1F && header[1] == 0x8B) {
		printf("%s: " G "[GZIP compressed format]\n" R, filename);
	} else if (header[0] == 0x52 && header[1] == 0x61 && header[2] == 0x72 && header[3] == 0x21) {
		printf("%s: " G "[RAR archive format]\n" R, filename);
	} else if (header[0] == 0x52 && header[1] == 0x49) {
		printf("%s: " G "[RIFF file format]\n" R, filename);
	} else if (header[0] == 0x4D && header[1] == 0x54) {
		printf("%s: " G "[MIDI file format]\n" R, filename);
	} else if (header[0] == 0x1A && header[1] == 0x45) {
		printf("%s: " G "[matroska file format]\n" R, filename);
	} else if (header[0] == 0x66 && header[1] == 0x74) {
		printf("%s: " G "[FLAC audio format]\n" R, filename);
	} else if (header[0] == 0x4F && header[1] == 0x67) {
		printf("%s: " G "[OGG audio format]\n" R, filename);
	} else {
		printf("%s: " E "[unknown file type]\n" R, filename);
	}
}

int ffile(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: file <filename>\n");
		return 1;
	}
	ftype(argv[1]);
	return 0;
}

// --- grep ---
int fgrep(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: %s <pattern> <file>\n", argv[0]);
		return 1;
	}
	int r = 0;
	for (int i = 2; i < argc; i++) {
		FILE *f = fopen(argv[i], "r");
		if (!f) {
			perror("fopen");
			r = 1;
			continue;
		}
		char buf[65536];
		while (fgets(buf, sizeof(buf), f)) {
			char *m;
			if ((m = strstr(buf, argv[1]))) {
				printf("%.*s\x1b[92m%s\x1b[0m%s", (int)(m - buf), buf, argv[1], m + strlen(argv[1]));
			}
		}
		fclose(f);
	}
	return r;
}

// --- rmdir ---
int frmdir(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <dir>\n", argv[0]);
		return 1;
	}
	int r = 0;
	for (int i = 1; i < argc; i++) {
		if (rmdir(argv[i]) == -1) {
			perror("rmdir");
			r = 1;
		}
	}
	return r;
}

// --- ascii2hex ---
int fa2x(int argc, char **argv) {
	if (argc == 1) {
		char z[65536];
		if (!fgets(z, sizeof(z), stdin)) {
			fprintf(stderr, "failed to read input\n");
			return 1;
		}
		z[strcspn(z, "\n")] = 0;
		for (int i = 0; i < (int)strlen(z); i++) {
			printf("\\x%02X", (unsigned char)z[i]);
		}
		printf("\n");
	} else {
		for (int argi = 1; argi < argc; argi++) {
			const char *z = argv[argi];
			for (int i = 0; i < (int)strlen(z); i++) {
				printf("\\x%02X", (unsigned char)z[i]);
			}
			printf("\n");
		}
	}
	return 0;
}

// --- sync ---
int ffsync(int argc, char **argv) {
	(void)argc;
	(void)argv;
	sync();
	printf("filesystem sync completed...\n");
	return 0;
}


// --- main ---
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("usage: %s <command> <args>\n\n" "commands: ls | cp | mkdir | cat | echo | kill | clear| rm | head | diff | file | grep | rmdir | ascii2hex | sync\n", argv[0]);
		return 1;
	}
	typedef struct {
		const char *c;
		int (*f)(int, char **);
	} m;
	m a[] = {
		{"ls", fls},
		{"cp", fcp},
		{"mkdir", fmkdir},
		{"cat", fcat},
		{"echo", fecho},
		{"kill", fkill},
		{"clear", fclear},
		{"rm", frm},
		{"head", fhead},
		{"diff", fdiff},
		{"file", ffile},
		{"grep", fgrep},
		{"rmdir", frmdir},
		{"ascii2hex", fa2x},
		{"sync", ffsync}
	};
	size_t n = sizeof(a) / sizeof(a[0]);
	const char *in = argv[1];
	for (size_t i = 0; i < n; i++) {
		if (strcmp(in, a[i].c) == 0) {
			return a[i].f(argc - 1, argv + 1);
		}
	}
	fprintf(stderr, "unknown command: %s...\n", in);
	return 1;
}
