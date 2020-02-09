# CS6291 Embedded Software Development

This class focused on the history of hardware and instruction set architecture design choices made specifically for embedded systems that increased
performance and/or reduced energy requirements. The projects I did revolved around implementing opposing ideas and measuring their benefits
and consequences. The first of these projects was done as three person teams where we collaborated using GitHub.

## Project 1 part 1: VLIW Scheduling

For this project, we took a set of assembly language instructions and analyzed the critical path of the instructions as well as their length. We then 
calculated the instruction length of the critical path using a four-wide scheduling pipeline using other scheduling techniques include the following:

* No other modifications
* Critical path prioritization with source-order tiebreaker
* Resource focused prioritization with source-order tiebreaker
* Fanout with source-order tiebreaker
* Critical path prioritization with resource tiebreaker
* Critical path prioritization with fanout tiebreaker

Afterwards, we wrote the code for implementing each of the schedulers and ran the code while evaluating the cycles and microseconds required for
execution, and wrote a scientific report on data and our analysis. The report can be found in the folder "Project 1" in a file named "Part1_Report.pdf".

## Project 1 part 2: Dependency Elimination and Speculation	

The focus of this project was on eliminating as many dependencies as possible and performing data speculation. Dependencies were done by eliminating
dependencies through register renaming as to not overwrite data before it is used. Speculation tries to predict if there will be a conflict between
two store instructions or a load and a store instruction. Based on the results, speculation will try to load or store data earlier than traditional
scheduling to increase the parallelization of the program. Together we wrote the code for the renaming and the renaming-with-speculation schedulers.
We then ran the sample program several times for each scheduler and wrote a discussion on the results. The report can be found in the folder 
"Project 1" in a file named "Part2_Report.pdf".

## Project 2: ARM vs. Thumb

In the second project, I was given a few sets programs and compiled them using the ARM and Thumb instruction sets to evaluate the difference between
small and large instruction sets. During the project I had to create maps of the program flow and show what functions the programs spent most of their
time in depending on the compliation mode. This then allowed me to single out specific functions to compile in ARM mode while the others were compiled
in Thumb mode with the idea that frequently called functions will perfom better when compiled using the ARM instruction set. I then ran the programs
under the various compilation methods to evaluate their performance using two different sets of input data and measuring the standard deviation. The
scientific report is in the folder "Project 2" in the file named "cchurch3_report-1.pdf".
