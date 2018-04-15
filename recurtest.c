/* Test a nameserver to see if it might allow recursive queries. */

#include<string.h>
#include<netinet/in.h>
#include<arpa/nameser.h>
#include<resolv.h>
#include<errno.h>
#include<netdb.h>
#include<unistd.h>
#include<pthread.h>

#include"dnsqry.h"

extern pthread_mutex_t mutex;

int
recurtest(struct hostent *host, char *domain)
{
	u_char		fixedans[PACKETSZ], *answer;
	int		len;

	answer = fixedans;

#ifdef DEBUG
	tdpf("pthread_mutex_lock(&mutex)");
#endif
	pthread_mutex_lock(&mutex);

	res_init();

	memcpy((void *)&_res.nsaddr_list[0].sin_addr,
	       (void *)host->h_addr_list[0], (size_t) host->h_length);

	len = res_query(domain, C_IN, T_A, answer, PACKETSZ);

#ifdef DEBUG
	tdpf("pthread_mutex_unlock(&mutex)");
#endif
	pthread_mutex_unlock(&mutex);

	if (len == -1)
		return 1;

	return 0;
}
