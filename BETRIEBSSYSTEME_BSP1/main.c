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

/// Vorgegeben sind 2 Grundfunktionen
///do_file und do_dir

int do_file(const char * file_name, const char * const * parms);
int do_dir(const char * dir_name, const char * const * parms);



///Functions for HELP
int check_params(int argc, const char * argv[]);
void view_help(void);




/*
 * ------------------------------------------------------------- main --
 */

int main(int argc, const char * argv[]) {
   
    
    check_params(argc,argv);
    
    
    
    return 0;
}


int check_params(int argc, const char * argv[])
{
    
    int i;
    
    printf("\nDie Anzahl der Parameter sind: %d\n", argc);
    printf("\nDas Programm heisst: %s\n", argv[0]);
    
   
    if(argc > 1)
    {
        for (i = 1; i < argc; i++)
        {
        
            printf("\nParam %d =  %s\n", i, argv[i]);
        
        }
    }
    else
    {
        printf("\nKEINE PARAMETER EINGEGEBEN\n");
        view_help();
    }
    
    return 0;
    
}

void view_help(void)
{

    printf("\n\n VIEW HELP VIEW HELP VIEW HELP VIEW HELP VIEW HELP\n\n");
    
}



/*
 * =================================================================== eof ==
 */
