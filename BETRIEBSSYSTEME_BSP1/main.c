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

//allowed_params[]
#define HELP_PARAM 0
#define NAME_PARAM 1
#define TYPE_PARAM 2
#define USER_PARAM 3
#define PRINT_PARAM 4
#define LS_PARAM 5
#define NOUSER_PARAM 6
#define PATH_PARAM 7

/*
 * ------------------------------------------------------------- global --
 */


const char * allowed_params[]={"-h","-name","-type", "-user", "-print", "-ls", "-nouser", "-path"};



/*
 * ------------------------------------------------------------- functions --
 */

/// Vorgegeben sind 2 Grundfunktionen
///do_file und do_dir

int do_file(const char * file_name, const char * const * parms); // NOT YET MADE
int do_dir(const char * dir_name, const char * const * parms);   // NOT YET MADE


///Functions for HELP
int check_params(int argc, const char * argv[]);
int which_location(const char *locationName);


void view_help(void);




/*
 * ------------------------------------------------------------- main --
 */

int main(int argc, const char * argv[]) {
   
    
    check_params(argc,argv);
    
    
    
    return 0;
}



/**
 * check_params()
 * Returns 0 = Everything is OK
 * Returns 1 = No Parameter given
 * Returns 2 = location is not correct
 */
int check_params(int argc, const char * argv[])
{
    
    int i;
    
    printf("\ncheck_params() number of parameters are: %d\n", argc -1);
    //printf("\nDas Programm heisst: %s\n", argv[0]);
    
    //is true when minimum one parameter is given
    if(argc > 1)
    {
        
        //TEST ---  overview about the given parameters
        for (i = 1; i < argc; i++)
        {
            printf("\ncheck_params() Param %d =  %s\n", i, argv[i]);
        }
    
      
        ///Check if first Param is -h -----------------------------------
        /// Display help
        if (strncmp(allowed_params[HELP_PARAM], argv[1] , strlen(argv[1])) == 0)
        {
            view_help();
            return 0;
        }
        
        
        ///Check if it is a File or Dir
        if (which_location(argv[1]) < 2 || which_location(argv[1]) < 0 )
        {
            printf("\ncheck_params() dir/file parameter is not correct\n");
            view_help();
            return 2;
        }
        

    }
    else
    {
        printf("\ncheck_params() no parameters found\n");
        view_help();
        return 1;
    }
    
    return 0;
    
}



/**
 *  which_location()
 *  return 1  -->   "ordinary file"
 *  return 2  -->   "directory"
 *  return 3  -->   "text orientated DEVICE"
 *  return 4  -->   "block oriented DEVICE"
 *  return 5  -->   "pipe"
 *  return 6  -->   "symbolic link";
 *  return 7  -->   "socket"
 *  return -1 -->   "undef"
 *  return -1 -->   "fail to open"
 */
int which_location(const char *locationName)
{
    FILE * fp;
    
    struct stat which_entry;
    
    //opens up file pointer readable
    fp = fopen(locationName, "r");
    
    if (fp == NULL)
    {
        fprintf(stderr,"Error file_o_dir() fopen()");
        return -1;
    }
    
    
    //int stat(const char *path, struct stat *buf);
    stat(locationName, &which_entry);
    
    //check which location it is
    if(S_ISREG(which_entry.st_mode)){        (void) fclose(fp);      return 1;}  //"ordinary file"
    else if(S_ISDIR(which_entry.st_mode)){   (void) fclose(fp);      return 2;}  //"directory"
    else if(S_ISCHR(which_entry.st_mode)){   (void) fclose(fp);      return 3;}  //"text orientated DEVICE"
    else if(S_ISBLK(which_entry.st_mode)){   (void) fclose(fp);      return 4;}  //"block orientated DEVICE"
    else if(S_ISFIFO(which_entry.st_mode)){  (void) fclose(fp);      return 5;}  //"pipe"
    else if(S_ISLNK(which_entry.st_mode)){   (void) fclose(fp);      return 6;}  //"symbolic link";
    else if(S_ISSOCK(which_entry.st_mode)){  (void) fclose(fp);      return 7;}  //"socket"
    else{                                    (void) fclose(fp);      return -1;} //"undef"
    
}



void view_help(void)
{
    printf("\nmyfind <file or directory> [ <aktion> ] ...\n");
    printf("\nOPTIONS:");
    printf("\n-user	<name>/<uid>    finde Directoryeinträge eines Users");
    printf("\n-name	<pattern>       finde Directoryeinträge mit passendem Namen");
    printf("\n-type	[bcdpfls]       finde Directoryeinträge mit passendem Typ");
    printf("\n-print                    gibt den Namen des Directoryeintrags aus\n");
}



/*
 * =================================================================== eof ==
 */
