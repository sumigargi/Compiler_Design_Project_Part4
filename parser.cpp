#include<iostream>
#include "parser.h"
#include<string>

namespace compiler {

	/* Internal variables global to the parser */
std::string		tokenstring;
enum tokentype	thistoken;
struct parsenoderec	*parsetree, *thisnode;
int		currentlevel = 1, numops;
std::string lexeme;

parsestack	pa;

void printToken(int i)
	{
     std::cout << tokenClassString[i];
	}

	struct ProcStackItem;
		
	const prod_item prod_array[] = 
	{

		/* Program ::= program id ( IdList ) ; Declarations  SubprogramDeclarations CompoundStatement . */
		{true,  stprogram},                  {true,  tokidentifier},              {true,  tokopenparen},               
		{false, IdList},                     {true,  tokcloseparen},              {true,  toksemicolon},              
		{false, Declarations},               {false, SubprogramDeclarations},     {false, CompoundStatement},          
		{true,  tokperiod},                 

		/* IdList ::= id _IdList_ */
		{true,  tokidentifier},                     {false, _IdList_},                   

		/* _IdList_ ::=  , id _IdList_ */
		{true,  tokcomma},                 {true,  tokidentifier},                     {false, _IdList_},                   
	

		/* _IdList_ ::=  <nil> */
		{false, tokepsilon},                      

		/* Declarations ::= var IdList : Type ; Declarations */
		{true,  stvariable},                 {false, IdList},                     {true,  tokcolon},                  
		{false, Type},                       {true,  toksemicolon},              {false, Declarations},               
	

		/* Declarations ::= <nil> */
		{false, tokepsilon},                      

		/* Type ::= StandardType */
		{false, StandardType},               

		/* Type ::= array [ num . . num ] of StandardType */
		{true,  tokarray},                  {true,  tokopenbracket},               {true,  tokconstant},                 
		{true,  tokperiod},                 {true,  tokperiod},                    {true,  tokconstant},                 
		{true,  tokclosebracket},              {true,  tokof},                     {false, StandardType},               
	

		/* StandardType ::= integer */
		{true,  tokinteger},                

		/* StandardType ::= real */
		{true,  tokreal},                   

		/* SubprogramDeclarations ::= SubprogramDeclaration ; SubprogramDeclarations */
		{false, SubprogramDeclaration},      {true,  toksemicolon},              {false, SubprogramDeclarations},     
	

		/* SubprogramDeclarations ::= <nil> */
		{false, tokepsilon},                      

		/* SubprogramDeclaration ::= SubprogramHead Declarations CompoundStatement */
		{false, SubprogramHead},             {false, Declarations},               {false, CompoundStatement},          
	

		/* SubprogramHead ::= function id Arguments : StandardType ; */
		{true,  tokfunction},                {true,  tokidentifier},              {false, Arguments},                  
		{true,  tokcolon},                  {false, StandardType},                {true,  toksemicolon},              
	

		/* SubprogramHead ::=  procedure id Arguments ; */
		{true,  tokprocedure},              {true,  tokidentifier},               {false, Arguments},                  
		{true,  toksemicolon},              

		/* Arguments ::= ( ParameterList ) */
		{true,  tokopenparen},               {false, ParameterList},              {true,  tokcloseparen},               
	

		/* Arguments ::= <nil> */
		{false, tokepsilon},                      

		/* ParameterList ::= IdList : Type _ParameterList_ */
		{false, IdList},                     {true,  tokcolon},                   {false, Type},                       
		{false, _ParameterList_},            

		/* _ParameterList_ ::=  ; IdList : Type _ParameterList_ */
		{true,  toksemicolon},               {false, IdList},                     {true,  tokcolon},                  
		{false, Type},                       {false, _ParameterList_},            

		/* _ParameterList_ ::= <nil> */
		{false, tokepsilon},                      

		/* CompoundStatement ::= begin OptionalStatements end */
		{true,  tokbegin},                  {false, OptionalStatements},         {true,  tokend},                    
	

		/* OptionalStatements ::= StatementList */
		{false, StatementList},              

		/* OptionalStatements ::= <nil> */
		{false, tokepsilon},                     

		/* StatementList ::= Statement _StatementList_ */
		{false, Statement},                  {false, _StatementList_},            

		/* _StatementList_ ::= ; Statement _StatementList_ */
		{true,  toksemicolon},              {false, Statement},                  {false, _StatementList_},            
		

		/* _StatementList_ ::= <nil> */
		{false, tokepsilon},                      

		/* Statement ::= id RestOfStatement */
		{true,  tokidentifier},                     {false, RestOfStatement},            

		/* Statement ::= CompoundStatement */
		{false, CompoundStatement},          

		/* Statement ::= if Expression then Statement else Statement */
		{true,  tokif},                      {false, Expression},                 {true,  tokthen},                   
		{false, Statement},                  {true,  tokelse},                    {false, Statement},                  
	

		/* Statement ::= while Expression do Statement */
		{true,  tokwhile},                  {false, Expression},                 {true,  tokdo},                     
		{false, Statement},                  

		/* RestOfStatement ::=  ( ExpressionList ) */
		{true,  tokopenparen},               {false, ExpressionList},             {true,  tokcloseparen},               
	

		/* RestOfStatement ::=  [ ExpressionList ] */
		{true,  tokopenbracket},               {false, ExpressionList},             {true,  tokclosebracket},               
	

		/* RestOfStatement ::=  <nil> */
		{false, tokepsilon},                      

		/* ExpressionList ::= Expression _ExpressionList_ */
		{false, Expression},                 {false, _ExpressionList_},           

		/* _ExpressionList_ ::= , Expression _ExpressionList_ */
		{true,  tokcomma},                  {false, Expression},                 {false, _ExpressionList_},           
	

		/* _ExpressionList_ ::= <nil> */
		{false, tokepsilon},                      

		/* Expression ::= SimpleExpression _Expression_ */
		{false, SimpleExpression},           {false, _Expression_},               

		/* _Expression_ ::= <nil> */
		{false, tokepsilon},                      

		/* _Expression_ ::= relop SimpleExpression */
		{false, relop},                      {false, SimpleExpression},           

		/* SimpleExpression ::= Term _SimpleExpression_ */
		{false, Term},                       {false, _SimpleExpression_},         

		/* SimpleExpression ::= Sign Term _SimpleExpression_ */
		{false, Sign},                       {false, Term},                       {false, _SimpleExpression_},         
	

		/* _SimpleExpression_ ::= addop Term _SimpleExpression_ */
		{false, addop},                      {false, Term},                       {false, _SimpleExpression_},         
	

		/* _SimpleExpression_ ::= <nil> */
		{false, tokepsilon},                     

		/* Term ::= Factor _Term_ */
		{false, Factor},                     {false, _Term_},                     

		/* _Term_ ::= mulop Factor _Term_ */
		{false, mulop},                      {false, Factor},                     {false, _Term_},                     
	

		/* _Term_ ::= <nil> */
		{false, tokepsilon},                      

		/* Factor ::= id _Factor_ */
		{true,  tokidentifier},                     {false, _Factor_},                   

		/* Factor ::= num */
		{true,  tokconstant},                 

		/* Factor ::= ( Expression ) */
		{true,  tokopenparen},               {false, Expression},                 {true,  tokcloseparen},               
	

		/* Factor ::= not Factor */
		{true,  toknot},                    {false, Factor},                     

		/* _Factor_ ::= <nil> */
		{false, tokepsilon},                      

		/* _Factor_ ::= ( ExpressionList ) */
		{true,  tokopenparen},               {false, ExpressionList},             {true,  tokcloseparen},               
	

		/* Sign ::= + */
		{true,  tokplus},                    

		/* Sign ::= - */
		{true,  tokminus},               

		/* relop ::= = */
		{true,  tokequals},                  

		/* relop ::= > */
		{true,  tokgreater},                     

		/* relop ::= < */
		{true,  tokless},                     

		/* relop ::= >= */
		{true,  tokgtequal},                    

		/* relop ::= <= */
		{true,  toklsequal},                     

		/* relop ::= <> */
		{true,  toknotequal},                    

		/* addop ::= + */
		{true,  tokplus},                    

		/* addop ::= - */
		{true,  tokminus},               

		/* addop ::= OR */
		{true,  tokor},                     

		/* mulop ::= * */
		{true,  tokstar},                    

		/* mulop ::= / */
		{true,  tokslash},                    

		/* mulop ::= DIV */
		{true,  tok_int_div},                

		/* mulop ::= MOD */
		{true,  tokmod},                    

		/* mulop ::= AND */
		{true,  tokand},                    

		/* assignop ::= := */
		{true,  tokassign}                 
	};



/*
 *	The index to the productions array, showing the
 *	starting position of the production and the number of items
 *	on its right-hand side. The first item is a dummy to get past the
 *	zeroth element of the array and any other second-entries of 0
 *	indicate an epsilon production.
 */

const prod_auxi_item prod_auxi_array[] = 
{
	{0,   10},  {10,  2},   {12,  3},   {15,  0},   {15,  6},   {21,  0},   {21,  1},   {22,  9},   
	{31,  1},   {32,  1},   {33,  3},   {36,  0},   {36,  3},   {39,  6},   {45,  4},   {49,  3},   
	{52,  0},   {52,  4},   {56,  5},   {61,  0},   {61,  3},   {64,  1},   {65,  0},   {65,  2},   
	{67,  3},   {70,  0},   {70,  2},   {72,  1},   {73,  6},   {79,  4},   {83,  3},   {86,  5},   
	{91,  2},   {93,  0},   {93,  2},   {95,  3},   {98,  0},   {98,  2},   {100, 0},   {100, 2},   
	{102, 2},   {104, 3},   {107, 3},   {110, 0},   {110, 2},   {112, 3},   {115, 0},   {115, 2},   
	{117, 1},   {118, 3},   {121, 2},   {123, 0},   {123, 3},   {126, 1},   {127, 1},   {128, 1},   
	{129, 1},   {130, 1},   {131, 1},   {132, 1},   {133, 1},   {134, 1},   {135, 1},   {136, 1},   
	{137, 1},   {138, 1},   {139, 1},   {140, 1},   {141, 1},   {142, 1}
};

const int parse_table[][43] = 
{
	{0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   2,  -1,  -1,  -1,   3,  -1,  -1,   3,  -1,  -1,  -1},
	{-1,   4,  -1,  -1,  -1,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,   6,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,   8,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  10,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  12,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  12,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  14,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  13,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  16,  -1,  15,  -1,  -1,  -1,  16,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  17,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  18,  -1,  -1,  19,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  20,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  21,  22,  21,  -1,  21,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  21,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  23,  -1,  23,  -1,  23,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  23,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  25,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  24,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  27,  -1,  28,  -1,  29,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  26,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  33,  -1,  -1,  -1,  -1,  33,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  32,  -1,  33,  -1,  30,  -1,  31,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  34,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  34,  34,  -1,  -1,  -1,  -1,  -1,  -1,  34,  -1,  -1,  -1,  -1,  34,  34,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  35,  -1,  -1,  -1,  36,  -1,  36,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  37,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  37,  37,  -1,  -1,  -1,  -1,  -1,  -1,  37,  -1,  -1,  -1,  -1,  37,  37,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  38,  -1,  38,  -1,  38,  38,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  39,  39,  39,  39,  39,  39,  -1,  -1,  -1,  -1,  -1,  38,  38,  -1,  -1,  38,  -1,  38,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  40,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  41,  41,  -1,  -1,  -1,  -1,  -1,  -1,  40,  -1,  -1,  -1,  -1,  40,  40,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  43,  -1,  43,  -1,  43,  43,  -1,  -1,  -1,  -1,  42,  -1,  -1,  -1,  -1,  43,  43,  43,  43,  43,  43,  42,  42,  -1,  -1,  -1,  43,  43,  -1,  -1,  43,  -1,  43,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  44,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  44,  -1,  -1,  -1,  -1,  44,  44,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  46,  -1,  46,  -1,  46,  46,  -1,  45,  45,  45,  46,  -1,  -1,  -1,  -1,  46,  46,  46,  46,  46,  46,  46,  46,  45,  45,  -1,  46,  46,  -1,  -1,  46,  -1,  46,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  49,  -1,  -1,  -1,  -1,  47,  48,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  51,  -1,  51,  -1,  51,  51,  -1,  51,  51,  51,  51,  -1,  -1,  -1,  -1,  51,  51,  51,  51,  51,  51,  51,  51,  51,  51,  -1,  51,  51,  -1,  52,  51,  -1,  51,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  53,  54,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  55,  60,  57,  59,  56,  58,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  63,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  61,  62,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  66,  67,  68,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  64,  65,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  69,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}
};





/*
 * GetParseNode() -	Get a parse node that will be pushed
 *			on the stack and then fill it with the
 *			appropriate information.
 */
struct parsenoderec	*getparsenode(bool termTag, int info)
{
	struct parsenoderec	*p;

