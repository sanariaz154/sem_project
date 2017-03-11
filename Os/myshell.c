#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>                    
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


#define blue  "\033[0;34m"    // for ls output in blue colour for green , set 34 to 32 .
#define none   "\033[0m"        /* to flush the previous colour*/
#define red   "\033[1;31m"        /* for warnings */
#define green  "\033[1;32m"


char* Output="myshell>:";
char* str=" \n";
char* Help="Syntax: exit\nSyntax: cd directory\nSyntax: ls [directory]\nSyntax: del file\nSyntax: ren srcfile dstfile\nSyntax: copy srcfile dstfile\nSyntax: cwd\nSyntax: touch filename\nSyntax: mkdir directoryname\nSyntax: cat filename\n";
#define MaxCommandLine 128
#define MaxParamNumber 2
#define cmdNo 10
char* cmdStr[cmdNo]=
{
	"exit", "cd", "ls", "del", "ren", "copy", "mkdir", "touch", "cwd", "cat"
};


/////////////////////////////////////////

void Exit(char* params[], int paramNumber)
{
	exit(0);
};
void renameFile(char* params[], int paramNumber)  
{
	if (paramNumber!=3)  // ren should have 3 parameters.1 its name (ren) 2 the oldname 3 the newname i.e ren anyfile myfile
		printf(Help);
	else
	{
		if (strcmp(params[1], params[2])==0)    //i.e. ren anyfile anyfile
			printf("%s The source file %s and the destination file %s can not be the same %s.\n ",red, params[1], params[2],none);
		else
		{
			if (access(params[1], F_OK)<0)    // access checks the existence of file error if it returns -ve value 
				printf("%sThe source file %s does not exist. %s\n",red, params[1],none);
			else
			    rename(params[1], params[2]);    // file renamed if no error 			
		}
	}
};
void copyFile(char* params[], int paramNumber)
{
	int status;
	if (paramNumber!=3)  // copy should have 3 parameters.1 its name (copy) 2 the source file 3 the destination file i.e cppy anyfile another
		printf(Help);
	else
	{
		if (fork()==0)
		{
			execv("/bin/cp", params);    //using linux cp command to copy.  the files are in param array
			exit(0);
		}
		else
		wait(&status);
	}
};
void deleteFile(char* params[], int paramNumber)
{
	if (paramNumber!=2)     // del should have 2 parameters. 1 its name (del) 2 the file to delete(anyfile)   i.e del anyfile
		printf(Help);
	else
	{
		if (access(params[1], F_OK)<0)     // to check if file exists or not
			printf("%sThe file %s does not exist%s \n", red,params[1],none);
		else
		{
			if (unlink(params[1])<0)  //unlink - remove a directory entry error if not authorized            
				printf("%scannot delete file %s %s\n",red, params[1],none);
		}
	}
};
void CD(char* params[], int paramNumber)
{
	if (paramNumber!=2)   // cd should have 2 parameters. 1 its name (cd) 2 the directory(dr)   i.e cd dr
		printf(Help);
	else
	{
		if (chdir(params[1])<0)         //change directory to given loTIMEion  error if directory not exist 
			printf("%sThe directory %s does not exist%s\n",red, params[1],none);
	        
	}
};
void dodir(char* path)     // using for ls 
{
	DIR* dp;
	struct dirent* dirnode;
	if ((dp=opendir(path))!=NULL)        //open directory on given path, if no path , open cur. dir
	{
		while ((dirnode=readdir(dp))!=NULL)  // read the content of cur. dir in dp and print
		{
			printf("%s %s %s\n ",blue, dirnode->d_name,none);  //colors 
		}
	}
	else
	   printf("%sThe directory %s does not exist%s \n ",red, path,none);
};
void LS(char* params[], int paramNumber)
{
	if (paramNumber!=1 && paramNumber!=2)   // ls could take a directory parameter as well  ie "ls dr"  or "ls"
		printf(Help);
	
	else
	{	if (paramNumber==1)
			dodir(".");        //  "." represents current directroy
		else
			dodir(params[1]);   // if a directory is given goto that directory
	}
};

