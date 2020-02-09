#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
//#include <regex>

#define INSTRUCTION_WIDTH 4

//Operation Delays
#define ADD_DLY 1
#define	SUB_DLY	1
#define MUL_DLY 2
#define LDW_DLY	3
#define	STW_DLY 1
#define MAX_DLY 3

//Functional Unit Counts
#define ALU_COUNT 4
#define MLU_COUNT 2
#define MLD_COUNT 1
#define MST_COUNT 1

//Reservation Table Positions
#define ALU_RT 0
#define MLU_RT 1
#define MLD_RT 2
#define MST_RT 3

std::vector<std::string> prologue;
std::vector<std::string> epilogue;

/******************************************************************************************************************
Author: Chris Church
******************************************************************************************************************/
class Instruction {
	public:
		std::string type;					//Instruction type (add, ldw, etc.)
		std::string operand1;				//First operand register
		std::string operand2;				//Second operand register
		std::string destination;			//Destination register
		int functional_unit;
		int delay;
		
		int local_reservation_table[];		//Functional units needed by the instruction
		
	std::string to_string()
	{
		if(type == "ldw" || type == "LDW" || type == "stw" || type == "STW" || type == "call" || type == "CALL" || type=="mov" || type=="MOV")
		{
			return "\tc0    " + type + " " + destination + " = " + operand1;
		}
		
		return "\tc0    " + type + " " + destination + " = " + operand1 + ", " + operand2;
	}
};

/*
Read instructions from an input file and return all instructions 
*/
std::vector<std::string> readInput(const char* fname)
{
    std::string str;
    std::vector<std::string> instructions;
    
    std::ifstream filestream(fname);
    
    while (std::getline(filestream, str))
    {
        if (epilogue.empty()) {
            std::size_t pos = str.find("\tc0    ");
            if (pos == 0) {
                instructions.push_back(str);
            } else {
                pos = str.find(";;");
                if (pos == 0)
                    instructions.push_back(str);
                else {
                    pos = str.find(".call printf");
                    if (pos == 0) {
                        epilogue.push_back(str);
                    } else {
                        if (!instructions.empty()) {
		            copy(instructions.begin(), instructions.end(), back_inserter(prologue));
                            instructions.clear();
                        }
                        prologue.push_back(str);
                    }
                }
            }
        } else {
            epilogue.push_back(str);
        }
    }
   
   return instructions;
}

/*
Print scheduled VLIW instructions to a file.
*/
void printOutput(const char* fname, std::vector<std::string> scheduledVLIW)
{
    std::ofstream outfile;
    outfile.open(fname);
  
    for (int i = 0; i < prologue.size(); i++)
        outfile << prologue[i] << "\n";

    for (int i = 0; i < scheduledVLIW.size(); i++)
        outfile << scheduledVLIW[i] << "\n";

    for (int i = 0; i < epilogue.size(); i++)
        outfile << epilogue[i] << "\n";

    outfile.close();
}


/***********************************************************
fixInstructions

Author: Chris Church

Removes delimeters and comments from instructions to create
a vector of only the instructions.
************************************************************/
std::vector<std::string> fixInstructions(std::vector<std::string> instructions)
{
	std::vector<std::string> better_instructions;
	int comment_location = 0;
	int instruction_start = 0;
	int counter = 0;
	
	for(int i = 0; i < instructions.size(); i++)
	{	
			counter++;
			
		// If the instruction is an operation, create the mask, and store the operation without the comment
		if(instructions[i].find("c0") != std::string::npos)
		{
			
			//Find where the operation begins and where the comment begins
			comment_location = instructions[i].find("##");
			instruction_start = (instructions[i].find("c0")) + 6;
			
			better_instructions.push_back(instructions[i].substr(instruction_start, comment_location - instruction_start));
		}			
		else
		{
			continue;
		}
	}
	
	return better_instructions;
}

/*************************************************************************
parseInstructions

Author: Chris Church

Stores the operands, destination, and instruction type in an 
array of Instruction objects
*************************************************************************/
int parseInstructions(Instruction *instruction_list, std::vector<std::string> str_instructions)
//std::vector<Instruction> parseInstructions(std::vector<std::string> str_instructions)
{
	//std::vector<Instruction> instruction_list = new Instruction[str_instructions.size()];
	int start;
	int end;
	
	for(int i = 0; i < str_instructions.size(); i++)
	{
		start = 0;
		end = 0;
		
		/* Type */
		end = str_instructions[i].find(" ");
		instruction_list[i].type = str_instructions[i].substr(0, end);
		//instruction_list[i].determineFunctionUnit();
		
		/* Destination */
		start = end + 1;
		end = str_instructions[i].find(" ", start);
		instruction_list[i].destination = str_instructions[i].substr(start, end - start);
		
		/* First Operand */
		start = end + 3;
		end = str_instructions[i].find(" ", start);
		
		//Handles loads and moves/calls where there is only one operand
		if(str_instructions[i].find("[", start) != std::string::npos)
		{
			end = str_instructions[i].size() - 1;
		}
		else if((str_instructions[i].find("(") != std::string::npos) || (str_instructions[i].find("atoi") != std::string::npos))
		{
			end = str_instructions[i].size() - 2;
		}
		
		end -= 1;
		instruction_list[i].operand1 = str_instructions[i].substr(start, end - start);
		
		/* Second Operand, if there is one */
		if(end != str_instructions[i].size() - 2)
		{
			start = end + 2;
			instruction_list[i].operand2 = str_instructions[i].substr(start, str_instructions[i].size());
		}
		
		//instruction_list[i].to_string();
	}

	return 0;
	//return instruction_list;
}

