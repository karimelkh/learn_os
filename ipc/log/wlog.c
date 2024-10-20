#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int
main (int argc, char *argv[])
{
  int shmid;
  key_t key;
  char *shm;
  size_t shm_sz = 1024;
  key = 2346;
  if ((shmid = shmget (key, shm_sz, 0666)) < 0)
    {
      perror ("shmget");
      exit (1);
    }
  if ((shm = shmat (shmid, NULL, 0)) == (char *) -1)
    {
      perror ("shmat");
      exit (1);
    }
  int fd = open ("file.log",
		 O_WRONLY | O_APPEND | O_CREAT,
		 S_IWUSR | S_IRUSR);
  if (fd < 0)
    {
      perror ("open");
      return 1;
    }
  if (write (fd, shm, shm_sz) < 0)
    {
      perror ("write");
      exit (1);
    }
  write (fd, "\n", 1);
  close (fd);
  exit (0);
}
