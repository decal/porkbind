#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/nameser.h>
#include<resolv.h>
#include<errno.h>
#include<signal.h>
#include<netdb.h>
#include<unistd.h>
#include<ctype.h>
#include<pthread.h>
#include"dnsqry.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void           *
vcheck(void *ptr)
{
	struct funcargs *da=(struct funcargs*)ptr;
	register char *p;
	char  *op;
	struct servers *llp=da->reserved;
	struct hostent *host;
	int		count     , sendlen;
	union {
		HEADER		hdr;
		unsigned char	buf[PACKETSZ];
	}		response;

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	/*
	 * Exclude gethostbyname() calls from each other since the function
	 * is is not reetrant.
	 */
#ifdef DEBUG
	tdpf("pthread_mutex_lock(&mutex)");
#endif
	pthread_mutex_lock(&mutex);

#ifdef DEBUG
	tdpf("gethostbyname");
#endif
	if (!(host = gethostbyname(llp->ns))) {
#ifdef DEBUG
		tdpf("pthread_mutex_unlock(&mutex)");
#endif
		pthread_mutex_unlock(&mutex);
		printf("Unable to resolve: %s\n", llp->ns);
		puts("Possible erreneous NS record.");
#ifdef DEBUG
		tdpf("pthread_exit");
#endif
		pthread_exit((void *)-1);
	}

	memcpy((void *)&_res.nsaddr_list[0].sin_addr,
	       (void *)host->h_addr_list[0], (size_t)host->h_length);

	spin();

#ifdef DEBUG
	tdpf("res_query");
#endif
	if (
	    (sendlen =
	     res_query(NS_VERSION_STRING, C_CHAOS, T_TXT,
		       (unsigned char *)&response, sizeof(response))) < 0) {
#ifdef DEBUG
		tdpf("pthread_exit");
#endif
		pthread_exit((void *)-1);
	}

#ifdef DEBUG
	tdpf("pthread_mutex_unlock(&mutex)");
#endif
	pthread_mutex_unlock(&mutex);

#ifdef DEBUG
		tdpf("recurtest");
#endif
 	if (!recurtest(host, RECURSIVE_QUERY_TEST_DOMAIN))
		printf("%s seems to have recursive queries enabled\n",llp->ns);

	spin();

#ifdef DEBUG
		tdpf("axfrtest");
#endif
	if (!axfrtest(host, da->dom))
		printf("%s appears willing to zone transfer %s\n",
		       llp->ns, da->dom);

	da->eom = response.buf + sendlen;
	da->cp = response.buf + sizeof(HEADER);
	da->cp += skipname(response.buf, da->cp, da->eom) + QFIXEDSZ;
	da->bufp = (char*)response.buf;
	da->cp += skiptodata(da);
	if (da->cp < da->eom) {
		if (da->type == T_TXT) {
			int flag=0;

			op = p = (char*)da->cp + 1;
#ifdef DEBUG
		tdpf("calloc");
#endif
			if (!(llp->ver = (char *)calloc(MAXDNAME + 1, sizeof(char))))
				vexit("calloc");

			count = da->eom - da->cp + 1;
			/* Paranoid strncpy(). */
			strncpy(llp->ver, op, count);
			
			for (p = llp->ver; *p; p++) {
                          if (!isprint(*p)) {
                            *p = 0;
                            break;
                          }
                        }

			printf("%s is %s", llp->ns, llp->ver);

			if(!strchr(llp->ver,'.'))
				llp->ver=(char*)-1;
			else
			{
				for (p = llp->ver; *p; p++) {
					if (isspace(*p)) {
						llp->ver = (char*)-1;
						break;
					}

					if(isdigit(*p))
						flag=1; 
				}
			}

			if(!flag)
				llp->ver = (char*)-1;

			if (p == llp->ver)
				llp->ver = (char*)-1;

			if (llp->ver==(char*)-1)
				printf(" (Cannot parse returned version string)");

			putchar('\n');
		} 
		else 
		{
			puts("Possible Malformed DNS response");
		}
	}
	spin();
#ifdef DEBUG
	tdpf("pthread_exit");
#endif
	pthread_exit((void *)0);
}
