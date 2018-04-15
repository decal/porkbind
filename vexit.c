#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"dnsqry.h"

void 
vexit(const char *func)
{
	perror(func);
	exit(EXIT_FAILURE);
}
