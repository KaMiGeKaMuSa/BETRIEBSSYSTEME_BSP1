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


 // ----------------------- STACK -- START


typedef struct param_stack {
    
    const char * s_parameter;
    const char * s_option;
    struct param_stack * s_next_param;
    
}param_stack;


param_stack * param_list = NULL;
int stack_count = 0;

// ----------------------- STACK -- END


const char * allowed_params[]={"-name","-type", "-user", "-print", "-ls", "-nouser", "-path", NULL};
char *fullpath = NULL;


//var for Error Handling = errno
extern int errno ;

/*
 * ------------------------------------------------------------- functions --
 */

/// Vorgegeben sind 2 Grundfunktionen
///do_file und do_dir


void do_file(const char * file_name, const char * parms, int parms_length,const char * argv[],int check_params_return);
void do_dir(const char * dir_name, const char * parms, int parms_length,const char * argv[], int check_params_return);
void check_print(const char * file_name, const char * parms, int parms_length);


///Functions for HELP
int check_params(int argc, const char * argv[]);
int check_param_options(const char * argv[], int aktiv_param_index);
int do_params(char *file_or_dir_name);
int which_location(const char *locationName);
void view_help(void);


param_stack * search_empty (param_stack * param_list);
int push(const char * param,const char * option);
int pop();


/*
 * ------------------------------------------------------------- main --
 */


int main(int argc, const char * argv[]) {
    
    //check params and if something went wrong exit state 1;
    int check_params_return = check_params(argc, argv);
    
    if (check_params_return > 1) return 1;
    
    if ((fullpath = realloc(fullpath, 256)) == NULL) {
        fprintf(stderr, "out of memory!\n");
        exit(1);
    }
	
    //TEST ---  overview about the given parameters
    printf("\ncheck_params() number of parameters exclusive programm_itself are: %d\n", argc -1); //FOR TEST

    int i;
    for (i = 1; i < argc; i++)
     {
     printf("\ncheck_params() Param %d =  %s\n", i, argv[i]);
     }
    
    
		// if directory then go to do_dir
        if (which_location(argv[1]) == 2) {
			strcpy(fullpath,argv[1]);
           do_dir(argv[1], argv[2], argc, argv,check_params_return);
        }
        // if file then go to do_file
        else {
			do_file(argv[1], argv[2], argc, argv,check_params_return);
        }

    
    return 0;
}

// MM:
void do_file(const char * file_name, const char * parms, int parms_length, const char * argv[], int check_params_return){
    // File auf alle Parameter überprüfen und ausgeben wenn nötig =>
	// Schleife for(int i = 0; i < parms.length; i++)
	// if (parms[i] == aktion1){ } else if (parms[i] == aktion2) {}.....
	// Wenn aktion einen zweiten Parameter braucht z.B. -user	<name> dann muss i=i+1 am schluß damit <name> nicht als eigene aktion ausgewertet wird
	// Wenn eine aktion/filter nicht mit dem file übereinstimmt dann setze Flag isValid auf 0 (false) und Schleife über die restlichen Parameter kann abgebrochen werden
	// Nachdem Schleife alle Parameter für das File überprüft hat check Flag isValid und printf wenn true
		
    
    //check_print(file_name, parms,parms_length);
    
    
    if (which_location(file_name) == 2){
        do_dir(file_name, parms,parms_length,argv, check_params_return);
    }
    
    
    int help_return= do_params((char*)file_name);
    

    if (help_return == 1) { //do_params() returns 1 == print imediately because of -ls or -print
        fprintf(stdout, "%s\n", file_name);
    }

    //When stack is empty -- default print
    if (help_return == 3){
        fprintf(stdout, "%s\n", file_name);
    }

}

