#include <iostream>
#include <vector>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <errno.h>
#include <locale>
#include <pwd.h>
#include <grp.h>
#include <iomanip>

using namespace std;
void outputColor(string files,struct stat s)
{
	//Hidden Dir
	if(files[0] == '.' && (s.st_mode & S_IFDIR))
	{
		printf("%c[%dm",0x1B,100);
		printf("%c[%dm", 0x1B,94);
		cout << files;
		printf("%c[%dm",0x1B,0);
	}
	//Hidden exec
	else if(files[0] == '.' && (s.st_mode & S_IXUSR))
	{	
		printf("%c[%dm",0x1B,100);
		printf("%c[%dm", 0x1B,92);
		cout << files;
		printf("%c[%dm",0x1B,0);
	}
	//Hidden
	else if(files[0] == '.')
	{
		printf("%c[%dm", 0x1B,100);
		cout << files;
		printf("%c[%dm",0x1B,0);
	}
	//Dir
	else if(s.st_mode & S_IFDIR)
	{	
		printf("%c[%dm", 0x1B,94);
		cout << files;
		printf("%c[%dm",0x1B,0);
	}
	//exec
	else if(s.st_mode & S_IXUSR)
	{
		printf("%c[%dm", 0x1B,92);
		cout << files;
		printf("%c[%dm",0x1B,0);
	}
	//standard
	else
	{
		cout << files;
	}
}

