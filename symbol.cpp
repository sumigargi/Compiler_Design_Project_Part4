

#include	"symbol.h"

namespace compiler {

	using namespace std;

	//	The key words and operators - used in initializing the symbol
	//	table

	char	*keyString[] = {"BEGIN", "CALL", "DECLARE",
			"DO", "ELSE", "END", "ENDIF", "ENDUNTIL", "ENDWHILE",
			"IF", "INTEGER", "PARAMETERS", "PROCEDURE", "PROGRAM",
			"READ", "REAL", "SET", "THEN", "UNTIL", "WHILE",
			"WRITE", "*", "+", "-", "/", "=", ";",
			",", ".", ">", "<", "!", "(", ")", "_float", 
};
	
	//	The names of the token classes in a format that can
	//	be printed in a symbol table dump

	char *tokenClassString[] = {   "begin     ", "call      ",
				"declare   ", "do        ", "else      ", "end       ", 
				"endif     ", "enduntil  ", "endwhile  ", "if        ", 
				"integer   ", "parameters", "procedure ", "program   ", 
				"read      ", "real      ", "set       ", "then      ",
				"until     ", "while     ", "write     ", "star      ", 
				"plus      ", "minus     ", "slash     ", "equals    ",
				"semicolon ", "comma     ", "period    ", "greater   ",
				"less      ", "notequal  ", "openparen ", "closeparen", 
				"float     ", "identifier", "constant  ", "error     ", 
				"eof       ", "unknown   "
};


	//	The names of the semantic types in a format that can be 
	//	printed  in a symbol table dump

	char *semanticTypeString[] =  {"unknown  ", "keyword  ", "program  ",
			    "parameter", "variable ", "temp. var",
			    "constant ", "enum     ", "struct   ",
			    "union    ", "procedure", "function ",
				"label    ", "literal  ", "operator "
};


	//	The names of the data types in a format that can be 
	//	printed  in a symbol table dump
	char *dataTypeString[] = {"unknown","none   ", "program",
		                "proced.", "integer", "real   "
};
	

	//Constructors
	ValRec::ValRec(){}

	ValRec::ValRec(tagType t, int v) : tag(t) {val.ival = v;}

	SymTabType::SymTabType(){}

	SymTabType::SymTabType(semanticType sType = stunknown, dataType dType = dtunknown, tokenType tType = tokunknown, int ownProc = -1, int outScope = -1, int nextScope = -1) :
	sType(sType), tType(tType), dType(dType), owningProc(ownProc), outerScope(outScope), nextScope(nextScope), value(tint, 0)
	{}

	//Main constructor to initialize the symbol table upon creation by setting up the name table with the keywords and operators and linking to the symbol table
	SymbolTable::SymbolTable() {
		

		thisProc = initProcEntry(-1);

		int tableIndex;

		//Install the keywords in the table
		for(int i = 0; i < NUMKEYWORDS; i++) {

			installName(keyString[i], tableIndex);
			setAttrib(stkeyword, i, tableIndex);

		}

		//Install the operators in the table
		for(int i = NUMKEYWORDS; i < NUMTOKENS; i++) {

			installName(keyString[i], tableIndex);
			setAttrib(stoperator, i, tableIndex);
		}
		
		installName(keyString[NUMTOKENS], tableIndex);
		setAttrib(stfunction, NUMTOKENS, tableIndex);
		installDataType(tableIndex, stfunction, dtreal);

		cout << "Symbol Table Initialized!" << endl;

	}


	//Installs a name in the name table. Returns false if the name already existed.
	bool SymbolTable::installName(const string name, int& tableIndex) {

		NameTable::iterator nameIndex;

		if(!isPresent(name, nameIndex)) {

			nameTable.insert(pair<string, int>(name,-1));
			nameIndex = nameTable.find(name);
			tableIndex = installAttrib(nameIndex);
			return false;
		
		}

		if(nameIndex->second == -1) {
			tableIndex = installAttrib(nameIndex);
			return false;
		}

		else
			tableIndex = nameIndex->second;
		
		return true;

	}

