/**
 * @file main.c
 * myfind
 * Beispiel 1
 *
 * @author Karin Kalman <karin.kalman@technikum-wien.at>
 * @author Michael Muellner <michael.muellner@technikum-wien.at>
 * @author Gerhard Sabeditsch <gerhard.sabeditsch@technikum-wien.at>
 * @date 2016/02/13
 *
 * @version $Revision: 0 $
 *
 *
 * URL: $HeadURL$
 *
 * Last Modified: $Author: Gerhard $
 */

/*
 * -------------------------------------------------------------- includes --
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <fnmatch.h>
#include <unistd.h>

/*
 * --------------------------------------------------------------- defines --
 */
#define DEBUGLEVEL 0


/*
 * ------------------------------------------------------------- functions --
 */

// 13.02.16 (gertsch) : Vorgegeben sind 2 Grundfunktionen
//do_file und do_dir

do_file(const char * file_name, const char * const * parms);
do_dir(const char * dir_name, const char * const * parms);



int main(int argc, const char * argv[]) {
   
    
    
    
    
    
    printf("Hello, World!\n");
    return 0;
}



/*
 * =================================================================== eof ==
 */
