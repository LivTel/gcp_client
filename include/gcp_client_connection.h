/* gcp_client_connection.h */
#ifndef GCP_CLIENT_CONNECTION_H
#define GCP_CLIENT_CONNECTION_H
/*  the following 3 lines are needed to support C++ compilers */
#ifdef __cplusplus
extern "C" {
#endif

/* external functions */
extern int GCP_Client_Connection_Open(void);
	
extern int GCP_Client_Connection_Get_Error_Number(void);
extern void GCP_Client_Connection_Error(void);
extern void GCP_Client_Connection_Error_String(char *error_string);

#ifdef __cplusplus
}
#endif

#endif
