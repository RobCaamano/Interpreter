/* Implementation of Recursive-Descent Parser
 * Programming Assignment 3
 * Roberto Caamano
*/

#include "parseInt.h"


map<string, bool> dVar;
map<string, Token> SymbolTable;
map<string, Value> TempsResults;

queue <Value> * ValQue;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int err_count = 0;

int ErrCount()
{
    return err_count;
}

void ParseError(int line, string message)
{
	++err_count;
	cout << line << ": " << message << endl;
}


bool Prog(istream& in, int& line)
{
	bool dl = false;
	LexItem tok = Parser::GetNextToken(in, line);

	
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			dl = StmtList(in, line);
			if( !dl  )
			{
				ParseError(line, "Incorrect Syntax in the Program");
				return false;
			}
				
			tok = Parser::GetNextToken(in, line);
			
			if (tok.GetToken() == END) {
				tok = Parser::GetNextToken(in, line);
				
				if (tok.GetToken() != PROGRAM) {
					
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}	
				return true;
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}	
		}
		else
		{
			ParseError(line, "Missing Program Name.");
				return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return false;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}

bool Stmt(istream& in, int& line){
	bool stat=true;
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case INT: case FLOAT: 
		Parser::PushBackToken(t);
		stat = DeclStmt(in, line);
		if(!stat)
		{
			ParseError(line, "Incorrect Declaration Statement.");
			return stat;
		}
		break;
	case IF: case WRITE: case IDENT: 
		Parser::PushBackToken(t);
		stat = ControlStmt(in, line);
		if(!stat)
		{
			ParseError(line, "Incorrect control Statement.");
			return stat;
		}
		break;
	default:
		Parser::PushBackToken(t);
	}
	return stat;
}

