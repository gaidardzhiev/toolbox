#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <ctype.h>

uint32_t n() {
	uint32_t v = 0;
	if (!_rdrand32_step(&v)) {
		fprintf(stderr, "error: RDRAND instruction failed...\n");
		exit(EXIT_FAILURE);
	}
	return v;
}

void s(char *b, size_t l) {
	size_t i = 0;
	while (i < l) {
		char c = (char)(n() % 256);
		if (isprint(c) && c != ' ') {
			b[i++] = c;}
	}
	b[l] = '\0';
}

int main() {
	char r[32 + 1];
	s(r, 32);
	printf("%s\n",r);
	return 0;
}
