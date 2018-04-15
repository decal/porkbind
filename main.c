/* porkbind by Derek Callaway <super@innu.org> */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/nameser.h>
#include<resolv.h>
#include<errno.h>
#include<netdb.h>
#include<unistd.h>
#include<signal.h>
#include<pthread.h>

#include"dnsqry.h"

static pthread_t **threads;
static void	handler(int);

int 
main(int argc, char **argv)
{
	struct funcargs	da = {0}, **fargs;
	struct servers *start, *llp = 0;
	struct hostent *host;
	/*
	 * Using char as variable type to perform bitwise operations upon
	 * because sizeof(char)==1 always according to standard.
	 */
	register char  *p;
	char           *server = NULL, *config = NULL, *domain, *op, recursion =
	0,		indiv = 0;
	int		count, membs;
	unsigned int	c;
	union {
		HEADER		hdr;
		unsigned char   buf[PACKETSZ];
	}		response;
	char		buf       [8096];

	if (argc == 1)
		usage(argv);

	signal(SIGINT, handler);
	signal(SIGSEGV, handler);

	setvbuf(stdout, buf, _IOFBF, (size_t) 8096);

	spin();

	while ((c = getopt(argc, argv, "s:c:ri")) != EOF) {
		switch (c) {
		case 0:
			puts(optarg);
			break;
		case '?':
			usage(argv);
		case 's':
			server = optarg;
			break;
		case 'c':
			config = optarg;
			break;
		case 'r':
			recursion = 1;
			break;
		case 'i':
			indiv = 1;
			break;
		}
	}
	res_init();
	_res.options &= ~(RES_DNSRCH | RES_DEFNAMES);
	_res.options |= RES_RECURSE;
	_res.retry = 2;
	if (server) {
		spin();
#ifdef DEBUG
		dputs("gethostbyname");
#endif
		if (!(host = gethostbyname(server))) {
			printf("Unable to resolve: %s\n", server);
			exit(EXIT_FAILURE);
		}
		spin();
		_res.nscount = 1;
		memcpy((void *)&_res.nsaddr_list[0].sin_addr,
		     (void *)host->h_addr_list[0], (size_t) host->h_length);
	}
	if (argv[optind])
		domain = argv[optind];
	else {
		if (!(domain = (char *)malloc(MAXDNAME + 1)))
			vexit("malloc");
		if (getdomainname(domain, MAXDNAME) < 0)
			vexit("getdomainname");
		if (!strlen(domain))
			usage(argv);
	}

	puts("\n-*- PorkBind v1.3 (http://innu.org/~super) -*-\n");

	op = da.dom = domain;

	if (!(start = llp = (struct servers *)calloc(1, sizeof(*start))))
		vexit("calloc");

	llp->next = NULL, llp->ns = llp->ver = NULL, membs = 1;

	do {
		if (indiv)
			break;
		spin();
#ifdef DEBUG
		dputs("res_query");
#endif
		if (
		    (c =
		     res_query(op, C_IN, T_NS, (unsigned char *)&response,
			       sizeof(response))) < 0) {
			printf("Unable to retrieve nameserver records for: %s\n",
			       domain);
			exit(EXIT_FAILURE);
		}
		spin();
		da.eom = response.buf + c;
		da.cp = response.buf + sizeof(HEADER);
		da.cp += skipname(response.buf, da.cp, da.eom) + QFIXEDSZ;
		count = ntohs(response.hdr.ancount) + ntohs(response.hdr.nscount);
		spin();
		while ((--count >= 0)) {
			if (da.cp < da.eom) {
				da.bufp = (char*)response.buf;
				da.cp += skiptodata(&da);
				if (da.type == T_NS) {
					if (!(llp->ns = (char *)malloc(MAXDNAME + 1)))
						vexit("malloc");
#ifdef DEBUG
		dputs("dn_expand");
#endif
					if (
					    (count =
					     dn_expand(response.buf, da.eom, da.cp, llp->ns,
						       MAXDNAME)) < 0)
						continue;
					if (!
					    (llp->next =
					     (struct servers *)calloc(1, sizeof(*(llp->next)))))
						vexit("calloc");
					llp = llp->next;
					membs++;
				}
				da.cp += da.dlen;
				if (!
				    (llp->next =
				     (struct servers *)calloc(1, sizeof(*(llp->next)))))
					vexit("calloc");
			}
		}
		spin();
		if (recursion) {
			p = op;
			while (*p && *p != '.')
				p++;
			if (*p)
				p++;
			op = p;
			if (*p) {
				while (*p && *p != '.')
					p++;
				if (!*p)
					recursion = 0;
			} else
				recursion = 0;
		}
	} while (recursion);

	spin();

	/* -i flag was used; only allocating for the one individual nameserver 
	 * that was provided on the command line.. */
	if (indiv) {
		if (!(start = llp = (struct servers *)calloc(1, sizeof(*start))))
			vexit("calloc");

		if (!(llp->ns = (char *)malloc((c = strlen(domain)) + 1)))
			vexit("malloc");

		strncpy(llp->ns, domain, c);
		*(llp->ns + c) = 0;

		if (!(llp->next = (struct servers *)malloc(sizeof(*(llp->next)))))
			vexit("malloc");

		llp = llp->next;
		membs++;
	}

	spin();

	llp->next = 0, llp->ns = llp->ver = 0;

	if (!(threads = (pthread_t **) malloc((membs + 1) * sizeof(*threads))))
		vexit("malloc");

	if (!(fargs = (struct funcargs **)malloc((membs + 1) * sizeof(*fargs))))
		vexit("malloc");

	for (llp = start, c = 0; llp->ns; llp = llp->next, c++) {
		if (!(threads[c] = (pthread_t *) malloc(sizeof(*threads[c]))))
			vexit("malloc");
		if (!(fargs[c] = (struct funcargs *)malloc(sizeof(*fargs[c]))))
			vexit("malloc");
		da.reserved = (void *)llp;
		memcpy((void *)fargs[c], (void *)&da,
		       (size_t) sizeof(struct funcargs));
		spin();
	}

	for (llp = start, c = 0; llp->ns; llp = llp->next, c++) {
		if(!c)
			spin();
#ifdef DEBUG
		dputs("pthread_create");
#endif
		pthread_create(threads[c], NULL, vcheck, fargs[c]);
	}

	threads[c] = 0;
	signal(SIGALRM, handler);
	alarm(TOTAL_TIMEOUT);

	for (c = 0; threads[c]; c++) {
#ifdef DEBUG
		dputs("pthread_join");
#endif
		spin();
		pthread_join(*threads[c], NULL);
	}

	spin();

	if (alarm(0))
		for (llp = start; llp->ns; llp = llp->next)
			if (!llp->ver)
				printf("%s timed out.\n", llp->ns);

	if(!start||!start->ns)
		printf("Unable to retrieve nameserver records for: %s\n",domain);
	else
		for (llp = start; llp->ns; llp = llp->next)
			if(!llp->ver)
				printf("%s timed out.\n",llp->ns);
			else
				report(llp, config);

	exit(EXIT_SUCCESS);
}

static void 
handler(int signum)
{
	switch (signum) {
	case SIGALRM:
		/*
		 * Other threads will pay attention to the signal if it's not
		 * ignored.
		 */
		signal(SIGALRM, SIG_IGN);
		for (signum = 0; threads[signum]; signum++)
			pthread_cancel(*threads[signum]);

		return;
	case SIGSEGV:
		signal(SIGSEGV, SIG_IGN);
		puts("ACK! Caught a SIGSEGV. :-(");
		puts("Please mail bugs to super@innu.org!");

		exit(EXIT_FAILURE);
	case SIGINT:
		signal(SIGINT, SIG_IGN);
		puts("Cancelling unexpended threads...");

		for (signum = 0; threads[signum]; signum++)
			pthread_cancel(*threads[signum]);

		puts("Quitting!");

		exit(EXIT_SUCCESS);
	}
}