/*****************************************************************************
 checkMemSyntax
 
 Author: atam8
 
 Helper function for checkDependency
 ****************************************************************************/
bool checkMemSyntax(std::string str)
{
	//std::regex memOffset ("0x[a-zA-Z0-9][a-zA-Z0-9]\\[$r[0-9]\\.[0-9]\\]");
	//std::regex memOffset ("\\[$r[0-9]\\.[0-9]\\]"); //[$r#.#] pattern
	std::string memOffset ("[$r"); 
	//std::cout << "\n" << str; //debug
	//if std::regex_search(str, memOffset)
	if (str.find(memOffset)!=std::string::npos)
	{
		//std::cout << "\nRegister offset memory access detected.\n";
		return true;
	}
	else
	{
		//std::cout << "\nMemory access not detected.\n";
		return false;
	}
}

/*******************************************************************************
 checkDependency 
 
 Author: atam8
 
 Function takes two operations as arguments where the first one is assumed to 
 precede the second in the original program
 Returns a boolean describing whether a dependency exists
********************************************************************************/
bool checkDependency(Instruction instr1, Instruction instr2)
{
	bool dependence = false;
	if (checkMemSyntax(instr1.destination)&&(checkMemSyntax(instr2.operand1)||checkMemSyntax(instr2.operand2)))
		dependence = true;  //memory RAW dependence
	if (checkMemSyntax(instr2.destination) && checkMemSyntax(instr1.destination))
		dependence = true;  //memory WAW dependence
	if (checkMemSyntax(instr2.destination)&&(checkMemSyntax(instr1.operand1)||checkMemSyntax(instr1.operand2)))
		dependence = true;  //memory WAR dependence
	
	if (instr2.operand1.find(instr1.destination) != std::string::npos) 
		dependence = true;  //register RAW dependence
	if (instr2.operand2.find(instr1.destination) != std::string::npos)  //operand 2 may have a line terminator character
		dependence = true;  //register RAW dependence
	
	if (instr1.destination.find(instr2.destination) != std::string::npos)
		dependence = true;  //register WAW dependence
	
	if (instr1.operand1.find(instr2.destination) != std::string::npos)
		dependence = true;  //register WAR dependence
	if (instr1.operand2.find(instr2.destination) != std::string::npos)  //operand 2 may have a line terminator character
		dependence = true;  //register WAR dependence
	
	return dependence;
}

/*******************************************************************************************************
markDependencies 

Author: atam8

Takes in pointer to array of Instruction objects and returns dependencies in a vector of integer vectors
Each vector index represents the corresponding instruction number and contains a vector of the instruction
numbers which must precede the instruction at the current index (incoming edges)
*******************************************************************************************************/
std::vector< std::vector<int> > markDependencies(Instruction instructions[], size_t instructionCount)
{
	std::vector<int> blankIntVector;
    std::vector< std::vector<int> > dependencyVector (instructionCount, blankIntVector);
   
	//take each element in instructions vector and build list of instructions referring to its results
	for (std::size_t i = 0; i < instructionCount; i++)
	{
	    for (std::size_t j = i+1; j < instructionCount; j++)
	    {
		    if(checkDependency(instructions[i],instructions[j]))
			dependencyVector[j].push_back(i);		//add former node number as incoming edge for latter node
	    }
	}

   return dependencyVector;
}

