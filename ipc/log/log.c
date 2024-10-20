#include <stddef.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MX_MSG_SZ 256

char *
get_time ()
{
  time_t current_time = time (NULL);
  char *t = ctime (&current_time);
  t[strlen (t) - 1] = '\0';
  return t;
}

char *
format_log (char *str)
{
  char *t = get_time ();
  char *fmt = (char *) malloc (strlen (t) + strlen (str));
  snprintf (fmt, strlen (t) + strlen (str) + 4, "[%s] %s", t, str);
  return fmt;
}

int
main (int argc, char *argv[])
{
  int p[2];
  int shmid;
  char *shm;
  size_t shm_sz = 1024;
  key_t key = 2346;
  int cstat;

  if (argc < 2)
    {
      fprintf (stderr, "pass log message as arg\n");
      exit (1);
    }

  if (pipe (p) < 0)
    {
      perror ("pipe");
      exit (1);
    }

  if (fork () == 0)
    {
      close (p[0]);
      char *fmt = format_log (argv[1]);
      if (write (p[1], fmt, MX_MSG_SZ) < 0)
	{
	  perror ("write");
	  exit (1);
	}
      exit (0);
    }

  wait (&cstat);

  if ((shmid = shmget (key, shm_sz, IPC_CREAT | 0666)) < 0)
    {
      perror ("shmget");
      exit (1);
    }
  if ((shm = shmat (shmid, NULL, 0)) == (char *) -1)
    {
      perror ("shmat");
      exit (1);
    }

  close (p[1]);
  char *log = (char *) malloc (sizeof (char) * shm_sz);
  if (read (p[0], log, MX_MSG_SZ) < 0)
    {
      perror ("read");
      exit (1);
    }
  strcpy (shm, log);
  exit (0);
}