bool StmtList(istream& in, int& line){
	bool stat;
	LexItem tok;
	stat = Stmt(in, line);
	if(stat)
	{
		tok = Parser::GetNextToken(in, line);
		while(tok == SEMICOL && stat)
		{
			stat = Stmt(in, line);
			tok = Parser::GetNextToken(in, line);
		}
        
		if(tok == END && stat)
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else if( tok == SEMICOL && !stat)
		{
			ParseError(line, "Syntactic error in Statement.");
			return false;
		}
		else if(tok != SEMICOL && tok != END && stat)
		{
			ParseError(line, "Missing a semicolon.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(tok);
	}	
	return stat;
	
}

bool ControlStmt(istream& in, int& line) {
	bool stat;

	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case WRITE:
		stat = WriteStmt(in, line);
		break;

	case IF:
		stat = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        stat = AssignStmt(in, line);
		
		break;
		
		
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return stat;
}

bool DeclStmt(istream& in, int& line) {
	bool stat = false;
	LexItem tok;
	LexItem t = Parser::GetNextToken(in, line);
	
	if(t == INT || t == FLOAT ) {
		stat = IdentList(in, line, t);
		if (!stat)
		{
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return stat;
		}
	}
	else{
		Parser::PushBackToken(t);
		ParseError(line, "Incorrect Type.");
		return false;
	}
	return true;
}

bool AssignStmt(istream& in, int& line) {
    bool varstat = false, stat = false;
    LexItem t;
    varstat = Var(in, line, t);
    string identifier = t.GetLexeme();
    Token type = SymbolTable[identifier];

    Value val;

    if (varstat) {
        t = Parser::GetNextToken(in, line);
        if (t == ASSOP) {
            stat = Expr(in, line, val);
            if (!stat) {
                ParseError(line, "Missing Expression in Assignment Statment");
                return stat;
            }

            if((type==FLOAT||type==INT) && val.IsChar()){
                ParseError(line, "Illegal Assignment Operation");
                return false;
            }
            if(type==FLOAT && val.IsInt()){
                TempsResults[identifier] = Value((int)val.GetInt());
            }
            else if(type==INT && val.IsReal()){
                TempsResults[identifier] = Value((int)val.GetReal());
            }
            else {
                TempsResults[identifier] = val;
            }
        }
        else if (t.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        }
        else {
            ParseError(line, "Missing Assignment Operator =");
            return false;
        }
    }
    else {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    return stat;
}

bool WriteStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after Write");
        while(!(*ValQue).empty()){
            ValQue->pop();
        }
        delete ValQue;
		return false;
	}
	

    while (!(*ValQue).empty()) {
        std::cout << ValQue->front();
        ValQue->pop();
    }
    cout << "\n";
	return ex;
}

bool IfStmt(istream& in, int& line) {
	bool ex=false ; 
	LexItem t;
	Value newval, stmtval;
	if( (t=Parser::GetNextToken(in, line)) != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	ex = LogicExpr(in, line, newval);
	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}

	if((t=Parser::GetNextToken(in, line)) != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	if(newval.IsBool())
	{
		if(newval.GetBool() )
		{
			bool st = ControlStmt(in, line);
			if( !st ) {
				ParseError(line, "Missing statement for if");
				return false;
			}
		}
		else
		{
			t = Parser::GetNextToken(in, line);
			while (t != SEMICOL)
			{
				t = Parser::GetNextToken(in, line);
			}
			Parser::PushBackToken(t);
		}
		
	}
	else{
		ParseError(line, "Run-Time Error-Illegal Type for If statement condition");
		return false;
	}
	
	return true;
}

bool Var(istream& in, int& line, LexItem& tok)
{
	string identStr;
	tok = Parser::GetNextToken(in, line);
	if (tok == IDENT){
		identStr = tok.GetLexeme();
		
		if (!(dVar.find(identStr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}

bool Expr(istream& in, int& line, Value& retVal) {

    Value val, difVal1;
	bool term = Term(in, line, val);
	LexItem tok;
	
	if( !term ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) 
	{
        term = Term(in, line, difVal1);
		if( !term ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}	
		
        if(tok == PLUS){
            if((val.IsInt() || val.IsReal()) && (difVal1.IsInt() || difVal1.IsReal()) ){
                val = val + difVal1;
            }
            else{
                ParseError(line, "Run-Time Error-Invalid Addition Operation");
                return false;
            }
        }
        else if(tok == MINUS){
            if((val.IsInt() || val.IsReal()) && (difVal1.IsInt() || difVal1.IsReal()) ){
                val = val - difVal1;
            }
            else{
                ParseError(line, "Run-Time Error-Invalid Subtraction Operation");
                return false;
            }
        }
        
        tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}	
	}
    retVal = val;
	Parser::PushBackToken(tok);
	return true;
}

bool LogicExpr(istream& in, int& line, Value & retVal)
{
	Value difVal1, difVal2;
	bool term = Expr(in, line, difVal1);
	LexItem tok;
	
	if( !term ) {
		return false;
	}
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == GTHAN  || tok == EQUAL) 
	{
		term = Expr(in, line, difVal2);
		if( !term ) 
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
		if(tok == GTHAN)
		{
			retVal = (difVal1 > difVal2);
		}
		else if(tok == EQUAL)
		{
			retVal = (difVal2 == difVal2);
		}
		if(retVal.GetType() == VERR)
		{
			ParseError(line, "Run-Time Error-Illegal Mixed Type operation");
			return false;
		}
		return true;
	}
	Parser::PushBackToken(tok);
	return true;
}

bool ExprList(istream& in, int& line) {
	bool stat = false;

    Value val;
	stat = Expr(in, line, val);
	if(!stat){
		ParseError(line, "Missing Expression");
		return false;
	}
    ValQue->push(val);
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		stat = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return stat;
}

bool IdentList(istream& in, int& line, LexItem type) {
	bool stat = false;
	string identStr;
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{

		identStr = tok.GetLexeme();
		if (!(dVar.find(identStr)->second))
		{
			dVar[identStr] = true;
			SymbolTable[identStr] = type.GetToken();
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		ParseError(line, "Missing Variable");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		stat = IdentList(in, line, type);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return stat;
}

bool Term(istream& in, int& line, Value& retVal) {
    Value val, difVal1;
	bool term = SFactor(in, line, val);
	LexItem tok;

	if( !term ) {
		return false;
	}
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  || tok == REM)
	{
        term = SFactor(in, line, difVal1);

		if( !term ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		

        if(tok == MULT){
            
            if((val.IsInt() || val.IsReal()) && (difVal1.IsInt() || difVal1.IsReal()) ){
                val = val * difVal1;
            }
            else{
                ParseError(line, "Run-Time Error-Invalid Multiplication Operation");
                return false;
            }
        }
        else if(tok == DIV){
            if(difVal1.GetInt() == 0){
                ParseError(line, "Run-Time Error-Illegal Division By 0");
                return false;
            }
            if((val.IsInt() || val.IsReal()) && (difVal1.IsInt() || val.IsReal()) ){
                val = val / difVal1;
            }
            else{
                ParseError(line, "Run-Time Error-Invalid Division Operation");
                return false;
            }
        }
        else if(tok == REM){
            if(val.IsInt() && difVal1.IsInt()){
                val = val % difVal1;
            }
            else{
                ParseError(line, "Run-Time Error-Invalid Remainder Operation");
                return false;
            }
        }
        
        tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
    retVal = val;
	Parser::PushBackToken(tok);
	return true;
}

bool Factor(istream& in, int& line, int sign, Value& retVal) {
	LexItem tok = Parser::GetNextToken(in, line);
	
	if( tok == IDENT ) {
		string lexeme = tok.GetLexeme();
		if (!(dVar.find(lexeme)->second))
		{
			ParseError(line, "Undefined Variable");
			return false;	
		}
        retVal = TempsResults[lexeme];
		return true;
	}
	else if( tok == ICONST ) {
		retVal = Value(std::stoi(tok.GetLexeme()) * sign);
		return true;
	}
	else if( tok == SCONST ) {
		retVal = Value(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) {
		retVal = Value(std::stof(tok.GetLexeme()) * sign);
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return false;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
		ParseError(line, "Unrecognized input");
	return 0;
}

bool SFactor(istream& in, int& line, Value& retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool stat;
	int sign = 1;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);
		
	stat = Factor(in, line, sign, retVal);
	return stat;
}