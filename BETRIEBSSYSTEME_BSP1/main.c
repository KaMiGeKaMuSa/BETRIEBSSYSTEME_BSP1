/**
 * @file main.c
 * myfind
 * Beispiel 1
 *
 * @author Karin Kalman <karin.kalman@technikum-wien.at>
 * @author Michael Mueller <michael.mueller@technikum-wien.at>
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
#define NAME_PARAM 0
#define TYPE_PARAM 1
#define USER_PARAM 2
#define PRINT_PARAM 3
#define LS_PARAM 4
#define NOUSER_PARAM 5
#define PATH_PARAM 6

/*
 * ------------------------------------------------------------- global --
 */

typedef struct param_stack {
    
    const char * s_parameter;
    const char * s_option;
    struct param_stack * s_next_param;
    
}param_stack;


param_stack * param_list;


const char * allowed_params[]={"-name","-type", "-user", "-print", "-ls", "-nouser", "-path"};







//var for Error Handling = errno
extern int errno ;

/*
 * ------------------------------------------------------------- functions --
 */

/// Vorgegeben sind 2 Grundfunktionen
///do_file und do_dir


void do_file(const char * file_name, const char * parms, int parms_length);
void do_dir(const char * dir_name, const char * parms, int parms_length);
void check_print(const char * file_name, const char * parms, int parms_length);


///Functions for HELP
int check_params(int argc, const char * argv[]);
int check_param_options(const char * argv[], int aktiv_param_index);
int which_location(const char *locationName);
void view_help(void);


param_stack * search_empty (param_stack * param_list);
void push(const char * param,const char * option);
void pop();


/*
 * ------------------------------------------------------------- main --
 */


int main(int argc, const char * argv[]) {
    
    
    //check_params(argc, argv);
    
    //Check if params are given
    
    if (argc > 1){
		// if directory then go to do_dir
        if (which_location(argv[1]) == 2) {
           do_dir(argv[1], argv[2], argc);
        }
        // if file then go to do_file
        else {
			do_file(argv[1], argv[2], argc);
        }
    
    }
	else{
		
        //Errorhandling corrected -- no errno is used because there is no errno code for this
        fprintf(stderr, "myfind(): Error: parameter format false\n");

	}
    
    
    return 0;
}

// MM:
void do_file(const char * file_name, const char * parms, int parms_length){
    // File auf alle Parameter überprüfen und ausgeben wenn nötig =>
	// Schleife for(int i = 0; i < parms.length; i++)
	// if (parms[i] == aktion1){ } else if (parms[i] == aktion2) {}.....
	// Wenn aktion einen zweiten Parameter braucht z.B. -user	<name> dann muss i=i+1 am schluß damit <name> nicht als eigene aktion ausgewertet wird
	// Wenn eine aktion/filter nicht mit dem file übereinstimmt dann setze Flag isValid auf 0 (false) und Schleife über die restlichen Parameter kann abgebrochen werden
	// Nachdem Schleife alle Parameter für das File überprüft hat check Flag isValid und printf wenn true
		check_print(file_name, parms,parms_length);

}

//MM:
void do_dir(const char * dir_name, const char * parms, int parms_length){
    // Schleife über alle Elemente des dir_name
	// . und .. überspringen da sonst Endlosschleife
	// check ob nächstes Element dir oder was anderes ist (isDirectory)
	// wenn was anderes rufe do_file auf || wenn dir rufe do_dir mit dem derzeitigen Element + counter um die Tiefe des Directorys zu wissen	
	DIR * dir_object;
	struct dirent * dir_element;
	
    //var to save errno state
    int errnum;
    
    
	if ((dir_object = opendir(dir_name)) == NULL) {
        
        //Correct Error Handling with Errno, state saved in errnum
        errnum = errno;
        fprintf(stderr, "Error opening: %s  Name: %s\n", strerror( errnum ), dir_name);
        return;
    }
	
	while ((dir_element = readdir(dir_object)) && dir_element) {
        
        if (strcmp(dir_element->d_name, ".") == 0){

            check_print(dir_element->d_name, parms, parms_length);
            
            continue; // dont process . Directory -> endless loop
        }
        
        
        if (strcmp(dir_element->d_name, "..") == 0) continue; // dont process .. directory
		
		//printf("location: %s, type: %d\n", dir_element->d_name, which_location(dir_element->d_name));
		
        
        //Whenn file is found, then do file
        if (which_location(dir_element->d_name) == 1){
			do_file(dir_element->d_name, parms,parms_length);
		}
        //GS: also other STUFF should be listed
        else {
            check_print(dir_element->d_name, parms, parms_length);
        }
        
	}
	
	 closedir(dir_object);
}