/*********************************************************************************************************
createCriticalPaths - function to be called by scheduleVLIW

Author: atam8

Accepts reference to vector of int vectors, which will hold longest critical path for instructions

Creates the longest critical path for each instruction for use by tiebreaking functions.
Creates the delay chain ending on each node given and then pushes back that node's dependency.
If there are multiple dependencies, the function pushes the instruction's latest dependency by source order 
(most likely to have the most dependencies) onto the critical path, since creating duplicate critical paths 
for all dependencies listed in the untrimmed dependency vector creates an impractical number of paths.
*********************************************************************************************************/
void createCriticalPaths (const std::vector< std::vector<int> > &dependenceVector, size_t instructionCount, std::vector<int> *criticalPaths)
{
	std::vector<int> tempVector; 
	
	//initialize each index with its own instruction
	for(size_t d = instructionCount - 1; d > 0; d--)
	{
		tempVector.push_back(d);
		criticalPaths[d] = tempVector;
		tempVector.clear();
	}
	
	for(size_t d = instructionCount - 1; d > 0; d--)  //look at all instructions and place them on correct critical path(s)
	{	
		for(size_t p = instructionCount - 1; p > 0; p--) //for critical paths with this node, add its dependencies
		{
			if(criticalPaths[p].back() == d)
			{
				if(dependenceVector[d].size() > 0)
					criticalPaths[p].push_back(dependenceVector[d].back()); 
				//the instruction with the latest source order most likely has the most dependencies
				else std::cout << "\nNo dependencies for node " << d;
			}	
		}
	}
	
}

/*********************************************************************************************************
pathRemaining - helper function

Author: atam8

Called by scheduleVLIW to prepare delay lengths for mostCriticalPath tiebreaker
Accepts instructions, longest critical paths and an instruction number 
Sums the delay length of the instruction and all others following it in its longest critical path
*********************************************************************************************************/
int pathRemaining (Instruction instructions[], std::vector<int> *criticalPaths, size_t instructionCount, int instrNum)
{
	int maxPathRem = 0;
	int pathRem = 0;
	int pathPoint = 0;
	//std::cout << "\nPath remaining invoked";
	//search dependency vectors which contain the instruction  
	//return the length of the longest path after the instruction

	for (size_t i = 0; i < instructionCount; i++) //check each delay path
	{
		pathPoint = 0;
		pathRem = 0;
		for (size_t j = 1; j < criticalPaths[i].size(); j++) 
		{
			if(criticalPaths[i][j]==instrNum)
			{
				pathPoint = j;
				//std::cout << "\nNode " << instrNum << " found at index " << j;
				break;
			}
		}
		//sum delays for the instruction of interest and all occurring after it
		for (size_t k = 0; k < pathPoint; k++)
		{
			pathRem += instructions[criticalPaths[i][k]].delay;
		}
		if(pathRem > maxPathRem)
			maxPathRem = pathRem;
	}
	return maxPathRem;
}

/*********************************************************************************************************
mostCriticalPath - Tiebreaker function

Author: atam8

Accepts vector of all computed delay paths, and a vector of the instructions to be considered
Returns vector of the instruction(s) contained in the longest dependency path(s)
*********************************************************************************************************/
std::vector<int> mostCriticalPath(int *criticalLengths, const std::vector<int> &instrNums)
{
	int longestPath = -1;
	int currentPath;
	std::vector<int> winners; //instruction(s) with longest by critical path(s) to be scheduled or tiebroken by some other means
	
	//find longest path length
	for (size_t i = 0; i < instrNums.size(); i++)
	{
		//std::cout << "\nMost critical path loop invoked";
		if (criticalLengths[i] > longestPath)
			longestPath = criticalLengths[i];
	}
	
	for (size_t j = 0; j < instrNums.size(); j++)
	{
		if (criticalLengths[j] == longestPath)
			winners.push_back(instrNums[j]);
	}
	
	return winners;
}

