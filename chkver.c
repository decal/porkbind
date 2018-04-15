#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"dnsqry.h"

int 
chkver(const char *fullver, const char *fullcon)
{
	register const char *p, *op;
	char           *ver, retcode = 0;

	for (p = (const char *)fullcon; *p; p++) {
		switch (*p) {
		case '<':
			op = ++p;

			while (isalnum(*p) || *p == '.' || *p == '-')
				p++;

			if (!(ver = (char *)malloc(p - op + 1)))
				vexit("malloc");

			strncpy(ver, op, p - op);
			*(ver + (p - op)) = 0;
		
			ver=genver(ver);

			if (vercmp(fullver, ver) < 0)
				retcode |= 0x1;
			else
				retcode &= ~0x1;

			p--;

			break;
		case '>':
			op = ++p;

			while (isalnum(*p) || *p == '.' || *p == '-')
				p++;

			if (!(ver = (char *)malloc(p - op + 1)))
				vexit("malloc");

			strncpy(ver, op, p - op);
			*(ver + (p - op)) = 0;

			ver=genver(ver);

			if (vercmp(fullver, ver) > 0)
				retcode |= 0x1;
			else
				retcode &= ~0x1;
			p--;
			break;
		case '=':
			op = ++p;

			while (isalnum(*p) || *p == '.' || *p == '-')
				p++;

			if (!(ver = (char *)malloc(p - op + 1)))
				vexit("malloc");

			strncpy(ver, op, p - op);
			*(ver + (p - op)) = 0;

			ver=genver(ver);

			if (!vercmp(fullver, ver))
				retcode |= 0x1;
			else
				retcode &= ~0x1;

			p--;

			break;

		case '|':
			if (retcode & 0x1)
				return (1);
			break;
		case '&':
			if (!(retcode & 0x1))
				return (0);
			break;
		}
	}

	return (retcode & 0x1);
}
