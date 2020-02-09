#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>


const int FAV_REG = 3; //First Available Register
const int LAV_REG = 56; //Last Available Register

//Operation Delays
const int MUL_DLY = 2;
const int LDW_DLY = 20; //Load delay 3 or 20 cycles************************
const int OTH_DLY = 1; //All other delays are 1

//Functional Unit Counts
const int ALU_COUNT = 4; //Number of ALU
const int MLU_COUNT = 2; //Number of Multiplication
const int LDW_COUNT = 1; //Number of Memory Load
const int STW_COUNT = 1; //Number of Memory Store
const int INS_WIDTH = 4; //Max Width of istructions

std::vector<std::string> prologue;
std::vector<std::string> epilogue;

/***********************************************************
class VLIWNode

Author: Victor Keyes

For storing all the information used in scheduling, 
renaming registers, and speculating loads.
************************************************************/
class VLIWNode
{
public:
	int nodeLine;
	std::string nodeUnit;					// "ALU", "MLU", "STW", "LDW"
	int nodeLatency;						//nodeLatency of this instruction
	int nodeCriticality;
	int nodeFanOut;
	std::vector<int> supressedLDW;			//Lines of the child Loads where edge will be supressed
	std::vector<int> nodePlace;				//nodePlace of the register in this code line (helpful in register renaming)
	std::vector<std::string> nodeConflict;	//list of what is dependent (register name/ memory location)
	std::vector<std::string> childKinds;	//list of the kinds of dependencies corresponding to childLines (RAW, WAW, WAR, MRAW ...etc.)
	std::vector<int> childLines;			//lines of instructions that are dependent on this instruction
	std::vector<int> childPlace;			//The Children of this node
	std::vector<int> parentLines;			
	int supressedSTW;						//Line where the parent STW will be supressed


	VLIWNode();

	VLIWNode(int, std::string, int, int, int, std::vector<int>, std::vector<int>, std::vector<std::string>,
		std::vector<std::string>, std::vector<int>, std::vector<int>, std::vector<int>, int);

	void SetVLIWNode(int, std::string, int, int, int, std::vector<int>, std::vector<int>, std::vector<std::string>,
		std::vector<std::string>, std::vector<int>, std::vector<int>, std::vector<int>, int);

	void ClearVLIWNode();

	void PrintVLIWNode();
};



VLIWNode::VLIWNode()
{
	this->nodeLine = 0;
	this->nodeUnit = "";
	this->nodeLatency = 1;
	this->nodeCriticality = 1;
	this->nodeFanOut = 0;
	this->supressedLDW = {};
	this->nodePlace = {};
	this->nodeConflict = {};
	this->childKinds = {};
	this->childLines = {};
	this->childPlace = {};
	this->parentLines = {};
	this->supressedSTW = 0;
}

VLIWNode::VLIWNode(int nodeLine, std::string nodeUnit, int nodeLatency, int nodeCriticality, int nodeFanOut, std::vector<int> supressedLDW,
	std::vector<int> nodePlace, std::vector<std::string> nodeConflict, std::vector<std::string> childKinds,
	std::vector<int> childLines, std::vector<int> childPlace, std::vector<int> parentLines, int supressedSTW)
{
	this->nodeLine = nodeLine;
	this->nodeUnit = nodeUnit;
	this->nodeLatency = nodeLatency;
	this->nodeCriticality = nodeCriticality;
	this->nodeFanOut = nodeFanOut;
	this->supressedLDW = supressedLDW;
	this->nodePlace = nodePlace;
	this->nodeConflict = nodeConflict;
	this->childKinds = childKinds;
	this->childLines = childLines;
	this->childPlace = childPlace;
	this->parentLines = parentLines;
	this->supressedSTW = supressedSTW;
}

void VLIWNode::SetVLIWNode(int nodeLine, std::string nodeUnit, int nodeLatency, int nodeCriticality, int nodeFanOut, std::vector<int> supressedLDW,
	std::vector<int> nodePlace, std::vector<std::string> nodeConflict, std::vector<std::string> childKinds,
	std::vector<int> childLines, std::vector<int> childPlace, std::vector<int> parentLines, int supressedSTW)
{
	this->nodeLine = nodeLine;
	this->nodeUnit = nodeUnit;
	this->nodeLatency = nodeLatency;
	this->nodeCriticality = nodeCriticality;
	this->nodeFanOut = nodeFanOut;
	this->supressedLDW = supressedLDW;
	this->nodePlace = nodePlace;
	this->nodeConflict = nodeConflict;
	this->childKinds = childKinds;
	this->childLines = childLines;
	this->childPlace = childPlace;
	this->parentLines = parentLines;
	this->supressedSTW = supressedSTW;
}

void VLIWNode::ClearVLIWNode()
{
	this->nodeLine = 0;
	this->nodeUnit = "";
	this->nodeLatency = 1;
	this->nodeCriticality = 1;
	this->nodeFanOut = 0;
	this->supressedLDW = {};
	this->nodePlace = {};
	this->nodeConflict = {};
	this->childKinds = {};
	this->childLines = {};
	this->childPlace = {};
	this->parentLines = {};
	this->supressedSTW = 0;
}


