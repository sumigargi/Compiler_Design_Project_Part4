#ifndef PARSER_H
#define PARSER_H
#include<iostream>
#include "symbol.h"
#include "scanner.h"

namespace compiler {

	#define		NUMNONTERMS	6
	#define		NUMPRODUCTIONS	7
	#define		MAXPARSESTACK	1000

	enum NTtype
	{
		Program,                    IdList,                     _IdList_,                   Declarations,               Type,                       
		StandardType,               SubprogramDeclarations,     SubprogramDeclaration,      SubprogramHead,             Arguments,                  
		ParameterList,              _ParameterList_,            CompoundStatement,          OptionalStatements,         StatementList,              
		_StatementList_,            Statement,                  RestOfStatement,            ExpressionList,             _ExpressionList_,           
		Expression,                 _Expression_,               SimpleExpression,           _SimpleExpression_,         Term,                       
		_Term_,                     Factor,                     _Factor_,                   Sign,                       relop,                      
		addop,                      mulop,                      assignop,                   
	};

	const std::string NTstring[] = 
	{
		"Program",                   "IdList",                    "_IdList_",                  "Declarations",              "Type",                      
		"StandardType",              "SubprogramDeclarations",    "SubprogramDeclaration",     "SubprogramHead",            "Arguments",                 
		"ParameterList",             "_ParameterList_",           "CompoundStatement",         "OptionalStatements",        "StatementList",             
		"_StatementList_",           "Statement",                 "RestOfStatement",           "ExpressionList",            "_ExpressionList_",          
		"Expression",                "_Expression_",              "SimpleExpression",          "_SimpleExpression_",        "Term",                      
		"_Term_",                    "Factor",                    "_Factor_",                  "Sign",                      "relop",                     
		"addop",                     "mulop",                     "assignop",                  
	};

	
	struct prod_item
	{
		bool      isTerm;
		int       type;
	};

	struct prod_auxi_item
	{
		int       start;
		int       len;
	};

	
struct parsenoderec	{

	int			level;
	bool isTerm;
	int			ParseItem, symtabentry;

};

/* The basic parse functions */
struct parsenoderec	*getparsenode(bool termtag, int info);

void			parse(void);
void			error(char message[], int linenum);

/*  The declarations and functions for handling the parse stack */
typedef	struct	{
	int		top;
	struct	parsenoderec		*parsptr[MAXPARSESTACK];
}	parsestack;


struct parsenoderec	*parsepop(void);
void			parsepush(struct parsenoderec *x);
bool    	parseempty(void);
void			initparsestack(void);
ProcStackItem		initparseentry(int tabindex);

/* The functions which are subsidiary to the function parse */
void	processnonterm(struct parsenoderec *thisnode);
void	printnonterm(int i);
int	matchtoken(int thistoken, int thattoken);



void printToken(int i);

}

#endif