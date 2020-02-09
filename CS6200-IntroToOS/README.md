# CS6200 Introduction to Graduate Operating Systems

This class focused on the history of operating systems and the design choices that greatly advanced the field. The projects I did were
implementations of the more modern advancements that are still standards today.

## Project 1: Multithreading

The focus was on multithreading using both pthread locks and condition variables with signalling. I implemented a file share server and client
by implementing the GETFILE protocol in C to make and reply to requests. On the server side, I used a boss-worker model to have one thread that handles
requests and put them on a shared queue for the worker threads to pick up when they have completed their previous task. A more detailed README
is found in the "Project 1" folder and is named "readme-student.md".

## Project 2: Multithreading Analysis

This project was less implementation and more analysis. A program was provided to me for both a client and server side file server implementing
the GETFILE protocol where the client makes a variable amount of file requests of various files sizes and the performance of the replies is
measured. I used several provided commandline parameters to adjust the following:

* NUMBER OF WORKER THREADS
	* Self explanatory
* WORKLOAD REQUEST PATTERN
	* Either fixed file (same file requested everytime), fixed size (different files but similar sizes), or mixed files (different files and sizes)
* WORKLOAD REQUEST SIZE
    * Size used for fixed file or fixed size
* WORKLOAD REQUEST RANGE
	* Minimum and maximum file size when using mixed files

I was instructed to come up with 4 parameter sets that I believed best tested as many aspects of the GETFILE implementation as possible, run those
tests, and write up my results in a scientific report which can be found in the folder "Project 2" and is named "Project2.pdf"