//just prints out the contents of a node in a clean format for bug checking
/***********************************************************
PrintVLIWNode

Author: Victor Keyes

just prints out the contents of a node in a clean format for bug checking (format below)

Line   Unit  Delay  Criticality  Fanout  [ Register Position (in the instruction) ]
										 [ Actual Registers/Memory ops that are in conflict ]
										 [ Type of conflict eg. "MayWAW" ]
										 [ List of children lines ] [ List of supressed children ]
										 [ List of children Register/Mem position ]
										 [ List of parents lines ] [ suppressed parent ]
************************************************************/
void VLIWNode::PrintVLIWNode()
{
	int sizeDepends = this->childLines.size();
	
	std::cout << this->nodeLine;
	if (this->nodeLine < 10) { std::cout << " "; }
	std::cout << "   ";
	std::cout << this->nodeUnit;
	for (int k = this->nodeUnit.length(); k < 6; k++){ std::cout << " "; }
	std::cout << " ";
	std::cout << this->nodeLatency << "    ";
	if (this->nodeLatency < 10) { std::cout << " "; }
	std::cout << this->nodeCriticality;
	if (this->nodeCriticality < 10) { std::cout << " "; }
	if (this->nodeCriticality < 100) { std::cout << " "; }
	std::cout << "   ";
	std::cout << this->nodeFanOut;
	if (this->nodeFanOut < 10) { std::cout << " "; }
	std::cout << "    [ ";
	
	for (int j = 0; j < sizeDepends; j++)
	{
		std::cout << this->nodePlace[j];
		if (j < sizeDepends - 1) { std::cout << ","; }
		for (int k = this->nodeConflict[j].length() - std::to_string(this->nodePlace[j]).length() + 2; k > 0; k--)
		{
			std::cout << ' ';
		}
	}
	std::cout << "]\n\t\t\t      [ ";
	
	for (int j = 0; j < sizeDepends; j++)
	{
		std::cout << this->nodeConflict[j];
		if (j < sizeDepends - 1) { std::cout << ","; }
		std::cout << "  ";
	}
	std::cout << "]\n\t\t\t      [ ";

	for (int j = 0; j < sizeDepends; j++)
	{
		std::cout << this->childKinds[j];
		if (j < sizeDepends - 1) { std::cout << ","; }
		for (int k = this->nodeConflict[j].length() - this->childKinds[j].length() + 2; k > 0; k--)
		{
			std::cout << ' ';
		}
	}
	std::cout << "]\n\t\t\t      [ ";

	for (int j = 0; j < sizeDepends; j++)
	{
		std::cout << this->childLines[j];
		if (j < sizeDepends - 1) { std::cout << ","; }
		for (int k = this->nodeConflict[j].length() - std::to_string(this->childLines[j]).length() + 2; k > 0; k--)
		{
			std::cout << ' ';
		}
	}
	std::cout << "]\t[ ";

	for (int j = 0; j < this->supressedLDW.size(); j++)
	{
		std::cout << this->supressedLDW[j];
		if (j < this->supressedLDW.size() - 1) { std::cout << ", "; }
		std::cout << ' ';
	}
	std::cout << "]\n\t\t\t      [ ";

	for (int j = 0; j < sizeDepends; j++)
	{
		std::cout << this->childPlace[j];
		if (j < sizeDepends - 1) { std::cout << ","; }
		for (int k = this->nodeConflict[j].length() - std::to_string(this->childPlace[j]).length() + 2; k > 0; k--)
		{
			std::cout << ' ';
		}
	}
	std::cout << "]\n\t\t\t      [ ";

	for (int j = 0; j < this->parentLines.size(); j++)
	{
		std::cout << this->parentLines[j];
		if (j < this->parentLines.size() - 1) { std::cout << ", "; }
		std::cout << ' ';
	}
	std::cout << "]\t[ " << this->supressedSTW << " ]\n\n";
}

//Read instructions from an input file and return all instructions
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
printInstructions

Author: Victor Keyes

Function to print clean parsed instructions to the command line for bug fixing purposes
************************************************************/
void printInstructions(std::vector<std::vector<std::string>> instructions)
{
	//just prints things out for bug checking
	for (int i = 0; i < instructions.size(); i++)
	{
		std::cout << i << "\t";
		for (int j = 0; j < instructions[i].size(); j++)
		{
			std::cout << instructions[i][j] << '\t';
			if (j > 0 && instructions[i][j].length() < 8) std::cout << '\t';
		}
		std::cout << '\n';

	}
	std::cout << "\n\n";
}

/***********************************************************
printNodes

Author: Victor Keyes

Function to print the graph of nodes to the command line for bug fixing purposes
************************************************************/
void printNodes(std::vector<VLIWNode> dependencies)
{
	for (int i = 0; i < dependencies.size(); i++) { dependencies[i].PrintVLIWNode(); }
}

/***********************************************************
printVectors

Author: Victor Keyes

For printing vectors of vectors of ints for bug fixing purposes
************************************************************/
void printVectors(std::vector<std::vector<int>> thisVector)
{
	//just prints things out for bug checking
	for (int i = 0; i < thisVector.size(); i++)
	{
		std::cout << "[ ";
		for (int j = 0; j < thisVector[i].size(); j++)
		{
			std::cout << thisVector[i][j];
			if (j < thisVector[i].size() - 1) { std::cout << ", "; }
		}
		std::cout << " ]\n";

	}
	std::cout << '\n';
}

