/*
 * This function tests for a nameserver's zone transfer capability.
 * AXFR Query construction code ripped from axfr.c by JimJones
 * The query is performed without any libresolv code.
 *
 * RETURN VALUE: 0 on success, non-zero on error.
 */

#include<sys/time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<netdb.h>
#include<arpa/nameser.h>
#include"dnsqry.h"

int 
axfrtest(struct hostent *he, char *ns)
{
	char		dnscode   [] = {0, 25, 8, 8, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
	char		dnscode2  [] = {0, 0, 252, 0, 01};
	char		buf       [4096] = {0}, *temp, *parse, *point;
	struct sockaddr_in name;
	struct timeval	timeout;
	fd_set		set;
	int		sock;

	spin();

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		vexit("socket");

	name.sin_family = AF_INET, name.sin_port = htons(53);
	name.sin_addr = *(struct in_addr *)he->h_addr;

	if (connect(sock, (struct sockaddr *)&name, sizeof(struct sockaddr))) {
		close(sock);

		return (-1);
	}

	spin();

	memcpy(buf, dnscode, (size_t) sizeof(dnscode));
	point = buf + sizeof(dnscode);

	if (!
	    (temp =
	     (char *)malloc((strlen(ns) * sizeof(char)) +
			    (sizeof(char)*2))))
		vexit("malloc");

	sprintf(temp, ".%s", ns);
	parse = strtok(temp, ".");
	*point = strlen(parse);
	point++;
	strcpy(point, parse);
	point += strlen(parse);

	while ((parse = strtok(NULL, "."))) {
		*point = strlen(parse);
		point++;
		strcpy(point, parse);
		point += strlen(parse);
	}

	free(temp);

	/*
	 * Tack on the trailing query information (AXFR and IN record), Fill
	 * in the DNS packet identification sequence with the process PID
	 */
	memcpy(point, dnscode2, (size_t) sizeof(dnscode2));

	if (strlen(ns) == 1)
		buf[1] = 17;
	else
		buf[1] = strlen(ns) + 18;
	buf[2] = getpid(), buf[3] = getpid(), point += sizeof(dnscode2);

	spin();

	send(sock, buf, (point - buf), 0);
	recv(sock, buf, 4096, 0);
	FD_ZERO(&set);
	FD_SET(sock, &set);

	/* Three seconds should be plenty. */
	timeout.tv_sec = AXFR_TIMEOUT, timeout.tv_usec = 0;
	select(sock + 1, &set, NULL, NULL, &timeout);

	spin();

	if (FD_ISSET(sock, &set))
		return (0);
	/* Only read the header; no zone information. :-\ */
	else
		return (-1);
}
