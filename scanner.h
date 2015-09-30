#ifndef SCANNER_H
#define SCANNER_H

#include<fstream>
#include<vector>
#include<string>
#include "symbol.h"

namespace compiler {

	extern char* keyString[];
	extern char* tokenClassString[];
	

	//Scanner class 
	class Scanner {

	public:

		void parse();
		//The only constructor for this class
		Scanner();
		
		void openFile(char*);		//Opens the file the name of which is passed as a parameter
		void displayFile();			//Displays the contents of the file
		void displayTable();	//Displays all the data in the symbol table
		void getLexemes();			//Gets all the lexemes present in the file and stores in a vector of strings
		int getToken(std::string&, int&);		//Gets the token for the next lexeme to be processed and returns token type and stores lexeme "value" in the string reference parameter
		void scanFile();			//Processes the entire list of lexemes and displays results 
		bool isWord(std::string);	//Checks to see if the parameter is a word or a number
		bool isFloat(std::string);	//Checks to see if the parameter is an integer or a float value
		int processWord(std::string, int&);	//Process a string lexeme
		int processNum(std::string, int&);	//Process a numeric lexeme

	private:
				
		std::ifstream ifs;
		size_t counter;
		std::vector<std::string> lexemes;
		SymbolTable sTable;

	};

}

#endif