/***********************************************************
parseInstructions

Author: Victor Keyes

Takes vector of strings vliw instructions and takes out extraneous stuff.
Makes a vector of a vector of strings, where each sub vector is an instruction
and each string in the sub vector is a part of the instruction.
************************************************************/
std::vector<std::vector<std::string>> parseInstructions(std::vector<std::string> unclean)
{
	std::vector<std::vector<std::string>> newInstructions;
	std::vector<std::string> part;
	std::string piece;
	int instLine = 0;
	char thisChar = ' ';

	for (int i = 0; i < unclean.size(); i++)
	{
		for (int j = 0; j < unclean[i].length(); j++)
		{
			thisChar = unclean[i][j];
			if ( (thisChar != ' ' && thisChar != '\t' && thisChar != ',' && thisChar != '=')
				|| (piece[0] == '(' && piece[piece.length()-1] != ')') )
			{
				piece = piece + thisChar;
			}
			if ( (piece != "" && piece != "c0" && piece != ";;" && !(piece[0] == '(' && piece[piece.length() - 1] != ')'))
				&& (j == unclean[i].length()-1 || thisChar == ' '))
			{
				part.push_back(piece);
				piece = "";
			}
			if (piece == ";;" || piece == "##")
			{
				j = unclean[i].length();
				piece = "";
			}
			if (piece == "c0") { piece = ""; }
		}
		if (part.size() != 0) { newInstructions.push_back(part); }
		part.clear();
		piece = "";
	}
	return newInstructions;
}

/***********************************************************
sortDependencies

Author: Victor Keyes

Order the dependencies in the vector of VLIWNode A selection sort
************************************************************/
void sortDependencies(std::vector<VLIWNode> &dependencies) 
{
	for (int i = 0; i < dependencies.size(); i++)
	{
		for (int j = 0; j < dependencies[i].childLines.size(); j++)
		{
			int smallest = j;
			for (int k = j + 1; k < dependencies[i].childLines.size(); k++)
			{
				if (dependencies[i].childLines[k] < dependencies[i].childLines[smallest]) { smallest = k; }
			}
			if (dependencies[i].childLines[smallest] < dependencies[i].childLines[j]) 
			{
				std::swap(dependencies[i].nodePlace[j], dependencies[i].nodePlace[smallest]);
				std::swap(dependencies[i].nodeConflict[j], dependencies[i].nodeConflict[smallest]);
				std::swap(dependencies[i].childKinds[j], dependencies[i].childKinds[smallest]);
				std::swap(dependencies[i].childLines[j], dependencies[i].childLines[smallest]);
				std::swap(dependencies[i].childPlace[j], dependencies[i].childPlace[smallest]);
			}
		}
	}
}

/***********************************************************
determineFanOutAndParents

Author: Victor Keyes

Helps with generateNodes. Determines the fan out and
adds the parents of the nodes to dependencies
Also adds supressed stores.
************************************************************/
void determineFanOutAndParents(std::vector<VLIWNode> &dependencies)
{
	int thisFanOut = 0;

	for (int i = 0; i < dependencies.size(); i++)
	{
		for (int j = 0; j < dependencies[i].childLines.size(); j++)
		{
			if (j == 0 || dependencies[i].childLines[j] != dependencies[i].childLines[j - 1])
			{ 
				thisFanOut++;
				dependencies[dependencies[i].childLines[j]].parentLines.push_back(i);
			}
		}
		dependencies[i].nodeFanOut = thisFanOut;
		thisFanOut = 0;

		for (int j = 0; j < dependencies[i].supressedLDW.size(); j++)
		{
			dependencies[dependencies[i].supressedLDW[j]].supressedSTW = i;
		}
	}
}


//helper function for determineCriticality (sorting the paths so we can easily remove duplicates)
void sortPaths(std::vector<std::vector<int>> &paths)
{
	for (int j = 0; j < paths.size(); j++)
	{
		int smallest = j;
		for (int k = j + 1; k < paths.size(); k++)
		{
			if (paths[k].back() < paths[smallest].back()) { smallest = k; }
		}
		if (paths[smallest].back() < paths[j].back())
		{
			std::swap(paths[smallest], paths[j]);
		}
	}
}

//helper function for determineCriticality (finds the path length given a path and VLIWNode)
int findPathLength(std::vector<int> path, std::vector<VLIWNode> dependencies)
{
	int thisPathLength = 0;

	for (int i = 0; i < path.size(); i++)
	{
		thisPathLength += dependencies[path[i]].nodeLatency;
	}

	return thisPathLength;
}