//MM:
void do_dir(const char * dir_name, const char * parms, int parms_length,const char * argv[], int check_params_return){
    // Schleife über alle Elemente des dir_name
	// . und .. überspringen da sonst Endlosschleife
	// check ob nächstes Element dir oder was anderes ist (isDirectory)
	// wenn was anderes rufe do_file auf || wenn dir rufe do_dir mit dem derzeitigen Element + counter um die Tiefe des Directorys zu wissen	
	DIR * dir_object;
	struct dirent * dir_element;
	
    //var to save errno state
    int errnum;
    
	strcat(fullpath, "/");
	//printf("fullPath: %s, dirname: %s\n", fullpath, dir_name);
    
	if ((dir_object = opendir(dir_name)) == NULL) {
        
        //Correct Error Handling with Errno, state saved in errnum
        errnum = errno;
        fprintf(stderr, "Error opening: %s  Name: %s%s\n", strerror( errnum ), fullpath, dir_name);
        return;
    }
	
    int save_stackcount = stack_count;
    
	while ((dir_element = readdir(dir_object)) && dir_element) {
        
		
		if (strcmp(dir_element->d_name, "..") == 0) continue;
		if (strcmp(dir_element->d_name, ".") == 0 && strcmp(fullpath, argv[1]) == 0) continue;
		
        	//initialise i and reset i
        	int i = 0;

        
        	// IF  RETURN OF CHECK PARAM IS NOT 1 ==> * Returns 1 = no params in Stack
        	if(check_params_return != 1)
        	{
                	for(i=0;i<=save_stackcount;i++)
                    	{
                            int help_return= do_params(dir_element->d_name);

                            if (help_return == 1 ) { //do_params() returns 1 == print imediately because of -ls or -print
                                    fprintf(stdout, "%s%s\n", fullpath, dir_element->d_name);
                                }
            
                    	}
        
            		if(stack_count == 0)
            		{
            			check_params(parms_length, argv);
            		}
        
        	}
	        else
        	{
            		fprintf(stdout, "%s%s\n", fullpath, dir_element->d_name);
        	}
		
		
		//MM: rekursives aufrufen der do_dir damit man in die directory reingehen kann:
		//printf("which_location: %d\n", which_location(dir_element->d_name));
        	if (which_location(dir_element->d_name) == 2) {
            		if (strcmp(dir_element->d_name, ".") == 0 || strcmp(dir_element->d_name, "..") == 0)
                		continue;
				strcat(fullpath,dir_element->d_name);
				//strcat(fullpath, "/");
				do_dir(fullpath, parms,parms_length,argv, check_params_return);
			}
    	}
	
	fullpath[strlen(fullpath)-strlen(dir_name)-1] = '\0';
		
	closedir(dir_object);
}

// MM:
void check_print(const char * file_name, const char * parms, int parms_length){
	
	// for schleife über stack
	// check derzeitiges element vom stack (param_list->s_parameter) auf derzeitiges element (file_name)
	// wenn stack ist gleich null 
	
    //GS: Question to MM :: WHY parms_length-2 and why start for() with i=ZERO  == ZERO == Name of Programm
    for (int i = 0; i < parms_length-2; i++){
		
		//printf("Filename: %s, parms: %s\n", file_name, parms); // For TESTING
		
		if (strcmp(&parms[i], allowed_params[PRINT_PARAM]) == 0){
				printf("%s\n", file_name);
		}
        else{
            
            
            // IF OTHER PARAMS -- OK ==== > PRINT ---
            
            
            
            //Errorhandling corrected -- no errno is used because there is no errno code for this
            fprintf(stderr, "myfind() - check_print(): Error: unknown parameter\n");
		}
	}
}


/**
 * check_params()
 * Returns 0 = everything is OK
 * Returns 1 = no params in Stack
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
    
    
    //printf("\ncheck_params() number of parameters exclusive programm_itself are: %d\n", argc -1); //FOR TEST
    
    
    //is true when minimum one parameter is given
    if(argc > 1)
    {
        
        
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
            
            if ((strncmp("-", argv[i] , 1) != 0 ) && stack_count == 0)
            {
            
                //Errorhandling corrected -- no errno is used because there is no errno code for this
                fprintf(stderr, "myfind() - check_params(): Error: unknown parameter %s -- only one dir/file allowed by myfind\n",argv[i]);
                
                view_help();
                
                return 5;
            }
            
            
            //Check if it's a PARAM
            if (strncmp("-", argv[i] , 1) == 0)
            {
                
                    //for() as log as allowed params found
                    //numb_of_allowed_params - 1 -> because array starts with ZERO
                    //ii = 1 because PARAM HELP should not listed behind
                    for (ii=0; ii < numb_of_allowed_params; ii++)
                    {
            
                        //if in one loop a ALLOWED PARAM is found, then check OK
                        if (strcmp(allowed_params[ii], argv[i] ) == 0){
                        
                       
                                /// Check OPTIONS--------------------------------------------------------------
                                //int check_param_options(const char * argv[], int aktiv_param_index)
                                if (check_param_options(argv,i) != 0)
                                {
                                
                                //PRINT OUT IS DONE IN check_param_options()
                                    
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
                        
                        //Errorhandling corrected -- no errno is used because there is no errno code for this
                        fprintf(stderr, "\nmyfind() - check_params(): Error: not allowed parameter %s\n",argv[i]);
                        
                        view_help();
                        
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
        //Errorhandling corrected -- no errno is used because there is no errno code for this
        fprintf(stderr, "\nmyfind() - check_params(): Error: no params are found\n");
        
        view_help();
        
        //Returns 3 = no parameter given
        return 3;
    }
    
    
    if(stack_count == 0)
    {
        //Returns 1 = no params in Stack
        return 1;
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
 *  return 7  -->   "socket"        //NOT INCLUDED ANYMORE
 *  return -1 -->   "undef"
 *  return -1 -->   "fail to open" //NOT INCLUDED ANYMORE
 */
