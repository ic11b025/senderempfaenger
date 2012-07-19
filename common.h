/**
 * @file common.h
 * Betriebsysteme Beispiel 3 - Headerfile.
 *
 * @author Mihajlo Milanovic <ic11b081@technikum-wien.at>
 * @author Florian Mrkvicka  <ic11b048@technikum-wien.at>
 * @author Roland Elentner   <ic11b025@technikum-wien.at>
 * @date 2012/05/19
 *
 * @version $Revision: 002 $
 *
 * @todo nothing
 *
 * URL: $HeadURL$
 *
 * Last modified: $Author: Roland $
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
/*
 * --------------------------------------------------------------- defines --
 */
/* 2 Keys für Semaphore. Zahl aus Matrikelnummer abgeleitet */
#define SEMKEY0 590250
#define SEMKEY1 590251
/* 1 Key für Shared Memory. Zahl aus Matrikelnummer abgeleitet */
#define SHMKEY0 590252
/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * --------------------------------------------------------------- globals --
 */
extern const char * executable;   /*speichert den Namen der ausführbaren Datei*/
/*
 * ------------------------------------------------------------- externals --
 */

/*
 * ------------------------------------------------------------- functions --
 */

extern int check_parameter(const int argc, char * const * para);
extern void print_usage(void);
extern void error_handling(const int errnr);

extern int create_sem(const int key, const int perm, const int initval);
extern int create_shm(const int key, const unsigned long size);
extern int * attach_shm(const int shmid, const int mode);

extern int send_data(const int semidr, const int semidw, int *shmaddr, long size);
extern int read_data(const int semidr, const int semidw, int *shmaddr, long size);

extern void clear_attshm(const void * shmaddr);
extern void clear_shm(const int shmid);
extern void clear_sem(const int keyr, const int keyw);
/*
 * =================================================================== eof ==
 */