/***********************************************************
determineCriticality

Author: Victor Keyes

For finding the longest path between this node and the end.
Adds up the latency for each operation in the critical path
and this number is the criticality.
************************************************************/
void determineCriticality(std::vector<VLIWNode> &dependencies)
{
	std::vector<std::vector<int>> allPaths;//A vector of vectors that are all possible paths through the nodes
	std::vector<int> tempPath;//A temporary path for use below
	std::vector<std::vector<int>> allLatency;//All the latencies cooresponding to all the paths for calculating paths
	bool foundOne = false;
	int lastNode = dependencies.size() - 1;
	int lastPathNumber;
	//std::vector<std::vector<int>> pathValues;
	int thisCriticality; //will be the criticality of a given path


	allPaths.push_back({ lastNode });
	for (int thisNode = lastNode; thisNode >= 0; thisNode--) //thisNode iterates through dependencies, a vector of VLIWNode from the last element
	{
		foundOne = false;
		lastPathNumber = allPaths.size()-1; //we do this because we will be adding paths and don't want to itterate through the added paths
		for (int thisPath = 0; thisPath < lastPathNumber+1; thisPath++) //thisPath iterates through all the existing paths we have created
		{
			thisCriticality = findPathLength(allPaths[thisPath], dependencies);
			if (thisNode == allPaths[thisPath].back() && dependencies[thisNode].nodeCriticality < thisCriticality)
			{
				dependencies[thisNode].nodeCriticality = thisCriticality;
			}
			if ( thisNode == allPaths[thisPath].back() && dependencies[thisNode].parentLines.size() > 0)
			{
				foundOne = true;
				tempPath = allPaths[thisPath];
				allPaths[thisPath].push_back(dependencies[thisNode].parentLines[0]);
				for (int addPath = 1; addPath < dependencies[thisNode].parentLines.size(); addPath++)
				{
					if (dependencies[thisNode].parentLines[addPath] != dependencies[thisNode].parentLines[addPath - 1])
					{
						allPaths.push_back(tempPath);
						allPaths.back().push_back(dependencies[thisNode].parentLines[addPath]);
					}
				}
				tempPath.clear();
			}
			else if (thisPath == lastPathNumber && !foundOne) //if we've gone through all the paths and didn't find one that started with this nodes line, we push it back with its parents
			{
				if (dependencies[thisNode].parentLines.size() > 0) 
				{
					allPaths.push_back({ thisNode, dependencies[thisNode].parentLines[0] });
					for (int addPath = 1; addPath < dependencies[thisNode].parentLines.size(); addPath++)
					{
						allPaths.push_back({ thisNode, dependencies[thisNode].parentLines[addPath] });
					}
				}
				else
				{
					allPaths.push_back({ thisNode });
				}
			}

		}


		sortPaths(allPaths); //probably not necessary, but why risk it

		/**/
		//want to remove paths that are shorter between two points. Remove (a, b, d) if it's shorter than (a, c, d)
		for (int thisPath = 0; thisPath < allPaths.size(); thisPath++) 
		{
			for (int checkPath = thisPath + 1; checkPath < allPaths.size(); checkPath++)
			{
				if (allPaths[thisPath].back() == allPaths[checkPath].back() && allPaths[thisPath][0] == allPaths[checkPath][0])		//if the first and last elements of two paths are the same
				{
					if (findPathLength(allPaths[thisPath], dependencies) < findPathLength(allPaths[checkPath], dependencies))		//if the path length of one is less than the other
					{
						allPaths.erase(allPaths.begin() + thisPath);
						checkPath = thisPath; //at completion of the loop we'll be back at checkPath = thisPath + 1
					}
					else
					{
						allPaths.erase(allPaths.begin() + checkPath);
						checkPath--; //we've removed a path, so we need to back up one when the loop increments
					}
					
				}
			}
		}


	}
	//printVectors(allPaths);/************************************************/

	//for (int i = 0; i < allPaths.size(); i++) { std::cout << findPathLength(allPaths[i], dependencies) << '\n'; }
	//printVectors(allLatency);/************************************************/
}


