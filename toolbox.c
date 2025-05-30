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
int ffile(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s path...\n", argv[0]);
		return 1;
	}
	struct stat s;
	for (int i = 1; i < argc; i++) {
		if (stat(argv[i], &s) == -1) {
			perror("stat");
			continue;
		}
		printf("%s: ", argv[i]);
		if (S_ISREG(s.st_mode)) printf("regular file\n");
		else if (S_ISDIR(s.st_mode)) printf("directory\n");
		else printf("other\n");
	}
	return 0;
}

// --- grep ---
int fgrep(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: %s pattern file...\n", argv[0]);
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
			if (strstr(buf, argv[1])) printf("%s", buf);
		}
		fclose(f);
	}
	return r;
}

// --- rmdir ---
int frmdir(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s dir...\n", argv[0]);
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

// --- main() ---
int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <command> <args>\n", argv[0]);
		return 1;
	}
	if (strcmp(argv[1], "ls") == 0) return fls(argc-1, argv+1);
	if (strcmp(argv[1], "cp") == 0) return fcp(argc-1, argv+1);
	if (strcmp(argv[1], "mkdir") == 0) return fmkdir(argc-1, argv+1);
	if (strcmp(argv[1], "cat") == 0) return fcat(argc-1, argv+1);
	if (strcmp(argv[1], "echo") == 0) return fecho(argc-1, argv+1);
	if (strcmp(argv[1], "kill") == 0) return fkill(argc-1, argv+1);
	if (strcmp(argv[1], "clear") == 0) return fclear(argc-1, argv+1);
	if (strcmp(argv[1], "rm") == 0) return frm(argc-1, argv+1);
	if (strcmp(argv[1], "head") == 0) return fhead(argc-1, argv+1);
	if (strcmp(argv[1], "diff") == 0) return fdiff(argc-1, argv+1);
	if (strcmp(argv[1], "file") == 0) return ffile(argc-1, argv+1);
	if (strcmp(argv[1], "grep") == 0) return fgrep(argc-1, argv+1);
	if (strcmp(argv[1], "rmdir") == 0) return frmdir(argc-1, argv+1);
	if (strcmp(argv[1], "ascii2hex") == 0) return fa2x(argc-1, argv+1);
	fprintf(stderr, "unknown command: %s\n", argv[1]);
	return 1;
}
