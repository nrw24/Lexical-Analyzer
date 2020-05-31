/*
 * parse.cpp
 */

#include "parse.h"


namespace Parser {
bool pushed_back = false;
Lex	pushed_token;

static Lex GetNextToken(istream& in, int& line) {
	if( pushed_back ) {
		pushed_back = false;
		return pushed_token;
	}
	return getNextToken(in, line);
}

static void PushBackToken(Lex& t) {
	if( pushed_back ) {
		abort();
	}
	pushed_back = true;
	pushed_token = t;
}

}

static int error_count = 0;

void
ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

ParseTree *Prog(istream& in, int& line)
{
	ParseTree *sl = Slist(in, line);
    
	if( sl == 0 )
		ParseError(line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}

// Slist is a Statement followed by a Statement List
ParseTree *Slist(istream& in, int& line) {
	ParseTree *s = Stmt(in, line);
	if( s == 0 )
		return 0;
    
	return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream& in, int& line) {
    ParseTree *s = 0;
    Lex t = Parser::GetNextToken(in, line);
    switch (t.GetToken()) {
        case IF:
            s = IfStmt(in, line);
            break;
            
        case LET:
            s = LetStmt(in, line);
            break;
            
        case PRINT:
            s = PrintStmt(in, line);
            break;
            
        case LOOP:
            s = LoopStmt(in, line);
            break;
        
        case END:
            Parser::PushBackToken(t);
            return 0;
        
        case DONE:
            return 0;
                  
        default:
            ParseError(line, "Invalid Token");
            return 0;
}
    if(Parser::GetNextToken(in,line) != SC) {
     ParseError(line, "Missing semicolon");
      delete s;
      return 0;
    }
    return s;
}

ParseTree *LetStmt(istream& in, int& line) {
    Lex token = Parser::GetNextToken(in, line);
    
    if(token != ID){
     ParseError(line, "Missing ID after LET");
     return 0;
    }
    
    ParseTree *ex = Expr(in, line);
        if (ex == 0) {
            ParseError(line, "Missing expression after id");
            return 0;

    }
    
    return new Let(token, ex);
}

ParseTree *PrintStmt(istream& in, int& line) {
    int l = line;
    ParseTree *ex = Expr(in, line);
        if (ex == 0) {
            ParseError(line, "Missing expression after PRINT");
            return 0;
}
    return new Print(l, ex);
}
ParseTree *LoopStmt(istream& in, int& line) {
    int l = line;
    ParseTree *ex = Expr(in, line);
    if (ex == 0) {
        ParseError(line, "Missing expression after LOOP");
        return 0;
        
}
    Lex token = Parser::GetNextToken(in, line);
    
    if( token != BEGIN) {
     ParseError(line, "Missing BEGIN after expression");
     return 0;
    }
    
    ParseTree *statement = Slist(in, line);
    
    if(statement == 0) {
     ParseError(line, "Missing statement list after BEGIN");
     return 0;   
        
    }
    if(Parser::GetNextToken(in, line) != END) {
     ParseError(line, "Missing END after statement list");
        return 0;
    }
    
    return new Loop(l, ex, statement);
}

ParseTree *IfStmt(istream& in, int& line) {
   // int l = line;
    ParseTree *ex = Expr(in, line);
    if (ex == 0) {
        ParseError(line, "Missing expression after if");
        return 0;
        
}
    Lex token = Parser::GetNextToken(in, line);
    
    if( token != BEGIN) {
     ParseError(line, "Missing BEGIN after expression");
     return 0;
    }
    
    ParseTree *statement = Slist(in, line);
    
    if(statement == 0) {
     ParseError(line, "Missing statement list after BEGIN");
     return 0;   
        
    }
    if(Parser::GetNextToken(in, line) != END) {
     ParseError(line, "Missing END after statement list");
        return 0;
    }
    
    return new If(token.GetLinenum(), ex, statement);
}
ParseTree *Expr(istream& in, int& line) {
	ParseTree *t1 = Prod(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Lex t = Parser::GetNextToken(in, line);

		if( t != PLUS && t != MINUS ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Prod(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new PlusExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2);
	}
}

ParseTree *Prod(istream& in, int& line) {
    ParseTree *t1 = Rev(in, line);
    
    if(t1 == 0) {
     return 0;   
    }
    
    while(true) {
     Lex token = Parser::GetNextToken(in, line);
        
     if (token != STAR && token != SLASH) {
      Parser::PushBackToken(token);
      return t1;
     }
     
     ParseTree *t2 = Rev(in, line);
     
     if (t2 == 0) {
      ParseError(line, "Missing expression after operator.");
      return 0;
     }
     
     if (token == STAR) {
      t1 = new TimesExpr(token.GetLinenum(), t1, t2);   
     }
     else {
      t1 = new DivideExpr(token.GetLinenum(), t1 , t2);   
     }
    }
}

ParseTree *Rev(istream& in, int& line) {
    Lex t = Parser::GetNextToken(in, line);
    
    if(t == BANG){
        ParseTree *p1 = Rev(in, line);
        if(p1 == 0) {
         ParseError(line, "Missing expersion after operator!");
            return 0;
            
        }
        return new BangExpr(t.GetLinenum(), p1);
            
    }
    else{
        Parser::PushBackToken(t);
        return Primary(in, line);
        
    }
}

ParseTree *Primary(istream& in, int& line) {
	Lex t = Parser::GetNextToken(in, line);

	if( t == ID ) {
		return new Ident(t);
	}
	else if( t == INT ) {
		return new IConst(t);
	}
	else if( t == STR ) {
		return new SConst(t);
	}
	else if( t == LPAREN ) {
		ParseTree *ex = Expr(in, line);
		if( ex == 0 ) {
			ParseError(line, "Missing expression after (");
			return 0;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return 0;
	}

	ParseError(line, "Primary expected");
	return 0;
}