/***********************************************************
generateNodes

Author: Victor Keyes

Generates a vector of Nodes with all the information we will 
need to reference for register renaming, speculating, and
scheduling. Calls sortDependencies, determineFanOutAndParents
and determineCriticality functions for those parts.
If our mode is 2 we will supress the conections between the
closest store above a load.
************************************************************/
std::vector<VLIWNode> generateNodes(std::vector<std::vector<std::string>> clean, int mode)
{
	VLIWNode thisNode;
	std::vector<VLIWNode> thisGraph;
	int startThisString;
	int lengthThisString;
	int leftOfEqual = 1;
	bool foundOne = false;
	bool foundAll = false;
	bool foundMemory = false;
	bool foundRead = false;
	bool foundMemRead = false;
	std::string thisRegister;
	std::string thisOffset; //if it is a memory operation thisOffset is the offset, otherwise and empty string.
	std::string checkRegister; //the string we are comparing to
	std::string checkOffset; //the offset we are comparing to

	for (int thisLine = 0; thisLine < clean.size(); thisLine++)
	{
		for (int thisRegLoc = (clean[thisLine].size() - 1); thisRegLoc > 0; thisRegLoc--) // thisLine and thisRegLoc itterate through picking the value to campare against
		{
			if (thisRegLoc != leftOfEqual && clean[thisLine][thisRegLoc] == clean[thisLine][leftOfEqual]) { thisRegLoc--; }//if we are looking at a register being set equal to itself, we skip it and will check it when we get to the other side of the equal sign

			startThisString = 0;
			lengthThisString = clean[thisLine][thisRegLoc].length();
			if (clean[thisLine][thisRegLoc].find('[') != std::string::npos)
			{
				startThisString = clean[thisLine][thisRegLoc].find('[') + 1;
				lengthThisString = lengthThisString - startThisString - 1;
				thisOffset = clean[thisLine][thisRegLoc].substr(0, startThisString - 1);
			}
			else { thisOffset = ""; }
			thisRegister = clean[thisLine][thisRegLoc].substr(startThisString, lengthThisString);


			for (int checkLine = thisLine + 1; checkLine < clean.size(); checkLine++)
			{
				for (int checkRegLoc = (clean[checkLine].size() - 1); checkRegLoc > 0; checkRegLoc--) // checkLine and checkRegLoc itterate through telling us the location of our dependency
				{
					if (checkRegLoc != leftOfEqual && clean[checkLine][checkRegLoc] == clean[checkLine][leftOfEqual]) { checkRegLoc--; }//if we are looking at a register being set equal to itself, we skip it and will check it when we get to the other side of the equal sign
					if (thisRegLoc != leftOfEqual) { checkRegLoc = leftOfEqual; } //if on right side of equals, we don't check the right side further down (this would be a RAR)

					startThisString = 0;
					lengthThisString = clean[checkLine][checkRegLoc].length();
					if (clean[checkLine][checkRegLoc].find('[') != std::string::npos)
					{
						startThisString = clean[checkLine][checkRegLoc].find('[') + 1;
						lengthThisString = lengthThisString - startThisString - 1;
						checkOffset = clean[checkLine][checkRegLoc].substr(0, startThisString - 1);
					}
					else { checkOffset = ""; }
					checkRegister = clean[checkLine][checkRegLoc].substr(startThisString, lengthThisString);


					if (thisOffset != "") //means it's a memory read or write. we want to find both the next possible memory collision and the next register change
					{
						if (checkRegister == thisRegister && checkOffset == thisOffset && !foundMemory) //equal register, equal offset (same memory location)
						{
							if (thisRegLoc == 1 && checkRegLoc > 1) 
							{ 
								thisNode.childKinds.push_back("MustRAW"); 
								foundMemRead = true;
								foundOne = true;
							}
							else if (thisRegLoc == 1 && checkRegLoc == 1) 
							{ 
								if (!foundMemRead) { thisNode.childKinds.push_back("MustWAW"); foundOne = true; }
								foundMemory = true;
							}
							else if (thisRegLoc > 1 && checkRegLoc == 1) 
							{ 
								if (!foundMemRead) { thisNode.childKinds.push_back("MustWAR"); foundOne = true; }
								foundMemory = true;
							}
						}
						else if (checkRegister != thisRegister && checkOffset != "" && !foundMemory) //not equal register, some offset (possible same mem. location)
						{
							if (thisRegLoc == 1 && checkRegLoc > 1) 		//this is a load after a store. We will supress the first one for mode 2
							{
								if (mode == 2)				//If mode is 2 we supress the connection to the closest load
								{
									bool stwBtwn = false;	//Looking for another "stw" between where we are and where we are checking
									for (int btwn = thisLine + 1; btwn < checkLine; btwn++)
									{
										if (clean[btwn][0] == "stw") { stwBtwn = true; }
									}
									if (!stwBtwn) { thisNode.supressedLDW.push_back(checkLine); }
									else
									{
										thisNode.childKinds.push_back("MayRAW");
										foundMemRead = true;
										foundOne = true;
									}
								} 
								else
								{ 
									thisNode.childKinds.push_back("MayRAW");
									foundMemRead = true;
									foundOne = true;
								}
							}
							else if (thisRegLoc == 1 && checkRegLoc == 1) 
							{ 
								if (!foundMemRead) { thisNode.childKinds.push_back("MayWAW"); foundOne = true; }
								foundMemory = true;
							}
							else if (thisRegLoc > 1 && checkRegLoc == 1) 
							{ 
								if (!foundMemRead) { thisNode.childKinds.push_back("MayWAR"); foundOne = true; }
								foundMemory = true;
							}
						}
						else if (checkRegister == thisRegister && checkOffset == "" && checkRegLoc == 1 && !foundAll) //register used for this memory operation is changing
						{
							foundOne = true;
							thisNode.childKinds.push_back("WAR");
							foundAll = true;
						}
					}
					else if (checkRegister == thisRegister && !(thisRegLoc > 1 && checkOffset != ""))//checks non-memory operations
					{
						foundMemory = true;
						if ((thisRegLoc == 1 && checkRegLoc > 1) || (thisRegLoc == 1 && checkRegLoc == 1 && checkOffset != "")) 
						{ 
							thisNode.childKinds.push_back("RAW");  
							foundRead = true;
							foundOne = true;
						}
						else if (thisRegLoc == 1 && checkRegLoc == 1) 
						{ 
							if (!foundRead) { thisNode.childKinds.push_back("WAW"); foundOne = true; }
							foundAll = true;
						}
						else if (thisRegLoc > 1 && checkRegLoc == 1) 
						{ 
							if (!foundRead) { thisNode.childKinds.push_back("WAR"); foundOne = true; }
							foundAll = true;
						}
					}
					if (foundOne)
					{
						thisNode.childLines.push_back(checkLine);
						thisNode.childPlace.push_back(checkRegLoc);
						thisNode.nodePlace.push_back(thisRegLoc);
						thisNode.nodeConflict.push_back(clean[thisLine][thisRegLoc]);
						foundOne = false;
					}
					if ((foundMemory && foundAll) || (checkLine == clean.size() - 1 && checkRegLoc == 1)) //we found both or we are at the end of the line, so stop checking 
					{
						checkLine = clean.size() - 1;
						checkRegLoc = 1;
						foundAll = false;
						foundMemory = false;
						foundRead = false;
						foundMemRead = false;
					}
				}
			}
		}
		
		thisNode.nodeLine = thisLine;

		if (clean[thisLine][0].find("ldw") != std::string::npos) { thisNode.nodeLatency = LDW_DLY; thisNode.nodeUnit = "LDW"; }
		else if (clean[thisLine][0].find("mpy") != std::string::npos) { thisNode.nodeLatency = MUL_DLY; thisNode.nodeUnit = "MLU";}
		else if (clean[thisLine][0].find("stw") != std::string::npos) { thisNode.nodeLatency = OTH_DLY; thisNode.nodeUnit = "STW";}
		else { thisNode.nodeLatency = OTH_DLY; thisNode.nodeUnit = "ALU";}

		thisGraph.push_back(thisNode);
		thisNode.ClearVLIWNode();
	}
	
	sortDependencies(thisGraph);
	determineFanOutAndParents(thisGraph); //add the fan out (number of edges out of each node) and parentLines (list of edges coming into this node)
	determineCriticality(thisGraph);//traverses through all the dependency paths and figures the criticality for each node in the graph adds number to the graph

	return thisGraph;
}


