/* test_put_file.c
*/
/**
 * Test putting a local file into the Google Cloud.
 * @author Chris Mottram
 * @version $Revision$
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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
 * The block size of memory to reallocate/load local file contents from.
 */
#define MEMORY_BLOCK_LENGTH  (1024*1024)

/**
 * Verbosity log level : initialised to LOG_VERBOSITY_VERY_VERBOSE.
 */
static int Log_Level = LOG_VERBOSITY_VERY_VERBOSE;
/**
 * The name of the google cloud storage bucket to connect to.
 */
static char Bucket_Name[STRING_LENGTH];
/**
 * The name of the google cloud storage filename to save the local file to.
 */
static char Google_Filename[STRING_LENGTH];
/**
 * The name of a local filename to put into the google cloud.
 */
static char Local_Filename[STRING_LENGTH];

static int Load_File(char *filename,void **file_contents,size_t *file_contents_length);
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
 * <li>We load the contents of the local file into memory (Load_File).
 * <li>We put the local file contents into the specified google cloud filename in the specified google bucket (GCP_Client_Read_Write_Write).
 * </ul>
 * @param argc The number of arguments to the program.
 * @param argv An array of argument strings.
 * @see #Parse_Arguments
 * @see #Log_Level
 * @see #Bucket_Name
 * @see #Google_Filename
 * @see #Local_Filename
 * @see #Load_File
 * @see ../cdocs/gcp_client_connection.html#GCP_Client_Connection_Open
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Filter_Level
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Filter_Function
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Log_Filter_Level_Absolute
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Set_Log_Handler_Function
 * @see ../cdocs/gcp_client_general.html#GCP_Client_General_Log_Handler_Stdout
 * @see ../cdocs/gcp_client_read_write.html#GCP_Client_Read_Write_Write
 */
int main(int argc, char *argv[])
{
	void *file_contents = NULL;
	size_t file_contents_length;
	
	/* parse arguments */
	fprintf(stdout,"test_put_file : Parsing Arguments.\n");
	if(!Parse_Arguments(argc,argv))
		return 1;
	fprintf(stdout,"test_put_file : Setting up gcp_client logging.\n");
	GCP_Client_General_Set_Log_Filter_Level(Log_Level);
	GCP_Client_General_Set_Log_Filter_Function(GCP_Client_General_Log_Filter_Level_Absolute);
	GCP_Client_General_Set_Log_Handler_Function(GCP_Client_General_Log_Handler_Stdout);
	/* open connection using the application-default gcloud authentication */
	fprintf(stdout,"test_put_file : Opening client connection.\n");
	if(!GCP_Client_Connection_Open())
	{
		GCP_Client_General_Error();
		return 2;
	}
	/* load the local file contents from disk */
	fprintf(stdout,"test_put_file : Loading file from local file '%s'.\n",Local_Filename);
	if(!Load_File(Local_Filename,&file_contents,&file_contents_length))
	{
		fprintf(stdout,"test_put_file : Load file '%s' failed.\n",Local_Filename);
		return 4;
	}
	/* put the contents of the local file into the google cloud file */
	fprintf(stdout,"test_put_file : Saving local file contents to google file '%s' in bucket '%s'.\n",
		Google_Filename,Bucket_Name);
	if(!GCP_Client_Read_Write_Write(Bucket_Name,Google_Filename,file_contents,file_contents_length))
	{
		GCP_Client_General_Error();
		return 3;
	}
	fprintf(stdout,"test_put_file : finished.\n");
	return 0;
}

/* ------------------------------------------------------------------
**          Internal functions 
** ------------------------------------------------------------------ */
/**
 * Load the specified local file contents into memory.
 * @param filename The filename to load the data from.
 * @param file_contents The address of a memory pointer to reallocate and store the contents of the local file into.
 * @param file_contents_length The address of a variable to store the loaded length of the file_contents, in bytes.
 * @return The routine returns TRUE on success, and FALSE on failure. 
 * @see #MEMORY_BLOCK_LENGTH
 */
int Load_File(char *filename,void **file_contents,size_t *file_contents_length)
{
	int fd = 0,retval,open_errno,read_errno,close_errno,done;
	size_t returned_size;


	/* open the file */
	fd = open(filename,O_RDONLY);
	if(fd < 0)
	{
		open_errno = errno;
		fprintf(stderr, "Load_File '%s' failed: Failed to open file (%d).\n",filename,open_errno);
		return FALSE;
	}
	done = FALSE;
	(*file_contents) = NULL;
	(*file_contents_length) = 0;
	while(done == FALSE)
	{
		if((*file_contents) == NULL)
		{
			(*file_contents) = (void*)malloc(MEMORY_BLOCK_LENGTH * sizeof(char));
		}
		else
		{
			(*file_contents) = (void*)realloc((*file_contents),
							  ((*file_contents_length)+MEMORY_BLOCK_LENGTH) * sizeof(char));
		}
		if((*file_contents) == NULL)
		{
			fprintf(stderr, "Load_File '%s' failed: Failed to reallocate memory (%ld).\n",
				filename,(*file_contents_length));
			return FALSE;
		}
		returned_size = read(fd,((*file_contents)+(*file_contents_length)),MEMORY_BLOCK_LENGTH);
		if(returned_size < 0)
		{
			read_errno = errno;
			fprintf(stderr, "Save_File '%s' failed: Failed to read file contents (%d,%ld,%ld,%d).\n",
				filename,fd,returned_size,(*file_contents_length),read_errno);
			return FALSE;
		}
		if(returned_size == 0) /* EOF */
			done = TRUE;
		(*file_contents_length) += returned_size;
	}/* end while */
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
		else if((strcmp(argv[i],"-i")==0)||(strcmp(argv[i],"-input_filename")==0))
		{
			if((i+1)<argc)
			{
				strncpy(Local_Filename,argv[i+1],STRING_LENGTH);
				Local_Filename[STRING_LENGTH-1] = '\0';
				i++;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:-input_filename requires a filename.\n");
				return FALSE;
			}
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
	fprintf(stdout,"Test Put File:Help.\n");
	fprintf(stdout,"This program puts a local file into google cloud storage.\n");
	fprintf(stdout,"test_put_file -b[ucket] <bucket name> -g[oogle_filename] <filename>\n");
	fprintf(stdout,"\t-i[nput_filename] <filename>[-help][-l[og_level <0..5>].\n");
	fprintf(stdout,"\t-bucket selects which google cloud bucket to interact with.\n");
	fprintf(stdout,"\t-google_filename specifies the google cloud filename to save the local file to.\n");
	fprintf(stdout,"\t-input_filename selects a local filename to upload into the cloud.\n");
	fprintf(stdout,"\tThe application default login is used (see 'gcloud auth application-default login').\n");
}
