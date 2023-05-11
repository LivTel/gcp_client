include ../../Makefile.common
include ../Makefile.common

INCDIR 		= $(GCP_CLIENT_SRC_HOME)/include
TESTDIR 	= test
BINDIR 		= $(GCP_CLIENT_BIN_HOME)/$(TESTDIR)/$(HOSTTYPE)
DOCSDIR 	= $(GCP_CLIENT_DOC_HOME)/$(TESTDIR)

LOGGING_CFLAGS	= -DLOGGING=10

GCS_DEPS       := google_cloud_cpp_storage
GCS_CXXFLAGS   := $(shell pkg-config $(GCS_DEPS) --cflags)
GCS_CXXLDFLAGS := $(shell pkg-config $(GCS_DEPS) --libs-only-L)
GCS_LIBS       := $(shell pkg-config $(GCS_DEPS) --libs-only-l)

CFLAGS 		= -g -I$(INCDIR) $(PCO_CFLAGS) $(LOGGING_CFLAGS) $(LOG_UDP_CFLAGS) $(SHARED_LIB_CFLAGS) 
LDFLAGS		= $(GCS_CXXLDFLAGS) $(GCS_LIBS) -lcfitsio -lstdc++

SRCS 		= test_connection.c test_get_file.c test_put_file.c
OBJS 		= $(SRCS:%.c=$(BINDIR)/%.o)
PROGS 		= $(SRCS:%.c=$(BINDIR)/%)
DOCS 		= $(SRCS:%.c=$(DOCSDIR)/%.html)

top: $(PROGS) docs


$(BINDIR)/%: $(BINDIR)/%.o
	$(CC) -o $@ $< -L$(LT_LIB_HOME) $(TIMELIB) $(SOCKETLIB) -lm -lc -l$(GCP_CLIENT_LIBNAME) $(LDFLAGS)

$(BINDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@  

docs: $(DOCS)

$(DOCS): $(SRCS)
	-$(CDOC) -d $(DOCSDIR) -h $(INCDIR) $(DOCFLAGS) $(SRCS)

depend:
	makedepend $(MAKEDEPENDFLAGS) -- $(CFLAGS) -- $(SRCS)

clean:
	$(RM) $(RM_OPTIONS) $(OBJS) $(PROGS) $(TIDY_OPTIONS)

tidy:
	$(RM) $(RM_OPTIONS) $(TIDY_OPTIONS)
