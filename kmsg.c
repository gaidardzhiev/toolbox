#include <stdio.h>
#include <stdlib.h>

void k() {
	char b[65536];
	FILE *fp;
	fp = fopen("/proc/kmsg", "r");
	if (fp == NULL) {
		perror("failed to open /proc/kmsg");
		exit(EXIT_FAILURE);
	}
	while (fgets(b, sizeof(b), fp) != NULL) {
		printf("%s", b);
	}
	fclose(fp);
}

int main(void) {
	k();
	return 0;
}
