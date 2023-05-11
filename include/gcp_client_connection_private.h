/* gcp_client_connection.h */
#ifndef GCP_CLIENT_CONNECTION_PRIVATE_H
#define GCP_CLIENT_CONNECTION_PRIVATE_H

/* c++ only header providing mangled c++ interfaces between c++ modules in the gcp_client library
** This header cannot be included in C client programs, or the exposed functions called from C code */
extern ::google::cloud::storage::Client GCP_Client_Connection_Get_Client(void);


#endif
