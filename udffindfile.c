/*
 * Helper program using libdvdread to find the offset (in 2 KiB UDF blocks)
 * of a file in a DVD UDF/ISO filesystem image.
 *
 * The first command line parameter is the name of file in question,
 * given as an absolute path/name within the DVD filesystem.
 * (Starting with '/'.)
 *
 * If a second parameter is given, it is taken as the expected size of the file.
 * (Can be given as decimal, or any other format understood by strtol --
 * including octal when starting with "0" or hex when starting with "0x".)
 *
 * If the size of the file determined by libdvdread doesn't match the expected size (if given),
 * the program errors out with an exit code of 100.
 *
 * This program assumes the DVD can be accessed through the device specified in DVD_DEVICE below.
 */
#define DVD_DEVICE "/dev/dvd"


#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>

#include <dvdread/dvd_udf.h>

int main(int argc, char *argv[])
{
   dvd_reader_t *dvd;
   char *file_name;
   long expected_size = -1;

   if (argc < 2 || argc > 3) {
      fprintf(stderr, "Usage: %s <UDF pathspec> [<expected size>]\n", argv[0]);
      exit(1);
   }
   file_name = argv[1];
   if (argc == 3) {
      char *end_ptr;

      errno = 0; expected_size = strtol(argv[2], &end_ptr, 0);
      if (errno || *end_ptr != '\0' || expected_size < 0)
         error(1, errno, "Invalid expected size \"%s\"", argv[2]);
   }

   dvd = DVDOpen(DVD_DEVICE);
   if (!dvd)
      error(2, errno, "Failed opening DVD");

   {
      uint32_t pos, size;

      pos = UDFFindFile(dvd, file_name, &size);
      if (!pos)
         error(3, 0, "File \"%s\" not found on DVD", file_name);

      if (expected_size >= 0 && size != expected_size)
         error(100, 0, "Error: File size on DVD (%u) doesn't match expected size (%ld)", size, expected_size);

      printf("%u\n", pos);
   }
   return 0;
}
