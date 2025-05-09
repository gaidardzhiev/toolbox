#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define X 2048
#define Z 128

void x(char*c) {
	char*a[Z];
	pid_t p,w;
	int s;
	char*t=strtok(c," \n");
	int i=0;
	while(t!=NULL&&i<Z-1) {
		a[i++]=t;
		t=strtok(NULL," \n");
	}
	a[i]=NULL;
	if((p=fork())==-1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(p==0) {
		if(execvp(a[0],a)==-1) {
			perror("exec");
			exit(EXIT_FAILURE);
		}
	} else {
		do {
			w=waitpid(p, &s,WUNTRACED);
		} while(!WIFEXITED(s)&&!WIFSIGNALED(s));
	}
}

int main() {
	char c[X];
	while(1) {
		printf("%s# ",getpwuid(getuid())->pw_name);
		if(fgets(c,sizeof(c),stdin)==NULL) {
			break;
		}
		if(strcmp(c,"exit\n")==0) {
			break;
		}
		x(c);
	}
	return 0;
}
