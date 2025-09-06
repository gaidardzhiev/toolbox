/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

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
#include <sys/wait.h>

#define X "\x1b[92m"
#define Y "\x1b[0m"
#define Z "\x1b[91m"

// --- ls --- //
int fls(int argc, char **argv) {
	const char *dir = argc > 1 ? argv[1] : ".";
	DIR *dp = opendir(dir);
	if (!dp) {
		perror(Z"opendir() failed"Y);
		return 1;
	}
	struct dirent *e;
	while ((e = readdir(dp))) printf("%s\n", e->d_name);
	closedir(dp);
	return 0;
}

// --- cp --- //
int fcp(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: toolbox %s <src> <dst>\n", argv[0]);
		return 1;
	}
	FILE *fs = fopen(argv[1], "rb");
	if (!fs) {
		perror(Z"fopen() src failed"Y);
		return 2;
	}
	FILE *fd = fopen(argv[2], "wb");
	if (!fd) {
		perror(Z"fopen() dst failed"Y);
		fclose(fs);
		return 3;
	}
	char buf[65536];
	size_t n;
	while ((n = fread(buf, 1, sizeof(buf), fs)))
		if (fwrite(buf, 1, n, fd) != n) {
			perror(Z"fwrite() failed"Y);
			fclose(fs);
			fclose(fd);
			return 4;
			break;
		}
	fclose(fs);
	fclose(fd);
	return 0;
}

// --- mkdir --- //
int fmkdir(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: toolbox %s <dir>\n", argv[0]);
		return 1;
	}
	if (mkdir(argv[1], 0777) == -1) {
		perror(Z"mkdir() failed"Y);
		return 1;
	}
	return 0;
}

// --- cat --- //
int fcat(int argc, char **argv) {
	if (argc == 1) {
		char buf[65536];
		while (fgets(buf, sizeof(buf), stdin)) fputs(buf, stdout);
		return 0;
	}
	FILE *f;
	for (int i = 1; i < argc; i++) {
		if (!(f = fopen(argv[i], "r"))) {
			perror(Z"fopen() failed"Y);
			continue;
		}
		char buf[65536];
		while (fgets(buf, sizeof(buf), f)) fputs(buf, stdout);
		fclose(f);
	}
	return 0;
}

// --- echo --- //
int fecho(int argc, char **argv) {
	for (int i = 1; i < argc; i++) printf("%s%c", argv[i], i < argc-1 ? ' ' : '\n');
	return 0;
}

// --- kill --- //
int fkill(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: toolbox %s <SIGNAL> <PID>\n", argv[0]);
		return 1;
	}
	int sig = atoi(argv[1]);
	pid_t pid = (pid_t)atoi(argv[2]);
	if (kill(pid, sig) == -1) {
		perror(Z"kill() failed"Y);
		return 1;
	}
	return 0;
}

// --- clear --- //
int fclear(int argc, char **argv) {
	printf("\033c");
	return 0;
}

// --- rm --- //
int frm(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: toolbox %s <file>\n", argv[0]);
		return 1;
	}
	int r = 0;
	for (int i = 1; i < argc; i++) {
		if (unlink(argv[i]) == -1) {
			perror(Z"unlink() failed"Y);
			r = 1;
		}
	}
	return r;
}

// --- head --- //
int fhead(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: toolbox %s <file> <lines>\n", argv[0]);
		return 1;
	}
	int lines = argc > 2 ? atoi(argv[2]) : 10;
	FILE *f = fopen(argv[1], "r");
	if (!f) {
		perror(Z"fopen() failed"Y);
		return 1;
	}
	char buf[65536];
	while (lines-- > 0 && fgets(buf, sizeof(buf), f)) printf("%s", buf);
	fclose(f);
	return 0;
}