/********************************************************************************************************
computeCriticalities - helper function

Calculates most highly demanded functional unit which will be passed along to mostResourceful tiebreaker

Author: atam8

-Accepts pointer to array of all instructions to be scheduled
-Reads type of operation in each instruction object and assigns its Instruction object the correct delay
-Computes criticalities for (memory, multiplication, ALU) and returns the integer constant corresponding 
to the most heavily demanded resource
*********************************************************************************************************/
int computeCriticalities (Instruction instructions[], size_t instructionCount)
{
	int memOps = 0;
	int multOps = 0;
	int ALUOps = 0;
	std::vector<double> criticalities (3, 0.0);
	int scarceResource = MLD_RT; 	//default to memory
	
	std::string opType;
	
	for (int i=0; i<instructionCount; i++)
	{
		opType = instructions[i].type;
			//memory load operations
			if ((opType.find("ldw") != std::string::npos) ||  (opType.find("LDW") != std::string::npos) || 
				(opType.find("ldh") != std::string::npos) ||  (opType.find("LDH") != std::string::npos) ||
				(opType.find("ldhu") != std::string::npos) || (opType.find("LDHU") != std::string::npos) ||
				(opType.find("ldb") != std::string::npos) || (opType.find("LDB") != std::string::npos) || 
				(opType.find("ldbu") != std::string::npos) || (opType.find("LDBU") != std::string::npos))
				{
					memOps++;
					instructions[i].functional_unit = MLD_RT;
					instructions[i].delay = LDW_DLY;
				}
			//memory store operations
			if ((opType.find("stw") != std::string::npos) || (opType.find("STW") != std::string::npos) ||
				(opType.find("sth") != std::string::npos) || (opType.find("STH") != std::string::npos) ||
				(opType.find("stb") != std::string::npos) || (opType.find("STB") != std::string::npos))
				{
					memOps++;
					instructions[i].functional_unit = MLD_RT;
					instructions[i].delay = STW_DLY;
				}
			//memory prefetch operations
			if ((opType.find("pft") != std::string::npos) || (opType.find("PFT") != std::string::npos))
				{
					memOps++;
					instructions[i].functional_unit = MLD_RT;
					instructions[i].delay = 2; 
				}
			//multiplication operations
			if ((opType.find("mpyll") != std::string::npos) || (opType.find("MPYLL") != std::string::npos) ||
				(opType.find("mpyllu") != std::string::npos) || (opType.find("MPYLLU") != std::string::npos) ||
				(opType.find("mpylh") != std::string::npos) || (opType.find("MPYLH") != std::string::npos) ||
				(opType.find("mpylhu") != std::string::npos) || (opType.find("MPYLHU") != std::string::npos) ||
				(opType.find("mpylhh") != std::string::npos) || (opType.find("MPYLHH") != std::string::npos) ||
				(opType.find("mpyhhu") != std::string::npos) || (opType.find("MPYHHU") != std::string::npos) ||
				(opType.find("mpyl") != std::string::npos) || (opType.find("MPYL") != std::string::npos) ||
				(opType.find("mpylu") != std::string::npos) || (opType.find("MPYLU") != std::string::npos) ||
				(opType.find("mpyhh") != std::string::npos) || (opType.find("MPYHH") != std::string::npos) ||
				(opType.find("mpyhu") != std::string::npos) || (opType.find("MPYHU") != std::string::npos) ||
				(opType.find("mpyhs") != std::string::npos) || (opType.find("MPYHS") != std::string::npos))
				{
					multOps++;
					instructions[i].functional_unit = MLU_RT;
					instructions[i].delay = MUL_DLY;
				}
			//ALU operations
			if ((opType.find("mov") != std::string::npos) || (opType.find("MOV") != std::string::npos) ||
				(opType.find("add") != std::string::npos) || (opType.find("ADD") != std::string::npos) ||
				(opType.find("addcg") != std::string::npos) || (opType.find("ADDCG") != std::string::npos) ||
				(opType.find("and") != std::string::npos) || (opType.find("AND") != std::string::npos) ||
				(opType.find("andc") != std::string::npos) || (opType.find("ANDC") != std::string::npos) ||
				(opType.find("divs") != std::string::npos) || (opType.find("DIVS") != std::string::npos) ||
				(opType.find("max") != std::string::npos) || (opType.find("MAX") != std::string::npos) ||
				(opType.find("maxu") != std::string::npos) || (opType.find("MAXU") != std::string::npos) ||
				(opType.find("min") != std::string::npos) || (opType.find("MIN") != std::string::npos) ||
				(opType.find("minu") != std::string::npos) || (opType.find("MINU") != std::string::npos) ||
				(opType.find("or") != std::string::npos) || (opType.find("OR") != std::string::npos) ||
				(opType.find("orc") != std::string::npos) || (opType.find("ORC") != std::string::npos) ||
				(opType.find("sh1add") != std::string::npos) || (opType.find("SH1ADD") != std::string::npos) ||
				(opType.find("sh2add") != std::string::npos) || (opType.find("SH2ADD") != std::string::npos) ||
				(opType.find("sh3add") != std::string::npos) || (opType.find("SH3ADD") != std::string::npos) ||
				(opType.find("sh4add") != std::string::npos) || (opType.find("SH4ADD") != std::string::npos) ||
				(opType.find("shl") != std::string::npos) || (opType.find("SHL") != std::string::npos) ||
				(opType.find("shr") != std::string::npos) || (opType.find("SHR") != std::string::npos) ||
				(opType.find("shru") != std::string::npos) || (opType.find("SHRU") != std::string::npos) ||
				(opType.find("sub") != std::string::npos) || (opType.find("SUB") != std::string::npos) ||
				(opType.find("sxtb") != std::string::npos) || (opType.find("SXTB") != std::string::npos) ||
				(opType.find("sxth") != std::string::npos) || (opType.find("SXTH") != std::string::npos) ||
				(opType.find("zxtb") != std::string::npos) || (opType.find("ZXTB") != std::string::npos) ||
				(opType.find("zxth") != std::string::npos) || (opType.find("ZXTH") != std::string::npos) ||
				(opType.find("xor") != std::string::npos) || (opType.find("XOR") != std::string::npos) ||
				(opType.find("cmpeq") != std::string::npos) || (opType.find("CMPEQ") != std::string::npos) ||
				(opType.find("cmpge") != std::string::npos) || (opType.find("CMPGE") != std::string::npos) ||
				(opType.find("cmpgeu") != std::string::npos) || (opType.find("CMPGEU") != std::string::npos) ||
				(opType.find("cmpgt") != std::string::npos) || (opType.find("CMPGT") != std::string::npos) ||
				(opType.find("cmpgtu") != std::string::npos) || (opType.find("CMPGTU") != std::string::npos) ||
				(opType.find("cmple") != std::string::npos) || (opType.find("CMPLE") != std::string::npos) ||
				(opType.find("cmpleu") != std::string::npos) || (opType.find("CMPLEU") != std::string::npos) ||
				(opType.find("cmplt") != std::string::npos) || (opType.find("CMPLT") != std::string::npos) ||
				(opType.find("cmpltu") != std::string::npos) || (opType.find("CMPLTU") != std::string::npos) ||
				(opType.find("cmpne") != std::string::npos) || (opType.find("CMPNE") != std::string::npos) ||
				(opType.find("nandl") != std::string::npos) || (opType.find("NANDL") != std::string::npos) ||
				(opType.find("norl") != std::string::npos) || (opType.find("NORL") != std::string::npos) ||
				(opType.find("orl") != std::string::npos) || (opType.find("ORL") != std::string::npos) ||
				(opType.find("slct") != std::string::npos) || (opType.find("SLCT") != std::string::npos) ||
				(opType.find("slctf") != std::string::npos) || (opType.find("SLCTF") != std::string::npos))
				{
					ALUOps++;
					instructions[i].functional_unit = ALU_RT;
					instructions[i].delay = ADD_DLY;
				}
	}
	
	criticalities[0] = (double)memOps;
	criticalities[1] = (double)(multOps/2.0);
	criticalities[2] = (double)(ALUOps/4.0);
	
	if(criticalities[1]>criticalities[0] && criticalities[1]>criticalities[2])
		scarceResource = MLU_RT;
	else if (criticalities[2]>criticalities[0] && criticalities[2]>criticalities[1])
		scarceResource = ALU_RT;
	
	return scarceResource;
}