	p = (struct parsenoderec *)malloc(sizeof(struct parsenoderec));
        p -> level = currentlevel;
	p -> isTerm = termTag;
	p -> ParseItem = info;
        return(p);
}

/*
 * ProcessNonterm() -	The details of looking up the nonterminal in the
 *			production table and pushing items on the stack
 */
void processnonterm(struct parsenoderec *thisnode)
{
	struct parsenoderec	*p/*, *q*/;
	int			prodnum, i;

	/*
	 *	Look up the nonterminal in the production table.
	 *	If the production number is 0, there is no production
	 *	and it is a parse error.
         */
	currentlevel = thisnode -> level +1;
	if ((prodnum = parse_table[thisnode -> ParseItem][thistoken]) == -1){
        	printf("%d\t", thistoken);
        	printToken(thistoken);
		error("Nonterminal - token mismatch", 1/*LINENUMBER*/);
        }

	/*
	 *	If there is a production, push the items
	 *	on the right-hand onto the parse stack in
	 *	reverse order.
         */
	if (prod_auxi_array[prodnum].len != 0)	{
		i = prod_auxi_array[prodnum].start
			+ prod_auxi_array[prodnum].len - 1;
		p = getparsenode(prod_array[i].isTerm,
				prod_array[i].type);
		parsepush(p);
        	
		for (i = prod_auxi_array[prodnum].start
					+ prod_auxi_array[prodnum].len - 2;
				i >= prod_auxi_array[prodnum].start;  --i)	{
			p = getparsenode(prod_array[i].isTerm,
				prod_array[i].type);
                	parsepush(p);
		}
	}

}



/*
 *  ParsePop() -	Remove the top element from the Parse Stack and
 			return it.
 *			Precondition: the stack is not empty.
 */
struct parsenoderec	*parsepop(void)
{
	if (parseempty())	{
		printf("Parse stack underflow\n");
                exit(10);
	}
	     
