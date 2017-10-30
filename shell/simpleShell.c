#include    <stdlib.h>
#include    <stdio.h>
#include    <unistd.h>
#include    <string.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include <fcntl.h>

/*
Estelle Byaer
CS 332
A simple C shell
The pipe doesn't work and I can't for the life of me figure out why because I'm doing
exactly what Sherri said in office hours, but I've spent upwards of 20 hours on this
now and I really can't take another late day on it.
*/
struct __Value {
  int outfile;
  int infile;
  char** args;
  };
typedef struct __Value Value;


char** readLineOfWords();
char** parseArgs(char** args);
int executeArgs(int numArgs, Value **execargs, int wait);
Value *makeValue(int output, int input, int wait, char** args, int numArgs);
Value *makeNull();



 int main() {
   char** args = malloc(sizeof(char*));
   *args = "S";
   while (args[0] != NULL) {
     free(args);
     printf("$ ");
     fflush(stdout);
     args = readLineOfWords();
     if(args[0] != NULL) {
      parseArgs(args);
    } else {
      break;
    }
   }
   return 0;
 }

//given a list of simple arguments and their values, executes them
 int executeArgs(int numArgs, Value **execargs, int wait) {
   int err = 0;
   int pid;
   int tempin;
   int tempout;
   int i;
   printf("exec %s\n", execargs[0]->args[0]);
   fflush(stdout);
   //no pipe: just execute
   if(numArgs == 1) {
     //redirect input and output, if applicable
     dup2(execargs[0]->infile, 0);
     dup2(execargs[0]->outfile, 1);

     pid = fork();
     if(pid == 0) {
       //execute child
       err = execvp(execargs[0]->args[0], execargs[0]->args);
     } else {
       //if no terminal &, wait
       if (wait == 0) {
         waitpid(pid, NULL, 0);
       }
     }
     if (err != 0) {
       printf("Encountered an error! Error code %i \n", err);
     }
     //reset stdin and stdout to their defaults
     dup2(tempin, 0);
     dup2(tempout, 1);

     //pipes
   } else {
     //iterate over num simple args
     int j = 0;
     for(i = 0; i < numArgs -1; i++) {
       //save initial values of stdin, stdout
       tempout = dup(1);
       tempin = dup(0);

       //initialize pipe
       int pipeF[2];
       pipe(pipeF);

       pid = fork();
       if(pid == 0) {
         //redirect input of first arg, if applicable
         printf("execargs infflie :%i\n", execargs[i]->infile);
         fflush(stdout);
        //  dup2(execargs[i]->infile, 0);
        //  //redirect output to pipe output
        //  dup2(pipeF[1], 1);
        //  //close pipe input
        //  close(pipeF[0]);
         //execute child 1
         while(execargs[i]->args[j] != NULL) {
           printf("hereargs[j]: %s\n", execargs[0]->args[0]);
           fflush(stdout);
           j++;
         }
         err = execvp(execargs[i]->args[0], execargs[i]->args);
       } else {
         close(pipeF[1]);
         if (wait == 0) {
          waitpid(pid, NULL, 0);
        }

       }
       if (err != 0) {
         printf("Encountered an error! Error code %i \n", err);
       }

       //close pipe output to ensure EoF
       pid = fork();
       if (pid == 0) {
         //redirect input and output

         dup2(execargs[i+1]->outfile, 1);
         dup2(pipeF[0], 0);
         err = execvp(execargs[i+1]->args[0], execargs[i+1]->args);

       } else {
         close(pipeF[0]);
         if (wait == 0) {
          waitpid(pid, NULL, 0);
        }
       }
       if (err != 0) {
         printf("Encountered an error! Error code %i \n", err);
       }
       //close input and output
       close(tempin);
       close(tempout);
     }
   }
  return err;
}