/********************************************************************************************************
mostResourceful - Tiebreaker function

Author: atam8

Accepts vector of instruction numbers and int number of most critical resource (calculated by computeCriticalities)
Returns vector of the instruction number(s) matching the resource or all instructions if none match
*********************************************************************************************************/
std::vector<int> mostResourceful(Instruction instructionList[], int valuableResource, const std::vector<int> &instrNums)
{
	std::vector<int> highDemand;
	for (int i = 0; i < instrNums.size(); i++)
	{
		if (instructionList[instrNums[i]].functional_unit == valuableResource)
			highDemand.push_back(instrNums[i]);
	}
	
	if (highDemand.size() == 0) //none matched scarcest resource
		highDemand = instrNums;
	
	return highDemand;
}

/********************************************************************************************************
fanoutWidth - helper function

Author: atam8

Helper function for widestFanout tiebreaker
Accepts vector of all dependencies and the instruction of interest
Returns number of instructions depending on the instruction of interest
*********************************************************************************************************/
int fanoutWidth (const std::vector< std::vector<int> > &dependenceVector, int instrNum)
{
	int fanout = 0;
	for (size_t i=0; i < dependenceVector.size(); i++)
	{
		for (size_t j=0; j < dependenceVector[i].size(); j++)
		{
			if (dependenceVector[i][j]==instrNum)
				fanout++;
		}
	}
	return fanout;
}

/********************************************************************************************************
widestFanout - Tiebreaker function

Author: atam8

Accepts vector of all dependencies (to be passed to helper function fanoutWidth) and vector of instructions
Returns vector of the instruction(s) with highest number of other instructions directly depending on it
*********************************************************************************************************/
std::vector<int> widestFanout (const std::vector< std::vector<int> > &dependenceVector, const std::vector<int> &instrNums)
{	
	int widestFan = -1;
	int currentFan;
	std::vector<int> recordedWidth (instrNums.size(), -1); //keep track of any ties by critical path
	std::vector<int> majorFans;  //instruction(s) with the widest fan-out(s) to be scheduled or tiebroken by some other means
	
	//calculate path lengths and keep track of longest
	for (size_t i = 0; i < instrNums.size(); i++)
	{
		recordedWidth[i] = fanoutWidth(dependenceVector, instrNums[i]);
		if (recordedWidth[i] >= widestFan)
			widestFan = recordedWidth[i];
	}
	
	for (size_t j = 0; j < instrNums.size(); j++)
	{
		if (recordedWidth[j] == widestFan)
			majorFans.push_back(instrNums[j]);
	}
	
	return majorFans;
}

/*********************************************************************************************************
firstSource - Tiebreaker function

Author: atam8

Accepts vector of instruction numbers and returns the first one by order of source program
*********************************************************************************************************/
int firstSource(const std::vector<int> &instrNums)
{
	int winner = -1;
	for (size_t i = 0; i < instrNums.size(); i++)
	{
		if (winner < 0 || instrNums[i] < winner)
			winner = instrNums[i];
	}	
	
	return winner;
}

