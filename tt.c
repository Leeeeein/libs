#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void stringAppTail(char* str, int n){
    int len = strlen(str);
    sprintf(str + len, ".socket[%d]", n);
}

int main()
{
	char buf[256];
	scanf("%s", buf);
	stringAppTail(buf, 11);
	printf("%s", buf);
	return 0;
}
