/*
 * Compare a version string while recognizing precedence of greater version
 * numbers that are delimited by periods. This algorithm is far from perfect
 * and will only function correctly when arguments are version strings of the
 * current BIND versioning scheme.
 */

#include<string.h>
#include"dnsqry.h"

int 
vercmp(const char *s1, const char *s2)
{
	register char  *p1, *p2;
	int		ret;

	ret = strcmp(s1, s2);
	p1 = strchr(s1, '.'), p2 = strchr(s2, '.');
	if (!(p1 && p2) || ret)
		return (ret);
	while (p1 && p2) {
		ret = strcmp(p1, p2);
		if (ret)
			return (ret);
		p1 = strchr(p1 + 1, '.'), p2 = strchr(p2 + 1, '.');
	}
	switch (p1 != NULL) {
	case 0:
		if (p2)
			return (-1);
		return (0);
	default:
		return (1);
	}
	/* Sedate compiler complaints. */
	return (0);
}
