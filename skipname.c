/*
 * Most systems probably won't need this sys/types.h include directive, but
 * I've come across some SVR4 systems where arpa/nameser.h doesn't include it
 * internally. It shouldn't break anything but will probably generate some
 * ugly compiler warnings.
 */
#include<sys/types.h>
#include<arpa/nameser.h>
#include<netinet/in.h>
#include<resolv.h>

int 
skipname(unsigned char *som, unsigned char *cp, unsigned char *eom)
{
	char		buf       [MAXDNAME + 1];
	return (dn_expand(som, eom, cp, buf, MAXDNAME));
}