void lArg(struct stat files)
{
	if(files.st_mode & S_IFDIR)
		cout << 'd';
	else if(!(files.st_mode & S_IFLNK)) 
	{
		cout << 'l';
	}
	else 
		cout << '-';
	printf((S_IRUSR & files.st_mode) ? "r" : "-");
	printf((S_IWUSR & files.st_mode) ? "w" : "-");
	printf((S_IXUSR & files.st_mode) ? "x" : "-");
	printf((S_IRGRP & files.st_mode) ? "r" : "-");
	printf((S_IWGRP & files.st_mode) ? "w" : "-");
	printf((S_IXGRP & files.st_mode) ? "x" : "-");
	printf((S_IROTH & files.st_mode) ? "r" : "-");
	printf((S_IWOTH & files.st_mode) ? "w" : "-");
	printf((S_IXOTH & files.st_mode) ? "x" : "-");
	cout << ' ' << files.st_nlink << ' ';
	struct passwd* pwd;
	if(!((pwd = getpwuid(files.st_uid)) != NULL))
	{
		perror("Error: getpwuid ");
		exit(1);
	}
	cout << pwd->pw_name << " ";
	struct group* gd;
	if(!((gd = getgrgid(files.st_gid))!= NULL))
	{
		perror("Error: getgrgid ");
		exit(1);
	}
	cout << gd->gr_name << " ";
	cout << setw(5) << right;
	cout << files.st_size << " ";
	char buff[20];
	struct tm* time = localtime(&(files.st_mtime));
	if(!strftime(buff,sizeof(buff), "%b %d %H:%M", time))
	{
		perror("Error: strftime: ");
		exit(1);
	}
	cout << buff << " ";
}
void ls(vector<string> arg, vector<string> dir, string currdir,bool mult)
{
	bool a = false;
	bool l = false;
	bool R = false;
	//flag test
	for(unsigned i = 0; i < arg.size() ; i++)
	{
		for(unsigned j =0; j < arg[i].size() ; j++)
		{
			if(arg[i][j] == '-') {}
			else if(arg[i][j] == 'a')
				a = true;
			else if(arg[i][j] == 'l')
				l = true;
			else if(arg[i][j] == 'R')
				R = true;
			else 
			{
				cerr << "ls: invalid option -- '" << arg[i][j] << "'\n" << "the arguments supported by this program are ('a','l','R')." << endl;
				exit(1);
			} 	
		}
	}
	// Test if parameter is a file
	struct stat fileTest;
	if(stat(currdir.c_str(),&fileTest) == -1)
	{
		perror("stat ");
		exit(1);
	}
	else
	{
		if(!(fileTest.st_mode & S_IFDIR))
		{
			outputColor(dir[0], fileTest);
			cout << endl;
			dir.erase(dir.begin());
			if(dir.size() != 0)
			{
				cout << endl;
				ls(arg,dir,dir[0],mult);
				exit(0);
			}
			exit(0);
		}
		
	}
	// Move files in directory to a vector
	vector<string> files;
	DIR *dirp;
	if (!(dirp = opendir(currdir.c_str())))
	{
		perror("opendir ");
		exit(1);
	}
	dirent *direntp;
	while ((direntp = readdir(dirp)))
	{
		if(errno != 0)
		{
			perror("readdir ");
			exit(1);
		}
		files.push_back(direntp->d_name);	
	}
	if(mult)
	{
		cout << dir[0] << ":" << endl;
	}
	//sort files in bash order
	sort(files.begin(),files.end(),locale("en_US.UTF-8"));
	//get the largest file name
	for(unsigned i=0;i<files.size();i++)
	{
		if(!a && files[i][0] == '.')
		{
			files.erase(files.begin()+i);
			i--;	
		}
	}
	unsigned bigFile = 0;
	for(unsigned i=0;i<files.size();i++)
	{
		if(files[i].size() > bigFile)
			bigFile = files[i].size();
	}
	bigFile = bigFile +2;
	unsigned rowNum = 80/bigFile;
	//ls or ls -a
	if(!l && !R)
	{
	struct stat clr;
		for(unsigned i =0;i < files.size();)
		{
			for(unsigned j=0;j < rowNum; j++,i++)
			{
				if(i <files.size())
				{
					string path = currdir;
					path.append("/");
					path.append(files[i]);
					if((stat(path.c_str(),&clr)) == -1)
					{
						perror("stat ");
						exit(1);
					}
					cout << setw(bigFile) << left;
					outputColor(files[i],clr);
				}
			}
			cout << endl;
		}
	}
	// -al
	struct stat filebuf;
	struct stat blkSize;
	int blocks = 0;
	if(l && !R)
	{
		for(unsigned i=0;i <files.size();i++)
		{
			string path = currdir;
			path.append("/");
			path.append(files[i]);
			if((stat(path.c_str(),&blkSize)) == -1)
			{
				perror("stat ");
				exit(1);
			}
			blocks = blocks + blkSize.st_blocks;	
		}
		cout << "total " <<  blocks/2 << endl;
		for(unsigned i=0;i <files.size();i++)
		{
			string path = currdir;
			path.append("/");
			path.append(files[i]);
			if((lstat(path.c_str(),&filebuf))== -1)
			{
				perror("stat ");
				exit(1);
			}
			lArg(filebuf);
			outputColor(files[i],filebuf);
			cout << endl;
		}
	}	
	vector<string> files2;
	// -alR
	struct stat filebuf2;
	struct stat blkSize2;
	if(R)
	{
		if(currdir == dir[0])
			cout << ".:" << endl;
		if(l)
		{
			for(unsigned i=0;i<files.size();i++)
			{
				string path2 = currdir;
				path2.append("/");
				path2.append(files[i]);
				if((stat(path2.c_str(),&blkSize2)) == -1)
				{
					perror("stat ");
					exit(1);
				}
				blocks = blocks+blkSize2.st_blocks;
			}
			cout << "total " << blocks/2 << endl;
		}
		for(unsigned i=0;i<files.size();)
		{
			for(unsigned j=0;j<rowNum;i++,j++)
			{
				if(i < files.size())
				{
					string path = currdir;
					path.append("/");
					path.append(files[i]);
					if((stat(path.c_str(),&filebuf2)) == -1)
					{
						perror("stat ");
						exit(1);
					}
					if(filebuf2.st_mode & S_IFDIR && (files[i] != "." && files[i] != ".."))
					{
						files2.push_back(files[i]);
					}
					if(!l)
					{
						cout << setw(bigFile) << left;
						outputColor(files[i],filebuf2);
					}
					else
					{
						string path = currdir;
						path.append("/");
						path.append(files[i]);
						if((lstat(path.c_str(),&filebuf2))== -1)
						{
							perror("stat ");
							exit(1);
						}
						lArg(filebuf2);
						outputColor(files[i],filebuf2);
						cout << endl;
					}
				}
			}
			if(!l)
			cout << endl;
		}
		//recursively call all directories that had directories
		if(files2.size() != 0)
		{
			int pathLen = dir[0].size();
			for(unsigned i=0;i<files2.size();i++)
			{
				string path = currdir;
				string pathPrt = currdir;
				pathPrt.erase(0,pathLen);
				path.append("/");
				pathPrt.append("/");
				path.append(files2[i]);
				pathPrt.append(files2[i]);
				cout << endl << "." << pathPrt << ":" <<  endl;
				ls(arg,dir,path,mult);
			}
		}
	}
	closedir(dirp);
	//check is there is second parameter
	if(dir.size() > 1)
	{
		dir.erase(dir.begin());
		cout << endl;
		ls(arg,dir,dir[0],mult);
	}
}
int main(int argc, char* argv[])
{
	vector <string> allArg;
	vector <string> dir;
	vector <string> arg;
	bool yesDir = false;
	//get all Arguments into a vector
	for(int i=0; i < argc-1  ; i++)
	{
		allArg.push_back(argv[i+1]);
		//cout << allArg[i] <<  " ";
	} 
	for(unsigned i=0; i < allArg.size(); i++)
	{	
		if(allArg[i][0]=='-') 
		arg.push_back(allArg[i]);
		else
		{	
			dir.push_back(allArg[i]);
			yesDir = true;
		}
	}
/*	for(int i=0;i < arg.size();i++)	
		cout <<"arg : " <<  arg[i] << endl;
	for(int i=0;i < dir.size();i++)
		cout <<"dir :" << dir[i] << endl;
*/	
	bool mult = false;
	if(!yesDir)
		dir.push_back(".");
	if(dir.size() > 1)
		mult = true;
	
	ls(arg,dir,dir[0],mult);
	return 0;
}
