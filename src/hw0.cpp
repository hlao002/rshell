#include <iostream>
#include <vector>
#include <string.h>
#include <boost/tokenizer.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
using namespace std;
using namespace boost;

//Replace all connectors with placeholders " , ".
void replaceCntr (string &cmd,string sym,int dNum)
{
        while(cmd.find(sym) != string::npos)
        {
                int found =cmd.find(sym);
                cmd.replace(found, dNum, ",");
        }
}
void bash()
{
// Getting the hostname
char hostname[128];
gethostname(hostname,sizeof hostname);

//Continue running bash till  "exit" is entered
while(1)
{

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
        //Replace connectors with placeholders.
        if(colon || ands || ors)
        {
                replaceCntr(cmd,connector,dNum);
//              cout << endl;
                while(cmd[cmd.size()-1] == ' '|| cmd[cmd.size()-1] == ',')
                {
                        if(cmd[cmd.size()-1] == ' ')
                                cmd.erase(cmd.size()-1,1);
                        if(cmd[cmd.size()-1] == ',' && cmd[cmd.size()-2] == ',')
                        {
                                cout << "syntax error near unexpected token '" << connector << "'" << endl;
                                bash();
                                exit(0);
                        }
                        if(cmd[cmd.size()-1] == ',')
                                cmd.erase(cmd.size()-1,1);
                        }
                }
//              cout << cmd << endl;    
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
        //test for multiple connector error using tokenized input vector.
        for(unsigned k=0;k<tok.size();k++)
        {
                if(tok[0] == "," )
                {
                        cout << "bash: syntax error near unexpected token '" << connector << "'" << endl;
                        bash();
                        exit(0);
                }
                else if(k>0 && (tok[k] == "," && tok[k-1] == ","))
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
                        //Test for fail execvp.
                        if(execvp(Argv[0], Argv) == -1)
                        {
                                perror("execvp");
                                delete[] Argv;
                                exit(1);
                        }
                }
                //PARENTS!!!
                else
                {
                        bool cs = true;
                        //Update status.
                        wait(&status);
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
}
int main(int argc, char** argv)
{
        bash();
        return 0;

}

