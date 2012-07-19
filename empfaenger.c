/**
* @file empfaenger.c
* Betriebssysteme Beispiel 3 - shared memory, semaphore
*
* @author Roland Elentner   <ic11b025@technikum-wien.at>
* @author Florian Mrkvicka  <ic11b048@technikum-wien.at>
* @author Mihajlo Milanovic <ic11b081@technikum-wien.at>
* @date 2012/06/07
*
* @version $Revision: 005 $
*
* @todo -
*
* URL: $HeadURL$
*
* Last Modified: $Author: Roland $
*/

/*
* -------------------------------------------------------------- includes --
*/
#include "common.h"
/*
* --------------------------------------------------------------- defines --
*/

/*
* -------------------------------------------------------------- typedefs --
*/

/*
* --------------------------------------------------------------- prototypes --
*/

/*
* --------------------------------------------------------------- globals --
*/ 
const char * executable;   /*speichert den Namen der ausführbaren Datei*/
/*
* ------------------------------------------------------------- functions --
*/

/**
*
* \brief implements the receiving process
*
* This is the main entry point for any C program.
*
* \param argc the number of arguments
* \param argv the arguments itselves (including the program name in argv[0])
*
* \return success or failure
*
*/
int main(int argc, char * const * argv)
{
    int semkeyr  = SEMKEY0;      /* Semaphore Key für Read Index */
    int semkeyw  = SEMKEY1;      /* Semaphore Key für Write Index */
    int semidr   = 0;            /* Semaphore ID für Read Index */
    int semidw   = 0;            /* Semaphore ID für Write Index */
    int shmkey   = SHMKEY0;      /* Shared Memory Key */
    int shmsize  = 0;            /* Shared Memory Größe */
    int shmid    = 0;            /* Shared Memory ID*/
    int *shmaddr = 0;            /* Shared Memory Adresse */

    executable   = argv[0];      /* Name der ausgeführten Datei für Fehlerausgabe */
    shmsize = check_parameter(argc, argv);  /* prüfe Commandline Parameter */
    
    /* erzeuge Read Semaphore; init mit 0 weil solange nichts geschrieben wurde soll nichts gelesen werden */
    if ((semidr = create_sem(semkeyr, 0660, 0)) == -1)        
		exit(EXIT_FAILURE);
    
    /* erzeuge Write Semaphore; init mit shmsize weil soviele Zeichen kann das Shared Memory noch aufnehmen */
    if ((semidw = create_sem(semkeyw, 0660, shmsize)) == -1)
		exit(EXIT_FAILURE);

    /* erzeuge Shared Memory */
    if ((shmid = create_shm(shmkey, shmsize)) == -1)
    {
		clear_sem(semidr,semidw);							/* im Fehlerfall Semaphore löschen */
		exit(EXIT_FAILURE);
	  }
	
	  /* Shared Memory im Lesemodus (1) einbinden */
    if ((shmaddr = attach_shm(shmid, SHM_RDONLY)) == (int *) -1)
	  {
		clear_sem(semidr,semidw);        /* im Fehlerfall Semaphore löschen */
		clear_shm(shmid);                /* im Fehlerfall Shared Memory löschen */
		exit(EXIT_FAILURE);
	  }
    
    /* aus dem Shared Memory lesen */
    if(read_data(semidr, semidw, shmaddr, shmsize) == -1)  
	  {
	  /* im Fehlerfall alles aufräumen */
		clear_sem(semidr, semidw);                    /* Semaphore löschen */
		clear_attshm(shmaddr);                        /* Shared Memory loslassen */
		clear_shm(shmid);                             /* Shared Memory löschen */
		exit(EXIT_FAILURE);
	  }
	  
    /* aus Shared Memory fertig gelesen, daher alles aufräumen */
    clear_sem(semidr, semidw);                    /* Semaphore löschen */
    clear_attshm(shmaddr);                        /* Shared Memory loslassen */
    clear_shm(shmid);                             /* Shared Memory löschen */

    errno = 0;
    if (fflush(stdout) != EOF)     /* leere alle Puffer. Wenn EOF zurückgegeben wird, dann EXIT_SUCCESS */
    {
        exit(EXIT_SUCCESS);
    }
    else                          /* Puffer wurden nicht vollständig geleert */
    {
        fprintf(stderr, "Buffer not written completely!\n");
        error_handling(errno);
        exit(EXIT_FAILURE);
    }
}

/*
* =================================================================== eof ==
*/