/***********************************************************
renameRegisters

Author: Victor Keyes

Uses the information stored in the vecor of nodes (from
generateNodes) to rename registers. After the registers are
renamed, we will need to re-generate the nodes as the
dependencies will have changed.
************************************************************/
std::vector<std::string> renameRegisters(std::vector<std::vector<std::string>> &instructions, std::vector<VLIWNode> dependencies)
{
	std::vector<std::string> freeRegisters;
	
	//make a vector of possible regisers from $r0.3 to $r0.56
	for (int i = LAV_REG; i > FAV_REG-1; i--) { freeRegisters.push_back("$r0." + std::to_string(i)); }

	//remove all the registers that are already in use.
	for (int thisLine = 0; thisLine < instructions.size(); thisLine++)
	{
		for (int thisRegLoc = 1; thisRegLoc < instructions[thisLine].size(); thisRegLoc++)
		{
			for (int i = freeRegisters.size()-1; i >= 0; i--)
			{
				if (instructions[thisLine][thisRegLoc].find(freeRegisters[i]) < instructions[thisLine][thisRegLoc].length())		//if we find a free register is being used
				{
					freeRegisters.erase(freeRegisters.begin() + i);												//we remove it
					i = 1;																						//since we found this register, we can move on to the next one

				}
			}
		}
	}

	//rename false dependencies (needs a check to make sure we haven't run out of registers)
	for (int i = 0; i < dependencies.size(); i++)
	{
		for (int j = 0; j < dependencies[i].childLines.size(); j++)
		{
			if ((dependencies[i].childKinds[j] == "WAW" || dependencies[i].childKinds[j] == "WAR") && instructions[dependencies[i].childLines[j]][0] != "mov") //if false dependency, rename.
			{
				if ( instructions[i][dependencies[i].nodePlace[j]].find( instructions[ dependencies[i].childLines[j] ][ dependencies[i].childPlace[j] ] ) != std::string::npos ) //if we haven't already renamed this register, then rename it.
				{
					std::string replaceThis = instructions[dependencies[i].childLines[j]][dependencies[i].childPlace[j]];
					instructions[dependencies[i].childLines[j]][dependencies[i].childPlace[j]] = freeRegisters.back(); //rename the first register
					
					for (int k = dependencies[i].childLines[j] + 1; k < instructions.size(); k++) //rename all the other registers
					{
						for (int m = 1; m < instructions[k].size(); m++) //looking through all the registers in all the lines after the first one
						{
							int pointOfReplace = instructions[k][m].find(replaceThis);
							int lengthOfReplace = freeRegisters.back().length();
							std::string thisOffset = instructions[k][m].substr(0, pointOfReplace);

							if (pointOfReplace < instructions[k][m].length())
							{
								if (thisOffset != "") { instructions[k][m] = thisOffset + freeRegisters.back() + ']'; }
								else { instructions[k][m] = freeRegisters.back(); }
							}
						}
					}
					freeRegisters.pop_back(); //remove a register from the list of free ones
				}
				int startLine = dependencies[i].childLines[j];
			}
		}
	}
	return freeRegisters;
}

/***********************************************************
formatInstruction

Author: Victor Keyes

helper function for make4Wide that puts all the bits back together 
for an instruction. They were broken apart in parseInstructions.
"\tc0    " + "add " + "$r0.5 " + "= " + "$r0.5, " + "$r0.3"
************************************************************/
std::string formatInstruction(std::vector<std::string> instruction)
{
	std::string oneString; //to rule them all

	oneString = "\tc0    ";
	for (int i = 0; i < instruction.size(); i++)
	{
		oneString += instruction[i];
		if (i == 1) { oneString += " ="; }
		if (i > 1 && i < instruction.size() - 1) { oneString += ", "; }
		else { oneString += ' '; }
	}

	//std::cout << oneString <<"\n";//for debugging***********************************************************************************************************
	return oneString; //to Mordor
}

/***********************************************************
generateChecks

Author: Victor Keyes

Creates all the verbage for the checks that will be created
if we do speculative loading.
************************************************************/
void generateChecks(std::vector<std::string> store, std::vector<std::string> loadd, 
	std::vector<std::string> &checkInstructions, std::vector<std::string> &penaltyTail, std::vector<std::string> registers, int &labelCounter)
{
	std::string stwOffset = store[1].substr(0, store[1].find('['));		//1
	std::string ldwOffset = loadd[2].substr(0, loadd[2].find('['));		//2
	std::string stwRegister = store[1].substr(store[1].find('[') + 1, store[1].length() - store[1].find('[')-2);	//1
	std::string ldwRegister = loadd[2].substr(loadd[2].find('[') + 1, loadd[2].length() - loadd[2].find('[')-2);	//2
	std::string regOne = registers[0];	//These two registers were unused by any of the instructions, so using them from all our check instrucitons doesn't bother me
	std::string regTwo = registers[1];	
	std::string branch = "$b0.1";

	checkInstructions.push_back("\tc0    add " + regOne + " = " + stwRegister + ", " + stwOffset);
	checkInstructions.push_back("\tc0    add " + regTwo + " = " + ldwRegister + ", " + ldwOffset);
	checkInstructions.push_back("\tc0    cmpne " + branch + " = " + regOne + ", " + regTwo);
	checkInstructions.push_back("\tc0    brf " + branch + ", " + "Label" + std::to_string(labelCounter + 1));
	checkInstructions.push_back(";;");
	checkInstructions.push_back("Label" + std::to_string(labelCounter) + ":");
	
	penaltyTail.push_back("Label" + std::to_string(labelCounter + 1) + ":");
	penaltyTail.push_back(formatInstruction(loadd));
	penaltyTail.push_back("\tc0    goto Label" + std::to_string(labelCounter));
	penaltyTail.push_back(";;");
	
	labelCounter += 2;
}

