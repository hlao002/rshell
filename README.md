RSHELL v1.0
---------------------------------------------------------------------------------------------------
by Hon Lao (Henry Lao)


Table of Contents
---------------------------------------------------------------------------------------------------
RSHELL
	-Introduction
	-Installation
	-Program Functions
	-Limitations and Bugs
LS
	-Introduction
	-Installation
	-Program Functions
	-Limitation and Bugs

-RSHELL
---------------------------------------------------------------------------------------------------

Introduction
---------------------------------------------------------------------------------------------------
This program (rshell) is a basic bash command module that is capable of running bash commands founded in /bin(eg. "mkdir", "pwd" , "ls" etc)

All bash commands can be found at (http://ss64.com/bash/)

Installation
--------------------------------------------------------------------------------------------------
To install and execute run rshell in Linux, enter the following commands into the terminal:

        1) git clone http://github.com/hlao002/rshell.git

        2) cd rshell

        3) git checkout hw0

        4) make

        5) bin/rshell

*Note: In order to correctly execute rshell, C++ boost library and g++ compiler must be installed in the OS.

Program Functions
---------------------------------------------------------------------------------------------------
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

Limitations and Bugs
----------------------------------------------------------------------------------------------------
-The bash command "echo" will only print out 1 space between arguments even if more spaces are used.

-Rshell cannot handle multiple types of connectors in a single user input, if more than one connector is used, rshell prioritize commands by (";" >  "&&" > "||"), while treating other connectors as commands.

-Rshell cannot treat "&" and "|" as "&&" and "||", instead, they will be treated like regular arguments
	Example: "echo dogs &&& echo cat" will not result in an error.

-Rshell cannot use the bash command "cd"

LS
-------------------------------------------------------------------------------------------------------

Introduction
-------------------------------------------------------------------------------------------------------
ls is a basic bash command program that replicates the ls bash command, ls is capable of running the flags (-a,-l,-R)

Installation
------------------------------------------------------------------------------------------------------
To install and execute run ls in Linux, enter the following commands into the terminal:

        1) git clone http://github.com/hlao002/rshell.git

        2) cd rshell

        3) git checkout hw1

        4) make

        5) bin/ls

*Note: In order to correctly execute ls, g++ compiler must be installed in the OS.

Program Functions
---------------------------------------------------------------------------------------------------
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



Limitations and Bugs
-------------------------------------------------------------------------------------------------------
-The gray background outputted along with any hidden files will cover the entirety of the file name and proceeding empty space.

-The flags can only be passed in exactly as they are stated, the lowercase of a capitalized flag will not work.

-The program will terminate if the there is a instance of a optional parameter error, and will not continue to the second.