int which_location(const char *locationName)
{
    struct stat which_entry;
	if ( fullpath != NULL) {
		char * tempStr = (char *) malloc(1 + strlen(fullpath)+ strlen(locationName) );
		strcpy(tempStr, fullpath);
		strcat(tempStr, locationName);
		
		stat(tempStr, &which_entry);
		
		free(tempStr);
    } else {
		stat(locationName, &which_entry);
	}
	
    //int stat(const char *path, struct stat *buf);
    
    
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
 * Returns 2 = push() failed
 */
int check_param_options(const char * argv[], int aktiv_param_index)
{
    //help var to make code better
    int aktiv_option = aktiv_param_index +1;
    int help =0;
    
    //-------------------------------------------------------------------------------------------NAME_PARAM
    if (strcmp(allowed_params[NAME_PARAM], argv[aktiv_param_index]) == 0)
    {
        
            //Push to PARAM_LIST and check if push() work well
            if (push(argv[aktiv_param_index], argv[aktiv_option]) != 0)
            {
                //Returns 2 = push() failed
                return 2;
            }
        
        

    }

    //-------------------------------------------------------------------------------------------TYPE_PARAM
    if (strcmp(allowed_params[TYPE_PARAM], argv[aktiv_param_index]) == 0)
    {
        
        int numb_of_allowed_options=0;
        int i=0;
        char * allowed_options[]={"b","c","d","p","f","l","s", NULL};
        

        //check if next argv is a param -> if so, then it's not allowed by this OPTION
        if (strncmp("-", argv[aktiv_option] , 1) == 0)
        {
  
            //Errorhandling corrected -- no errno is used because there is no errno code for this
            fprintf(stderr, "\nmyfind() - check_param_options(): parameter %s option %s not exist\n", argv[aktiv_param_index], argv[aktiv_option]);
            
            view_help();
            
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
            for( i=0; i < numb_of_allowed_options; i++)
            {
                
     
                if(strcmp(argv[aktiv_option], allowed_options[i]) == 0) {
                    
                    
                    //Push to PARAM_LIST and check if push() work well
                    if (push(argv[aktiv_param_index], argv[aktiv_option]) != 0)
                    {
                        //Returns 2 = push() failed
                        return 2;
                    }
                
                    help =1;
                    
                }

                
            }
            
            if( help != 1) {
            //Errorhandling corrected -- no errno is used because there is no errno code for this
            fprintf(stderr, "\nmyfind() - check_param_options(): parameter %s option %s not exist\n", argv[aktiv_param_index], argv[aktiv_option]);
            
            view_help();
            
            //Returns 1 = param_option not OK
            return 1;
            }
            else {
                help = 0; //reset help
            }
            
            
        }
        else{
            
            //Errorhandling corrected -- no errno is used because there is no errno code for this
            fprintf(stderr, "\nmyfind() - check_param_options(): parameter %s option %s not exist\n", argv[aktiv_param_index], argv[aktiv_option]);
            
            view_help();
            
            //Returns 1 = param_option not OK
            return 1;
        }
        
        
        
    }

    //-------------------------------------------------------------------------------------------USER_PARAM
    if (strcmp(allowed_params[USER_PARAM], argv[aktiv_param_index]) == 0)
    {
        //check if next argv is a param -> if so, then it's not allowed by this OPTION
        if (strncmp("-", argv[aktiv_option] , 1) == 0)
        {
            
            //Errorhandling corrected -- no errno is used because there is no errno code for this
            fprintf(stderr, "\nmyfind() - check_param_options(): parameter %s option %s not exist\n", argv[aktiv_param_index], argv[aktiv_option]);
            
            view_help();
            
            //Returns 1 = param_option not OK
            return 1;
        }
        
        
        //Push to PARAM_LIST and check if push() work well
        if (push(argv[aktiv_param_index], argv[aktiv_option]) != 0)
        {
            //Returns 2 = push() failed
            return 2;
        }
        
        
    }
    
    //-------------------------------------------------------------------------------------------PRINT_PARAM
    if (strcmp(allowed_params[PRINT_PARAM], argv[aktiv_param_index]) == 0)
    {
    
        //Push to PARAM_LIST and check if push() work well
        if (push(argv[aktiv_param_index], "") != 0)
        {
            //Returns 2 = push() failed
            return 2;
        }
    
        
    }
    
    //-------------------------------------------------------------------------------------------LS_PARAM
    if (strcmp(allowed_params[LS_PARAM], argv[aktiv_param_index]) == 0)
    {
    
		
		
        //Push to PARAM_LIST and check if push() work well
        if (push(argv[aktiv_param_index], "") != 0)
        {
            //Returns 2 = push() failed
            return 2;
        }
        
    }
    
    //-------------------------------------------------------------------------------------------NOUSER_PARAM
    if (strcmp(allowed_params[NOUSER_PARAM], argv[aktiv_param_index]) == 0)
    {
        
        //Push to PARAM_LIST and check if push() work well
        if (push(argv[aktiv_param_index], "") != 0)
        {
            //Returns 2 = push() failed
            return 2;
        }
        
    }
    
    //-------------------------------------------------------------------------------------------PATH_PARAM
    if (strcmp(allowed_params[PATH_PARAM], argv[aktiv_param_index]) == 0)
    {
        
		if (push(argv[aktiv_param_index], argv[aktiv_option]) != 0)
            {
                //Returns 2 = push() failed
                return 2;
            }
        
    }
    
    
    
    return 0;
}


/**
 *
 * push(const char * param,const char * option)
 *
 * Returns 0 = everything OK
 * Returns 1 = Malloc Error
 * Returns 2 = Search in Stack Error
 */
int push(const char * param,const char * option)
{
    
    param_stack * new = malloc(sizeof( param_stack));
    
    if(new == NULL)
    {
        //Errorhandling corrected -- no errno is used because there is no errno code for this
        fprintf(stderr, "\nmyfind() - push(): Error: Malloc\n");

        // Returns 1 = Malloc Error
        return 1;
    }
    
    new->s_parameter = param;
    new->s_option = option;
    new->s_next_param = NULL;
    
    if(param_list == NULL){
    param_list = new;
    stack_count++;
    
    }
    else{
    
        param_stack * help = search_empty(param_list);
        
        if(help == NULL)
        {

            //Errorhandling corrected -- no errno is used because there is no errno code for this
            fprintf(stderr, "\nmyfind() - push(): Error: Search in Stack\n");
            
            // Returns 2 = Search in Stack Error
            return 2;
        }
        
        help->s_next_param = new;
        stack_count++;

    }
    
    // Returns 0 = everything OK
    return 0;
}

//search_empty()
// Returns NULL = NOT OK
// Returns NOT NULL = OK == IF s_next_param == NULL
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


//pop ()
// delete the first param in Stack => now the second Param is the first
// return 0 = Everything OK
// return 1 = Stack is Empty, No pop allowed
int pop ()
{
    
    
    if(stack_count == 0)
    {
        //Errorhandling corrected -- no errno is used because there is no errno code for this
        fprintf(stderr, "\nmyfind() - pop(): Error: nothing in stack\n");
        
        return 1;
    }
    
    param_stack * help = param_list;
    
    param_list = param_list->s_next_param;
    
    free (help);
    help = NULL;
    
    stack_count--;
    
    return 0;
}




//do_params()
//
// returns 0 == everything ok
// returns 1 == print imediately because of -ls or -print
// returns 2 == not print this line
// returns 3 == Stack is Empty

int do_params(char *file_or_dir_name)
{
    
    typedef enum yes_no {YES,NO}yes_no;
    
    yes_no print_it = NO;
    
    
    //Check if Steck is empty
    if(stack_count == 0)
    {
        return 3;
    }
    
    
    //-------------------------------------------------------------------------------------------TYPE_PARAM
    if(strcmp(allowed_params[TYPE_PARAM], param_list->s_parameter) == 0)
    {
        
        //char * allowed_options[]={"b","c","d","p","f","l","s", NULL};


        //WHEN FILE, THEN---------------------------------------f
        if(strcmp("f", param_list->s_option) == 0)
        {
        
            //IF IT's CORRECT, THEN PRINT_IT = YES
            if( which_location((const char*)file_or_dir_name) == 1) { //*  which:location  return 1  -->   "ordinary file"
                print_it = YES;
            }
            else{
                print_it = NO;
                // returns 2 == not print this line
                return 2;
            }
  
        }
        

        //WHEN DIRECTORY, THEN----------------------------------d
        if(strcmp("d", param_list->s_option) == 0)
        {
            
            //IF IT's CORRECT, THEN PRINT_IT = YES
            if( which_location((const char*)file_or_dir_name) == 2) { //*  which:location  return 2  -->   "directory"
                print_it = YES;
            }
            else{
                print_it = NO;
                // returns 2 == not print this line
                return 2;
            }
            
        }
        
        
        //WHEN text special, THEN-------------------------------c
        if(strcmp("c", param_list->s_option) == 0)
        {
            
            //IF IT's CORRECT, THEN PRINT_IT = YES
            if( which_location((const char*)file_or_dir_name) == 3) { //*  which:location  return 3  -->   "text orientated DEVICE"
                print_it = YES;
            }
            else{
                print_it = NO;
                // returns 2 == not print this line
                return 2;
            }
            
        }
        
  
        
        //WHEN block special, THEN------------------------------b
        if(strcmp("b", param_list->s_option) == 0)
        {
            
            //IF IT's CORRECT, THEN PRINT_IT = YES
            if( which_location((const char*)file_or_dir_name) == 4) { //*  which:location  return 4  -->   "block oriented DEVICE"
                print_it = YES;
            }
            else{
                print_it = NO;
                // returns 2 == not print this line
                return 2;
            }
            
        }
        
        
        //WHEN pipe, THEN---------------------------------------p
        if(strcmp("p", param_list->s_option) == 0)
        {
            
            //IF IT's CORRECT, THEN PRINT_IT = YES
            if( which_location((const char*)file_or_dir_name) == 5) { //*  which:location  return 5  -->   "pipe"
                print_it = YES;
            }
            else{
                print_it = NO;
                // returns 2 == not print this line
                return 2;
            }
            
        
        }
        
        
        //WHEN symbolic link, THEN-------------------------------l
        if(strcmp("l", param_list->s_option) == 0)
        {
            
            //IF IT's CORRECT, THEN PRINT_IT = YES
            if( which_location((const char*)file_or_dir_name) == 6) { //*  which:location  return 6  -->   "symbolic link";
                print_it = YES;
            }
            else{
                print_it = NO;
                // returns 2 == not print this line
                return 2;
            }
            
            
        }
        
        
        
        /*
        //
        // GS: -- ATTENTION -- WHICH LOCATION HAS TO BE UPDATE --> SOCKET RETURN MUST BE IMPLEMENTED AGAIN
        //WHEN socket, THEN---------------------------------------s
        if(strcmp("s", param_list->s_option) == 0)
        {
            
            //IF IT's CORRECT, THEN PRINT_IT = YES
            if( which_location((const char*)file_or_dir_name) == 7) {
                print_it = YES;
            }
            else{
                // returns 2 == not print this line
                return 2;
            }
            
            
        }
        */
        
        
        
    }
    
    
    //-------------------------------------------------------------------------------------------NAME_PARAM
    if(strcmp(allowed_params[NAME_PARAM], param_list->s_parameter) == 0)
    {
		if (fnmatch(param_list->s_option, file_or_dir_name, 0) == 0) { print_it = YES; }
			else {
				print_it = NO; // returns 2 == not print this line 
			return 2; } 
    }
    

    
    //-------------------------------------------------------------------------------------------USER_PARAM
    if(strcmp(allowed_params[USER_PARAM], param_list->s_parameter) == 0)
    {
        
    }
    

    
    //-------------------------------------------------------------------------------------------PRINT_PARAM
    if(strcmp(allowed_params[PRINT_PARAM], param_list->s_parameter) == 0)
    {
        //POP FOR NEXT PARAMETER
        pop();

        return 1;
        
    }
    

    
    //-------------------------------------------------------------------------------------------LS_PARAM
    if(strcmp(allowed_params[LS_PARAM], param_list->s_parameter) == 0)
    {
		struct stat sb;
		char outputString;
		struct tm *tmp;
		char outstr[200];

		stat(file_or_dir_name, &sb);
	
		struct passwd *pw = getpwuid(sb.st_uid);
		struct group  *gr = getgrgid(sb.st_gid);
		
		tmp = localtime(&sb.st_mtime);
		strftime(outstr, sizeof(outstr), "%b %d %H:%M", tmp);
		    
		printf("%6ld %4lld %s%s%s%s%s%s%s%s%s%s %3lld %s %s %8ld %s %s%s\n", 
			(long) sb.st_ino, 
			(long long) sb.st_blocks / 2, 
				(S_ISDIR(sb.st_mode)) ? "d" : "-", 
				(sb.st_mode & S_IRUSR) ? "r" : "-",
				(sb.st_mode & S_IWUSR) ? "w" : "-",
				(sb.st_mode & S_IXUSR) ? "x" : "-",
				(sb.st_mode & S_IRGRP) ? "r" : "-",
				(sb.st_mode & S_IWGRP) ? "w" : "-",
				(sb.st_mode & S_IXGRP) ? "x" : "-",
				(sb.st_mode & S_IROTH) ? "r" : "-",
				(sb.st_mode & S_IWOTH) ? "w" : "-",
				(sb.st_mode & S_IXOTH) ? "x" : "-",
			(long long) sb.st_nlink,
			pw->pw_name,
			gr->gr_name,
			sb.st_size,
			outstr,
			fullpath, 
			file_or_dir_name
			);
		
        //POP FOR NEXT PARAMETER
        pop();
        
        return 0;
    }
    
    

    //-------------------------------------------------------------------------------------------NOUSER_PARAM
    if(strcmp(allowed_params[NOUSER_PARAM], param_list->s_parameter) == 0)
    {

			struct stat buf;
        
			if (lstat((const char*)file_or_dir_name, &buf) != 0) {

				printf("Error! lstat-Eintrag konnte nicht ermittelt werden. \n");
				return 2; 
			}

			if (getpwuid(buf.st_uid) == NULL) {
				print_it = YES;
			}
			else {
				print_it = NO;
				// returns 2 == not print this line
				return 2;
            }
		
    }
    
    

    //-------------------------------------------------------------------------------------------PATH_PARAM
    if(strcmp(allowed_params[PATH_PARAM], param_list->s_parameter) == 0)
    {
        //mit path wird der ganze Pfad der Datei mit dem jeweiligen Pattern überprüft (fnmatch!!)
		char * tempStr = (char *) malloc(1 + strlen(fullpath)+ strlen(file_or_dir_name) );
		strcpy(tempStr, fullpath);
		strcat(tempStr, file_or_dir_name);
		
		if (fnmatch(param_list->s_option, tempStr, 0) == 0) { print_it = YES; }
			else {
				print_it = NO; // returns 2 == not print this line 
			return 2; } 
			
		free(tempStr);
		
    }
    
    
    
    //POP FOR NEXT PARAMETER
    pop();
    
    
    //OVER-ALL-PRINT -> PRINT WENN EVERYTHING LOOKS GOOD
    
    if(print_it == YES){
    
        fprintf(stdout, "%s%s\n", fullpath, file_or_dir_name);

    }
    
    return 0;
}





void view_help(void)
{
    printf("\n###############-------------   HELP START  -------------###############");
    printf("\nmyfind <file or directory> [ <parameters> ] ...\n");
    printf("\nPARAMETERS:");
    printf("\n-user	<name>/<uid>    finde Directoryeinträge eines Users         ");
    printf("\n-name	<pattern>       finde Directoryeinträge mit passendem Namen ");
    printf("\n-type	[bcdpfls]       finde Directoryeinträge mit passendem Typ   ");
    printf("\n-print                gibt den Namen des Directoryeintrags aus    ");
    printf("\n-ls                                                               ");
    printf("\n-nouser                                                           ");
    printf("\n-path                                                             ");
    printf("\n###############-------------    HELP END   -------------###############\n");

    
}





/*
 * =================================================================== eof ==
 */
