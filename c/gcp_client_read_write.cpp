/* gcp_client_read_write.c
** google cloud platform C wrapper library around google-cloud-cpp c++ library.
** File object read/write routines.
*/
/**
 * Google Cloud Platform object (file) read/werite routines.
 * @author Chris Mottram
 * @version $Revision$
 */
#include "google/cloud/storage/client.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "log_udp.h"
#include "gcp_client_general.h"
#include "gcp_client_read_write.h"
#include "gcp_client_connection_private.h"

/* defines */
/**
 * Amount of bytes to grow buffers by.
 */
#define READ_WRITE_BUFFER_RESIZE_LENGTH (1024*1024)

/* data types */

/* internal variables */
/**
 * Revision Control System identifier.
 */
static char rcsid[] = "$Id$";

/**
 * Variable holding error code of last operation performed.
 */
static int Read_Write_Error_Number = 0;
/**
 * Local variable holding description of the last error that occured.
 * @see gcp_client_general.html#GCP_CLIENT_GENERAL_ERROR_STRING_LENGTH
 */
static char Read_Write_Error_String[GCP_CLIENT_GENERAL_ERROR_STRING_LENGTH] = "";

/* --------------------------------------------------------
** External Functions
** -------------------------------------------------------- */
/**
 * Routine to read the contents of the file filename in the specified google cloud platform bucket.
 * The contents of the file are read into a reallocatable memory area pointer (file_contents_ptr) which
 * should be freed when it has been finished being used.
 * @param bucket_name The name of the bucket.
 * @param filename The filename of the file within the google cloud storage bucket.
 * @param file_contents_ptr The address of a void pointer, on a successful return from this routine a pointer to an
 *         allocated area of memory is returned, with the contents of the file in memory.
 * @param file_contents_length The address of a size_t variable, on a successful return from this routine
 *        the size_t pointed to by this variable conbtains the number of bytes in the loaded file 
 *        (the amount of memory allocated).
 * @return The routine returns TRUE on success, and FALSE on failure. If it fails, Read_Write_Error_Number / 
 *         Read_Write_Error_String should contain details of the failure.
 * @see #READ_WRITE_BUFFER_RESIZE_LENGTH
 * @see #Read_Write_Error_Number
 * @see #Read_Write_Error_String
 */