// --- file --- //
void felf(const char *filename) {
	FILE *file;
	unsigned char e_ident[64];
	file = fopen(filename, "rb");
	if (!file) {
		perror(Z"error opening ELF file"Y);
		return;
	}
	if (fread(e_ident, 1, sizeof(e_ident), file) < sizeof(e_ident)) {
		perror(Z"error reading ELF header"Y);
		fclose(file);
		return;
	}
	fclose(file);
	printf("architecture: ");
	switch (e_ident[4]) {
	case 1:
		printf(X"[32-bit]\n"Y);
		break;
	case 2:
		printf(X"[64-bit]\n"Y);
		break;
	default:
		printf(X"[unknown]\n"Y);
		break;
	}
	printf("CPU type: ");
	unsigned char e_machine[2] = {0, 0};
	file = fopen(filename, "rb");
	if (!file) {
		perror(Z"error reopening ELF file"Y);
		return;
	}
	if (fseek(file, 18, SEEK_SET) != 0 || fread(e_machine, 1, 2, file) < 2) {
		perror(Z"error reading ELF machine type"Y);
		fclose(file);
		return;
	}
	fclose(file);
	unsigned short machine = e_machine[0] | (e_machine[1] << 8);
	switch (machine) {
	case 0x28:
		printf(X"[ARM]\n"Y);
		break;
	case 0x3E:
		printf(X"[x86_64]\n"Y);
		break;
	case 0x03:
		printf(X"[x86]\n"Y);
		break;
	default:
		printf(Z"[unknown]\n"Y);
		break;
	}
	printf("data encoding: ");
	switch (e_ident[5]) {
	case 1:
		printf(X"[little endian]\n"Y);
		break;
	case 2:
		printf(X"[big endian]\n"Y);
		break;
	default:
		printf(Z"[unknown]\n"Y);
		break;
	}
	printf("version: "X"[%d]\n"Y, e_ident[6]);
	printf("OS/ABI: "X"[%d]\n"Y, e_ident[7]);
	printf("ABI version: "X"[%d]\n"Y, e_ident[8]);
}

void fmagic(const char *filename) {
	FILE *file;
	unsigned char header[16];
	file = fopen(filename, "rb");
	if (!file) {
		perror(Z"fopen() failed"Y);
		return;
	}
	if (fread(header, 1, sizeof(header), file) < 4) {
		perror(Z"fread() failed"Y);
		fclose(file);
		return;
	}
	fclose(file);
	if (header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' && header[3] == 'F') {
		printf("%s: "X"[ELF executable linkable format]\n"Y, filename);
		felf(filename);
	} else if (header[0] == '#' && header[1] == '!') {
		printf("%s: "X"[POSIX shell script]\n"Y, filename);
	} else if (header[0] == 0x42 && header[1] == 0x5A) {
		printf("%s: "X"[BZIP2 compressed format]\n"Y, filename);
	} else if (header[0] == 0x50 && header[1] == 0x4B) {
		printf("%s: "X"[ZIP file format]\n"Y, filename);
	} else if (header[0] == 0x37 && header[1] == 0x7A) {
		printf("%s: "X"[7-Zip compressed format]\n"Y, filename);
	} else if (header[0] == 0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
		printf("%s: "X"[PNG portable network graphics]\n"Y, filename);
	} else if (header[0] == 'B' && header[1] == 'M') {
		printf("%s: "X"[BMP bitmap image file]\n"Y, filename);
	} else if (header[0] == 'G' && header[1] == 'I' && header[2] == 'F') {
		printf("%s: "X"[GIF graphics interchange format]\n"Y, filename);
	} else if (header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46) {
		printf("%s: "X"[PDF document format]\n"Y, filename);
	} else if (header[0] == 0x4D && header[1] == 0x5A) {
		printf("%s: "X"[PE executable format]\n"Y, filename);
	} else if (header[0] == 0x7F && header[1] == 'S' && header[2] == 'N' && header[3] == 'A') {
		printf("%s: "X"[SNAX file format]\n"Y, filename);
	} else if (header[0] == 0x1F && header[1] == 0x8B) {
		printf("%s: "X"[GZIP compressed format]\n"Y, filename);
	} else if (header[0] == 0x52 && header[1] == 0x61 && header[2] == 0x72 && header[3] == 0x21) {
		printf("%s: "X"[RAR archive format]\n"Y, filename);
	} else if (header[0] == 0x52 && header[1] == 0x49) {
		printf("%s: "X"[RIFF file format]\n"Y, filename);
	} else if (header[0] == 0x4D && header[1] == 0x54) {
		printf("%s: "X"[MIDI file format]\n"Y, filename);
	} else if (header[0] == 0x1A && header[1] == 0x45) {
		printf("%s: "X"[matroska file format]\n"Y, filename);
	} else if (header[0] == 0x66 && header[1] == 0x74) {
		printf("%s: "X"[FLAC audio format]\n"Y, filename);
	} else if (header[0] == 0x4F && header[1] == 0x67) {
		printf("%s: "X"[OGG audio format]\n"Y, filename);
	} else {
		printf("%s: "Z"[unknown file type]\n"Y, filename);
	}
}

int ffile(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: toolbox file <filename>\n");
		return 1;
	}
	fmagic(argv[1]);
	return 0;
}