/***************************************************************************************
topologicalOrdering

Author: Chris Church

Creates the topological ordering for the instructions based on their dependencies using
Kahn's Algorithm
***************************************************************************************/
std::vector<int> topologicalOrdering(int tiebreakMode, Instruction* instructionList, std::vector< std::vector<int> > dependencies, int * criticalLengths, int valuableResource)
{										
	// Declare and Initialize Variables 
	std::vector<int> topological_order;									//Empty list that will contain the sorted elements
	std::vector<int> no_incoming;										//Set of all nodes with no incoming edge
	std::vector<int> current_node;
	int tiebreaker;				
	int erasePoint;
	std::vector<int> tiebreakCandidates;
	
	//Find all nodes with no dependencies
	for(int i = 0; i < dependencies.size(); i++)
	{
		if(instructionList[i].type == "call" || instructionList[i].type == "CALL")
		{
			topological_order.insert(topological_order.begin(), i);
		}
		else if(dependencies[i].empty())
		{
			no_incoming.push_back(i);
		}
	}
	
	// Kahn's Algorithm for finding a topological ordering
	//while there are nodes without dependencies
	while(!no_incoming.empty())
	{
		//Remove node with no dependencies from its current vector to the topological order vector
			
			tiebreakCandidates.clear();
			erasePoint = -1; 		//should throw segmentation fault if not set properly by later loop
			//pass no_incoming vector to appropriate tiebreak functions to get a single instruction
			
			switch (tiebreakMode)
			{
				case 0:  //Source
				{
					tiebreaker = firstSource(no_incoming);
					break;
				}
				case 1:  //Critical path - Source
				{
					tiebreakCandidates = mostCriticalPath(criticalLengths, no_incoming);					
					if (tiebreakCandidates.size() == 1)
						tiebreaker = tiebreakCandidates[0];
					else
						if (tiebreakCandidates.size() > 1)
							tiebreaker = firstSource(no_incoming);
						else 
						{
							tiebreaker = -1;
							std::cout << "\nNo tiebreak candidates supplied by mostCriticalPath.";
						}
					
					break;
				}
				case 2:  //Resource - Source
				{
					tiebreakCandidates = mostResourceful(instructionList, valuableResource, no_incoming);
					if (tiebreakCandidates.size() == 1)
						tiebreaker = tiebreakCandidates[0];
					else
						if (tiebreakCandidates.size() > 1)
							tiebreaker = firstSource(no_incoming);
						else 
						{
							tiebreaker = -1;
							std::cout << "\nNo tiebreak candidates supplied by mostCriticalPath.";
						}
					
					break;
				}
				case 3:  //Fanout - Source
				{
					tiebreakCandidates = widestFanout(dependencies, no_incoming);
					if (tiebreakCandidates.size() == 1)
						tiebreaker = tiebreakCandidates[0];
					else
						if (tiebreakCandidates.size() > 1)
							tiebreaker = firstSource(no_incoming);
						else 
						{
							tiebreaker = -1;
							std::cout << "\nNo tiebreak candidates supplied by mostCriticalPath.";
						}
					
					break;
				}
				case 4:  //Critical Path - Resource - Source
				{
					tiebreakCandidates = mostCriticalPath(criticalLengths, no_incoming);
					if (tiebreakCandidates.size() <= 1)
						tiebreaker = tiebreakCandidates[0];
					else
					{
						tiebreakCandidates = mostResourceful(instructionList, valuableResource, no_incoming);
						if (tiebreakCandidates.size() == 1)
							tiebreaker = tiebreakCandidates[0];
						else
							if (tiebreakCandidates.size() > 1)
								tiebreaker = firstSource(no_incoming);
							else 
							{
								tiebreaker = -1;
								std::cout << "\nNo tiebreak candidates supplied by mostCriticalPath.";
							}
					}
					
					break;
				}
				case 5:  //Critical Path - Fanout - Source
				{
					tiebreakCandidates = mostCriticalPath(criticalLengths, no_incoming);
					if (tiebreakCandidates.size() <= 1)
						tiebreaker = tiebreakCandidates[0];
					else 
					{
						tiebreakCandidates = widestFanout(dependencies, no_incoming);
						if (tiebreakCandidates.size() <= 1)
							tiebreaker = tiebreakCandidates[0];
						else
							if (tiebreakCandidates.size() > 1)
								tiebreaker = firstSource(no_incoming);
							else 
							{
								tiebreaker = -1;
								std::cout << "\nNo tiebreak candidates supplied by mostCriticalPath.";
							}
					}
					break;
				}
				default:
				tiebreaker = -5; //tiebreaking failed
			}
			
			//std::cout << "\nTiebreaker node: " << tiebreaker;
		
		for (size_t e = 0; e < no_incoming.size(); e++)
		{
			if (no_incoming[e] == tiebreaker)
				erasePoint = e;
		}
		
		topological_order.push_back(tiebreaker);
		no_incoming.erase(no_incoming.begin() + erasePoint); //remove added node, (no_incoming.begin() + #)
		
		//For each node with a dependency on the node just added to the topological order, delete that dependency
		for(int i = 0; i < dependencies.size(); i++)
		{
			if(dependencies[i].empty())
			{
				continue;
			}
			
			//Remove dependency
			for(int j = 0; j < dependencies[i].size(); j++)
			{
				if(dependencies[i][j] == topological_order.back())
				{
					dependencies[i].erase(dependencies[i].begin() + j);
					break;
				}
			}
			
			//If the node has no more dependencies, add it to the vector of nodes with no dependencies
			if(dependencies[i].empty())
			{
				//Insert instruction with no dependencies to set of instructions with no dependencies
				no_incoming.push_back(i);
			}
		}
	}
	
	//If there are still dependencies then
	if(topological_order.size() < dependencies.size())
	{
		//return error   (graph has at least one cycle)
		printf("\nError: Cycle Detected in Graph!\n");
		exit(1);
	}
	//Else, print the topological order
	else
	{
		printf("\nTopological Order\n");
		
		for(int i = 0; i < topological_order.size(); i++)
		{
			printf("%d: %d\n", i, topological_order[i]);
		}
		
		//The topologically sorted order
		return topological_order;
	}
}

