#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include "nittalk.h"


int main() {
    char *args[64];
    char *token;
    char *buffer=NULL;                                      ///  Assigning a pointer to BUFFER and setting it to NULL  
    size_t size=0;                                          ///  Setting the variable size to 0, the type of this variable is memory in bytes (it is unsigned)

    while(1){                                               ///  Setiing up a while loop

        int i=0;

        printf("octa-shell--");                             ///  Printing ....
        fflush(stdout);                                     ///  The code needs to push out the statement present inside the printf fucntion
        ssize_t chars_read=getline(&buffer,&size,stdin);    ///  ssize_t (it is signed), the getline function stores the input entered by the user and stores it in the buffer, while it returns the number of characters entered by the user including the new line character, when the user clicks the enter and chars_read collects the number.

        if(chars_read== -1){                                ///  If the user presses to move to the EOF (End of File)
            break;                                          ///  The infinite loop is broken when it is EOF
        }

        token=strtok(buffer," \t\n");                       /// strtok function is called to split the string
        args[i]=token;                                      /// Each token is then given as an input into a list as a string
        i=1;                                                /// Value of i is updated

        if(args[0]==NULL){                                  /// If the user clicks enter without typing any text
            continue;                                       /// The while loop again continues 
        }

        while(token !=NULL){                                /// Setting up a while loop for tokenising the remaining string
            token = strtok(NULL, " \t\n");                  /// NULL is typed, which makes the strtok function to start from where it left previously
            args[i]=token;                                  /// It is then entered into the list
            i++;                                            /// Value of i is incremented

            if(i>=63){                                      /// We are writing a if condition for checking if i value is greater than 63
                args[63]=NULL;                              /// NULL is added to the end of the array
                printf("Too many arguements");              /// This prevents buffer overflow when there is a 65 byte arguement
                break;                                      /// The loop is broken
            }
        }

        /*int j = 0;

        while(args[j] != NULL) {                            /// Setting up a while loop for printing the tokenised string
            printf("Token %d: %s\n", j, args[j]);           /// Calling the printf function...
            j++;                                            /// j value is incremented
        }*/
        
        if(strcmp(args[0],"cd")==0){                        /// If the command typed by the starts with cd which is a built in command

            if(args[1]==NULL){                              /// If the user types only cd
                printf("octa-shell--:Expected an arguement\n");      /// calling the printf function.......
            }

            else {                                          
                if(chdir(args[1])!=0){                      /// If the OS succesfully takes the contents and puts into the location mentioned in args[1], it will return 0. If it fails, it wont return 0.
                    printf("ERROR\n");                      /// Error is printed if the operation is failed
                }
            }
            continue;                                       /// This makes the code to start from while loop again from the first and prevents the code to move towards the fork statement
            
        }

        else if (strcmp(args[0], "nittalk") == 0) {         /// If the first arguement entered by the user is "nittalk"  

            if (args[1] == NULL) {                          /// If only nittalk is entered without any other corresponding arguements....
                printf("nittalk: missing arguments\n");     /// Error is printed
                continue;
            }

            if (strcmp(args[1], "-listen") == 0) {          /// If the next arguement entered by the user is"-listen      

                int port;

                if (args[2] == NULL) {                                 /// If there are no succesive arguements entered  
                    printf("Enter the port number for listening\n");
                    fflush(stdout);                                    /// Asking the user to enter the port no 
                    char port_input[10];                               /// Creating a pointer array to hold the port no 
                    fgets(port_input,sizeof(port_input),stdin);        /// getting the input via fgets() function

                    char *clean_port_text = strtok(port_input, "\n");  /// Stripping the new line character when the user presses ENTER key

                    if (clean_port_text != NULL) {                     /// Checking if the port is NULL...
                        port = atoi(clean_port_text);                  /// If it is not null means it is converted to integer
                        }

                    else {
                        port = 4443;                                   /// If there is no port number entered means, it is set to default as 4443
                    }

                    if (port <= 1023 || port > 65535) {                                                         /// port number lies in the range from 1023 to 65535
                        printf("nittalk: Invalid port. Please choose a number between 1024 and 65535.\n");      /// If it is out of the range means the error is printed 
                        continue;                                                                               /// The loop is continued  
                    }
                }

                else{
                    port = atoi(args[2]);                              /// A variable is made to hold the casted version of args[2] from ASCII value to int    
                    pid_t net_pid = fork();
                    if (net_pid == 0) {
                        server1(port);                                 /// The port number is then passed through the receiver function that is server1 function
                        free(buffer);
                         exit(0);
                    }

                    else if (net_pid > 0) {
                        wait(NULL);
                    }
                    continue;
                }
            }
    
            else if (strcmp(args[1], "-s") == 0) {                                                /// If args[1] is "-s" instead of "listen"
        
       
                if (args[2] == NULL || args[3] == NULL || args[4] == NULL) {                      /// If consecutive arguements are not given        
                    printf("nittalk: invalid syntax. Usage: nittalk -s <IP> -f <filename>\n");    /// Error is printed
                    continue;
                }
        
                if (strcmp(args[3], "-f") != 0) {                                                 /// If args[3] is not "-f"
                    printf("nittalk: expected '-f' flag before filename\n");                      /// File name is required
                    continue;
                }

                char *target_ip = args[2];                                                        /// A pointer variable is declared to hold the IP address entered by the user
                char *filename = args[4];                                                         /// A pointer variable is used to hold the filename declared by the user  

                pid_t net_pid = fork();
                if (net_pid == 0) {
                    client1(target_ip, filename, 4443);                                           /// The filenamen, IP address and 4443 (port number) is passed through the sender function that is client1 function
                    free(buffer);
                    exit(0);
                }

                else if (net_pid > 0) {
                    wait(NULL);
                }
                continue;
            }
            else {
                printf("nittalk: unknown flag '%s'\n", args[1]);                                  /// Error is printed 
            }
        }


        if(strcmp(args[0], "exit") == 0) {                  /// If the command entered by the user is exit.....
            break;                                          /// Programme is exited smoothly
        }

        pid_t pid=fork();                                   /// The parent code is cloneddd
        if(pid<0){                                          /// The clone faileddd, if negative is zero
            printf("ERROR");                                /// printf function is calledd.....
        }

        else if(pid==0){                                    /// The clone is executing the user's command, this is the cloned code  
            execvp(args[0], args);                          /// The child programme is getting destroyed on its one while trying to implement the command entered by the user
            printf("COMMAND NOT FOUND\n");                  /// The child code destroys itself, if it is destroyed means the computer wont reach this line, but if it is reached means there is an error
            free(buffer);
            exit(1);                                        /// The code is exited
        }   
 
        else if(pid>0){                                     /// You are in the parent code
            wait(NULL);                                     /// The parent code is put to sleep, till the command line is executed
        }
    }

    free(buffer);                                           /// The memory is freed, so that there is no memory leak.
    return 0;
}
