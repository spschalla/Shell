# Author: Saketh Challa
# Implements a shell program with both an interactive and batch mode for processing commands. 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

 // associated global fields
char ** possible_path = NULL;
int path_size;
bool path_set = false;
long total_loop = -1;


int number_arguments(char * buffer) {

  char * temp;
  int counter = 0;
  char * buffer_dup = strdup(buffer);

  while ((temp = strsep( & buffer_dup, " ")) != NULL) {
    counter++;
  }

  free(temp);
  return counter;
}

void parse_line(char ** arg_list, char * buffer) {

  char * temp;
  int counter = 0;

  char * buff_dup = strdup(buffer);
  while ((temp = strsep( & buff_dup, " ")) != NULL) {

    if (counter == 0) {
      arg_list[counter] = temp;
      counter++;
    } else {
      arg_list[counter] = temp;
      counter++;
    }
  }
  if (counter == 0) {
  //  arg_list[counter] = malloc(sizeof(char) * strlen(buff_dup)+1);
    strcpy(arg_list[counter], buff_dup);
  }
}
	




void trimLeadingSpaces(char *buffer, char *newBuffer) {

    int i=0;
    int j=0;
    int k=0; 
    while (buffer[i] == ' ') {
        i++;
    }   
    for (j = i, k = 0;
         buffer[j] != '\0'; j++, k++) {
           newBuffer[k] = buffer[j];
    }
    newBuffer[k] = '\0';

    k = strlen(newBuffer);

    while(k > 0 && isspace((unsigned char)newBuffer[k-1]))
            k--;
      newBuffer[k] = '\0';
}

void removeSpace(char * buffer, char * newbuff) {

  int counter_buff;
  int counter_new = 0;
  
  bool firstNonSpaceChar = false;
  for (int i = 0; i < strlen(buffer); i++) {

    // current character
    char curr = buffer[i];

    // is curr a space
    if (isspace(curr) == 0) {
      // Checks to see if previous is a space
      if (i > 0 && isspace(buffer[i - 1]) != 0) {

	newbuff[counter_new] = ' ';
        counter_new++;

      }

      //newbuff[counter_new] = buffer[i];
      //printf("[%c]", curr);
      newbuff[counter_new] = curr;
      counter_new++;

    }

  }

  newbuff[counter_new] = '\0';

}

int isReservedCommand(char ** arg_list) {
  int return_val = 0;
  if ((strcmp(arg_list[0], "cd") == 0) || (strcmp(arg_list[0], "path") == 0) || (strcmp(arg_list[0], "exit") == 0) || (strcmp(arg_list[0], "loop") == 0)) {
    return_val = 1;
  }
  return return_val;
}

char * validPath(char * command) {

  char currentCommand[128];
  char *ptr;

  for (int i = 0; i < path_size; i++) {
    //char *currentCommand = possible_path[i];
    strcpy(currentCommand, possible_path[i]);
   //printf("PossiblePath[i] %s\n", possible_path[i]);
   //printf("currentCommand path  %s\n", currentCommand);
    strcat(currentCommand, command);

    int access_value = access(currentCommand, X_OK);
    //	 	printf("Current Command is and accessvalue is %s%i\n", currentCommand, access_value);	
    if (access_value == 0) {
      ptr = (char *) malloc(strlen(currentCommand) + 1);
      strcpy(ptr, currentCommand);
      return ptr;
    }

  }
  return NULL;

}