void Print()
{	write(STDOUT_FILENO, Output, strlen(Output))!=strlen(Output);   // write the string Output to shell
};


void MKDIR(char* params[], int paramNumber)
{
	if(paramNumber!=2)
        printf(Help);

        else 
    {      if((mkdir(params[1],00777))==-1) 
                        printf("%serror in creating dir%s\n",red,none);

    }

};


void TOUCH(char* params[], int paramNumber)
{
	if(paramNumber!=2)
        printf(Help);

        else 
    {      if((open(params[1], O_RDWR | O_CREAT, 0777))<0) 
                        printf("%serror in creating file, already exists%s\n",red,none);

    }

};


void GetCWD(char* params[], int paramNumber)
{
	if(paramNumber!=1)
        printf(Help);
 char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
     {printf("%s",green);
  fprintf(stdout, "Current working dir: %s\n", cwd);
     printf("%s",none); }
   else
       perror(" error");

 
};

void CAT(char* params[], int paramNumber)
{
	if(paramNumber!=2)
           printf(Help);
   else{ 
    char c[1000];
   FILE *fptr;
   fptr=fopen(params[1],"w");
	   if(fptr==NULL)
	      printf("%sError!%s",red,none);
	    
	   else{
	   printf("%sEnter a sentence:\n%s",green,none);
	   gets(c);
	   fprintf(fptr,"%s",c);
	   fclose(fptr);
	   
	      }
    }
};



void (*cmdArr[cmdNo])(char* params[], int paramNumber)=
{
	Exit, CD, LS, deleteFile, renameFile, copyFile, MKDIR, TOUCH, GetCWD, CAT
};     // commandarray contain all the functions name, the desire functn'll be return along with parameters in (char* params[], int paramNumber)


int CommandOption(char* buf, char* params[], int* paramNumber)    // buf contain our input
{
	int i;
	*paramNumber=0;
	if ((params[*paramNumber]=strtok(buf, str))!=NULL)//breaks string str into a series of tokens using the string str where space  
		{                                                                                   //or \n is found.return NULL if no token left
		for (i=cmdNo-1; i>=0; i--)
		{
			if (strcmp(params[*paramNumber], cmdStr[i])==0)//if inpt is any1 frm commndStr="exit", "cd", "ls","del","ren","copy"
                                     break;	
		}
		//when not found, i==-1
		if (i==-1)
		{
			return i;
		}
	}
	else
	{
		return -1;
	}	
	(*paramNumber)++;
	//the maximum param number is only 2
	while (1)
	{
		if ((params[*paramNumber]=strtok(NULL, str))==NULL)  // when string ends, strtok and break 
		{	break;
		}
		(*paramNumber)++;
		if (*paramNumber==4) //this means the param number is more than 2 and it is wrong
			return -1;
	}

	return i;           //now our input command is in param array whith its parameter numbers
};








////////////////////////////////////////
int main()
{  
      system("clear");
	char buf[MaxCommandLine];    //maxcommandline =128
	int n, paramNumber;
	int commandType;
	char* params[4];
	Print();         // to print output on shell
	while ((n=read(STDIN_FILENO, buf, MaxCommandLine))>0)  //read input from shell if no error in reading
	{     
		buf[n]='\0';
		commandType=CommandOption(buf, params, &paramNumber);
		if (commandType==-1)        //. if there's error like parameternumber are more than 2 etc returned from commandoption 
			printf("%sillegal command%s\n",red,none);
		else
			cmdArr[commandType](params, paramNumber);	// incommandArray[commandType] the function asked to call will be 
          // returned i.e.              exitshell(params, paramNumber);		

		Print();
	}
	return 0;
}