/* read lines code written by Sherri Goings
*/
 char** parseArgs(char** args) {
   // A line may be at most 100 characters long, which means longest word is 100 chars,
   // and max possible tokens is 51 as must be space between each
   size_t MAX_WORD_LENGTH = 100;
   size_t MAX_NUM_WORDS = 51;

   // allocate memory for array of array of characters (list of words)
   char *copyArgs[MAX_NUM_WORDS];

 //populate array
   int j;
   for (j=0; j<MAX_NUM_WORDS; j++) {
     copyArgs[j][MAX_WORD_LENGTH];
   }

   //array of structs to hold relevant info about all simpe commands
   Value *argList[MAX_NUM_WORDS];
   int numArgs = 0;

   int wait = 0;
   int i = 0;
   int copyI = 0;
   int num;

   argList[numArgs] = malloc(sizeof(Value));
   argList[numArgs]->outfile = 1;
   argList[numArgs]->infile = 0;
   //iterate through all read args
   while (args[i] != NULL) {
     if(args[i]) {
       //input redirects
       if (*args[i] == '<') {
         //get next string after operator
        i++;
        int fd = open(args[i], O_RDONLY, 0644);
        if (!fd) {
          printf("%s not found. Please enter a valid file name.", args[i]);
        }
        argList[numArgs]->infile = fd;
      }
      else if (*args[i] == '>'){
        //output redirect
        i++;
        int fd = open(args[i], O_CREAT|O_WRONLY, 0644);
        if (!fd) {
          printf("%s not found. Please enter a valid file name.", args[i]);
        }
        argList[numArgs]->outfile = fd;

        //sould parent wait?
      } else if (*args[i] == '&') {
        i++;
        //make sure it's at end of line
        if (args[i] != NULL) {
          printf("The operator '&' should only come at the end of a line.\n");
        } else {
          wait = 1;
        }
        //pipe exists: NULL-terminate copy of args, complete parse for that
        //arg set, and pass it to the array
      } else if (*args[i] == '|') {
        for (copyI; copyI < MAX_NUM_WORDS ;copyI++){
          copyArgs[copyI] = NULL;
        }
        argList[numArgs]->args = copyArgs;
        int j = 0;
        while(argList[numArgs]->args[j] != NULL) {
          printf("args[%i]: %s\n",j, argList[numArgs]->args[j]);
          fflush(stdout);
          j++;
        }
        //reset
        copyArgs[MAX_NUM_WORDS];
        for (j=0; j<MAX_NUM_WORDS; j++) {
          copyArgs[j][MAX_WORD_LENGTH];
        copyI = 0;
        }

        printf("numArgs is %i\n", numArgs);

        // argList[numArgs] = parsedArgs;
        printf("should be ls: %s\n", argList[0]->args[0]);
        fflush(stdout);
        numArgs++;

      } else {
        // a normal token
        copyArgs[copyI] = args[i];
        copyI++;
      }
    }
    i++;
   }
   //if done parsing, add remaining simple operation to array
   for (copyI; copyI < MAX_NUM_WORDS ;copyI++){
     copyArgs[copyI] = NULL;
   }
   argList[numArgs]->args = copyArgs;
   j = 0;
  //  while(parsedArgs->args[j] != NULL) {
  //    printf("args[%i]: %s\n",j, parsedArgs->args[j]);
  //    fflush(stdout);
  //    j++;
  //  }
   printf("should be ls: %s\n", argList[0]->args[0]);
   printf("numArgs is %i\n", numArgs);
  //  argList[numArgs] = parsedArgs;
   printf("should be ls: %s\n", argList[0]->args[0]);
   printf("should be cat: %s\n", argList[1]->args[0]);
   numArgs ++;
   executeArgs(numArgs, argList, wait);
 }


 char** readLineOfWords() {

   // A line may be at most 100 characters long, which means longest word is 100 chars,
   // and max possible tokens is 51 as must be space between each
   size_t MAX_WORD_LENGTH = 100;
   size_t MAX_NUM_WORDS = 51;

   // allocate memory for array of array of characters (list of words)
   char** words = (char**) malloc( MAX_NUM_WORDS * sizeof(char*) );
   int i;
   for (i=0; i<MAX_NUM_WORDS; i++) {
     words[i] = (char*) malloc( MAX_WORD_LENGTH );
   }

   // read actual line of input from terminal
   int bytes_read;
   char *buf;
   buf = (char*) malloc( MAX_WORD_LENGTH+1 );
   bytes_read = getline(&buf, &MAX_WORD_LENGTH, stdin);

   // take each word from line and add it to next spot in list of words
   i=0;
   char* word = (char*) malloc( MAX_WORD_LENGTH );
   word = strtok(buf, " \n");
   while (word != NULL && i<MAX_NUM_WORDS) {
     strcpy(words[i++], word);
     word = strtok(NULL, " \n");
   }

   // check if we quit because of going over allowed word limit
   if (i == MAX_NUM_WORDS) {
     printf( "WARNING: line contains more than %d words!\n", (int)MAX_NUM_WORDS );
   }
   else
     words[i] = NULL;

   // return the list of words
   return words;
 }

 Value *makeNull() {
  Value *emptyList = malloc(sizeof(Value));
  if (!emptyList) {
    printf("We're out of memory!");
    exit(1);
  }
  emptyList->outfile = 1;
  emptyList->infile = 0;
  return emptyList;
}

/*
 * Create a value of a specifc type.
 */
Value *makeValue(int output, int input, int wait, char** args, int numArgs) {
  Value *newValue = makeNull();
  newValue->outfile = output;
  newValue->infile = input;
  newValue->args = args;
  return newValue;
}