void errorReached() {

  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

// Handles the Reserved Functions Appropriately
void ReservedFunctions(char ** arg_list, int num_args) {
  // Handles the case when exit is called	
  //printf("ReservedFunctions [%s] \n", arg_list[0]);

  if (strcmp(arg_list[0], "exit") == 0) {
    //printf("Exit has been reached\n");
    if (num_args != 1) {
      errorReached();
    }
    exit(0);
  }
  // Handles the case when cd is called
  else if (strcmp(arg_list[0], "cd") == 0) {
    if (num_args != 2) {
      errorReached();

    }
    int returnValue = chdir(arg_list[1]);
  } else if (strcmp(arg_list[0], "path") == 0) {

    //printf("path num_args %i \n", num_args);
    char ** newPossiblePath = malloc(sizeof(char * ) * (num_args - 1));

    for (int i = 1; i < num_args; i++) {
      char *possible_path = strdup(arg_list[i]);
      newPossiblePath[i - 1] = strcat(possible_path, "/");
    }

    possible_path = newPossiblePath;
    path_size = num_args - 1;
    path_set = true;
    //printf("coming to path_set to true %i \n", path_size);

  } else if (strcmp(arg_list[0], "loop") == 0) {

    // Converts the number of loop arguments in a global variable 

    // Need to figure out what I should do with an incorrect number of arguments	 
    if (num_args < 2) {
      errorReached();
    }
   
    else {
 	    
      bool print_output_flag = false;

      // Check for how to handle non-interger values 
      total_loop = atoi(arg_list[1]);
      if (total_loop < 0) {
	errorReached();
        return;
      }

      char ** newArgsString = malloc(num_args * sizeof(char * ));
      char * numIterations = arg_list[1];
      int current_position = 0;
      int dollar_position = -1;
      //printf("Loop before for loop %i \n", num_args);
      for (int i = 2; i < num_args; i++) {

	//printf(" %i\t%s\n", i, arg_list[i]);
        if (arg_list[i][0] == '$') {
          print_output_flag = true;
          newArgsString[current_position] = arg_list[i];
          dollar_position = current_position;
          current_position++;
          continue;
        }

        //printf("Parsing args here %i\t%s\n", current_position, arg_list[i]);
        newArgsString[current_position] = arg_list[i];
        current_position++;

      }

      //printf("came out of for loop\n");

      int counter = 1;

      while (counter <= total_loop) {

        if (print_output_flag) {

          char newString[80];
          sprintf(newString, "%i", counter);
          strcpy(newArgsString[dollar_position], strdup(newString));
        }
        //int rc = fork();
        //if (rc == 0) {

          char * command_path = NULL;

          if (path_set == false) {
            command_path = strdup("/bin/");
            strcat(command_path, newArgsString[0]);
          }

          // Allow execution if paths exist
          else if (path_set  && path_size != 0) {
            command_path = validPath(newArgsString[0]);
          }
          else if (path_set  && path_size == 0) {
            errorReached();
	    counter++;
	    continue;
          }

	  //printf("command_path %s newArgsString %s\n", command_path, newArgsString);
        int rc = fork();
        if (rc == 0) {
          if (command_path != NULL) {
	    //printf("command_path %s newArgsString %s\n", command_path, newArgsString);
            execv(command_path, newArgsString);
          } 

        } else if (rc > 0) {
          wait(NULL);

        }

        counter++;

      }

    }
    //printf("out of loop processing \n");   
  }
}

void interactive_mode() {

  while (1) {
    int pid = getpid();
    // printf("pid = %i \n", pid);

    printf("wish> ");
    char string[50];
    char nospacestring[50];
    gets(string);

    removeSpace(string, nospacestring);
    int ret = number_arguments(nospacestring);

    // printf("number of arguments %i \n", ret);

    char ** arg_list = malloc((ret + 1) * sizeof(char * ));
    arg_list[ret] = NULL;
    parse_line(arg_list, nospacestring);

    // printf("before calling coming here for isReserved nospacestring %s arg_list %s \n", nospacestring, arg_list[0]);
    int isReserved = isReservedCommand(arg_list);

    if (isReserved == 1) {
      // printf("coming here for isReserved %s \n", arg_list[0]);
      ReservedFunctions(arg_list, ret);
      continue;

    } else {


        char * command_path = NULL;

        if (path_set == false) {

          command_path = strdup("/bin/");
	  strcat(command_path, arg_list[0]);
       	  int access_value = access(command_path, X_OK);
	  if (access_value != 0) command_path = NULL; 
	
	}

        // Allow execution if paths exist
        else if (path_set == true && path_size != 0) {
          command_path = validPath(arg_list[0]);
        }

	if (command_path == NULL) {
            errorReached();
	    continue;
	}

        int rc = fork();

        if (rc == 0) {
        if (command_path != NULL) {
          execv(command_path, arg_list);
        }
      } else if (rc > 0) {
	int parentPid = getpid();
	// printf("parent process pid %i \n", parentPid);

        wait(NULL);

      }
    }
  }

}

void batch_mode(char * argument) {

  // Checks to see if the file pointer is empty
  FILE * fp;

  if ((fp = fopen(argument, "r")) == NULL) {
 	  errorReached();	  
	  exit(1);	
  }

  char buffer[256];

  while (1) {

    if (fgets(buffer, 256, fp) == NULL) {
        
    	break;
    
    }
    
    // Removes all white spaces from the buffer
    char noSpaceBuffer[256];
    //removeSpace(buffer, noSpaceBuffer);
    trimLeadingSpaces(buffer, noSpaceBuffer);
    //printf("trimTrailingSpaces [%i] \n", strlen(noSpaceBuffer));

    if (strlen(noSpaceBuffer) <= 1) {
    	continue;
    }
    
    //printf("[%s]\n", noSpaceBuffer);
    // Number of arguments in the commandString
    int ret = number_arguments(noSpaceBuffer);
    // List strings size of # arguments and last element null
    char ** arg_list = malloc((ret + 1) * sizeof(char * ));
    arg_list[ret] = NULL;
    parse_line(arg_list, noSpaceBuffer);
    /* 
    for (int i=0; i<ret; i++) {
    
    	printf("%i [%s]\n", i, arg_list[i]);
    
    }*/
        //printf("command = [%s] \n", arg_list[0]); 
    int isReserved = isReservedCommand(arg_list);
//        printf("1 command = [%s] \n", arg_list[0]);

    if (isReserved == 1) {

      //printf("going here %d \n", ret);
      ReservedFunctions(arg_list, ret);
      //printf("coming out of reserved function \n");
      continue;

    } else {


        //printf("command = [%s] \n", arg_list[0]);
        char * command_path = NULL;

        if (path_set == false) {

          command_path = strdup("/bin/");
          strcat(command_path, arg_list[0]);
          int access_value = access(command_path, X_OK); 
	  if (access_value != 0) command_path = NULL; 
	}
	
        // Allow execution if paths exist
        else if (path_set == true && path_size != 0) {
          command_path = validPath(arg_list[0]);
        }
        if (command_path == NULL) {

          errorReached();
	  continue;
        }
	
      	int rc = fork();

      	if (rc == 0) {
        if (command_path != NULL) {

          execv(command_path, arg_list);

        }

      } else if (rc > 0) {
        wait(NULL);

      }
    }
  }

  fclose(fp);

}

int main(int argc, char ** argv) {

  if (argc == 1) {
    interactive_mode();
  }

  if (argc == 2) {
    batch_mode(argv[1]);
  }

  if (argc > 2) {
    	errorReached();
	exit(1);
  }

  return 0;
}
