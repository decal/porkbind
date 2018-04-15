/*
 * This inserts a dash into alpha-beta release version numbers so they are 
 * properly compared by report()/chkver(). A '-' is less than an 'a' in ascii
 * but not in BIND's verion numbering scheme; this must be compensated for.
 *
 * This fix is applied to both the version member of the servers structure and
 * the condition member of the configuration entry structure.
 */

#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"dnsqry.h"

char*
genver(char*aver)
{
	register const char *p;
	register char*op;
	char*ret;

	ret=malloc(strlen(aver)*2+1);
	if(!ret)
		vexit("malloc");

	op=ret;

	for(p=aver;*p;p++)
	{
		if(isalpha(*p))
		{
			if(isupper(*p))
				*op++=tolower(*p);
			else
			{
				*op++='-';
				*op++=toupper(*p);
			}
		}
		else
		  *op++=*p; 
	}

	*op=0;

	free(aver);

	return ret;
}
