#include "minget.h"

char fullPath[PATH_MAX] = "";

/*main for minget*/
int main(int argc, char *const argv[])
{
	/*allocates space for all our command line parsed arguements*/
   struct minOptions options;
   options.verbose = 0;
   options.partition = -1;
   options.subpartition = -1;
   options.imagefile = malloc(NAME_MAX);
   options.path = malloc(PATH_MAX);
   options.fullPath = malloc(PATH_MAX);
   options.destFile = malloc(PATH_MAX);
   options.hasDestFile = 0;

   /*calls the function to parse the args*/
   parseArgs(argc, argv, &options, IS_MINGET);
   strcpy(fullPath, options.fullPath);

   struct minixConfig config;
   config.image = NULL;

   /* gets the right image partition */
   getMinixConfig(options, &config);
   image = config.image;
   zone_size = config.zone_size;
   numInodes = config.sb.ninodes;

   /* Read the root directory table */
   fseekPartition(image, (2 + config.sb.i_blocks + 
   	config.sb.z_blocks) * config.sb.blocksize, SEEK_SET);

   /* creates the itable */
   iTable = (struct inode*) malloc(numInodes * sizeof(struct inode));
   fread(iTable, sizeof(struct inode), numInodes, image);
   struct inode destFile = traversePath(iTable, 
   	config.sb.ninodes, options.path);

   /* redirects to stdout or destionation path user specified*/
	char *data = copyZones(destFile);
	if (options.hasDestFile == HAS_DESTFILE) {
		FILE *writeToThisFile = fopen(options.destFile, "w");
   	if (writeToThisFile == NULL) {
   		printf("Can't write to this file!\n");
   	}
   	else {
		   if (MIN_ISREG(destFile.mode)) {
		   	fprintf(writeToThisFile, "%s", data);
		   }
			else {
				fprintf(writeToThisFile,
					"%s: Not a regular file\n", 
					fullPath);	
			}
   	}
	}
	else {
		if (MIN_ISREG(destFile.mode)) {
	   	fprintf(stdout, "%s", data);
	   }
		else {
			fprintf(stdout, 
				"%s: Not a regular file\n",
				fullPath);	
		}
	}

   exit(EXIT_SUCCESS);
}