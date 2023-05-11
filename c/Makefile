include ../../Makefile.common
include ../Makefile.common

INCDIR 		= $(GCP_CLIENT_SRC_HOME)/include
BINDIR		= $(GCP_CLIENT_BIN_HOME)/c/$(HOSTTYPE)
DOCSDIR 	= $(GCP_CLIENT_DOC_HOME)/cdocs


GCS_DEPS := google_cloud_cpp_storage

GCS_CXXFLAGS   := $(shell pkg-config $(GCS_DEPS) --cflags)
GCS_CXXLDFLAGS := $(shell pkg-config $(GCS_DEPS) --libs-only-L)
GCS_LIBS       := $(shell pkg-config $(GCS_DEPS) --libs-only-l)


# Do we wish to link with CFITSIO routines?
FITSCFLAGS = -DCFITSIO=1 -I$(CFITSIOINCDIR) 
CFITSIOLIB = -L$(LT_LIB_HOME) -lcfitsio

LOGGING_CFLAGS	= -DLOGGING=10

CFLAGS 		= -g -I$(INCDIR) $(FITSCFLAGS) $(GCS_CXXFLAGS) $(LOGGING_CFLAGS) $(LOG_UDP_CFLAGS) $(SHARED_LIB_CFLAGS)
LDFLAGS		= $(GCS_CXXLDFLAGS) $(GCS_LIBS)

SRCS 		= gcp_client_general.cpp gcp_client_connection.cpp gcp_client_read_write.cpp
HEADERS		= $(SRCS:%.cpp=$(INCDIR)/%.h)
OBJS 		= $(SRCS:%.cpp=$(BINDIR)/%.o)
DOCS 		= $(SRCS:%.cpp=$(DOCSDIR)/%.html)

top: shared docs

shared: $(LT_LIB_HOME)/lib$(GCP_CLIENT_LIBNAME).so

$(LT_LIB_HOME)/lib$(GCP_CLIENT_LIBNAME).so: $(OBJS)
	g++ $(CCSHAREDFLAG) $(LDFLAGS) $(OBJS) -o $@

$(BINDIR)/%.o: %.cpp
	g++ -c $(CFLAGS) $< -o $@  

docs: $(DOCS)

$(DOCS): $(SRCS) $(HEADERS)
	-$(CDOC) -d $(DOCSDIR) -h $(INCDIR) $(DOCFLAGS) $(SRCS)

depend:
	makedepend $(MAKEDEPENDFLAGS) -- $(CFLAGS) -- $(SRCS)

clean:
	$(RM) $(RM_OPTIONS) $(OBJS) $(LT_LIB_HOME)/lib$(GCP_CLIENT_LIBNAME).so $(TIDY_OPTIONS)

tidy:
	$(RM) $(RM_OPTIONS) $(TIDY_OPTIONS)
	-(cd $(INCDIR); $(RM) $(RM_OPTIONS) $(TIDY_OPTIONS);)

