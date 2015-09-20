/* 
 * tcp — trivially copyafile
 *
 * The tcp utility copies the contents of the source to target.
 * If target is a directory, tcp will copy source into this directory
 */

#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>

#define BUFFERSIZE 1024

static void usage(void);
static int copyFile(char *, char *);


int
main(int argc, char *argv[])
{
  char *src = argv[1];
  char *dest = argv[2];

  // set_program_name(argv[0]);

  if (argc != 3) usage();

  exit(copyFile(src, dest));
}

static int
copyFile(char *src, char *dest)
{  
  int src_fd, dest_fd, nums;
  char buf[BUFFERSIZE];
  char bufpath[1024];
  char bufpath2[1024];
  char *filename = basename(src);
  struct stat srcbuf, destbuf;

   //get the source stat
  if (lstat(src, &srcbuf) < 0){
    fprintf(stderr, "Unable to open %s : %s\n", src,
				strerror(errno));
		exit(EXIT_FAILURE);
  }

  if (!S_ISREG(srcbuf.st_mode)) {
    fprintf(stderr, "source isn't a file\n");
    exit(EXIT_FAILURE);
  }

  //get the target stat
  if (dest[strlen(dest)-1] == '/') strcat(dest, filename) ;
  else {
    if (lstat(dest, &destbuf) < 0) ;
    else if (S_ISDIR(destbuf.st_mode)) {
      strcat(dest, "/");
      strcat(dest, filename);
    } else if (!S_ISREG(destbuf.st_mode)) {
      fprintf(stderr, "Target isn't file/dirctory\n");
      exit(EXIT_FAILURE);
    }
  }

 // Whether the source and target are same file
  realpath(src, bufpath);
  realpath(dest, bufpath2);
  if(strcmp(bufpath, bufpath2)  == 0 ) {
    fprintf(stderr, "cp: source and target are identical (not copied).\n");  
    exit(EXIT_FAILURE);
  }
    
  // Open the source and target file
  if ((src_fd = open(src, O_RDONLY)) == -1 ) {
    fprintf(stderr, "Unable to open %s : %s\n", src,
				strerror(errno));
		exit(EXIT_FAILURE);
  }
  
  if ((dest_fd = open(dest, O_WRONLY | O_CREAT | O_EXCL, 
          srcbuf.st_mode)) == -1) {
    fprintf(stderr, "Unable to open %s : %s\n", dest,
				strerror(errno));
		exit(EXIT_FAILURE);

  }

  // Copy the files
  while ((nums = read(src_fd, buf, BUFFERSIZE)) > 0) {
    if (write(dest_fd, buf, nums ) != nums) {
      fprintf(stderr, "Target is unable to write : %s\n",
					strerror(errno));
			exit(EXIT_FAILURE); 
    }
  }
  if (nums < 0) {
      fprintf(stderr, "Source is unable to read : %s\n",
					strerror(errno));
			exit(EXIT_FAILURE);    
  }
  
  // Close the files
  if (close(src_fd) == -1 ) {
    fprintf(stderr, "Source close failed: %s\n", strerror(errno));
  }
  if (close(dest_fd) == -1) {
    fprintf(stderr, "target close failed: %s\n", strerror(errno));
  }

  return EXIT_SUCCESS;
}

static void
usage(void)
{
    fprintf(stderr, "usage: tcp source target\n");
    exit(EXIT_FAILURE);
}
