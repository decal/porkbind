#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<sys/types.h>
#include<unistd.h>
#include"dnsqry.h"

#define BUFSIZE 1024

struct config_entry {
	char           *cond;
	char           *vuln;
	char           *advi;
	struct config_entry *next;
};

inline static void synerr(void);

int 
report(struct servers *llp, const char *config)
{
	FILE           *cf;
	char		buf       [BUFSIZE], *op, flag = 0;
	register char  *p;
	struct config_entry *confent, *start;
	size_t		len;
	int		ret;

	if (!(confent = (struct config_entry *)malloc(sizeof(*confent))))
		vexit("malloc");

	confent->next = NULL;

	if (config)
	{
		if(!(cf=(fopen(config,"r"))))
		{
			fprintf(stderr,"Unable to open: %s!\n",config);

			vexit("fopen");
		}
	}
	else if (!(cf = fopen("porkbind.conf", "r")))
	{
		fprintf(stderr,"Unable to open: porkbind.conf!\n");

		vexit("fopen");
	}

	if (!llp->ver||llp->ver==(char*)-1)
		return (0);

	/* Fix for alpha/beta version numbers. */
	llp->ver=genver(llp->ver);

	/*
	 * Read configuration file entries into memory and organize them into
	 * a data structure.
	 */
	start = confent, confent->next = NULL;
	while (fgets(buf, BUFSIZE, cf)) {
		op = buf, p = op;
		/* Comment -- ignore. */
		while (isspace(*p))
			p++;
		if (*p == '#')
			continue;
		while (!isspace(*p) && *p)
			p++;
		/* Blank line */
		if (!*p)
			continue;
		*p = 0;
		len = p - op;
		if (!(confent->cond = (char *)malloc(len + 1)))
			vexit("malloc");
		/* Paranoid strncpy()'s follow. */
		strncpy(confent->cond, op, len);
		*(confent->cond + len) = 0;
		p++;
		while (*p != '"' && isspace(*p))
			p++;
		op = ++p;
		while (*p && *p != '"')
			p++;
		if (!*p)
			synerr();
		*p = 0;
		len = p - op;
		if (!(confent->vuln = (char *)malloc(len + 1)))
			vexit("malloc");
		strncpy(confent->vuln, op, len);
		*(confent->vuln + len) = 0;
		p++;
		while (isspace(*p))
			p++;
		op = p;
		while (!isspace(*p) && *p)
			p++;
		if (!*p)
			synerr();
		*p = 0;
		len = p - op;
		while (!(confent->advi = (char *)malloc(len + 1)))
			vexit("malloc");
		strncpy(confent->advi, op, len);
		*(confent->advi + len) = 0;
		if (!
		    (confent->next =
		     (struct config_entry *)malloc(sizeof(*confent->next))))
			vexit("malloc");
		confent = confent->next;
		confent->next = NULL;
	}

	if (confent->next) {
		free(confent->next);
		confent->next = NULL;
	}

	for (confent = start; confent->next; confent = confent->next) {
		if ((ret = chkver(llp->ver, confent->cond))) {
			if (!flag)
				printf("\n%s may be vulnerable to...\n", llp->ns);
			printf("\t%s: %s\n",
			       confent->vuln, confent->advi);
			flag = 1;
		}
	}

	return (0);
}

static void 
synerr(void)
{
	puts("Syntax error in porkbind.conf");
	exit(EXIT_FAILURE);
}
