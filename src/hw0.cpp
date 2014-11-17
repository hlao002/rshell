#include <iostream>
#include <vector>
#include <string.h>
#include <boost/tokenizer.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>
using namespace std;
using namespace boost;

//Replace all connectors with placeholders " , ".
void replaceCntr (string &cmd,string sym,int dNum)
{
        while(cmd.find(sym) != string::npos)
        {
                int found =cmd.find(sym);
                cmd.replace(found, dNum, " , ");
        }
}
void replaceRedir (string &cmd)
{
	bool multIn = false;
	bool multOut = false;
	int in=0;
	int out=0;
	int pipe=0;
	for(int i=0;i < cmd.size();i++)
	{
		string msg;
		if(cmd.compare(i,1,"<")==0)
		{
			in++;
			if(in > 1)
			{
				multIn = true;
				msg = "input";
			}
			cmd.replace(i,1," < ");
			i = i+3;
		}
		else if((cmd.compare(i,1,">")==0)&& (cmd.compare(i+1,1,">")!=0) && (cmd.compare(i-1,1,">")!=0)) 
		{
			out++;
			if(out > 1)
			{
				multOut = true;
				msg = "output";
			}
			cmd.replace (i, 1, " > ");
			i = i+3;
		}
		else if(cmd.compare(i,2,">>")==0)
		{
			out++;
			if(out > 1)
			{
				multOut = true;
				msg = "output";
			}
			cmd.replace(i, 2, " >> ");
			i = i+4;
		}
		else if(cmd.compare(i,1,"|") ==0)
		{
			cmd.replace(i,1," | ");
			i = i+3;
		}
		if(multIn || multOut)
		{
			cout << "Error: This program only supports one instance of " << msg << " redirection." << endl;
			exit(1);
		}
	}
}
int returnIndex(char* Argv[],int size,string sym)
{
	int cnt = -1;
	for (int i =0; i < size; i++)
	{
		cnt = i;
		if(strcmp(Argv[i],sym.c_str())==0)
			return cnt;
		else
			cnt = -1;
	}
	return cnt;
}
void inputRedir(char* Argv[],int index)
{
	int fd = open(Argv[index+1], O_RDONLY);
	if(fd == -1)
	{
		perror("open ");
		exit(1);
	}
	if(close(0) == -1)
	{
		perror("close");
		exit(1);
	}
	if(dup2(fd,0) == -1)
	{
		perror("dup2");
		exit(1);
	}
	char*temp[256];
	for(int i=0;i < index;i++)
	{
		temp[i] = Argv[i];
	}
	temp[index] = NULL;
	if(execvp(Argv[0],temp) == -1)
	{
		perror("execvp ");
		exit(1);
	}
}
void outputRedir(char* Argv[],int index, bool output2)
{
	int fd;
	if(output2)
	{
		fd=open(Argv[index+1],O_WRONLY|O_CREAT|O_APPEND,0777);
		if (fd == -1)
		{
			perror("open ");
			exit(1);
		}
	}
	else
	{
		fd=open(Argv[index+1],O_WRONLY|O_CREAT,0777);
		if (fd == -1)
		{
			perror("open ");
			exit(1);
		}
	}
	if(close(1) == -1)
	{
		perror("close");
		exit(1);
	}	
	if(dup2(fd,1) == -1)
	{
		perror("dup2");
		exit(1);
	}
	char*temp[256];
	for(int i=0;i < index; i++)
		temp[i] = Argv[i];
	temp[index] = NULL;
	if(execvp(Argv[0],temp) == -1)
	{
		perror("execvp ");
		exit(1);
	}
}
void piping(char* Argv[],int index, int size)
{
	char** first;
	first = new char*[256];
	char** second;
	second = new char*[256];
	int end2Index = 0;
	for(int i=0; i < index; i++)
	{
		first[i] = Argv[i];
//		cout << first[i]<< endl;
	}
	first[index] = NULL;
	for(int i= index +1; Argv[i] != '\0'; i++)
	{
		second[end2Index] = Argv[i];
		end2Index++;
//		cout << second[end2Index] << endl;
	}
	second[end2Index] = NULL;
	int fd[2];
	if (pipe(fd) == -1)
	{
		perror ("pipe");
		exit(1);
	}
	int pid = fork();
	
	if (pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else if (pid == 0)
	{
		if(dup2(fd[1],1) == -1)
		{
			perror("dup2");
			exit(1);
		}
		if(close(fd[0]) == -1)
		{
			perror("close");
			exit(1);
		}
		if(execvp(first[0],first)== -1)
		{
			perror("execvp");
		}
		exit(1);
	}
	else
	{
		if(close(fd[1]) == -1)
		{
			perror("close");
			exit(1);
		}

		int stdIn = dup(0);
		if (stdIn == -1)
		{
			perror("dup");
			exit(1);
		}
		if(dup2(fd[0],0)== -1)
		{
			perror("dup2");
			exit(1);
		}
		if(wait(0) == -1)
		{
			perror("wait");
			exit(1);
		}
		int pipe2Index = returnIndex(second,end2Index,"|");
		if(pipe2Index == -1)
		{
			int pid = fork();
			if(pid == -1)
			{
				perror("fork");
			}
			else if (pid == 0)
			{
				if(execvp(second[0],second) == -1)
					perror("execvp");
				exit(1);	
			}
			else
			{
				if(wait(0) == -1)
				{
					perror("wait");
					exit(1);
				}
			}
		}
		else
		{
			piping(second,pipe2Index,end2Index);
		}
		if(dup2(stdIn,0) == -1)
		{
			perror("dup2-1");
			exit(1);
		}
		delete[] first;
		delete[] second;
	}
	
}
void bash()
{
// Getting the hostname
char hostname[128];
gethostname(hostname,sizeof hostname);
	cout << getlogin() << "@" << hostname << "$ ";
        string cmd;
        string connector;
        int status = 0;
        int dNum;
        getline(cin, cmd);
        bool colon = false;
        bool ands = false;
        bool ors = false;
        // Test for comments and delete all lines after "#".
        if (cmd.find("#")!=string::npos)
        {
                int found = cmd.find("#");
                cmd.erase(found,cmd.size()-found);
        }
        //Test for ";".
        if (cmd.find(";")!=string::npos)
        {
                dNum = 1;
                connector = ";";
                colon = true;
        }
        //Test for "&&".
        else if (cmd.find("&&")!=string::npos)
        {
                dNum = 2;
                ands = true;
                connector = "&&";
        }
        //Test for "||".
        else if (cmd.find("||")!=string::npos)
        {
                dNum = 2;
                ors = true;
                connector = "||";
        }
	replaceRedir(cmd);
        //Replace connectors with placeholders.
        if(colon || ands || ors)
	{
                replaceCntr(cmd,connector,dNum);
                while(cmd[cmd.size()-1] == ' ')
              	{  
                                cmd.erase(cmd.size()-1,1);
                }
	}
        vector<string> tok;
        //Tokenizes userinput (now with "," as connector placeholders).
        char_separator<char> sep(",");
        char_separator<char> spac(" ");
        tokenizer<char_separator<char> > toks(cmd, spac);
        //Push tokenized userinput into vector.
        for(tokenizer<char_separator<char> >::iterator it=toks.begin();it!=toks.end();it++)
        {
                tok.push_back(*it);
        }
        //test fior multiple connector error using tokenized input vector.
        for(unsigned k=0;k<tok.size();k++)
        {
                if(tok[0] == "," )
                {
                        cout << "bash: syntax error near unexpected token '" << connector << "'" << endl;
                        bash();
                        exit(0);
                }
                if(k>0 && (tok[k] == "," && tok[k-1] == ","))
                {
                        cout << "bash: syntax error near unexpected token '" << connector << "'" << endl;
                        bash();
                        exit(0);
                }
    	}
        // Tokenizes input with the removal of connectors and spaces.
        tokenizer<char_separator<char> > tokens(cmd, sep);
        tokenizer<char_separator<char> >::iterator it1=tokens.begin();
        for(;it1!=tokens.end();it1++)
        {

                int argvCount = 0;
                int j=0;
                //Count the size of Argv
                tokenizer<char_separator<char> > tokens2(*it1,spac);
                for(tokenizer<char_separator<char> >::iterator itCount=tokens2.begin();itCount!=tokens2.end();itCount++)
                {
                        argvCount++;
                }
                char** Argv = new char*[argvCount+1];
                for(tokenizer<char_separator<char> >::iterator it2=tokens2.begin();it2!=tokens2.end();it2++,j++)
                {
                        //store a command and its arguments into Argv.
                        string placeHolder = *it2;
                        Argv[j] = new char[placeHolder.size()];
                        strcpy(Argv[j],placeHolder.c_str());
                }
                // Test for exit using Argv.
                unsigned shouldExit = 1;
                // strcmp outputs 0 if true. 
                shouldExit = strcmp(Argv[0],"exit");
                if(shouldExit == 0)
                {
                        exit(0);
                }
                Argv[j]= NULL;
		int size = j;
                //fork the function.
                int pid = fork();
                //CHILD!!
                if(pid == -1)
                {
                        //Test for fail forks.
                        perror("fork");
                        delete[] Argv;
                        exit(1);
                }
                else if(pid == 0)
                {
			bool regExec = true;
			int inputRe = returnIndex(Argv,size,"<");
			int outputRe = returnIndex(Argv,size,">");
			int outputRe2 = returnIndex(Argv,size,">>");
			int pipeIndex = returnIndex(Argv,size,"|");
			if(inputRe != -1) 
			{
				regExec = false;
				inputRedir(Argv,inputRe);
			}
			if(outputRe != -1) 
			{
				regExec = false;
				outputRedir(Argv,outputRe,false);\
			}
			else if(outputRe2 != -1) 
			{
				regExec = false;	
				outputRedir(Argv,outputRe2,true);
			}
			if(pipeIndex != -1)
			{
				regExec = false;
				piping(Argv,pipeIndex,size);
			}
                        //Test for fail execvp.
                        if(regExec)
			{
                  	  	if(execvp(Argv[0], Argv) == -1)
	                        {
        	                        perror("execvp");
                	                delete[] Argv;
                        	        exit(1);
      	                	 }
		      	}
                }
                //PARENTS!!!
                else
                {
                        bool cs = true;
			//Update status.
			if (-1 == wait(&status))
			{
				perror("waitpid");
				exit(1);
			}
                        //My child is a failure :[.
                        if(status > 0)
                        {
                                cs = false;
                        }
                        //Test all cases to see if a second command should run  
                        bool cont = false;
                        if((colon) || (cs && ands) || (!cs && ors))
                        {
                                cont = true;
                                continue;
                        }
                        if (cont && (tok[tok.size()-1]== ","))
                        {
                                bash();
                                exit(0);
                        }

                        //If not return to beginning of terminal.
                        else
                        {
                                bash();
                                exit(0);
                        }
                }
        }
}
int main(int argc, char** argv)
{
        bash();
        return 0;
}
