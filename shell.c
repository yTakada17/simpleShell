#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>

#define SIZE 1024
#define MAX_ARGS 32
#define FORE_GROUND 1
#define BACK_GROUND 0

void copyArgs(char str[SIZE], char *params[SIZE]);
void findPath(char *command);
int findAND(char *params[SIZE]);
void removePidFromList(char c[][SIZE], int pid[], int n);

int main() {
  
  int status, i=0,end;
  char back[MAX_ARGS][SIZE]={};
  int backp[MAX_ARGS]={};
  pid_t child;
  printf("\n\t Shell start\n");
  
  while (1) {

    //Initialize
    int bgStatus=FORE_GROUND;
    char cmd[SIZE]={}, tmp[SIZE]={};
    char *args[MAX_ARGS]={NULL};
    //

    printf(">>");
    gets(cmd);
    
    if ((strcmp(cmd, "exit") && strcmp(cmd, "quit")) == 0) {
      break;
    }
    else if (strcmp(cmd, "") == 0) {
      continue;
    }
    else if(strcmp(cmd, "jobs")==0){
      printf("jobs command\n");
      for(i=0;strcmp(back[i], "")!=0;i++){
	printf("[%d]: PID:%d, %s\n", i, backp[i], back[i]);
      }
    }
    else if(strcmp(cmd, "fg")==0){
      printf("fg command\n");
    }
    else {

      //copy command to args
      strcpy(tmp, cmd);

      //add all cmd to jobs list
      i=0;
      while(strcmp(back[i], "")!=0){
	i++;
      }
      strcpy(back[i], cmd);
      //
      copyArgs(tmp, args);
   
      memset(cmd, '\0', SIZE);
      strcpy(cmd, args[0]);
      findPath(cmd);
      bgStatus=findAND(args); 

      //if foreground, remove cmd from jobs list
      if(bgStatus==FORE_GROUND){
	i=0;
	while(strcmp(back[i], "")!=0){
	  i++;
	}
	i--;
	memset(back[i], '\0', SIZE);
      }

      if ((child = fork()) == -1) {
	perror("fork");
	exit(EXIT_FAILURE);
      }
      if (child == 0) {
	//in child
	
	//command	  
	if (execv(cmd, args) == -1) {
	  perror("execv");
	  exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
      }
      else {
	//in parent

	//if background, add pid to backp
	if(bgStatus==BACK_GROUND){
	  i=0;
	  while(backp[i]!=0){
	    i++;
	  }
	  backp[i]=child;
	}

	//If '&' isn't in cmd, do foreground
	//If '&' is in cmd, do nothing
	if(bgStatus==FORE_GROUND){
	  waitpid(child, &status, 0);
	}
	
	end=waitpid(-1,&status,WNOHANG);
	if(end>0){
	  printf("END PID:%d\n", end);
	  removePidFromList(back, backp, end);
	}

      }
    }
  }
  printf("\n\t Shell end.\n");
  
  return 0;
}



void copyArgs(char str[SIZE], char *params[SIZE]){
   
  int i=0;
  char *tmp=NULL;

  tmp=strtok(str, " ");
  params[i]=tmp;
  while(tmp!=NULL){
    tmp=strtok(NULL, " ");
    if(tmp!=NULL){
      i++;
      params[i]=tmp;
    }
  } 
}


void findPath(char *command){
  char longPath[SIZE]={};
    char *shortPath=NULL;
  char tmp[SIZE]={};
  
  strcpy(longPath, getenv("PATH"));
  shortPath=strtok(longPath, ":");
  // 
  strcpy(tmp, shortPath);
  strcat(tmp, "/");
  strcat(tmp, command);

  while(1){
    if(access(tmp, X_OK)==0){
      break;
    }
    shortPath=strtok(NULL, ":");
    if(shortPath!=NULL){
      memset(tmp, '\0', SIZE);
      strcpy(tmp, shortPath);
      strcat(tmp, "/");
      strcat(tmp, command);
    }
  }
  

  memset(command, '\0', SIZE);
  strcpy(command, tmp);
}

int findAND(char *params[SIZE]){

  int i=0;
  while(params[i]!=NULL){
    if(strcmp("&", params[i])==0){
      params[i]=NULL;
      return BACK_GROUND;
    }
    i++;
  }
  return FORE_GROUND;
}

void removePidFromList(char c[][SIZE], int pid[], int n){
  int i=0, j=0;
  char tmp_c[MAX_ARGS][SIZE]={};
  int tmp_p[MAX_ARGS]={};

  for(i=0;i<MAX_ARGS;i++){
    if(n==pid[i]){
      
      for(i=i+1;pid[i]!=0;i++){
	strcpy(c[i-1],c[i]);
	pid[i-1]=pid[i];
      }
      memset(c[i-1],'\0',SIZE);
      pid[i-1]=0;
  
      break;
    }
  }
}
