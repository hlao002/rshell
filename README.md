## RSHELL v1.1
---------------------------------------------------------------------------------------------------
by Hon Lao (Henry Lao)


###Table of Contents


RSHELL
```
	-Introduction
	-Installation
	-Program Functions
	-Limitations and Bugs
```

RSHELL v1.1
(Redirection & Piping)
```
	-Introduction
	-Program Function
	-Limitations and Bugs
```
RSHELL v1.2
(PATH,Signal Interupt,cd)
```
	-Introduction
	-Program Function
	-Limitations and Bugs
```

LS
```
	-Introduction
	-Installation
	-Program Functions
	-Limitation and Bugs
```

##RSHELL
---------------------------------------------------------------------------------------------------

###Introduction

This program (rshell) is a basic bash command module that is capable of running bash commands founded in /bin(eg. "mkdir", "pwd" , "ls" etc)

All bash commands can be found at (http://ss64.com/bash/)

###Installation

To install and execute run rshell in Linux, enter the following commands into the terminal:

```
	1) git clone http://github.com/hlao002/rshell.git
        2) cd rshell
        3) git checkout hw0
        4) make
        5) bin/rshell
```

*Note: In order to correctly execute rshell, C++ boost library and g++ compiler must be installed in the OS.

### Program Functions

-Rshell supports all bash commands and its respective arguments found in /bin.

        Example: ls -a -b

-Multiple commands can be executed by using the supported connectors.

-Rshell supports the ";", "&&" and "||" connectors.

        Example: ls -a -b && echo pie

-The ";" connector will always execute the next command.

-The "&&" connector will execute the next command only if the current command executed successfully.

-The "||" connector will execute the next command only if the current command failed to execute.

-Comments can be inserted into the input by the "#" symbol, which will cause all consecutive inputs to be ignored by rshell.

        Example: In the command "echo cake # the cake is a lie" all input after the "#" sign will be ignored.

-To exit rshell, enter "exit" as a command.

-If a connector precedes "exit", "exit" will only be executed if the connector requirements are reached.

        Example: "echo cat || exit" will not execute the exit command.

-If the the userinput contains empty commands in between connectors, rshell will not execute any of the commands.

        Example: "echo cat ; ;" will not execute echo cat and an error will be printed out.

-If the first command of the userinput is a connector, rshell will not execute any proceeding commands and will return an error.

        Example: "; echo cat" will cause rshell to print out an error.

###Limitations and Bugs
-The bash command "echo" will only print out 1 space between arguments even if more spaces are used.

-Rshell cannot handle multiple types of connectors in a single user input, if more than one connector is used, rshell prioritize commands by (";" >  "&&" > "||"), while treating other connectors as commands.

-Rshell cannot treat "&" and "|" as "&&" and "||", instead, they will be treated like regular arguments
	Example: "echo dogs &&& echo cat" will not result in an error.

-Rshell cannot use the bash command "cd"

##RSHELL v1.1 (Redirection and Piping)
---------------------------------------------------------------------------------------------------

###Introduction

Update v1.1 of Rshell will now include the feature of I/O redirection as well as piping. For additional information on the functionality of I/O redirection: [click here](http://www.gnu.org/software/bash/manual/bashref.html#Redirections). For additional information on the functionality of pipes: [click here](http://unixhelp.ed.ac.uk/CGI/man-cgi?pipe+2).

###Program Functions

-Rshell v1.1 will support input redirection '<', output redirection '>' '>>', and piping '|'.

-The program can only support one instance of each method of redirection, but can support multiple pipes.

-The program **CANNOT** support the connectors of the original Rshell and Rshell v1.1 together.

-The '<' symbol will execute input redirection given the syntax (command '<' file).

-The '>' symbol will execute output redirection (destructive) given the syntax (command '>' file).

-The '>>' symbol will execute output redirection (appending) given the syntax (command '>' file).

-The '|' symbol will execute piping and can be chained multiple times ( file '|' command '|' command '|' command)

-Rshell v1.1 supports a extra function where rshell will let you perform Output Redirection on file descriptor of your choice by placing a number before output redirection symbols.

	Example: g++ main.cpp 2> errors
	Example: g++ main.cpp 2>> errors

-The functions can be use together in a single command. Example: (wc -l < file|command|command >> output.txt)

-If no new features are invoked, rshell v1.1 will execute the same as rshell v1.0.

-Rshell v1.1 will terminate if more than one instance of each method of redirection is passed in.

###Limitations and Bugs

-Rshell v1.1 cannot support instances where I/O redirection symbol is before filename and command.

	Example: < test.txt wc -l

-Rshell v1.1 cannot support here strings.

	Example: cat <<< "Does Not Support"

-Rshell v1.1 cannot support command syntax that are not stated in this README

-Rshell v1.1 cannot support multple instances of each method of redirection.

-Rshell v1.1 cannot support file to file redirection. (Permission Denied Error)

-Rshell will terminate on instances of fail system calls (fork, open, read, dup2, dup, wait)

#RSHELL v1.2 (Path,Signal Interupt, cd)
------------------------------------------------------------------------------------------------------

###Introduction

Update verison 1.2 of rshell will include, the use of the "PATH" variable to perform execv, ^C interrupt of foreground job, and implimentation of cd command.

###Program Functions

- When executing a bash command, rshell will now search multiple directories with the PATH variable manually to execute "execv" system call. 
*NOTE* - There should be no change to the input and output of rshell.

- ^C will now no longer terminate rshell. Instead, it will interrupt the current foreground process.

- cd is now implimented into rshell.

- cd will change the current directory to "HOME" path if no parameters are passed in.

- cd can be used alongside all features from Rshell v1.0.

	Example: cd (directory) ; echo cat

###Limitations and Bugs

- ^C will be printed out when using the ^C feature.

- More than one parameter being passed into cd will result in an error.


#LS
-------------------------------------------------------------------------------------------------------

###Introduction

ls is a basic bash command program that replicates the ls bash command, ls is capable of running the flags (-a,-l,-R)

###Installation

To install and execute run ls in Linux, enter the following commands into the terminal:

        1) git clone http://github.com/hlao002/rshell.git

        2) cd rshell

        3) git checkout hw1

        4) make

        5) bin/ls

*Note: In order to correctly execute ls, g++ compiler must be installed in the OS.

###Program Functions

-ls supports the flags -l,-a,-R as well as multiple optional parameters

-The flags and optional parameters can be inputed into the console in any order, as long as it is preceeded by a '-'

	Example: bin/ls -alR, bin/ls parameter -lRa, bin/ls -la parameter.

-The -a flag will output all files, including hidden ones.

-The -l flag will output the files in a long listing format.

-The -R flag will output the all files, as well as all files within those files.

-The program will terminate after initial iteration

-If no optional parameters are passed in, ls will output the files in the current directory

-If a incorrect optional parameter is passed in, ls will terminate.

-All hidden files will be printed with a gray background, all directories will be printed in blue font, and all executables will be printed in green font.



###Limitations and Bugs

-The gray background outputted along with any hidden files will cover the entirety of the file name and proceeding empty space.

-The flags can only be passed in exactly as they are stated, the lowercase of a capitalized flag will not work.

-The program will terminate if the there is a instance of a optional parameter error, and will not continue to the second.

