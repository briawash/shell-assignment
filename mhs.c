/*
Brianca Washington
ID: 1001132562

*/

// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports ten argumentss
int findCmd(char *cmd_str);
void execFunc(char* cmd, char**attributes);
void listPids(void);

void sigStuff(int choice);
void cdCommand(char*new);
int pastpids[15];

static  int histNum=0;
static  int numpids=0;

char history[50][255];
char currentDirectory[50];


int main()
{
  int found,i,catch=0, val=0;

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    int i = 0;
    for( i = 0; i< 255; i++ ) 
    {
      history[0][i]='\0';
      history[1][i]='\0';
      history[2][i]='\0';
      history[3][i]='\0';
      history[4][i]='\0';
      history[5][i]='\0';
      history[6][i]='\0';
      history[7][i]='\0';
      history[8][i]='\0';
      history[9][i]='\0';

    }
    histNum++;
	

    //save the last entry for history and bg and rerunnning proccess

  
   //make sure the string is not empty
  if(token[0]=='\0')
  {
      catch=1;
      histNum--;  
  }
  //if its not NULL 
  if(catch!=1)
  {
    // find the command but 
    val=findCmd(cmd_str);
    // the user wants to quit
    if (val== -1)
    {
      return 0;
    }
    
    found=histNum%50 -1;
    strncpy(history[found],cmd_str, strlen(cmd_str+1));
  }
  else
  {
    // reset the catch
      catch=0;
  }

    free( working_root );
  }
  return 0;
}

int findCmd(char *cmd_str)
{
    int i, num;
   /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];
    char *peice;
    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
    for(i=0;i<50;i++)
    {
      currentDirectory[i]='\0';
    }
    getcwd(currentDirectory,50);
    // go through and find the command 
    // then execute them
    if(strcmp(token[0],"history")==0)
    {
      
      for (i=0;i<50;i++)
      {
        if (i==histNum-1)
	{
          break;
        }
        printf("%d: %s\n",i+1, history[i] );
      }

    }
    else if(strcmp(token[0] ,"cd")==0)
    {
      cdCommand(token[1]);
    }
   
    else if(strcmp(token[0] ,"listpids")==0)
    {
      printf("listpids Command");
      listPids();
    }
     else if(strcmp(token[0],"bg")==0)
     {
       sigStuff(1);
    }
    else if(strcmp(token[0],"exit")==0)
    {
      return -1;
      
    }
     else if(strcmp(token[0],"quit")==0)
     {
      return -1;
      
    }
  
    else if(strcmp(token[0],"cltrl-z")==0)
    {
       sigStuff(2);
      
    }
     else if(*token[0]=='!')
     {
       peice=strtok(token[0],"!");
       num=atoi(peice);
        if(num>histNum)
	{
          printf("Command not in history.");
        }
        else
	{
          findCmd(history[num-1]);
        }  
      
    }
   else if(strcmp(token[0],"ctrl-c")==0){
     sigStuff(3);
      
    }
    else
    {

      execFunc(token[0], token);
      
    }
 
return 0;
}
/*
This function finds the comman
*/
 void execFunc( char *cmd, char** args)
 {

  int token=0,i;
  pid_t child_pid;
  int status, retval=0;
  char *usrDir="usr/bin/\0";
  char *binDir="/bin/\0";
  char *localDir="usr/local/bin/\0";
  char command[10], path[40];
  //save this pid
  pastpids[numpids%15]=(int)child_pid;
  numpids++;

  for(i=0;i<10;i++)
  {
    command[i]='\0';
    path[i]='\0';
    path[10+i]='\0';
    path[20+i]='\0';
    path[30+i]='\0';
  } 
  // create the paths
  //one by one check to see if 
  //the command is in the directory


  strcpy(command,cmd);
  strcpy(path, localDir);
  strcat(path, command);

  child_pid = fork();

    if( child_pid == 0 )
    {
       retval=execvp(path, args  );
       if(retval<0){
        for(i=0;i<10;i++)
	{
      
           path[i]='\0';
          path[10+i]='\0';
          path[20+i]='\0';
         path[30+i]='\0';
        }
        strcpy(path,usrDir);
        strcat(path, command);


         retval=execvp(path, args  );
         if(retval<0)
	 {
          
          for(i=0;i<10;i++)
	  {
            path[i]='\0';
            path[10+i]='\0';
            path[20+i]='\0';
            path[30+i]='\0';
          }
        strcpy(path, binDir);
        strcat(path, command);
          retval=execvp(path,args);
          if(retval<0)
	  {
            printf("%s : Command not found\n", command);
          }
         }

       }  
      exit( EXIT_SUCCESS );

    }

    waitpid( child_pid, &status, 0 );
}

//
//
void cdCommand(char *new)
{

char* temp;
int status,length,i; 

  temp=strtok(new,"\n");
  status= chdir(new);
  //back out of the directory
  if(strcmp(new, "..")==0)
  {
    //sep the last / off
     //currentDirectory=
    length=strlen(currentDirectory);
    i=length-1;
    //cut off the current file path
    while(currentDirectory[i]!='/')
    {
      currentDirectory[i]='\0';
      i--;
    }
    // actually change directory
    chdir(currentDirectory);
  }
  if (status==0)
  {
    // 
    strcat(currentDirectory,new);
  }
  else
  {
      //we didnt find anythinhg
      printf("%s: location not found",new);
  }





}
void listPids()
{

  int i=numpids%15, count=1;
  //print the the pids out
  while(i==(numpids%15 -1) ||i==14)
  {
    printf("%d: %d\n",count, pastpids[i]  );
    i++;
    count++;
  }
  i=0;

 while(i!=numpids%15)
 {
  // print the remaining 
  printf("%d: %d\n",count, pastpids[i] );
  i++;
 count++;
 }
} 
void sigStuff(int choice)
{
  //bg
  if (choice==1)
  {
   

  }
  //C
  else if(choice ==2)
  {
    kill(pastpids[numpids%15],SIGSTOP);

  }
  //Z
  else if(choice==3)
  {
    kill(pastpids[numpids%15],SIGSTOP);


  }



}
