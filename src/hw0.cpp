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
#include <stdlib.h>
using namespace std;
using namespace boost;
void bash();
void sigInteruptC(int i)
{
	signal(SIGINT,SIG_IGN);
}
//Replace all connectors with placeholders " , ".
void replaceCntr (string &cmd,string sym,int dNum)
{
        while(cmd.find(sym) != string::npos)
        {
                int found =cmd.find(sym);
                cmd.replace(found, dNum, " , ");
        }
}
void replaceRedir (string &cmd,int& num,char& numChar)
{
	bool multIn = false;
	bool multOut = false;
	int in=0;
	int out=0;
	for(int i=0;(size_t)i < cmd.size();i++)
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
			if(isdigit(cmd[i-1]))
			{
				i--;
				num = cmd.at(i) - '0';
				numChar = cmd.at(i);
				string temp="";
				temp.append(" ");
				temp.push_back(numChar);
				temp.append("> ");
				cmd.replace(i,2,temp);
				i=i+4;
			}
			else
			{
				cmd.replace (i, 1, " > ");
				i = i+3;
			}
		}
		else if(cmd.compare(i,2,">>")==0)
		{
			out++;
			if(out > 1)
			{
				multOut = true;
				msg = "output";
			}
			if(isdigit(cmd[i-1]))
			{
				i--;
				num = cmd.at(i) - '0';
				numChar = cmd.at(i);
				string temp="";
				temp.append(" ");
				temp.push_back(numChar);
				temp.append(">> ");
				cmd.replace(i,3,temp);
				i=i+5;
			}
			else
			{
				cmd.replace (i, 2, " >> ");
				i = i+4;
			}
		}
		else if(cmd.compare(i,1,"|") ==0)
		{
			cmd.replace(i,1," | ");
			i = i+3;
		}
		if(multIn || multOut)
		{
			cout << "Error: This program only supports one instance of " << msg << " redirection." << endl;
			bash();
			exit(0);
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
	Argv[index] = 0;
	int fd = open(Argv[index+1], O_RDONLY);
	if(fd == -1)
	{
		perror("open");
		bash();
		exit(0);
	}
	if(dup2(fd,0) == -1)
	{
		perror("dup2");
		bash();
		exit(0);
	}
}
void outputRedir(char* Argv[],int index, bool output2,int num)
{
	Argv[index] = 0;
	int fd;
	if(output2)
	{
		fd=open(Argv[index+1],O_WRONLY|O_CREAT|O_APPEND,0777);
		if (fd == -1)
		{
			perror("open ");
			bash();
			exit(0);
		}
	}
	else
	{
		fd=open(Argv[index+1],O_WRONLY|O_CREAT|O_TRUNC,0777);
		if (fd == -1)
		{
			perror("open");
			bash();
			exit(0);
		}
	}
	if(num == -1)
	{
		if(dup2(fd,1) == -1)
		{
			perror("dup2");
			bash();
			exit(0);
		}
	}
	else
	{
		if(dup2(fd,num) == -1)
		{
			perror("dup2");
			bash();
			exit(0);
		}
	}
}
void runExecvp(char* Argv[], int size,int num,char numChar)
{
	string numOut = "";
	numOut.push_back(numChar);
	numOut.append(">");
	string numOut2 = "";
	numOut2.push_back(numChar);
	numOut2.append(">>");
	int inIndex = returnIndex(Argv,size,"<");
	int outIndex = returnIndex(Argv,size,">");
	int out2Index = returnIndex(Argv,size,">>");
	int out3Index = returnIndex(Argv,size,numOut);
	int out4Index = returnIndex(Argv,size,numOut2);
//	cout << numOut << numOut2 << endl;
//	cout << inIndex << outIndex << out2Index << out3Index << out4Index << endl;
	if(inIndex != -1)
	{
		inputRedir(Argv,inIndex);
//		Argv[inIndex] =0;
	}
	if(outIndex != -1)
	{
		outputRedir(Argv,outIndex,false,num);
//		Argv[outIndex]=0;
	}
	else if(out2Index != -1)
	{
		outputRedir(Argv,out2Index,true,num);
//		Argv[out2Index] =0;
	}
	else if(out3Index != -1)
	{
		outputRedir(Argv,out3Index,false,num);
	}
	else if(out4Index != -1)
	{
		outputRedir(Argv,out4Index,true,num);
	}
	string path = getenv("PATH");
	if(path.empty())
		perror("getev");
	vector<string>dirPaths;
	while(!(path.find(":") == string::npos))
	{
		dirPaths.push_back(path.substr(0,path.find(":")));
		path = path.substr(path.find(":")+1);
	}
	int isExec = 0;
	for(unsigned i=0;i<dirPaths.size();i++)
	{
		string temp = Argv[0];
		dirPaths[i].append("/");
		dirPaths[i].append(temp);
			if((isExec = (access(const_cast<char*> (dirPaths[i].c_str()),X_OK)))== 0)
			{
				if(execv(const_cast<char*>(dirPaths.at(i).c_str()),Argv) == -1)
				{
					perror("execv");
					bash();
					exit(0);
				}
			}
	}
	if(isExec == -1)
	{
		perror("access");
		exit(1);
	}
}
void piping(char* Argv[], int size, int num,char numChar)
{
	char* first[256];
	char* second[256];
	int end2Index = 0;
	int index = returnIndex(Argv,size,"|");
	if(index == -1)
	{
		runExecvp(Argv,size,num,numChar);
	}
	else
	{
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
//			cout << second[end2Index] << endl;
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
			runExecvp(first,index,num,numChar);
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
			piping(second,end2Index,num,numChar);
			if(dup2(stdIn,0) == -1)
			{
				perror("dup2");
				exit(1);
			}
		}
	}
}
void bash()
{
signal(SIGINT,sigInteruptC);
// Getting the hostname
char hostname[128];
gethostname(hostname,sizeof hostname);
	cout << getlogin() << "@" << hostname << "$ ";
        string cmd;
        string connector;
        int status = 0;
        int dNum;
	char numChar = '\0';
	int num = -1;
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
	for(unsigned i=0;i < cmd.size();i++)
	{
		if(cmd[i] == ' ')
		{
			cmd.erase(i,1);
			i--;
		}
		else
			break;
	}
	if (cmd.size()== 0)
	{
		bash();
		exit(0);
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
	replaceRedir(cmd,num,numChar);
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
			if(placeHolder.size() == 0)
			{
				bash();
				exit(0);
				
			}
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
		if(strcmp(Argv[0],"cd")== 0)
		{
			if(j == 1)
			{
				cout << "running cd" << endl;
				char *home = getenv("HOME");
				if(chdir(home) == -1) 
					perror("chdir");
			}
			else
			{
				cout << "running cd ." << endl;
				if(-1 == chdir(Argv[1]))
					perror("chdir");
			}
			bash();
			exit(0);
		}  
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
/*			if(strcmp(Argv[0],"cd")== 0)
			{
				if(j == 1)
				{
					cout << "running cd" << endl;
					char *home = getenv("HOME");
					if(chdir(home) == -1) 
						perror("chdir");
				}
				else
				{
					cout << "running cd ." << endl;
					if(-1 == chdir(Argv[1]))
						perror("chdir");
				}
			}
			else
*/				piping(Argv,j,num,numChar);
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
				delete Argv;
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
