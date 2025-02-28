** README FOR OPERATING SYSTEMS PROJECT 1 **

-Overview-
These programs fulfill the requirements for the first project of my Operating Systems class. MultiThreading.c is a multi-threaded program which simulates a store's activities for the day. The threads function as outgoing deliveries and incoming shipments of products. The store stock and the net profit function as global variables and shared resources for the threads to use. When deliveries go out, the store stock decreases and the net profit increases. When shipments come in, the store stock increases and net profit decreases. IPC.c is a program which showcases inter-process communication by function as a parent process for MultiThreading.c. IPC.c generates an end-of-day report showing the number of failed deliveries and successful deliveries. 

-Instructions-
Ensure that you are running a linux environment before attempting to compile these programs as they use linux-specific libraries. 

To run the programs, you must first compile them in some way. It is recommended to use gcc and run the command "gcc [program name] -o [program name]" in the terminal (replace the program name with whichever program you want to compile).

Once compiled, you can run the program from the terminal by navigating to the correct directory and running the command "./[program name] [arg]" (replace the program name with whichever program you want to run and insert an integer for the argument which determines the number of outgoing delivery trucks). There are 35 shipments in a day and each delivery truck has 10 deliveries to make with a varying number of products in each, so the number of delivery trucks you specify will determine if all shipments for the day finish or not.
