#include "scanner.h"
#include "parser.h"

#include<iostream>
#include<string>

using namespace std;

namespace compiler {

	extern enum NTtype;
	extern std::string		tokenstring;
	extern enum tokentype	thistoken;
	extern struct parsenoderec	*parsetree, *thisnode;
	extern int		currentlevel, numops;
	extern std::string lexeme;
	
	int LINENUMBER = 1;
	//Function Definitions
	
	//	//Initializing tokenType vector 
	Scanner::Scanner(): counter(0) {}


	//Tries to open the file with the handler and file name passed as parameters. If the file does not open, displays error message and quits program
	void Scanner::openFile(char* fileName) {
		
		ifs.open(fileName);

		if(!ifs) {

			cerr << "File could not be opened." << endl;

			exit(5);

		}

	}

	//Displays the contents of the file
	void Scanner::displayFile() {

		string line;

		cout << endl << "INPUT PROGRAM READ FROM FILE:" << endl << endl;
		cout << "****************************************" << endl;

		//Reads and displays, line-by-line, the entire file
		while(getline(ifs, line)) {

			cout << line << endl;

		}

		cout << "****************************************" << endl;

		//Reset file pointer
		ifs.clear();
		ifs.seekg(0);


	}

	//Parses the entire input file and pushes each lexeme inside the vector of strings (which is a private data member of the class)
	void Scanner::getLexemes() {

		string lexeme;
		string line;

		//Removes any old data that might be in the vector of lexemes
		lexemes.clear();

		while(getline(ifs, line)) {
		
			LINENUMBER++;

			size_t count = 0;

			//Count is the counter which runs up until the length of the line that has been read from the file
			while(count < line.length()) {

			//Clear the lexeme string which we are using to store the parsed lexeme in; after the value of lexeme is established, it is pushed into the vector of lexemes
				lexeme.clear();

			//Skip over blank spaces in the line
				while(line[count] == ' ')
					count++;

			//Stores first character of the word being read
				lexeme = line[count++];

			//Read next characters only if first character is a letter or a number; otherwise, it is an operator and we have to store it as is
				if((lexeme >= "a" && lexeme <= "z") || (lexeme >= "A" && lexeme <= "Z") || (lexeme >= "0" && lexeme <= "9"))
					while((count < line.length()) && (isdigit(line[count]) || isalpha(line[count]) || (line[count] == '.'))) {

						lexeme += line[count++];

					}

				//Add parsed lexeme to vector of lexemes
					lexemes.push_back(lexeme);
				

			}

		}

	}


	//Takes as a parameter a string by reference. The "value" of the lexeme that is processed is stored in the string and can be read by the caller
	//The method returns the token type of the lexeme that is processed
	int Scanner::getToken(string& lexeme, int& tableIndex) {
		
		tableIndex = 0;
		//If the counter is reached the size of the lexemes vector, all entries have been processed and a -1 is returned
		if(counter == lexemes.size())
			return -1;

		//Read next lexeme from the list/vector
		lexeme = lexemes[counter++];
	
		if(isWord(lexeme))
			return(processWord(lexeme, tableIndex));

		return(processNum(lexeme, tableIndex));
			
	}

	//Processes a word lexeme and adds to symbol table
	int Scanner::processWord(string lexeme, int& tableIndex) {
				
		if(sTable.installName(lexeme, tableIndex)) 
			return sTable.tokenClass(tableIndex);

		else
			sTable.setAttrib(stunknown, tokidentifier, tableIndex);
		
		return tokidentifier;

	}


	//Processes a numeric lexeme and adds to symbol table
	int Scanner::processNum(string lexeme, int& tabIndex) {
					
		if(sTable.installName(lexeme, tabIndex))
			return(sTable.tokenClass(tabIndex));

		sTable.setAttrib(stunknown, tokconstant, tabIndex);

		if(isFloat(lexeme)) {
			
			sTable.installDataType(tabIndex, stliteral, dtreal);
			sTable.setRValue(tabIndex, atof(lexeme.c_str()));
				
		}

		else {

			sTable.installDataType(tabIndex, stliteral, dtinteger);
			sTable.setRValue(tabIndex, atoi(lexeme.c_str()));

		}

		return tokconstant;

	}

	//Scans the file and displays all the tokens with the lexemes
	void Scanner::scanFile() {

		int tableIndex;
		string lexeme;
		int token;
	
		cout << endl << endl << "PARSED FILE:" << endl << endl;
		cout << "****************************************" << endl;
	
		token = getToken(lexeme, tableIndex);

		parse();

		////Get token for next lexeme to be processed and keep looping until a -1 is returned
		//while((token = getToken(lexeme, tableIndex)) != -1) {
		//	
		//	cout << lexeme << "\t" << tokenClassString[token] << endl;
		//	//cout << lexeme << "\t" << tokenTypes[token] << endl;

		//}
		cout << "****************************************" << endl;
		//cout << endl << "END OF FILE" << endl;
		
			
	cin.get();

	}

	//Displays the symbol table in a proper(ish) format
	void Scanner::displayTable() {
		
		cout << "Final Symbol Table:" << endl;
		sTable.display();

	}

	//Function to check if the parameter is a word or a number. Needed to process the lexeme correctly and add proper values to the Symbol Table
	bool Scanner::isWord(string lexeme) {

		if(isdigit(lexeme[0]))
			return false;

		return true;

	}

	//Function to check if the parameter is an integer or a float value
	bool Scanner::isFloat(string lexeme) {

		if(lexeme.find(".") < lexeme.size())
			return true;
		
		return false;
	}


	/*
 * Parse() -	This procedure checks the production table to
 *		make certain that there is a production for    
 *		which this nonterminal can be expanded that    
 *		begins with this token.  If there isn't, this  
 *		is a fatal syntactic error; the compiler will  
 *		terminate execution.                           
 *
 *		Then it pushes its right sentential form on    
 *		the stack after linking them to their next     
 *		node.                                          
 */

void Scanner::parse()
{
	int	i, lines = 0;

	initparsestack();
	parsetree = getparsenode(false, Program);
	parsepush(parsetree);
	int tableIndex = 0;

	do	{
		/*
		 *  Look up the production in the production table.  
		 *  If not there, terminate with an error message. 
		 */
		thisnode = parsepop();
		for (i = 0;  i < thisnode -> level;  i++)
				printf("   ");
		printf("%d    ", thisnode -> level);
		if (thisnode -> isTerm == true)
			printToken(thisnode -> ParseItem);
		else
			printnonterm(thisnode -> ParseItem);
                putchar('\n');

		if (thisnode -> isTerm == true)	{
			/* If its's a terminal, match it to the
				lookahead and get a new lookahead token */

			if (matchtoken(thistoken, thisnode -> ParseItem))
				thisnode -> symtabentry = tableIndex;
			else error("Parsing error - token does not required"
					" terminal", LINENUMBER);
			thistoken = (tokentype)getToken(lexeme, tableIndex);
		}
		/* Expand the nonterminal and push the items on
                        	the right hand side in reverse order */
                else processnonterm(thisnode);
		if (++lines%10 == 0)
			getchar();      	
        }  while(!parseempty());
}
}