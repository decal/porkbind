<pre>
PorkBind (Nameserver security scanner) v1.3        `----,\    )
by Derek Callaway 08 Aug 2008                       `--==\\  /
<http://innu.org/~super>                             `--==\\/
                                                 .-~~~~-.Y|\\_
                                              @_/        /  66\_
                                                |    \   \   _(")
                                                \   /-| ||'--'
                                           jgs   \_\  \_\\

This program retrieves version information for the nameservers of a domain
and produces a report that describes possible vulnerabilities of each. 
Vulnerability information is configurable through a configuration
file; the default is porkbind.conf. Each nameserver is tested for recursive
queries and zone transfers. The code is parallelized with libpthread.

Tested on FreeBSD 7.0 and Linux 2.6.

Much thanks to Albitz and Liu for the excellent coverage of resolver library
routines in their book, _DNS and BIND_.

`axfrtest.c` was originally written by JimJones.

Please send bugs, comments, 0day, etc. to <super@innu.org>.
</pre>
