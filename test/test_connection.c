/* test_connection.c
*/
/**
 * Test the Google Cloud Services client connection routine.
 * @author Chris Mottram
 * @version $Revision$
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "log_udp.h"
#include "gcp_client_general.h"
#include "gcp_client_connection.h"

/**
 * Length of some of the strings used in this program.
 */
#define STRING_LENGTH        (256)
/**
 * Verbosity log level : initialised to LOG_VERBOSITY_VERY_VERBOSE.
 */
static int Log_Level = LOG_VERBOSITY_VERY_VERBOSE;
/**
 * The name of the google cloud storage bucket to connect to.
 */
static char Bucket_Name[STRING_LENGTH];

static int Parse_Arguments(int argc, char *argv[]);
static void Help(void);

/* ------------------------------------------------------------------
**          External functions 
** ------------------------------------------------------------------ */
/**
 * Main program.
 * <ul>
 * <li>We parse the arguments with Parse_Arguments.
 * <li>
 * <li>
 * <li>
 * <li>
 * </ul>
 * @param argc The number of arguments to the program.
 * @param argv An array of argument strings.
 * @see #Parse_Arguments
 * @see #Log_Level
 * @see ../cdocs/gcp_client_connection.html#GCP_Client_Connection_Open
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Filter_Level
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Filter_Function
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Log_Filter_Level_Absolute
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Handler_Function
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Log_Handler_Stdout
 */
int main(int argc, char *argv[])
{
	/* parse arguments */
	fprintf(stdout,"test_connection : Parsing Arguments.\n");
	if(!Parse_Arguments(argc,argv))
		return 1;
	fprintf(stdout,"test_connection : Setting up gcp_client logging.\n");
	GCP_Client_General_Set_Log_Filter_Level(Log_Level);
	GCP_Client_General_Set_Log_Filter_Function(GCP_Client_General_Log_Filter_Level_Absolute);
	GCP_Client_General_Set_Log_Handler_Function(GCP_Client_General_Log_Handler_Stdout);
	/* open connection using the application-default gcloud authentication */
	fprintf(stdout,"test_connection : Opening client connection.\n");
	if(!GCP_Client_Connection_Open())
	{
		GCP_Client_General_Error();
		return 2;
	}
	fprintf(stdout,"test_connection : finished.\n");
	return 0;
}

/* ------------------------------------------------------------------
**          Internal functions 
** ------------------------------------------------------------------ */
/**
 * Routine to parse command line arguments.
 * @param argc The number of arguments sent to the program.
 * @param argv An array of argument strings.
 * @see #STRING_LENGTH
 * @see #Bucket_Name
 * @see #Log_Level
 * @see #Help
 */
static int Parse_Arguments(int argc, char *argv[])
{
	int i,retval;

	for(i=1;i<argc;i++)
	{
		if((strcmp(argv[i],"-b")==0)||(strcmp(argv[i],"-bucket")==0))
		{
			if((i+1)<argc)
			{
				strncpy(Bucket_Name,argv[i+1],STRING_LENGTH);
				Bucket_Name[STRING_LENGTH-1] = '\0';
				i++;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:-bucket requires a bucket name.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-help")==0))
		{
			Help();
			return FALSE;
		}
		else if((strcmp(argv[i],"-l")==0)||(strcmp(argv[i],"-log_level")==0))
		{
			if((i+1)<argc)
			{
				retval = sscanf(argv[i+1],"%d",&Log_Level);
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Failed to parse log level %s.\n",argv[i+1]);
					return FALSE;
				}
				i++;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:-log_level requires a number 0..5.\n");
				return FALSE;
			}
		}
		else
		{
			fprintf(stderr,"Parse_Arguments:argument '%s' not recognized.\n",argv[i]);
			return FALSE;
		}
	}/* end for */
	return TRUE;
}

/**
 * Help routine.
 */
static void Help(void)
{
	fprintf(stdout,"Test Connection:Help.\n");
	fprintf(stdout,"This program calls Google Cloud Services Client conenction routine.\n");
	fprintf(stdout,"test_connection [-b[ucket] <bucket name>][-help][-l[og_level <0..5>].\n");
	fprintf(stdout,"\t-bucket selects which google cloud bucket to interact with.\n");
	fprintf(stdout,"\tThe application default login is used (see 'gcloud auth application-default login').\n");
}
