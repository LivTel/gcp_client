/* gcp_client_connection.c
** google cloud platform C wrapper library around google-cloud-cpp c++ library
*/
/**
 * Connection routines. Used to create wrapped instances of the Client object.
 * @author Chris Mottram
 * @version $Revision$
 */
#include "google/cloud/storage/client.h"
#include <iostream>
#include "log_udp.h"
#include "gcp_client_general.h"
#include "gcp_client_connection.h"

/* defines */
/* data types */
/**
 * Data type holding local data to gcp_client_general. This consists of the following:
 * <dl>
 * <dt>Client_Connection</dt> <dd>A Client instance to use for connection.</dd>
 * </dl>
 */
struct Connection_Struct
{
	::google::cloud::storage::Client Client_Connection;
};

/* internal variables */
/**
 * Revision Control System identifier.
 */
static char rcsid[] = "$Id$";
/**
 * The instance of Connection_Struct that contains local data for this module.
 * @see #General_Struct
 */
static struct Connection_Struct Connection_Data;

/**
 * Variable holding error code of last operation performed.
 */
static int Connection_Error_Number = 0;
/**
 * Local variable holding description of the last error that occured.
 * @see gcp_client_general.html#GCP_CLIENT_GENERAL_ERROR_STRING_LENGTH
 */
static char Connection_Error_String[GCP_CLIENT_GENERAL_ERROR_STRING_LENGTH] = "";
/* --------------------------------------------------------
** External Functions
** -------------------------------------------------------- */
/**
 * Create a google cloud services Client connection instance.
 * We authenticate using the google cloud application-default credentials (gcloud auth application-default login).
 * @see #Connection_Data
 */
int GCP_Client_Connection_Open(void)
{
	Connection_Error_Number = 0;
	namespace gcs = ::google::cloud::storage;
#if LOGGING > 0
	GCP_Client_General_Log_Format(LOG_VERBOSITY_TERSE,"GCP_Client_Connection_Open:Started.");
#endif
	Connection_Data.Client_Connection = gcs::Client();
#if LOGGING > 0
	GCP_Client_General_Log_Format(LOG_VERBOSITY_TERSE,"GCP_Client_Connection_Open:Finished.");
#endif
	return TRUE;
}

/**
 * Routine to return the current value of the error number.
 * @return The value of Connection_Error_Number.
 * @see #Connection_Error_Number
 */
int GCP_Client_Connection_Get_Error_Number(void)
{
	return Connection_Error_Number;
}

/**
 * The error routine that reports any errors occuring in a standard way.
 * @see #Connection_Error_Number
 * @see #Connection_Error_String
 * @see gcp_client_general.html#GCP_Client_General_Get_Current_Time_String
 */
void GCP_Client_Connection_Error(void)
{
	char time_string[32];

	GCP_Client_General_Get_Current_Time_String(time_string,32);
	/* if the error number is zero an error message has not been set up
	** This is in itself an error as we should not be calling this routine
	** without there being an error to display */
	if(Connection_Error_Number == 0)
		sprintf(Connection_Error_String,"Logic Error:No Error defined");
	fprintf(stderr,"%s GCP_Client_Connection:Error(%d) : %s\n",time_string,
		Connection_Error_Number,Connection_Error_String);
}

/**
 * The error routine that reports any errors occuring in a standard way. This routine places the
 * generated error string at the end of a passed in string argument.
 * @param error_string A string to put the generated error in. This string should be initialised before
 * being passed to this routine. The routine will try to concatenate it's error string onto the end
 * of any string already in existance.
 * @see #Connection_Error_Number
 * @see #Connection_Error_String
 * @see gcp_client_general.html#GCP_Client_General_Get_Current_Time_String
 */
void GCP_Client_Connection_Error_String(char *error_string)
{
	char time_string[32];

	GCP_Client_General_Get_Current_Time_String(time_string,32);
	/* if the error number is zero an error message has not been set up
	** This is in itself an error as we should not be calling this routine
	** without there being an error to display */
	if(Connection_Error_Number == 0)
		sprintf(Connection_Error_String,"Logic Error:No Error defined");
	sprintf(error_string+strlen(error_string),"%s GCP_Client_Connection:Error(%d) : %s\n",time_string,
		Connection_Error_Number,Connection_Error_String);
}

/* --------------------------------------------------------
** External inter-module c++ Functions
** -------------------------------------------------------- */
/**
 * External inter-module function to provide other modules with the client reference pointer.
 * @return The Client instance created in GCP_Client_Connection_Open.
 * @see #Connection_Data
 * @see #GCP_Client_Connection_Open
 */
::google::cloud::storage::Client GCP_Client_Connection_Get_Client(void)
{
	return Connection_Data.Client_Connection;
}

