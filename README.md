# Basic Process Manager for MacOS 

##Initial Features
* Supports OS X 10.8 and higher
* Displays a list of running processes and applications
* Allows the user to kill any process, including processes they normally wouldn’t be allowed to kill (e.g. processes owned by root)
* Automatically keeps the list updated as processes start and exit
* Reports error messages on NSAlert
* Uses Grand Central Dispatch for asynchronous tasks
* Written in Objective-C, Objective C++, C++14


##Known limitations:
* Only tested on Sierra (10.12) 
* May still need some improvements in the process cleanup/synchronization during exit
