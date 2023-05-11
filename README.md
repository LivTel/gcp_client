# Google Cloud Platform Client side C library

Client-side libraries for integrating the google cloud APIs with C programs.
This wraps the google cloud storage C++ libraries with a C layer that alloows us to
read and write files to/from google cloud storage using a simple C API.

Development has been paused at the moment, so the software is somewhat alpha in nature. The basics do appear to work though.

# Prerequisites

- Install the google cloud command line utilities. This will allow you to setup a default authentication regime for the C programs. See https://cloud.google.com/storage/docs/gsutil_install#deb][1] for details
- Install the google-cloud-cpp C++ API (https://github.com/googleapis/google-cloud-cpp) . You need to install this in such a way that the gcp_client code can access the libraries and it's dependencies, i.e. using a package manager does not work (vcpkg), and you should follow the packaging instructions:  https://github.com/googleapis/google-cloud-cpp/blob/main/doc/packaging.md
- An LT development environment.

## Internal Dependencies

- **log_udp**: https://github.com/LivTel/log_udp/

# Directories

- **c** The C library wrapper, written in C++, that exposes a C interface
- **include** The header files for the C library, the C API.
- **test** Test programs for the library.

# Running the test programs

The current C layer authentication scheme just uses the default-application authentication mechanism. More complex authentication could be provided by wrapping other Client constructors.

Therefore:

```
gcloud auth application-default login
gcloud config set project ltstoragetest
```

Then, to read a file from google cloud storage:

```
/home/dev/bin/gcp_client/test/x86_64-linux/test_get_file -bucket standard_bucket_test_002 -google_filename cjm/h_e_20230408_1_1_1_9.fits -output_filename h_e_20230408_1_1_1_9_google.fits 
```

To write a file to google cloud storage:

```
/home/dev/bin/gcp_client/test/x86_64-linux/test_put_file -input_filename h_s_20230510_85_2_1_9.fits.gz -bucket standard_bucket_test_002 -google_filename cjm/h_s_20230510_85_2_1_9.fits.gz
```

Reading the *test/test_get_file.c* and *test/test_put_file.c* (and the associated Makefile) should give you a start point for figuring out how to use this library in your own C code.
