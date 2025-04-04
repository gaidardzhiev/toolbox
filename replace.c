#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define BUFFER 65536
#define H"\x72\x65\x67\x63\x6F\x6D\x70\x28\x29\x20\x66\x61\x69\x6C\x65\x64"
#define E "\x75\x73\x61\x67\x65\x3A\x20\x25\x73\x20\x3C\x66\x69\x6C\x65\x6E\x61\x6D\x65\x3E\x20\x3C\x70\x61\x74\x74\x65\x72\x6E\x3E\x20\x3C\x72\x65\x70\x6C\x61\x63\x65\x6D\x65\x6E\x74\x3E"
#define X "\x63\x6F\x75\x6C\x64\x20\x6E\x6F\x74\x20\x6F\x70\x65\x6E\x20\x66\x69\x6C\x65\x2E\x2E\x2E"

void r(FILE*file,const char*pattern, const char*replacement) {
	char line[BUFFER];
	regex_t regex;
	regmatch_t match;
	if(regcomp(&regex,pattern,REG_EXTENDED)!=0) {
		fprintf(stderr,H"\n");
		return;
	}
	while(fgets(line,sizeof(line),file)) {
		char*pos=line;
		char buffer[BUFFER];
		buffer[0]='\0';
		while(regexec(&regex,pos,1, &match,0)==0) {
			strncat(buffer,pos,match.rm_so);
			strcat(buffer,replacement);
			pos+=match.rm_eo;
		}
		strcat(buffer,pos);
		printf("%s",buffer);
	}
	regfree(&regex);
}

int main(int z,char*x[]) {
	if(z!=4) {
		fprintf(stderr,E"\n",x[0]);
		return EXIT_FAILURE;
	}
	const char*filename=x[1];
	const char*pattern=x[2];
	const char*replacement=x[3];
	FILE*file=fopen(filename,"r");
	if(!file) {
		perror(X);
		return EXIT_FAILURE;
	}
	r(file,pattern,replacement);
	fclose(file);
	return EXIT_SUCCESS;
}
