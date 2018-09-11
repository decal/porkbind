CC 	= cc
FLAGS	= -O2 -Wall
OFLAGS 	= -c -pthread # -DDEBUG
# You may have to add -lpthread here
LIBS	= -pthread -lresolv
OBJS    = axfrtest.o main.o chkver.o genver.o vercmp.o skiptodata.o skipname.o \
	  recurtest.o report.o vcheck.o usage.o vexit.o spin.o
EXE     = porkbind
all: $(EXE)
$(EXE): $(OBJS)
	$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(LIBS)
main.o: main.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) main.c
axfrtest.o: axfrtest.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) axfrtest.c
chkver.o: chkver.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) chkver.c
genver.o: genver.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) genver.c
vercmp.o: vercmp.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) vercmp.c
skiptodata.o: skiptodata.c
	$(CC) $(FLAGS) $(OFLAGS) skiptodata.c
skipname.o: skipname.c
	$(CC) $(FLAGS) $(OFLAGS) skipname.c
recurtest.o: recurtest.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) recurtest.c
report.o: report.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) report.c
vcheck.o: vcheck.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) vcheck.c
spin.o: spin.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) spin.c
usage.o: usage.c dnsqry.h
	$(CC) $(FLAGS) $(OFLAGS) usage.c
vexit.o: vexit.c
	$(CC) $(FLAGS) $(OFLAGS) vexit.c
clean: 
	-rm -f core $(OBJS)
clobber: clean 
	-rm -f $(EXE)