	return(pa.parsptr[--pa.top]);
}

/*
 * ParsePush() -	Place the item given as an argument onto the
 *			top of the Parse Stack.
 *			Precondition: the stack is not full
 */
void	parsepush(struct parsenoderec *x)
{
	if (pa.top == MAXPARSESTACK)	{
		printf("Parse stack overflow\n");
                exit(10);
	}
	pa.parsptr[pa.top++] = x;
}

/*
 * ParseEmpty() -	Returns True if the Parse stack is empty,
 *			False if it is not empty.
 */
bool    parseempty()
{
	return(pa.top == 0);
}

/*
 * InitParseStack() -	Initialize the Parse stack by setting top to zero.
 */
void	initparsestack(void)
{
	pa.top = 0;
}


/*
 * MatchToken() -	Returns TRUE of the lookahead token matches
 *			the expected terminal, FALSE if not.
 */
int	matchtoken(int thistoken, int thattoken)
{
	return(thistoken == thattoken);
}

/*
 * PrintNonterm() -	Given its enumeration, print the corresponding
 *			nonterminal.
 */
void	printnonterm(int i)
{
	std::cout << NTstring[i];
}

/*
 * error() -	A catch-all routine for compiling errors.  It prints an
 *		error message including line number and then terminates
 *		execution.
 */
void	error(char message[], int linenum)
{
	printf("%s on line %d\n", message, linenum);
        exit(4);
}

}