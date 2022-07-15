# Makefile for ccal

# If a different C++ compiler is to be used, specify it below
CXX=gcc

# If you don't want to use the year cache, comment the following line
#CXXFLAGS += -DUSE_YEARCACHE -I../novas3
CXXFLAGS += -I../novas3

# If the compiler doesn't use namespace, uncomment the following line
#CXXFLAGS += -DNO_NAMESPACE

# Binary installation directory
BINDIR=/usr/local/bin

# Man page installation directory
MANDIR=/usr/local/man

# If install is not available, use cp instead
INSTALL=install -c

COMPONENTS= \
	lunaryear \
	mphases \
	htmlmonth \
	psmonth \
	moonphase \
	misc \
	solarterm \
	tt2ut \
	../novas3/novas \
	../novas3/novascon \
	../novas3/nutation \
	../novas3/solsys3 \
	../novas3/readeph0 \
	yearcache
# If you don't want to use the year cache, comment the previous line
# End of list

# End of configuration parameters

OBJS=$(COMPONENTS:%=%.o)

CFLAGS += -O2
CXXFLAGS += -O2

ccal:	ccal.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lm -lstdc++

install:	ccal
	./mkinstalldirs $(BINDIR)
	$(INSTALL) ccal $(BINDIR)
	$(INSTALL) ccalpdf $(BINDIR)

install-man:
	./mkinstalldirs $(MANDIR)/man1
	$(INSTALL) -m 0644 ccal.1 $(MANDIR)/man1
	$(INSTALL) -m 0644 ccalpdf.1 $(MANDIR)/man1

clean:
	$(RM) *.o core