// MM:
void check_print(const char * file_name, const char * parms, int parms_length){
	
    //GS: Question to MM :: WHY parms_length-2 and why start for() with i=ZERO  == ZERO == Name of Programm
    for (int i = 0; i < parms_length-2; i++){
		
		//printf("Filename: %s, parms: %s\n", file_name, parms); // For TESTING
		
		if (strcmp(&parms[i], allowed_params[PRINT_PARAM]) == 0){
				printf("%s\n", file_name);
		}
        else{
            //Errorhandling corrected -- no errno is used because there is no errno code for this
            fprintf(stderr, "myfind() - check_print(): Error: unknown parameter\n");
		}
	}
}



/**
 * check_params()
 * Returns 0 = everything is OK

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
    
    
    printf("\ncheck_params() number of parameters exclusive programm_itself are: %d\n", argc -1); //FOR TEST
    
    
    //is true when minimum one parameter is given
    if(argc > 1)
    {
        
        //TEST ---  overview about the given parameters
        for (i = 1; i < argc; i++)
        {
            printf("\ncheck_params() Param %d =  %s\n", i, argv[i]);
        }
    
        
        ///Check if first param is a File or Dir, when not, then not correct
        if (which_location(argv[1]) == -1)
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
                    for (ii=0; ii <= numb_of_allowed_params; ii++)
                    {
            
                        //if in one loop a ALLOWED PARAM is found, then check OK
                        if (strncmp(allowed_params[ii], argv[i] , strlen(argv[i])) == 0){
                        
                       
                                /// Check OPTIONS--------------------------------------------------------------
                                //int check_param_options(const char * argv[], int aktiv_param_index)
                            if (check_param_options(argv,i) != 0)
                                {
                                //* Returns 5 = not allowed option found
                                return 5;
                                }
                            
                        
                        params_ok = YES;
                        //exit for()
                        ii=numb_of_allowed_params;
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
    struct stat which_entry;
    
    //int stat(const char *path, struct stat *buf);
    stat(locationName, &which_entry);
    
    //check which location it is
    if(S_ISREG(which_entry.st_mode)){        return 1;}  //"ordinary file"
    else if(S_ISDIR(which_entry.st_mode)){   return 2;}  //"directory"
    else if(S_ISCHR(which_entry.st_mode)){   return 3;}  //"text orientated DEVICE"
    else if(S_ISBLK(which_entry.st_mode)){   return 4;}  //"block orientated DEVICE"
    else if(S_ISFIFO(which_entry.st_mode)){  return 5;}  //"pipe"
    else if(S_ISLNK(which_entry.st_mode)){   return 6;}  //"symbolic link";
    //else if(S_ISSOCK(which_entry.st_mode)){  (void) fclose(fp);      return 7;}  //"socket"
    else{                                    return -1;} //"undef"
    
}



/**
 *check_param_options(PARAMETER ARRAY,AKTIV PARAMETER)
 *
 * Returns 0 = everything OK
 * Returns 1 = param_option not allowed
 */