	//Checks to see if the name is already present in the name table. If found, the iterator parameter is updated with the location.
	bool SymbolTable::isPresent(const string name, NameTable::iterator& iter) {

		iter = nameTable.find(name);

		if(iter != nameTable.end())
			return true;

		return false;
	}

	//Creates an entry in the attribute table for the name table entry passed as a parameter and establishes a link between the two
	//The return value is the entry in the symbol table that is added
	int SymbolTable::installAttrib(NameTable::iterator& iter) {

		int tableIndex;

		tableIndex = symbolTable.size();
		iter->second = tableIndex;
		symbolTable.push_back(SymTabType(stunknown, dtunknown));

		symbolTable[symbolTable.size() -1].thisname = iter;

		return tableIndex;

	}

	//Sets the attribute for the entry in the symbol table with the semantic type and token type passed as parameters
	void SymbolTable::setAttrib(int symbol, int token, int tableIndex) {
		
		symbolTable[tableIndex].sType = (semanticType)symbol;
		symbolTable[tableIndex].tType = (tokenType)token;

		//Keywords and operators have no data type
		if(symbolTable[tableIndex].sType == stkeyword || symbolTable[tableIndex].sType == stoperator)
			symbolTable[tableIndex].dType = dtnone;

		//For other types, set the data type as unknown for now
		else
			symbolTable[tableIndex].dType = dtunknown;

		//Set the scope 
		if(symbolTable[tableIndex].tType == tokidentifier && thisProc.proc != -1)
			if (thisProc.sstart == -1)    {
               thisProc.sstart = tableIndex;
                     thisProc.snext = tableIndex;
          }
          else {
               symbolTable[thisProc.snext].nextScope = tableIndex;
                    thisProc.snext = tableIndex;
          }

	}


	//Display the symbol table
	//Formatting is a little iffy for now
	void SymbolTable::display() {

		cout << "********************************************************************************" << endl;

		cout << "Index\t" << "Name\t" << "Token\t\t" << "Symbol\t\t" << "Data\t" << "Value\t" << "Owning\t" << "Label" << endl;
		cout << "\t\tClass\t\tType\t\tType\t\tProcedure" << endl << endl;

		for(size_t i = 0; i < symbolTable.size(); i++) {

			cout << i << '\t' << symbolTable[i].thisname->first << "\t" << tokenClassString[symbolTable[i].tType] << "\t" << semanticTypeString[symbolTable[i].sType] << "\t" << dataTypeString[symbolTable[i].dType] << "\t";

			if(symbolTable[i].value.tag == tint)
				cout << symbolTable[i].value.val.ival << '\t';
			
			else
				cout << symbolTable[i].value.val.rval << '\t';

			if(symbolTable[i].owningProc == -1)
				cout << "GLOBAL\t";
			else
				cout << symbolTable[i].owningProc << "\t";

			cout << endl;
		}

		cout << endl;

	}

		
	//Initialize the scope 
	ProcStackItem SymbolTable::initProcEntry(int tableIndex) {

	
		ProcStackItem thisproc;
		thisproc.proc = tableIndex;
		thisproc.sstart = -1;
		thisproc.snext = -1;
	
		return(thisproc);
	
	}


	//Returns the token type of the parameter
	tokenType SymbolTable::tokenClass(int tableIndex) {
		return symbolTable[tableIndex].tType;
	}

	
	//Install the data type for the parameter with the provided parameters
	void SymbolTable::installDataType(int tableIndex, semanticType sem, dataType data) {

		symbolTable[tableIndex].sType = sem;
		symbolTable[tableIndex].dType = data;

	}


	//Set the float value 
	void SymbolTable::setRValue(int tableIndex, float f) {

		symbolTable[tableIndex].value.tag = treal;
		symbolTable[tableIndex].value.val.rval = f;

	}

	//Get the float value
	float SymbolTable::getRValue(int tableIndex) {
		return symbolTable[tableIndex].value.val.rval;

	}

	//Integer Setter
	void SymbolTable::setIValue(int tableIndex, int i) {

		symbolTable[tableIndex].value.tag = tint;
		symbolTable[tableIndex].value.val.ival = i;

	}

	//Integer Getter
	int SymbolTable::getIValue(int tableIndex) {
		return symbolTable[tableIndex].value.val.ival;

	}
}