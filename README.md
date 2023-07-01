
# MyTop

This is a **TOP** (Table of Processes) Program built in C with Terminal UI for monitoring of the various processes and their contributions to the Memory and CPU usage of the machine along with some other vital attributes like Priorities, UpTime, State, and User ID Associated with the Process.

The program is written in **C** and gets information about the Running System and kernel status only by reading files in the hierarchy.  Since the program makes use of only the system files of Linux System, it is designed to work on almost all Linux Platforms.

![MyTop](https://imgur.com/mXTsWcP.png)


## Features

The program reads a number of system files in `/proc/<pid>` of a process to get information about the process like :-


- **pid :**  It stands for Process Id or unique process Id of the task, which wraps periodically, never rebooting at zero.
- **user:** It is the effective user Id (or name) of the owner of the task
- **PR:**  It shows the task's priority.
- **NI:**  It is the task's nice value. A negative NI defines higher priority, and a positive NI defines lower priority.
- **UpTime:**  It shows the CPU time that the task has utilized since it began.
- **S:** The current status of the process (zombie, sleeping, running, uninterruptedly sleeping, or traced). The status of the process can be one of the following:
	-   D:  Uninterruptible sleep
	-   R:  Running
	-   S:  Sleeping
	-   T:  Traced (stopped)
	-   Z:  Zombie
- **VIRT:** How much virtual memory the process is using.
- **RES:** How much physical RAM the process is using, measured in kilobytes.
- **%CPU:** The percentage of the processor time used by the process.
- **%MEM:** The percentage of physical RAM used by the process.
- **Program:**  The name of the Program that is running.

 

Along with these information about individual processes, some information about the system is also provided, which can be used to monitor the overall usage of system resources : 

- UpTime of the system.
- Idle Percentage of the system since start
- Number of Cores and CPU Utilization of each core
- Average CPU utilization percentage
- Main Memory and Swap Memory Used
- Battery Percentage along with Status

Most of the data is colour coded properly to provide the user with better data visualisation. 


### The difference between VIRT and RES

-   VIRT stands for the virtual size of a process, which is the sum of memory it is actually using, memory it has mapped into itself (for instance the video card's RAM for the X server), files on disk that have been mapped into it (most notably shared libraries), and memory shared with other processes. VIRT represents how much memory the program is able to access at the present moment.

-   RES stands for the resident size, which is an accurate representation of how much actual physical memory a process is consuming.


## Current Issues and Future Developments

 1. The program runs indefinitely until the user exits the application and thus itself uses ~100% cpu percentage (of 1 core) while it runs, thereby affecting the actual system stats.
 2. Scrolling over processes list is limited by the display rate of the program. Also, system attributes which are at the top should remain fixed while scrolling past proceses.
 3. Some memory optimisations can be done by extensive use of global variables for display variables.
 4. Program can be made much more interactive by adding features like :-
	 - Sorting and Searching on the processes based on one or more attributes
	 - Actions on processes like priority modification, process kill, etc.
	 - More features can be added like temperatures at different zones and corresponding fan-speeds with some control over them.
 5.  Display of kernel and os information.

![enter image description here](https://imgur.com/XFGCzA0.png)
	

## Installing/Using the program

To use the program, first download the repository by downloading it directly, or clone it using 

>`git clone https://github.com/MrTejas/mytop`

Then run the `top.c` program using the `run.sh` script by first giving it execution permission
> `chmod +x run.sh`

Now run the program
> `./run.sh` 