/*******************************************************************************************
listScheduling

Author: Chris Church

Takes the instructions, dependencies, and topological ordering to create a reservation and 
start creatingthe four wide schedule
*******************************************************************************************/
std::vector<std::string> listScheduling(Instruction *instructions, std::vector<int> topological_order, std::vector<std::vector<int> > dependencies)
{
	
	Instruction current_instruction;
	std::vector<std::string> scheduled_instructions;
	std::vector<int> *schedule;
	std::string type;
	int **reservation_table;
	int *instruction_location;
	int reservation_table_size;
	int delay;
	int earliest_slot;
	int reservation_table_limit;
	int pushed_instructions;
	int lastInstruction = 1;
	
	reservation_table_size = topological_order.size() * MAX_DLY;				//Need to clean out nops at end
	reservation_table = new int*[5];
	schedule = new std::vector<int> [reservation_table_size];
	instruction_location = new int[reservation_table_size];
	instruction_location[0] = 0;
	
	//std::cout << "\nList Scheduling variables initialized.";
	for(int i = 0; i < 5; ++i)
	{
		reservation_table[i] = new int[reservation_table_size];
	}
	
	//std::cout << "\nList Scheduling reservation table created.";
	//Find where each operation should be scheduled 
	for(int i = 1; i < topological_order.size(); i++)
	{
		current_instruction = instructions[topological_order[i]];
		earliest_slot = 1; //spot 0 reserved for call instruction
		
		//Examine the location and type of each dependency to find the earliest slot the current operation can be scheduled
		for(int j = 0; j < dependencies[topological_order[i]].size(); j++)
		{
			//Determine the earliest slot
			if(instruction_location[dependencies[topological_order[i]][j]] + instructions[dependencies[topological_order[i]][j]].delay > earliest_slot)
			{
				earliest_slot = instruction_location[dependencies[topological_order[i]][j]] + instructions[dependencies[topological_order[i]][j]].delay;
			}
		}
		
		if(current_instruction.functional_unit == ALU_RT)
		{
			reservation_table_limit = ALU_COUNT;
		}
		else if(current_instruction.functional_unit == MLU_RT)
		{
			reservation_table_limit = MLU_COUNT;
		}
		else if(current_instruction.functional_unit == MLD_RT)
		{
			reservation_table_limit = MLD_COUNT;
		}
		else if(current_instruction.functional_unit == MST_RT)
		{
			reservation_table_limit = MST_COUNT;
		}
		else
		{
			printf("ERROR: Invalid Type\n");
			exit(1);
		}
		
		//Examine reservation table for slot availability
		for(int k = earliest_slot; k < reservation_table_size; k++)
		{
			//Check if the required functional unit is at capacity, or if the slot has enough instructions
			if(reservation_table[current_instruction.functional_unit][k] != reservation_table_limit && reservation_table[4][k] < INSTRUCTION_WIDTH)
			{
				schedule[k].push_back(topological_order[i]);
				reservation_table[4][k]++;
				reservation_table[current_instruction.functional_unit][k]++;
				//for (int m = 0; m < current_instruction.delay; m++)  //no pipelining
					//reservation_table[current_instruction.functional_unit][k+m]++;
				instruction_location[topological_order[i]] = k;
				if(k > lastInstruction)
					lastInstruction = k;
				break;
			}
		}
	}
	schedule[0].push_back(topological_order[0]); 				//first instruction in topological order should be call
	//std::cout << "\nList Scheduling order determined.";
	//std::cout << "\nReservation Table Size: " << reservation_table_size;
	//std::cout << "\nSchedule size: " << (*schedule).size();
	
	for (int a = 0; a < lastInstruction + 1; a++)
	{
		std::cout << "\nInstruction " << a << ": ";
		for (int b = 0; b < schedule[a].size(); b++)
			std::cout << schedule[a][b] << " ";
	}
	
	for(int m = 0; m < lastInstruction + 1; m++)
	{
		
		while(!schedule[m].empty())
		{
			scheduled_instructions.push_back(instructions[schedule[m].back()].to_string());
			schedule[m].pop_back();
		}
		
		scheduled_instructions.push_back(";;");
	}
	std::cout << "\nList Scheduling complete.\n";
	return scheduled_instructions;
}


