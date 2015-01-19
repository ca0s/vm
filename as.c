#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "assembler.h"
#include "opcodes.h"

int main (int argc, char *argv[])
{
	int i, mode, fd, ret;
	char *file = NULL;
	char *map = NULL;
	char **tokens;
	struct stat info;
	
	if (argc < 3)
		uso();
	
	for (i=1; i<argc; i++) {
		if (strcmp (argv[i], "-f")==0 && argc-1 > i)
			file = argv[i+1];
		if (strcmp (argv[i], "-a")==0)
			mode = 1;
		if (strcmp (argv[i], "-d")==0)
			mode = 0;
	}
	
	if (!file)
		uso();
	
	if ((fd = open (file, O_RDONLY)) < 0) {
		perror ("Error: cannot open file.\n");
		exit (-1);
	}
	fstat (fd, &info);
	
	if ((map = mmap (NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		perror ("Error: cannot map file into memory.\n");
		exit (-1);
	}

	if (mode == 1)
		ret = assemble (map, info.st_size);
	else ret = disas (map, info.st_size);
	
	if (ret < 0)
		printf ("Finished with errors.\n");
	else printf ("Finished.\n");
	
	munmap (map, info.st_size);
	close (fd);
	return 0;
}

void uso()
{
	printf ("ASasasasd\n");
	exit (-1);
}
