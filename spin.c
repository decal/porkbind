#include<stdio.h>
#include"dnsqry.h"

pthread_mutex_t spintex=PTHREAD_MUTEX_INITIALIZER;

void
spin(void)
{
	static char	spinchars[5] = "|/-\\";
	static char    *p = &spinchars[0];

#ifdef DEBUG
	tdpf("pthread_mutex_lock(&spintex)");
#endif
	pthread_mutex_lock(&spintex);

	if (!*p)
		p = &spinchars[0];

	fprintf(stderr, "\b%c", *p++);

#ifdef DEBUG
	tdpf("pthread_mutex_unlock(&spintex)");
#endif
	pthread_mutex_unlock(&spintex);

	fflush(stderr);

	return;
}
