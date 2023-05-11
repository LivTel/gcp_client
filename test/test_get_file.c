/* test_get_file.c
*/
/**
 * Test the Google Cloud Services client connection routine.
 * @author Chris Mottram
 * @version $Revision$
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "log_udp.h"
#include "gcp_client_general.h"
#include "gcp_client_connection.h"
#include "gcp_client_read_write.h"

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
/**
 * The name of the google cloud storage filename to download.
 */
static char Google_Filename[STRING_LENGTH];
/**
 * The name of a local filename to save the downloaded data into.
 */
static char Local_Filename[STRING_LENGTH];

static int Save_File(char *filename,void *file_contents,size_t file_contents_length);
static int Parse_Arguments(int argc, char *argv[]);
static void Help(void);

/* ------------------------------------------------------------------
**          External functions 
** ------------------------------------------------------------------ */
/**
 * Main program.
 * <ul>
 * <li>We parse the arguments with Parse_Arguments.
 * <li>We setup the GCP_Client library logging.
 * <li>We connect to the google cloud by calling GCP_Client_Connection_Open.
 * <li>We read the specified google file from the specified google bucket into memory (GCP_Client_Read_Write_Read).
 * <li>We save the read data into the specified local filename.
 * </ul>
 * @param argc The number of arguments to the program.
 * @param argv An array of argument strings.
 * @see #Parse_Arguments
 * @see #Log_Level
 * @see #Bucket_Name
 * @see #Google_Filename
 * @see #Local_Filename
 * @see #Save_File
 * @see ../cdocs/gcp_client_connection.html#GCP_Client_Connection_Open
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Filter_Level
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Filter_Function
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Log_Filter_Level_Absolute
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Handler_Function
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Log_Handler_Stdout
 * @see ../cdocs/gcp_client_read_write.html#GCP_Client_Read_Write_Read
 */
int main(int argc, char *argv[])
{
	void *file_contents = NULL;
	size_t file_contents_length;
	
	/* parse arguments */
	fprintf(stdout,"test_get_file : Parsing Arguments.\n");
	if(!Parse_Arguments(argc,argv))
		return 1;
	fprintf(stdout,"test_get_file : Setting up gcp_client logging.\n");
	GCP_Client_General_Set_Log_Filter_Level(Log_Level);
	GCP_Client_General_Set_Log_Filter_Function(GCP_Client_General_Log_Filter_Level_Absolute);
	GCP_Client_General_Set_Log_Handler_Function(GCP_Client_General_Log_Handler_Stdout);
	/* open connection using the application-default gcloud authentication */
	fprintf(stdout,"test_get_file : Opening client connection.\n");
	if(!GCP_Client_Connection_Open())
	{
		GCP_Client_General_Error();
		return 2;
	}
	/* get the google file into memory */
	fprintf(stdout,"test_get_file : Reading google file '%s' from bucket '%s'.\n",Google_Filename,Bucket_Name);
	if(!GCP_Client_Read_Write_Read(Bucket_Name,Google_Filename,&file_contents,&file_contents_length))
	{
		GCP_Client_General_Error();
		return 3;
	}
	/* save the file to disk */
	fprintf(stdout,"test_get_file : Saving file of length %ld to local file '%s'.\n",
		file_contents_length,Local_Filename);
	if(!Save_File(Local_Filename,file_contents,file_contents_length))
	{
		fprintf(stdout,"test_get_file : Save file '%s' of length %ld failed.\n",
			Local_Filename,file_contents_length);
		return 4;
	}
	fprintf(stdout,"test_get_file : finished.\n");
	return 0;
}

/* ------------------------------------------------------------------
**          Internal functions 
** ------------------------------------------------------------------ */
/**
 * Save the specified file contents to the specified filename.
 * @param filename The filename to save the data to.
 * @param file_contents The contents to write to file.
 * @param file_contents_length The length of the file_contents, in bytes.
 * @return The routine returns TRUE on success, and FALSE on failure. 
 */
int Save_File(char *filename,void *file_contents,size_t file_contents_length)
{
	int fd = 0,retval,open_errno,write_errno,close_errno;
	size_t returned_size;
		
	fd = open(filename,O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG);
	if(fd < 0)
	{
		open_errno = errno;
		fprintf(stderr, "Save_File '%s' failed: Failed to open file (%d).\n",filename,open_errno);
		return FALSE;
	}
	returned_size = write(fd,file_contents,file_contents_length);
	if(returned_size < file_contents_length)
	{
		write_errno = errno;
		fprintf(stderr, "Save_File '%s' failed: Failed to write file contents (%d,%ld,%ld,%d).\n",
			filename,fd,returned_size,file_contents_length,write_errno);
		return FALSE;
	}
	retval = close(fd);
	if(retval != 0)
	{
		close_errno = errno;
		fprintf(stderr, "Save_File '%s' failed: Failed to close file (%d).\n",filename,close_errno);
		return FALSE;
	}
	return TRUE;
}

/**
 * Routine to parse command line arguments.
 * @param argc The number of arguments sent to the program.
 * @param argv An array of argument strings.
 * @see #STRING_LENGTH
 * @see #Bucket_Name
 * @see #Google_Filename
 * @see #Local_Filename
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
		else if((strcmp(argv[i],"-g")==0)||(strcmp(argv[i],"-google_filename")==0))
		{
			if((i+1)<argc)
			{
				strncpy(Google_Filename,argv[i+1],STRING_LENGTH);
				Google_Filename[STRING_LENGTH-1] = '\0';
				i++;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:-google_filename requires a filename.\n");
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
		else if((strcmp(argv[i],"-o")==0)||(strcmp(argv[i],"-output_filename")==0))
		{
			if((i+1)<argc)
			{
				strncpy(Local_Filename,argv[i+1],STRING_LENGTH);
				Local_Filename[STRING_LENGTH-1] = '\0';
				i++;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:-output_filename requires a filename.\n");
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
	fprintf(stdout,"This program calls downloads a file from google cloud storage and saves it locally.\n");
	fprintf(stdout,"test_get_file -b[ucket] <bucket name> -g[oogle_filename] <filename>\n");
	fprintf(stdout,"\t-o[utput_filename] <filename>[-help][-l[og_level <0..5>].\n");
	fprintf(stdout,"\t-bucket selects which google cloud bucket to interact with.\n");
	fprintf(stdout,"\t-google_filename selects which google cloud filename to download.\n");
	fprintf(stdout,"\t-output_filename selects a local filename to save the downloaded file into.\n");
	fprintf(stdout,"\tThe application default login is used (see 'gcloud auth application-default login').\n");
}
