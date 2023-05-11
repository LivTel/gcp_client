/* gcp_client_read_write.h */
#ifndef GCP_CLIENT_READ_WRITE_H
#define GCP_CLIENT_READ_WRITE_H

/*  the following 3 lines are needed to support C++ compilers */
#ifdef __cplusplus
extern "C" {
#endif

extern int GCP_Client_Read_Write_Read(char* bucket_name,char* filename,
				      void **file_contents_ptr,size_t *file_contents_length);
extern int GCP_Client_Read_Write_Write(char* bucket_name,char* filename,
				       void *file_contents_ptr,size_t file_contents_length);
	
extern int GCP_Client_Read_Write_Get_Error_Number(void);
extern void GCP_Client_Read_Write_Error(void);
extern void GCP_Client_Read_Write_Error_String(char *error_string);

	
#ifdef __cplusplus
}
#endif

#endif
