#include <iostream>
#include <stdio.h>
#include <bitset>
#include <math.h>
#include <fstream>
#include "vliwEncoder.h"

#define MAX_INSTR_WIDTH 100
#define MASK_LIMIT 8

/*
Read instructions from an input file and return a char array 
*/
std::vector<std::string> readInput(const char* fname)
{
    std::string str;
    std::vector<std::string> instructions;
    
    std::ifstream filestream(fname);
    
    while (std::getline(filestream, str))
    {
        int current_position = 0;
        std::size_t pos = str.find("\tc0    ");
        if (pos == 0)
        {
            instructions.push_back(str);
        } else {
            std::size_t f = str.find(";;");
            if (f == 0)
                instructions.push_back(str);
        }
    }
   
   return instructions;
}

/*
Print an encoded VLIW instruction to a file. The encoded instruction should be a character array and terminated by '\0' character
*/
void printOutput(const char* fname, std::vector<std::string> encodedVLIW)
{
    std::ofstream outfile;
    outfile.open(fname);
	
    for(int i = 0; i < encodedVLIW.size(); i++)
       outfile << encodedVLIW[i] << "\n";

    outfile.close();
}

/*
TODO : Write any helper functions that you may need here. 

*/


/*
Input : std::vector<std::string> instructions. The input is a vector of strings. Each string in the vector is one line in the vex code. A line can be a instruction or a ';;'

Returns : std::vector<std::string>

The function should return a vector of strings. Each string should be a line of VLIW encoded instruction with masked encoding scheme
*/
std::vector<std::string>  maskedVLIW(std::vector<std::string> instructions)
{
	/* Declare and initialize variables */
    std::vector<std::string> encodedVLIW;	
	std::vector<std::string> currentInstruction;
	std::string mask;
	std::string vliwInstruction;
	
	int mask_size = 0;
	int comment_location = 0;
	int instruction_start = 0;
	int instruction_count = 0;

    /* TODO : Implement your code here */
	
	/* Loop through instructions while converting them into wider instructions */
	for(int i = 0; i < instructions.size(); i++)
	{
		// If the instruction is an operation, create the mask, and store the operation without the comment
		if(instructions[i].find("c0") != std::string::npos)
		{	
			//Find where the operation begins and where the comment begins
			comment_location = instructions[i].find("##");
			instruction_start = (instructions[i].find("c0")) + 6;
			
			//Add a 0 to the mask if the operation is a NOP and a 1 if it's anything else
			if(instructions[i].find("NOP") == std::string::npos)
			{
				mask += "1";
				instruction_count++;
			}
			else
			{
				mask += "0";
				continue;
			}
			
			//If there is a comment, remove it and store just the operation
			if(comment_location != std::string::npos)
			{
				currentInstruction.push_back(instructions[i].substr(instruction_start, comment_location - instruction_start));
			}
			else
			{
				currentInstruction.push_back(instructions[i].substr(instruction_start, instructions[i].size() - instruction_start));
			}
		}
		else if(instructions[i].find(";;") != std::string::npos)
		{
			//Add starting characters and mask to VLIW instruction
			vliwInstruction = "c0\t";
			vliwInstruction += mask;
			
			mask_size += mask.size();
			
			//Add instructions
			for(int j = 0; j < currentInstruction.size(); j++)
			{
				vliwInstruction += "\t";
				vliwInstruction += currentInstruction[j];
			}
			
			//Add new VLIW instruction to the encoded vector
			encodedVLIW.push_back(vliwInstruction);
			encodedVLIW.push_back(";;");
			
			//Reset mask and VLIW instruction string
			mask = "";
			vliwInstruction = "";
			currentInstruction.erase(currentInstruction.begin(), currentInstruction.end());
		}
	}

	printf("Masked Encoding Size: %d\n", (mask_size + (instruction_count * 32)));
	
    return encodedVLIW;
}

/*
Input : std::vector<std::string> instructions. The input is a vector of strings. Each string in the vector is one line in the vex code. A line can be a instruction or a ';;'

Returns : std::vector<std::string>

The function should return a vector of strings. Each string should be a line of VLIW encoded instruction with template encoding scheme
*/
std::vector<std::string>  templateVLIW(std::vector<std::string> instructions)
{
    std::vector<std::string> encodedVLIW;
	std::vector<std::string> currentInstruction;
	std::string mask;
	std::string vliwInstruction;
	std::bitset<MASK_LIMIT> maskBits;
	
	int mask_size = 0;
	int template_bits = 0;
	int width = 0;
	int last_instruction = 0;
	int comment_location = 0;
	int instruction_start = 0;
	int instruction_count = 0;

	for(int i = 0; i < instructions.size(); i++){
		// If the instruction is an operation, create the mask, and store the operation without the comment
		if(instructions[i].find("c0") != std::string::npos)
		{	
			//Find where the operation begins and where the comment begins
			comment_location = instructions[i].find("##");
			instruction_start = (instructions[i].find("c0")) + 6;
			
			//If there is a comment, remove it and store just the operation
			if(comment_location != std::string::npos)
			{
				currentInstruction.push_back(instructions[i].substr(instruction_start, comment_location - instruction_start));
			}
			else
			{
				currentInstruction.push_back(instructions[i].substr(instruction_start, instructions[i].size() - instruction_start));
			}
		}
		else if(instructions[i].find(";;") != std::string::npos)
		{
			//Find instruction width
			if(width == 0)
			{
				width = currentInstruction.size();
				template_bits = log2(width);
			}
			
			//Add instructions
			for(int j = 0; j < currentInstruction.size(); j++)
			{
				if(currentInstruction[j].find("NOP") != std::string::npos)
				{
					continue;
				}
				
				instruction_count++;
				
				maskBits = j - last_instruction;
				mask += (maskBits.to_string()).substr((MASK_LIMIT - template_bits), template_bits);
				last_instruction = j;
				
				vliwInstruction += "\t";
				vliwInstruction += currentInstruction[j];
			}
			
			//Add starting characters and mask to VLIW instruction
			vliwInstruction = mask + vliwInstruction;
			vliwInstruction = "c0\t" + vliwInstruction;
			
			//Add new VLIW instruction to the encoded vector
			encodedVLIW.push_back(vliwInstruction);
			encodedVLIW.push_back(";;");
			
			//Reset mask and VLIW instruction string
			mask_size += mask.size();
			mask = "";
			vliwInstruction = "";
			last_instruction = 0;
			currentInstruction.erase(currentInstruction.begin(), currentInstruction.end());
		}
	}
	
	printf("Template Encoding Size: %d\n", (mask_size + (instruction_count * 32)));

    return encodedVLIW;
}


int main(int argc, char *argv[])
{

   if(argc != 2) {
       std::cout << "Invalid parameters \n";
       std::cout << "Expected use ./vliwEncoder <input file name>\n";
   }
 
   const char* inputFile = argv[1];
   const char* maskedOutput = "maskedEncoding.txt";
   const char* templateOutput = "templateEncoding.txt";

   std::vector<std::string> instructions;
   std::vector<std::string> maskedEncoding;
   std::vector<std::string> templateEncoding;
 
   /* Read instructions from the file */
   instructions = readInput(inputFile);

   /* Encode instructions using masked and template encoding */
   maskedEncoding = maskedVLIW(instructions);
   templateEncoding = templateVLIW(instructions);

   /* Print encoded instructions to file */
   printOutput(maskedOutput,maskedEncoding);
   printOutput(templateOutput,templateEncoding);
}