int check_param_options(const char * argv[], int aktiv_param_index)
{
    //help var to make code better
    int aktiv_option = aktiv_param_index +1;
    
    //-------------------------------------------------------------------------------------------NAME_PARAM
    if (strcmp(allowed_params[NAME_PARAM], argv[aktiv_param_index]) == 0)
    {
        //check if next argv is a param -> if so, then it's not allowed by this OPTION
        if (strncmp("-", argv[aktiv_option] , 1) == 0)
        {
            //Returns 1 = param_option not OK
            return 1;
        }
        else {
            
            
            //Push to PARAM_LIST
            push(argv[aktiv_param_index], argv[aktiv_option]);
         
       
        }
    
   
        
        
        
        // Everything is allowed ? <pattern>
        //{
        //}
        

    }

    //-------------------------------------------------------------------------------------------TYPE_PARAM
    if (strcmp(allowed_params[TYPE_PARAM], argv[aktiv_param_index]) == 0)
    {
        
        int numb_of_allowed_options=0;
        int i=0;
        char * allowed_options[]={"b","c","d","p","f","l","s"};
        

        //check if next argv is a param -> if so, then it's not allowed by this OPTION
        if (strncmp("-", argv[aktiv_option] , 1) == 0)
        {
            //Returns 1 = param_option not OK
            return 1;
        }
        
        i=0;
        ///Check max Allowed Options---------START
        while(allowed_options[i] != NULL)
        {
            numb_of_allowed_options++;
            i++;
        }
        ///Check max Allowed Options---------END
        
        
        // only one param at once is allowed
        if (strlen(argv[aktiv_option]) == 1)
        {

            //for( i=0; i < numb_of_allowed_options; i++)
            for( i=0; i < 6; i++)
            {
                
     
                if(strcmp(argv[aktiv_option], allowed_options[i]) == 0) {
                    //Push to PARAM_LIST
                    push(argv[aktiv_param_index], argv[aktiv_option]);
                }

                
            }
            
        }
        else{
            
            //Returns 1 = param_option not OK
            fprintf(stderr, "myfind() check_param_options()- : Error: parameter format false\n");
            fprintf(stderr, "myfind() check_param_options()- : %s\n",argv[aktiv_param_index + 1]);
            return 1;
        }
        
        
        
    }

    //-------------------------------------------------------------------------------------------USER_PARAM
    if (strcmp(allowed_params[USER_PARAM], argv[aktiv_param_index]) == 0)
    {
        //check if next argv is a param -> if so, then it's not allowed by this OPTION
        if (strncmp("-", argv[aktiv_option] , 1) == 0)
        {
            //Returns 1 = param_option not OK
            return 1;
        }
        
        //Push to PARAM_LIST
        push(argv[aktiv_param_index], argv[aktiv_option]);
        
        //Do something with argv[aktiv_param_index]  and  argv[aktiv_option]..................
        
    }
    
    //-------------------------------------------------------------------------------------------PRINT_PARAM
    if (strcmp(allowed_params[PRINT_PARAM], argv[aktiv_param_index]) == 0)
    {
    // After this PARAM, no OPTIONS are required
    
        //Push to PARAM_LIST
        push(argv[aktiv_param_index], "");
        
        
    //Do something with argv[aktiv_param_index]
    
        
    }
    
    //-------------------------------------------------------------------------------------------LS_PARAM
    if (strcmp(allowed_params[LS_PARAM], argv[aktiv_param_index]) == 0)
    {
      // After this PARAM, no OPTIONS are required
        
    //Do something with argv[aktiv_param_index]  and  argv[aktiv_option]..................
        
    }
    
    //-------------------------------------------------------------------------------------------NOUSER_PARAM
    if (strcmp(allowed_params[NOUSER_PARAM], argv[aktiv_param_index]) == 0)
    {
     // After this PARAM, no OPTIONS are required
    
        //Push to PARAM_LIST
        push(argv[aktiv_param_index], "");
        
     //Do something with argv[aktiv_param_index]  and  argv[aktiv_option]..................
        
    }
    
    //-------------------------------------------------------------------------------------------PATH_PARAM
    if (strcmp(allowed_params[PATH_PARAM], argv[aktiv_param_index]) == 0)
    {
     // After this PARAM, no OPTIONS are required
    
        //Push to PARAM_LIST
        push(argv[aktiv_param_index], "");
        
     //Do something with argv[aktiv_param_index]  and  argv[aktiv_option]..................
        
    }
    
    
    
    return 0;
}


void push(const char * param,const char * option)
{

    param_stack new;
    new.s_parameter = param;
    new.s_option = option;
    new.s_next_param = NULL;
    
    if(param_list == NULL){
    param_list = &new;
    }
    else{
    
        param_stack * help = search_empty(param_list);
        
        help = &new;


    }
    
    
}


param_stack * search_empty (param_stack * start)
{
    
    if (start == NULL)
    {
        return NULL;
    }
    
    if (start->s_next_param == NULL)
    {
        return start;
    }
    else {
        return search_empty(start->s_next_param);
    }
    

    return NULL;
}


void pop ()
{
    
    param_stack * help = param_list;
    
    param_list = param_list->s_next_param;
    
    free (help);
    
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