/***********************************************************
make4Wide

Author: Victor Keyes

Schedules the instructions in 4 wide VEX format. If our supressed
parent is non 0 and our supressed children are non-empty, we
see if we speculated. If we did we transform to a ldw.d and
add all the check instructions.
************************************************************/
std::vector<std::string> make4Wide(std::vector<std::vector<std::string>> &instructions, std::vector<VLIWNode> dependencies, std::vector<std::string> registers)
{
	std::vector<std::string> instructionList;
	std::vector<std::string> checkInstructions;		//the check instructions that will be generated
	std::vector<std::string> penaltyTail;			//all the end instructions that get added... at the end
	
	int ALUs = ALU_COUNT;
	int MLUs = MLU_COUNT;
	int LDWs = LDW_COUNT;
	int STWs = STW_COUNT;
	int labelCounter = 0;			//it counts the labels

	std::vector<int> remaining;		//the instructions that are still not scheduled
	std::vector<int> inProgress;	//instructions currently awaiting their latency to run out
	std::vector<int> canSchedule;	//instructions currently with zero parents
	//int locHighCrit = 0;
	//int locHighFan = 0;

	instructionList.push_back(formatInstruction(instructions[0])); //add the first instruction without scheduling it
	instructionList.push_back(";;");
	for (int i = 1; i < dependencies.size(); i++) { remaining.push_back(i); } //make a list of remaining instructions

	while (remaining.size() > 0)
	{

		for (int i = 0; i < remaining.size(); i++)
		{
			if (dependencies[remaining[i]].parentLines.size() == 0)
			{
				std::string functUnit = dependencies[remaining[i]].nodeUnit;
				if ((functUnit == "ALU" && ALUs > 0) ||
					(functUnit == "MLU" && MLUs > 0) ||
					(functUnit == "LDW" && LDWs > 0) ||
					(functUnit == "STW" && STWs > 0))
				{
					canSchedule.push_back( remaining[i] );
				}
			}
		}

		for (int i = 0; i < INS_WIDTH; i++)			//go through a whole line of instruction
		{
			int willSchedule = 0;
			for (int j = 0; j < canSchedule.size(); j++)
			{
				if (canSchedule.size() > 0)
				{
					if (dependencies[canSchedule[j]].nodeCriticality > dependencies[canSchedule[willSchedule]].nodeCriticality) { willSchedule = j; }
					else if (dependencies[canSchedule[j]].nodeCriticality == dependencies[canSchedule[willSchedule]].nodeCriticality)
					{
						if (dependencies[canSchedule[j]].nodeFanOut > dependencies[canSchedule[willSchedule]].nodeFanOut) { willSchedule = j; }
					}
				}
			}
			if (willSchedule < canSchedule.size())
			{
				if (dependencies[canSchedule[willSchedule]].supressedSTW != 0)//If it's a load and .supressedSTW (the store above) is in remaining turn it into ldw.d
				{
					for (int j = 0; j < remaining.size(); j++)
					{
						if (remaining[j] == dependencies[canSchedule[willSchedule]].supressedSTW) { instructions[canSchedule[willSchedule]][0] = "ldw.d"; }
					}
				}
				else if (dependencies[canSchedule[willSchedule]].supressedLDW.size() > 0)							//We are a store with supressed loads (and everyone knows that's totally fine)
				{
					for (int j = 0; j < dependencies[canSchedule[willSchedule]].supressedLDW.size(); j++)
					{
						if (instructions[dependencies[canSchedule[willSchedule]].supressedLDW[j]][0] == "ldw.d") 	//If we've already speculated a load ahead of this store and thus created a "ldw.d" (a monster)
						{
							generateChecks(instructions[canSchedule[willSchedule]], instructions[dependencies[canSchedule[willSchedule]].supressedLDW[j]], 
								checkInstructions, penaltyTail, registers, labelCounter);
						}
					}
				}
				instructionList.push_back(formatInstruction(instructions[canSchedule[willSchedule]])); //adding this one to the schedule
				std::cout << canSchedule[willSchedule] << '\n';//for debugging*******************************************************************************************************************************
				inProgress.push_back(canSchedule[willSchedule]);								//add this instruction line to the line of in progress
				for (int j = 0; j < remaining.size(); j++)										//remove this instruction from the remaining instructions
				{
					if (remaining[j] == canSchedule[willSchedule])				//if the code line in remaining equals the code line that we are scheduling
					{ 
						remaining.erase(remaining.begin() + j);					//we remove it from the lines we have left remaining
						j = remaining.size() - 1;								//we found what we were looking for, so we can skip to the end of the loop
					}		
				}
				std::string functUnit = dependencies[canSchedule[willSchedule]].nodeUnit;		//remove the functional unit the instruction we added uses
				if (functUnit == "ALU") { ALUs--; }
				else if (functUnit == "MLU") { MLUs--; }
				else if (functUnit == "LDW") { LDWs--; }
				else if (functUnit == "STW") { STWs--; }
			}
			canSchedule.clear();				
			for (int j = 0; j < remaining.size(); j++)							//re-do the instructions that can be scheduled, because our scheduling could have changed that
			{
				if (dependencies[remaining[j]].parentLines.size() == 0)
				{
					std::string functUnit = dependencies[remaining[j]].nodeUnit;
					if ((functUnit == "ALU" && ALUs > 0) ||
						(functUnit == "MLU" && MLUs > 0) ||
						(functUnit == "LDW" && LDWs > 0) ||
						(functUnit == "STW" && STWs > 0))
					{
						canSchedule.push_back(remaining[j]);
					}
				}
			}
		}

		instructionList.push_back(";;");			//we went through a whole line of instruction
		std::cout << ";;" << '\n';//for debugging*******************************************************************************************************************************
		if (checkInstructions.size() > 0)
		{
			for (int i = 0; i < checkInstructions.size(); i++)
			{
				instructionList.push_back(checkInstructions[i]);
			}
			checkInstructions.clear();
		}

		for (int i = 0; i < inProgress.size(); i++)				//adjust in progress for the end of a cycle
		{
			dependencies[inProgress[i]].nodeLatency -= 1;		//subtract 1 from all in progress operations
			if (dependencies[inProgress[i]].nodeLatency == 0)	//if the latency for this instruction is now 0
			{
				for (int j = 0; j < dependencies.size(); j++)	//remove the instruction with latency of 0 from all parent lists 
				{
					for (int k = 0; k < dependencies[j].parentLines.size(); k++)	//going through every parent of each instruction
					{
						if (dependencies[j].parentLines[k] == inProgress[i])		//if this parent is the same as the one with Latency 0
						{
							dependencies[j].parentLines.erase(dependencies[j].parentLines.begin() + k);		//erase this parent
							k--;					//fix our counter
						}
					}
				}
				std::string functUnit = dependencies[inProgress[i]].nodeUnit;
				if (functUnit == "ALU") { ALUs++; }				//add in functional units
				else if (functUnit == "MLU") { MLUs++; }
				else if (functUnit == "LDW") { LDWs++; }
				else if (functUnit == "STW") { STWs++; }
				inProgress.erase(inProgress.begin() + i);		//erase it from in progress
				i--;											//re-adjust our counter
			}
		}

	}
	if (penaltyTail.size() > 0)
	{
		instructionList.push_back("\tc0    goto Label" + std::to_string(labelCounter));
		instructionList.push_back(";;");
		for (int i = 0; i < penaltyTail.size(); i++)
		{
			instructionList.push_back(penaltyTail[i]);		//adding in all the goto instructions
		}
		instructionList.push_back("Label" + std::to_string(labelCounter) + ":");
		instructionList.push_back(";;");
	}

	/*
	While remaining to schedule size > 0
		Go through remaining and find ones that can be scheduled (with no parents)
		
		For number 1 to number of instruction slots 4
			See if its functional unit is in use?
			Go through can be scheduled find the one with the highest criticality or fan out
				Add it to the list
				Add it to inProgress
				Remove From Remaining
				Remove its functional unit from table
				If we schedule it, we need to remove the things that need its functional unit from canSchedule
		
		Add the ";;"

		Go through InProgress
			subtract from latency
			if latency is 0
				remove from all parent lists
				remove from inProgress
				add its functional unit back in
	*/

	return instructionList;
}


