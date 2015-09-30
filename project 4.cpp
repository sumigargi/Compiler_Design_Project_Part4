/*              *Gaurav Chaudhary (0524740)
   Team members: Rajat Mathur (0506563)   
                 Vandita Sharma (0532299)

   Compiler Design and Construction
   Term Project-4
   
   
#include "scanner.h"
#include "symbol.h"

using namespace std;
using namespace compiler;

int main() {
		
	Scanner scan;
	scan.openFile("input.txt");		//Opening file
	scan.displayFile();				//Displays the contents of the entire file as is
	scan.getLexemes();				//Parses the file and stores all the lexemes in a vector of strings for easy processing afterwards
	scan.scanFile();				//Parses all the lexemes and prepares the symbol, name and attribute tables
	scan.displayTable();			//Display the final symbol table

}
