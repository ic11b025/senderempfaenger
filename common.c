/**
* @file common.c
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
* Last Modified: $Author: Michi $
*/

/*
* -------------------------------------------------------------- includes --
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sem182.h>
#include <unistd.h>
#include <limits.h>
#include "common.h"

/*
* --------------------------------------------------------------- defines --
*/

/*
* -------------------------------------------------------------- typedefs --
*/

/*
* --------------------------------------------------------------- globals --
*/

/*
* ------------------------------------------------------------- functions --
*/

/**
*
* check the command line parameter
*
* \param const char const * para - die Kommandozeilenparameter
* \param const int argc - Anzahl der Kommandozeilenparameter
*
* \return int Größe des Shared Memory
*/
int check_parameter(const int argc, char * const * argv)
{
	int number = -1;
	char* endptr = NULL;
	int gopt = -1;

	errno = 0;
	
	while ((gopt = getopt(argc, argv, "+m:")) != -1) 
	{
		if (gopt == 'm') 
		{
			number = strtol(optarg, &endptr, 10);
			if (errno != 0)
				print_usage();
			/* prüfe ob strtol() einen sinnvollen Wert zurückgeben hat */
			if (*endptr != '\0' || number <= 0 || number > INT_MAX || optarg == '\0') 
			{
				print_usage();
			}
		}
		/* Fehler. Es wurden unbekannte Parameter übergeben */
		if (gopt == '?') 
			print_usage();
			
	}
	
	/* Fehler. Es wurden zusätzliche Parameter übergeben */
	if (optind < argc)
	{
		print_usage();
	}

	return number;
	
}

/**
*
* print the usage and exit with error
*
* \param void
*
* \return EXIT_FAILURE always
*/
void print_usage(void)
{
    fprintf(stderr, "Usage: %s -m <ringbuffer size>\n", executable);
    exit(EXIT_FAILURE);
}

/**
*
* \brief prints error messages
*
* \param const int errnr - Error number
*
* \return void
*/
void error_handling(const int errnr)
{
    if (errnr == -2)
    {
    fprintf(stderr, "%s: One or more resources were not removed; check ipcs to see which one!", executable);
    }
    else
    {
    fprintf(stderr, "%s: %s\n", executable, strerror(errnr));
    }
}

/**
*
* \brief Semaphore erzeugen und initialisieren
*
* \param const int key Semaphore Key
* \param const int perm Semaphore Permissions
* \param const int initval intitialer Wert der Semaphore
*
* \return int Semaphore ID
*/
int create_sem(const int key, const int perm, const int initval)
{
    int semid = -1;
    errno = 0;
    if ((semid = seminit(key, perm, initval)) == -1)
    {
        /* wenn Semaphore bereits existiert hat, ID holen */
        if (errno == EEXIST)
        {
            if ((semid = semgrab(key)) == -1)
            { 
            error_handling(errno);
            return -1;
            }
        }
        else
        {
			  error_handling(errno);
			  return -1;
		    }
    }
    return semid;
}

/**
*
* \brief Shared Memory erzeugen
*
* \param const int key Semaphore Key
* \param const unsigned long initval intitialer Wert der Semaphore
*
* \return int Shared Memory ID
*/
int create_shm(const int key, const unsigned long size)
{   
    int shmid = -1;
    errno = 0;
    if ((shmid = shmget(key, sizeof(int) * size, 0660|IPC_CREAT)) == -1)
    {
		error_handling(errno);
		return -1;
    }
    return shmid;
}

/**
*
* \brief Shared Memory einhängen
*
* \param const int shmid Shared Memory ID
* \param const int mode Schreibmodus (0) oder Lesemodus (1)
*
* \return int * Shared Memory Adresse
*/

int * attach_shm(const int shmid, const int mode)
{
    int * shmaddr = 0;
    errno = 0;
    if ((shmaddr = shmat(shmid, NULL, mode)) == (int *) -1)
    {
		error_handling(errno);
		return (int *) -1;
    }
  
    return shmaddr;
}

/**
*
* \brief Daten in Shared Memory schreiben
*
* \param const int semidr Semaphore ID für Leseindex
* \param const int semidw Semaphore ID für Schreibindex
* \param int *shmaddr Adresse des Shared Memories
*
* \return 0 wenn OK, -1 im Fehlerfall
*/
int send_data(const int semidr, const int semidw, int *shmaddr, long size)
{
    int buffer = 0;
    long i = 0;
    errno = 0;
    
    do
    {
        /* dekrementiere Schreibindex um 1 bevor 1 Zeichen geschrieben wird */
        if (P(semidw) == -1)
        {
        if (errno == EINTR)
        continue;
        return -1;
        }
        buffer = fgetc(stdin);
        *(shmaddr+i) = buffer;
        /* inkrementiere Leseindex um 1 nachdem 1 Zeichen geschrieben wurde */
        if (V(semidr) == -1)
        {
        return -1;
		    }
		    /* wenn Ende des Ringpuffers erreicht ist, beginne wieder an Stelle 0 */ 
        i++; 
        if (i==size) i=0;
    } while (buffer != EOF);
    return 0;
}

/**
*
* \brief Daten aus Shared Memory lesen
*
* \param const int semidr Semaphore ID für Leseindex
* \param const int semidw Semaphore ID für Schreibindex
* \param int *shmaddr Adresse des Shared Memories
* \param const long size Größe des Shared Memories
*
* \return 0 wenn OK, -1 im Fehlerfall
*/
int read_data(const int semidr, const int semidw, int *shmaddr, const long size)
{
    int buffer = 0;
    long i = 0;
    errno = 0;
    while (buffer != EOF)
    {
       /* für jedes gelesene Zeichen wird Lesezähler um 1 verringert */
       if (P(semidr) == -1)
       {
			 if (errno == EINTR)
			 continue;
       return -1;
       }
       buffer = *(shmaddr+i);
       /* für jedes gelesene Zeichen wird Schreibzähler um 1 erhöht */
       if (V(semidw) == -1)
       {
			    return -1;
		   }
        
       if (buffer != EOF)
			 fputc(buffer, stdout);
       
       /* wenn Ende des Ringpuffers erreicht ist, beginne wieder an Stelle 0 */ 
       i++; 
       if (i==size) i=0;
    }
    return 0;
}

/**
*
* \brief Semaphore freigeben
*
* \param const int keyr - Semaphore ID für Leseindex
* \param const int keyw - Semaphore ID für Schreibindex
*
* \return void
*/
void clear_sem(const int keyr, const int keyw)
{
    if (semrm(keyr) == - 1)
    {
        error_handling(-2);
        exit(EXIT_FAILURE);
	}
    if (semrm(keyw) == -1)
    {
        error_handling(-2);
        exit(EXIT_FAILURE);
    }
}

/**
*
* \brief Shared Memory aushängen
* 
* \param const void * shmaddri - Shared Memory Adresse
*
* \return void
*/
void clear_attshm(const void * shmaddr)
{
    if (shmdt(shmaddr) == -1)
    {
        error_handling(-2);
        exit(EXIT_FAILURE);
    }
}

/**
*
* \brief Shared Memory freigeben
*
* \param const int shmid - Shared Memory ID
*
* \return void
*/
void clear_shm(const int shmid)
{
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        error_handling(-2);
        exit(EXIT_FAILURE);
    }
}

/*
* =================================================================== eof ==
*/