// --- grep --- //
int fgrep(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: toolbox %s <pattern> <file>\n", argv[0]);
		return 1;
	}
	int r = 0;
	for (int i = 2; i < argc; i++) {
		FILE *f = fopen(argv[i], "r");
		if (!f) {
			perror(Z"fopen() failed"Y);
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

// --- rmdir --- //
int frmdir(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: toolbox %s <dir>\n", argv[0]);
		return 1;
	}
	int r = 0;
	for (int i = 1; i < argc; i++) {
		if (rmdir(argv[i]) == -1) {
			perror(Z"rmdir() failed"Y);
			r = 1;
		}
	}
	return r;
}

// --- ascii2hex --- //
int fa2x(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: toolbox %s <text>\n", argv[0]);
		return 1;
	}
	const char *z = argv[1];
	for (int i = 0; i < (int)strlen(z); i++) {
		printf("\\x%02X", (unsigned char)z[i]);
	}
	printf("\n");
	return 0;
}


// --- sync --- //
int ffsync(int argc, char **argv) {
	sync();
	printf(X"filesystem cache committed to disk...\n"Y);
	return 0;
}

//  --- true --- //
int ftrue(int argc, char **argv) {
	return 0;
}

// --- false --- //
int ffalse(int argc, char **argv) {
	return 1;
}

// --- tty --- //
int ftty(int argc, char **argv) {
	char *tty_name = ttyname(STDIN_FILENO);
	if (tty_name)
		printf("%s\n", tty_name);
	else
		printf(Z"not a tty...\n"Y);
	return 0;
}

// --- kmsg --- //
int fkmsg(int argc, char **argv) {
	char buf[65536];
	FILE *fp;
	fp = fopen("/proc/kmsg", "r");
	if (fp == NULL) {
		perror(Z"fopen() failed"Y);
		return 1;
	}
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		printf("%s", buf);
	}
	fclose(fp);
	return 0;
}

// --- shell --- //
int fsh(int argc, char **argv) {
	const int ARGS = 64;
	char *in = NULL;
	size_t cap = 0;
	ssize_t len;
	while (1) {
		printf("shell@%s ", getpwuid(getuid())->pw_name);
		fflush(stdout);
		len = getline(&in, &cap, stdin);
		if (len == -1) {
			printf("\n");
			break;
		}
		if (len > 0 && in[len - 1] == '\n') {
			in[len - 1] = '\0';
		}
		if (in[0] == '\0') {
			continue;
		}
		char *args[ARGS];
		int n = 0;
		char *tok = strtok(in, " \t");
		while (tok != NULL && n < ARGS - 1) {
			args[n++] = tok;
			tok = strtok(NULL, " \t");
		}
		args[n] = NULL;
		if (strcmp(args[0], "exit") == 0) {
			break;
		}
		if (strcmp(args[0], "cd") == 0) {
			char *dir = (n > 1) ? args[1] : getenv("HOME");
			if (dir == NULL) {
				fprintf(stderr, Z"cd: $HOME not set\n"Y);
			} else if (chdir(dir) != 0) {
				perror(Z"cd failed"Y);
			}
			continue;
		}
		pid_t pid = fork();
		if (pid < 0) {
			perror(Z"fork() failed"Y);
			free(in);
			return 1;
		} else if (pid == 0) {
			execvp(args[0], args);
			perror(Z"execvp() failed"Y);
			return 1;
		} else {
			int status;
			do {
				if (waitpid(pid, &status, 0) == -1) {
					perror(Z"waitpid() failed"Y);
					free(in);
					return 1;
				}
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}
	free(in);
	return 0;
}

// --- main --- //
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("usage:\n\t%s <command> <args>\n\n" "commands:\n\tls | cp | mkdir | cat | echo | kill | clear | rm | head | file | grep | rmdir | ascii2hex | sync | true | false | tty | kmsg | shell\n", argv[0]);
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
		{"file", ffile},
		{"grep", fgrep},
		{"rmdir", frmdir},
		{"ascii2hex", fa2x},
		{"sync", ffsync},
		{"true", ftrue},
		{"false", ffalse},
		{"tty", ftty},
		{"kmsg", fkmsg},
		{"shell", fsh}
	};
	size_t n = sizeof(a) / sizeof(a[0]);
	const char *in = argv[1];
	for (size_t i = 0; i < n; i++) {
		if (strcmp(in, a[i].c) == 0) {
			return a[i].f(argc - 1, argv + 1);
		}
	}
	printf("unknown command: "Z"%s\n"Y, in);
	return 1;
}