int GCP_Client_Read_Write_Read(char* bucket_name,char* filename,
				      void **file_contents_ptr,size_t *file_contents_length)
{
	::google::cloud::storage::Client client;
	char *ch_ptr;
	int done;
	
	Read_Write_Error_Number = 0;
#if LOGGING > 1
	GCP_Client_General_Log_Format(LOG_VERBOSITY_TERSE,"GCP_Client_Read_Write_Read(bucket=%s,filename=%s):Started.",
				      bucket_name,filename);
#endif
	if(bucket_name == NULL)
	{
		Read_Write_Error_Number = 1;
		sprintf(Read_Write_Error_String, "GCP_Client_Read_Write_Read: bucket_name was NULL.");
		return FALSE;
	}
	if(filename == NULL)
	{
		Read_Write_Error_Number = 2;
		sprintf(Read_Write_Error_String, "GCP_Client_Read_Write_Read: filename was NULL.");
		return FALSE;
	}
	if(file_contents_ptr == NULL)
	{
		Read_Write_Error_Number = 3;
		sprintf(Read_Write_Error_String, "GCP_Client_Read_Write_Read: file_contents_ptr was NULL.");
		return FALSE;
	}
	if(file_contents_length == NULL)
	{
		Read_Write_Error_Number = 4;
		sprintf(Read_Write_Error_String, "GCP_Client_Read_Write_Read: file_contents_length was NULL.");
		return FALSE;
	}
	/* get client from connection module */
	client = GCP_Client_Connection_Get_Client();
	/* create a reader to start reading the specified object */
#if LOGGING > 1
	GCP_Client_General_Log_Format(LOG_VERBOSITY_VERBOSE,
				      "GCP_Client_Read_Write_Read:ReadObject(bucket=%s,filename=%s).",
				      bucket_name,filename);
#endif
	auto reader = client.ReadObject(bucket_name,filename);
	if(! reader)
	{
		Read_Write_Error_Number = 9;
		sprintf(Read_Write_Error_String,"GCP_Client_Read_Write_Read: Failed to read '%s' from '%s' "
			"with status '%s'.",filename,bucket_name,reader.status().message().c_str());
		return FALSE;
	}
	(*file_contents_ptr) = NULL;
	(*file_contents_length) = 0;
	done = FALSE;
	while(done == FALSE)
	{
		/* allocate more memory for the file contents */
		if((*file_contents_ptr) == NULL)
		{
			(*file_contents_ptr) = (void*)malloc(((*file_contents_length)+READ_WRITE_BUFFER_RESIZE_LENGTH)*
							     sizeof(char));
		}
		else
		{
			(*file_contents_ptr) = (void*)realloc((*file_contents_ptr),
							     ((*file_contents_length)+READ_WRITE_BUFFER_RESIZE_LENGTH)*
							     sizeof(char));
		}
		if((*file_contents_ptr) == NULL)
		{
			Read_Write_Error_Number = 10;
			sprintf(Read_Write_Error_String,"GCP_Client_Read_Write_Read: Failed to read '%s' from '%s' "
				": memory allocation error with size %ld.",filename,bucket_name,(*file_contents_length));
			return FALSE;
		}
		ch_ptr = ((char*)(*file_contents_ptr))+(*file_contents_length);
		/* load the next part of the file into memory */
		reader.read(ch_ptr,READ_WRITE_BUFFER_RESIZE_LENGTH);
		if(! reader)
		{
			if(reader.eof())
			{
				done = TRUE;
				(*file_contents_length) += reader.gcount();
			}
			else
			{
				done = TRUE;
				Read_Write_Error_Number = 12;
				sprintf(Read_Write_Error_String,"GCP_Client_Read_Write_Read: Failed to read '%s' from '%s' "
					": file read failed after %ld of %ld bytes (%s).",filename,bucket_name,
					reader.gcount(),(*file_contents_length),reader.status().message().c_str());
				return FALSE;
			}
		}
		if(reader.eof() == FALSE)
			(*file_contents_length) += READ_WRITE_BUFFER_RESIZE_LENGTH;
	}/* end while not done */
	reader.Close();
	/* we could resize the (*file_contents_ptr) to preceisly match (*file_contents_length) if we wanted to here,
	** this would prevent small files being returned in a 1 Meg buffer */
#if LOGGING > 1
	GCP_Client_General_Log_Format(LOG_VERBOSITY_TERSE,
				      "GCP_Client_Read_Write_Read(bucket=%s,filename=%s):Finished reading %ld bytes.",
				      bucket_name,filename,(*file_contents_length));
#endif
	return TRUE;
}

/**
 * Routine to write the contents of the supplied memory pointer to the specified 
 * filename in the specified google cloud platform bucket.
 * @param bucket_name The name of the bucket.
 * @param filename The filename of the file within the google cloud storage bucket.
 * @param file_contents_ptr A void pointer, to an allocated area of memory of length file_contents_length,
 *        containing the data to write into the specified google cloud platform file.
 * @param file_contents_length A size_t containing the number of bytes in the memory area pointed to by file_contents_ptr.
 * @return The routine returns TRUE on success, and FALSE on failure. If it fails, Read_Write_Error_Number / 
 *         Read_Write_Error_String should contain details of the failure.
 * @see #Read_Write_Error_Number
 * @see #Read_Write_Error_String
 */
