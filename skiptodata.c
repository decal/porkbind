/* (see skipname.c comment) */
#include<sys/types.h>
#include<arpa/nameser.h>
#include"dnsqry.h"

int 
skiptodata(struct funcargs *da)
{
	unsigned char  *tmp_cp = da->cp;
	int		in;
	if ((in = skipname((unsigned char*)da->bufp, tmp_cp, da->eom)) < 0)
		return (-1);
	tmp_cp += in;
	GETSHORT(da->type, tmp_cp);	/* type */
	GETSHORT(da->class, tmp_cp);	/* class */
	GETLONG(da->ttl, tmp_cp);	/* ttl */
	GETSHORT(da->dlen, tmp_cp);	/* dlen */
	return (tmp_cp - (da->cp));
}
