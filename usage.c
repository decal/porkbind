#include<stdio.h>
#include<stdlib.h>
#include"dnsqry.h"

void 
usage(char **argv)
{
	printf("-*- PorkBind v1.3 by Derek Callaway (http://innu.org/~super) -*-\n\n");
	printf("%s [-c file] [-s server] [-ri] domain\n", *argv);
	printf("\tfile is configuration file to use\n");
	printf("\t(default is " PORKBIND_CONF ")\n");
	printf("\tserver is the nameserver to forward queries through\n");
	printf("\t(default is /etc/resolv.conf nameservers)\n");
	printf("\t-r recursively scans subdomains (i.e. sub.host.dom then host.dom)\n");
	printf("\t-i tests an individual server\n");
	printf("\tdomain is the (sub)domain name whose nameservers to probe\n\n");

	exit(EXIT_SUCCESS);
}