int GCP_Client_Read_Write_Write(char* bucket_name,char* filename,
				       void *file_contents_ptr,size_t file_contents_length)
{
	namespace gcs = ::google::cloud::storage;
	gcs::Client client;

	Read_Write_Error_Number = 0;
	if(bucket_name == NULL)
	{
		Read_Write_Error_Number = 5;
		sprintf(Read_Write_Error_String, "GCP_Client_Read_Write_Write: bucket_name was NULL.");
		return FALSE;
	}
	if(filename == NULL)
	{
		Read_Write_Error_Number = 6;
		sprintf(Read_Write_Error_String, "GCP_Client_Read_Write_Write: filename was NULL.");
		return FALSE;
	}
	if(file_contents_ptr == NULL)
	{
		Read_Write_Error_Number = 7;
		sprintf(Read_Write_Error_String, "GCP_Client_Read_Write_Write: file_contents_ptr was NULL.");
		return FALSE;
	}
	if(file_contents_length == 0)
	{
		Read_Write_Error_Number = 8;
		sprintf(Read_Write_Error_String, "GCP_Client_Read_Write_Read: file_contents_length was 0.");
		return FALSE;
	}
#if LOGGING > 1
	GCP_Client_General_Log_Format(LOG_VERBOSITY_INTERMEDIATE,
				      "GCP_Client_Read_Write_Write:Starting writing %ld bytes to bucket '%s' filename '%s'.",
				      file_contents_length,bucket_name,filename);
#endif
	/* get client from connection module */
	client = GCP_Client_Connection_Get_Client();
	/* create a writer to write the specified object to the cloud */
#if LOGGING > 1
	GCP_Client_General_Log_Format(LOG_VERBOSITY_VERBOSE,
				      "GCP_Client_Read_Write_Write:WriteObject(bucket=%s,filename=%s).",
				      bucket_name,filename);
#endif
	auto writer = client.WriteObject(bucket_name,filename);
	if(! writer)
	{
		Read_Write_Error_Number = 11;
		sprintf(Read_Write_Error_String,"GCP_Client_Read_Write_Write: Failed to write '%s' to '%s'.",
			filename,bucket_name);
		return FALSE;
	}
	/* write data */
#if LOGGING > 1
	GCP_Client_General_Log_Format(LOG_VERBOSITY_VERBOSE,
		       "GCP_Client_Read_Write_Write:writing data of length %ld bytes to bucket '%s' filename '%s'.",
				      file_contents_length,bucket_name,filename);
#endif
	writer.write((const char*)file_contents_ptr,file_contents_length);
	/* close stream */
#if LOGGING > 5
	GCP_Client_General_Log(LOG_VERBOSITY_VERY_VERBOSE,"GCP_Client_Read_Write_Write:Closing stream.");
#endif
	writer.Close();
	/* check for success/failure */
#if LOGGING > 5
	GCP_Client_General_Log(LOG_VERBOSITY_VERY_VERBOSE,"GCP_Client_Read_Write_Write:Checking metadata for success.");
#endif
	auto metadata = std::move(writer).metadata();
	if (!metadata)
	{
		Read_Write_Error_Number = 12;
		sprintf(Read_Write_Error_String,"GCP_Client_Read_Write_Write: Failed to write '%s' to '%s' "
			"with status '%s'.",filename,bucket_name,std::move(metadata).status().message().c_str());
		return FALSE;
	}
#if LOGGING > 1
	GCP_Client_General_Log_Format(LOG_VERBOSITY_INTERMEDIATE,
				      "GCP_Client_Read_Write_Write:Finished writing to bucket '%s' filename '%s'.",
				      bucket_name,filename);
#endif
	return TRUE;
}

/**
 * Routine to return the current value of the error number.
 * @return The value of Read_Write_Error_Number.
 * @see #Read_Write_Error_Number
 */
int GCP_Client_Read_Write_Get_Error_Number(void)
{
	return Read_Write_Error_Number;
}

/**
 * The error routine that reports any errors occuring in a standard way.
 * @see #Read_Write_Error_Number
 * @see #Read_Write_Error_String
 * @see gcp_client_general.html#GCP_Client_General_Get_Current_Time_String
 */
void GCP_Client_Read_Write_Error(void)
{
	char time_string[32];

	GCP_Client_General_Get_Current_Time_String(time_string,32);
	/* if the error number is zero an error message has not been set up
	** This is in itself an error as we should not be calling this routine
	** without there being an error to display */
	if(Read_Write_Error_Number == 0)
		sprintf(Read_Write_Error_String,"Logic Error:No Error defined");
	fprintf(stderr,"%s GCP_Client_Read_Write:Error(%d) : %s\n",time_string,
		Read_Write_Error_Number,Read_Write_Error_String);
}

/**
 * The error routine that reports any errors occuring in a standard way. This routine places the
 * generated error string at the end of a passed in string argument.
 * @param error_string A string to put the generated error in. This string should be initialised before
 * being passed to this routine. The routine will try to concatenate it's error string onto the end
 * of any string already in existance.
 * @see #Read_Write_Error_Number
 * @see #Read_Write_Error_String
 * @see gcp_client_general.html#GCP_Client_General_Get_Current_Time_String
 */
void GCP_Client_Read_Write_Error_String(char *error_string)
{
	char time_string[32];

	GCP_Client_General_Get_Current_Time_String(time_string,32);
	/* if the error number is zero an error message has not been set up
	** This is in itself an error as we should not be calling this routine
	** without there being an error to display */
	if(Read_Write_Error_Number == 0)
		sprintf(Read_Write_Error_String,"Logic Error:No Error defined");
	sprintf(error_string+strlen(error_string),"%s GCP_Client_Connection:Error(%d) : %s\n",time_string,
		Read_Write_Error_Number,Read_Write_Error_String);
}
