#include<stdio.h>
#include<netdb.h>
#include<pthread.h>

/* Debugging printf() */
#define dputs(A) fprintf(stderr,A "\n");
/* Concurrent debugging printf() */
#define tdpf(A) fprintf(stderr,A " %ld\n",(long)pthread_self());
/* Flush the local output buffer upon exit(). */
#define exit(A) fputc('\b',stderr);fflush(stderr);exit(A)
/* Timeout for all operations. */
#define TOTAL_TIMEOUT 22
/* Timeout for an individual AXFR query. */
#define AXFR_TIMEOUT 4
/* DNS Version Query: CHAOS TXT VERSION.BIND. */
#define NS_VERSION_STRING "VERSION.BIND."
/* Domain to use when testing for recursive queries. */
#define RECURSIVE_QUERY_TEST_DOMAIN "internic.net"

struct servers {
    char *ns;
    char *ver;
    struct servers *next;
};

struct list {
    char *num;
    struct list *next;
};

struct funcargs {
    unsigned long ttl;
    char *dom;
    char *bufp;
    unsigned char *cp;
    unsigned char *eom;
    void *reserved;
    unsigned short class;
    unsigned short dlen;
    unsigned short type;
};

int skiptodata(struct funcargs *);
int axfrtest(struct hostent *, char *);
int recurtest(struct hostent *, char *);
void *vcheck(void *);
int report(struct servers *, const char *);
void spin(void);
int chkver(const char *, const char *);
char*genver(char*);
int vercmp(const char *, const char *);
int skipname(unsigned char *, unsigned char *, unsigned char *);
void vexit(const char *);
void usage(char **);
