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
int check_param_options(const char * argv[], int param_index);
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
 * Returns 0 = everything is OK
 * Returns 1 = print out HELP
 * Returns 2 = location is not correct
 * Returns 3 = no parameter given
 * Returns 4 = not allowed parameter found
 * Returns 5 = not allowed option found
 */
int check_params(int argc, const char * argv[])
{
    
    //Value for PARAM COUNT --- allowed_params[]
    int numb_of_allowed_params=0;
    
    //OK?
    typedef enum OK {NO,YES} OK;
    OK params_ok;
    
    
    //help values
    int i=0;
    int ii=0;
    
    
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
    
      
        /// Display help
        /// compare allowed_param HELP with first param
        if (strncmp(allowed_params[HELP_PARAM], argv[1] , strlen(argv[1])) == 0)
        {
            view_help();
            //Returns 1 = print out HELP
            return 1;
        }
        
        
        ///Check if first param is a File or Dir, when not, then not correct
        if (which_location(argv[1]) > 2 || which_location(argv[1]) < 0 )
        {
            view_help();
            //Returns 2 = location is not correct
            return 2;
        }
        
        
        ///Check other params are OK ---------------------------------------------------------
        

        
        //reset to 0 for while()
        i=0;
        
        
        ///Check max Allowed Params---------START
        while(allowed_params[i] != NULL)
        {
            numb_of_allowed_params++;
            i++;
        }
        ///Check max Allowed Params---------END
        
        
        /// CHECK ALL GIVEN PARAMS
        //reset params_ok to NO --> CHECK LOGIK
        params_ok = NO;
        //2 because 0 is programm itself and 1 is already checked before -- argc check
        for (i = 2; i < argc; i++)
        {
            
            //Check if it's a PARAM
            if (strncmp("-", argv[i] , 1) == 0)
            {
                
                    //for() as log as allowed params found
                    //numb_of_allowed_params - 1 -> because array starts with ZERO
                    //ii = 1 because PARAM HELP should not listed behind
                    for (ii=1; ii <= numb_of_allowed_params-1; ii++)
                    {
            
                        //if in one loop a ALLOWED PARAM is found, then check OK
                        if (strncmp(allowed_params[ii], argv[i] , strlen(argv[i])) == 0){
                        
                       
                                /// Check OPTIONS--------------------------------------------------------------
                                //int check_param_options(const char * argv[], int param_index)
                                if (check_param_options(argv, i) != 0)
                                {
                                //* Returns 5 = not allowed option found
                                return 5;
                                }
                            
                        
                        params_ok = YES;
                        //exit for()
                        ii=numb_of_allowed_params-1;
                        }
                    

                    }
                
                    if (params_ok != YES)
                    {
                        //Returns 4 = not allowed parameter found
                        return 4;
                    }
                    else
                    {
                        //params_ok reset to NO -> because in next loop, next PARAMETER had to be checked again
                        params_ok = NO;
                    }
            
            
            }
        
        
        }
        
        
        

    }
    else
    {
        printf("\ncheck_params() no parameters found\n");
        view_help();
        //Returns 3 = no parameter given
        return 3;
    }
    //Returns 0 = everything is OK
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

/**
 *check_param_options()
 *
 * Returns 0 = everything OK
 * Returns 1 = param_option not allowed
 */
int check_param_options(const char * argv[], int param_index)
{
    
    // HELP_PARAM is not relevant
    
    //-------------------------------------------------------------------------------------------NAME_PARAM
    if (strncmp(allowed_params[NAME_PARAM], argv[param_index] , strlen(argv[param_index])) == 0)
    {
        //check if next argv is a param -> if so, then it's not allowed by this OPTION
        if (strncmp("-", argv[param_index + 1] , 1) == 0)
        {
            //Returns 1 = param_option not OK
            return 1;
        }
        
        // Everything is allowed ? <pattern>
        //{
        //}
        

    }

    //-------------------------------------------------------------------------------------------TYPE_PARAM
    if (strncmp(allowed_params[TYPE_PARAM], argv[param_index] , strlen(argv[param_index])) == 0)
    {
        
        int numb_of_allowed_options=0;
        int i=0;
        long int option_length = strlen(argv[param_index + 1]);
        
        char * allowed_options[]={"b","c","d","p","f","l","s"};
        char * help= malloc(option_length +1 * sizeof(char));
        

        //check if next argv is a param -> if so, then it's not allowed by this OPTION
        if (strncmp("-", argv[param_index + 1] , 1) == 0)
        {
            //Returns 1 = param_option not OK
            return 1;
        }
        
        // only "b c d p f l s" is allowed
        
        
        ///////////////////////////////////// Gerhard -- is working on this -- 14.02.16
        /*
        strncpy(help,argv[param_index + 1], option_length +1);
        
        printf("\nTEST KOPIE = %s\n",help);
        
        
        //search in a string for needle
        //char * strstr(char *string, char *needle);
        

        ///Check max Allowed Options---------START
        while(allowed_options[i] != NULL)
        {
            numb_of_allowed_options++;
            i++;
        }
        ///Check max Allowed Options---------END

          //  if(strstr(argv[param_index + 1], allowed_options[i]) == NULL) {
          //  }
        
        
        
         */
        
        
    }

    //-------------------------------------------------------------------------------------------USER_PARAM
    if (strncmp(allowed_params[USER_PARAM], argv[param_index] , strlen(argv[param_index])) == 0)
    {
        //check if next argv is a param -> if so, then it's not allowed by this OPTION
        if (strncmp("-", argv[param_index + 1] , 1) == 0)
        {
            //Returns 1 = param_option not OK
            return 1;
        }
        
        
        
        //everything is allowed
        
    }
    
    //-------------------------------------------------------------------------------------------PRINT_PARAM
    if (strncmp(allowed_params[PRINT_PARAM], argv[param_index] , strlen(argv[param_index])) == 0)
    {
     // After this PARAM, no OPTIONS are required
        
    }
    
    //-------------------------------------------------------------------------------------------LS_PARAM
    if (strncmp(allowed_params[LS_PARAM], argv[param_index] , strlen(argv[param_index])) == 0)
    {
      // After this PARAM, no OPTIONS are required
        
    }
    
    //-------------------------------------------------------------------------------------------NOUSER_PARAM
    if (strncmp(allowed_params[NOUSER_PARAM], argv[param_index] , strlen(argv[param_index])) == 0)
    {
     // After this PARAM, no OPTIONS are required
        
    }
    
    //-------------------------------------------------------------------------------------------PATH_PARAM
    if (strncmp(allowed_params[PATH_PARAM], argv[param_index] , strlen(argv[param_index])) == 0)
    {
     // After this PARAM, no OPTIONS are required
        
    }
    
    
    
    return 0;
}






void view_help(void)
{
    printf("\nmyfind <file or directory> [ <parameters> ] ...\n");
    printf("\nPARAMETERS:");
    printf("\n-user	<name>/<uid>    finde Directoryeinträge eines Users         ");
    printf("\n-name	<pattern>       finde Directoryeinträge mit passendem Namen ");
    printf("\n-type	[bcdpfls]       finde Directoryeinträge mit passendem Typ   ");
    printf("\n-print                gibt den Namen des Directoryeintrags aus    ");
    printf("\n-ls                                                               ");
    printf("\n-nouser                                                           ");
    printf("\n-path                                                             \n");
    
}



/*
 * =================================================================== eof ==
 */
