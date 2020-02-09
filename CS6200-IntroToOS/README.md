# CS6200 Introduction to Graduate Operating Systems

This class focused on the history of operating systems and the design choices that greatly advanced the field. The projects I did were
implementations of the more modern advancements that are still standards today.

## Project 1: Multithreading

The focus was on multithreading using both pthread locks and condition variables with signaling. I implemented a file share server and client
by implementing the GETFILE protocol in C to make and reply to requests. On the server side, I used a boss-worker model to have one thread that handles
requests and put them on a shared queue for the worker threads to pick up when they have completed their previous task. A more detailed README
is found in the "Project 1" folder and is named "readme-student.md".

## Project 2: Multithreading Analysis

This project involved less implementation and more analysis. A program was provided to me for both a client and server side file server implementing
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

## Project 3: Shared Memory and Caching

The purpose of this project was to take an existing GETFILE client-server implementation and implement a proxy server that uses a smart cache. The 
smart cache improved the performance of the file transfers but all communicated with the actual server view shared memory accesses. The first task 
was to create a single-threaded solution, and the second task was to create a multithreaded one. A more detailed README is found in the "Project 3" 
folder and is named "readme-student.md".

## Project 4: Remote Procedure Calls

The goal of this project was to implement two processes on the same machine where the first process sends an image to the second process and the 
second process returns a downsampled version of the original image. The process communication was implemented using the Sun RPC library to 
demonstrate the differences in RPC and Message Passing. First, I made a single threaded version of the application and for the second part, I made a
a multithreaded version. A more detailed README is found in the "Project 4" folder and is named "readme-student.md".