/*
Inputs:
    - std::vector<std::string> instructions. The input is a vector of strings. Each
      string in the vector is an instruction in the original vex code.
    - <int> mode: value indicating which heuristic ordering to use
Returns : std::vector<std::string>

The function should return a vector of strings. Each string should be a scheduled instruction or ;; that marks the end of a VLIW instruction word.
*/
std::vector<std::string>  scheduleVLIW(std::vector<std::string> instructions,
                                       int mode)
{
	std::vector< std::vector<int> > instructionDependencies;
	std::vector<int> blankIntVector;
    std::vector<std::string> scheduledVLIW;
	std::vector<std::string> better_instructions;
	std::vector<int> topological_order;
	Instruction *instruction_list;
	int *reservation_table;
	int expensiveResource;	
	std::vector<int> *criticalPathsArray;
	int * criticalLengthsArray;
	std::size_t numberOfInstructions;
	
	better_instructions = fixInstructions(instructions);
	numberOfInstructions = better_instructions.size();
	instruction_list = new Instruction[numberOfInstructions];
	
	//Parsing
	parseInstructions(instruction_list, better_instructions);
	std::cout << "\nInstructions stored to objects.";
	//call computeCriticalities to set functional units for instructions and find most heavily demanded resource
	expensiveResource = computeCriticalities(instruction_list, numberOfInstructions);
	std::cout << "\nCriticalities computed."; 
	//Dependencies
	instructionDependencies = markDependencies(instruction_list, numberOfInstructions);
	printf("\nNumber of Dependencies: %d", instructionDependencies.size());
	
	//entirely for debugging purposes
	
	for (size_t i = 0; i < instructionDependencies.size(); i++)
	{
		std::cout << "\nInstruction #" << i << " has dependencies: ";

		for (size_t j = 0; j < instructionDependencies[i].size(); j++)
			std::cout << instructionDependencies[i][j] << " ";
	}
	
	criticalPathsArray = new std::vector<int> [numberOfInstructions];	
	createCriticalPaths(instructionDependencies, instructionDependencies.size(), criticalPathsArray);
	
	for (size_t a = 0; a < instructionDependencies.size(); a++)
	{
		std::cout << "\nCritical Path " << a << ": ";
		for (size_t b = 0; b < criticalPathsArray[a].size(); b++)
			std::cout << criticalPathsArray[a][b] << " <- ";
	}
	
	criticalLengthsArray = new int[numberOfInstructions];

	for (std::size_t i = 0; i < numberOfInstructions; i++)
	{
		criticalLengthsArray[i] = pathRemaining(instruction_list, criticalPathsArray, numberOfInstructions, i);
		std::cout << "\nLongest Critical Length for node " << i << ": " << criticalLengthsArray[i];
	}
	

	
	//Topological Ordering: Kahn's Algorithm
	topological_order = topologicalOrdering(mode, instruction_list, instructionDependencies, criticalLengthsArray, expensiveResource);
	std::cout << "\nTopological Order complete.";
	//List Scheduling
		//Reservation Tables
		//Tiebreaking
	scheduledVLIW = listScheduling(instruction_list, topological_order, instructionDependencies);
	//std::cout << "\nList Scheduling complete.";
	
	//Final Prep for Output

    return scheduledVLIW;
}

int main(int argc, char *argv[])
{

   if(argc != 4) {
       std::cout << "Invalid parameters \n";
       std::cout << "Expected use ./vliwScheduler ";
       std::cout << "<input file name> <output file name> <mode>\n";
   }
 
   const char* inputFile = argv[1];
   const char* vliwSchedulerOutput = argv[2];
   int mode = atoi(argv[3]);

   std::vector<std::string> instructions;
   std::vector<std::string> scheduledVLIW;
 
   /* Read instructions from the file */
   instructions = readInput(inputFile);

   /* Schedule instructions */
   scheduledVLIW = scheduleVLIW(instructions, mode);

   /* Print scheduled instructions to file */
   printOutput(vliwSchedulerOutput, scheduledVLIW);
   
   return 0;
}
