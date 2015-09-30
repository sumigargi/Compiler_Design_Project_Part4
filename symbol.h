#ifndef SYMBOL_H
#define SYMBOL_H

#include	<iostream>		// Necessary for I/O Stream functions
#include	<vector>		//Necessary for storing the Symbol Table
#include	<fstream>		// Necessary for File I/O function
#include    <map>			//Necessary for storing the Name Table
#include	<string>		//Necessary for storing, well, strings. 

//	Constant declarations
#define   	NUMKEYWORDS     21
#define		NUMTOKENS	    34

namespace compiler {

	extern char* tokenClassString[];

	//The name table is stored in a map having two fields - One to store the actual name and the other to store its link in the symbol table.
	//A Map seemed a good choice for this purpose, since it ensures there's no duplication in the table and the data is kept sorted.
	//The biggest advantage is that the time complexity for a search on the map is log(n), thereby removing the need for any hashing functions. 
	typedef std::map<std::string, int> NameTable;


	// The tokens
	enum	tokenType {tokbegin, tokcall, tokdeclare, tokdo, 
				tokelse,tokend, tokendif, tokenduntil,  tokendwhile, tokif, 
				tokinteger, tokparameters, tokprocedure, tokprogram, tokread,
				tokreal, tokset, tokthen, tokuntil, tokwhile, tokwrite,
				tokstar, tokplus, tokminus, tokslash, tokequals,
				tokcolon, toksemicolon, tokcomma, tokperiod, tokgreater, tokless,
				tokgtequal, toklsequal, toknotequal, tokopenparen, tokcloseparen, tokfloat,
				tokidentifier, tokarray, tokopenbracket, tokclosebracket,
				tokconstant, toknot, tokor, tok_int_div, tokmod, tokand,
				tokof, tokassign, tokfunction, tokerror, tokeof, tokunknown, tokepsilon
	};

	// The semantic types, i.e, keywords, procedures, variables, constants
	enum semanticType	{stunknown, stkeyword, stprogram,
						stparameter, stvariable, sttempvar,
						stconstant, stenum, ststruct, stunion, stprocedure,
						stfunction, stlabel, stliteral, stoperator
	};

	
	// The data types, i.e, real and integer
	enum dataType  {dtunknown, dtnone, dtprogram, dtprocedure,
					dtinteger, dtreal
	};

	// The value can be either integer or real.  We save the tag which
	// tells is which it is and store the result in a union.
	enum tagType   {tint, treal};

	union valType  {
		 int	ival;
		float	rval;
	};

	// The structure that stores the tag and the value
	struct ValRec{

		ValRec();
		ValRec(tagType, int);
		tagType   tag;
		valType  val;
	};

	
	
	struct ProcStackItem {
     int  proc;
     int  sstart, snext;

	};


	//The symbol table structure
struct SymTabType   {

		SymTabType();
		SymTabType(semanticType, dataType, tokenType, int, int, int);
		
		semanticType			sType;
		tokenType				tType;
		dataType				dType;
		int						owningProc;
	    NameTable::iterator     thisname;
		int						outerScope, nextScope;
		ValRec					value;
     
	};


	//The main class, bringing everything together
	class SymbolTable {

	public:

		SymbolTable();
		
		void			display();
		bool			installName(const std::string, int&);				//Installs a name in the name table. Returns false if name is already in the table
		bool			isPresent(const std::string, NameTable::iterator&);	//Checks if name is present in the name table. If name is present, index is stored in iterator parameter
		int				installAttrib(NameTable::iterator&);				//Installs an entry in the attribute table
		void			setAttrib(int, int, int);							//Sets the attribute for entry with the semantic and token type passed as parameters
		ProcStackItem	initProcEntry(int);									//Initializes the procedure entry
		tokenType		tokenClass(int);									//Returns the token type of the parameter
		void			installDataType(int, semanticType, dataType);		//Installs the data type for the parameter passed
		void			setRValue(int, float);								//Sets the float value for the ValRec value 
		float			getRValue(int);										//Returns the float value for the ValRec value 
		void			setIValue(int, int);								//Sets the integer value for the ValRec value 
		int				getIValue(int);										//Returns the integer value for the ValRec value 
	
	private:
		
		NameTable				nameTable;									//Name Table stored as a map
		std::vector<SymTabType> symbolTable;								//Symbol table stored as a vector for dynamic memory management
		ProcStackItem			thisProc;									//Procedure entry

	};

}

#endif