/*
Inputs:
    - std::vector<std::string> instructions. The input is a vector of strings. Each
      string in the vector is an instruction in the original vex code.
    - <int> mode: value indicating which heuristic ordering to use

Returns : std::vector<std::string>

The function should return a vector of strings. 
Each string should be a scheduled instruction or ;; that marks the end of a VLIW instruction word.
*/
std::vector<std::string>  scheduleVLIW(std::vector<std::string> instructions, int mode)
{
	std::vector<std::string> scheduledVLIW;
	std::vector<std::vector<std::string>> parsedInstructions = parseInstructions(instructions);
	std::vector<VLIWNode> nodeGraph = generateNodes(parsedInstructions, mode);
	std::vector<std::string> availableRegisters;
	
	//printInstructions(parsedInstructions);/************************************************/
	//printNodes(nodeGraph);/****************************************************************/

	if (mode > 0) 
	{ 
		availableRegisters = renameRegisters(parsedInstructions, nodeGraph);		//we directly manipulate "parsedInstructions" and it returns the registers we have available
		nodeGraph = generateNodes(parsedInstructions, mode);
	}

	printInstructions(parsedInstructions);/************************************************/
	printNodes(nodeGraph);/****************************************************************/

	//for (int i = 0; i < availableRegisters.size(); i++) { std::cout << availableRegisters[i] << "   "; }
	//std::cout << '\n';
	

	scheduledVLIW = make4Wide(parsedInstructions, nodeGraph, availableRegisters);
	printInstructions(parsedInstructions);/************************************************/

    return scheduledVLIW;
}


int main(int argc, char *argv[])
{

   if(argc != 4) {
       std::cout << "Invalid parameters \n";
       std::cout << "Expected use ./vliwSpeculation ";
       std::cout << "<input file name> <output file name> <mode>\n";
   }
 
	const char* inputFile = argv[1];
	const char* vliwSpeculationOutput = argv[2];
	int mode = atoi(argv[3]);

   std::vector<std::string> instructions;
   std::vector<std::string> scheduledVLIW;
 
   /* Read instructions from the file */
   instructions = readInput(inputFile);

   /* Schedule instructions */
   scheduledVLIW = scheduleVLIW(instructions, mode);

   /* Print scheduled instructions to file */
   printOutput(vliwSpeculationOutput, scheduledVLIW);
}
