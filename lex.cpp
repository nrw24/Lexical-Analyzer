/* 
 * lex.cpp
 *
 * CS280 - Fall 2019
 */

#include <map>
#include <cctype>
#include <bits/stdc++.h>
#include "lex.h"


using namespace std;

static map<string, Token> keywords = {
    {"if", IF},
    {"print", PRINT},
    {"let", LET},
    {"loop", LOOP},
    {"begin", BEGIN},
    {"end", END},
    
};
static map<Token, string> printTok = {
		{ IF, "IF" },
		{ LET, "LET" },
		{ PRINT, "PRINT" },
		{ LOOP, "LOOP" },
		{ BEGIN, "BEGIN" },
		{ END, "END" },

		{ ID, "ID" },
        { BANG, "BANG"},
		{ INT, "INT" },
		{ STR, "STR" },

		{ PLUS, "PLUS" },
		{ MINUS, "MINUS" },
		{ STAR, "STAR" },
		{ SLASH, "SLASH" },
		{ LPAREN, "LPAREN" },
		{ RPAREN, "RPAREN" },

		{ SC, "SC" },
    

		{ ERR, "ERR" },

		{ DONE, "DONE" }
};
ostream& operator<<(ostream& out, const Lex& tok) 
{
 Token t = tok.GetToken();
 out << printTok[t]; 
     if (t == ID || t == INT || t == STR || t == ERR) {
		out << "(" << tok.GetLexeme() << ")";
	}
	return out;

}


Lex checkToken(const string& lexeme, int linenum) {
    Token t = ID;
    
    auto kw = keywords.find(lexeme);
    if(kw != keywords.end()) 
        t = kw->second;
    
    return Lex(t, lexeme, linenum);
    
}
Lex getNextToken(istream& in, int& linenum) {
    
    enum LexState { BEGIN, INID, INSTRING, ININT, INCOMMENT }
    state = BEGIN;
    string lexeme;
    char c;
    int comments = 0;
    //while lex != ERR || lex != DONE get nextToken
 bool escape = false;
    while(in.get(c) ) {
    switch(state){
       case BEGIN:
                if ( c == '\n') {
                    linenum++;
                }
            
                if ( isspace(c) ) {
                    continue;    
                }
            
                lexeme = c;
               
                
                if( isalpha(c) ) {
                    state = INID;   
               
                }
                else if (isdigit(c) ) { 
                           state = ININT;   
                }
                 else if( c == '"') {
                           state = INSTRING;   
                           escape = false;

                       }
            //Comment
                else if (c == '/' && in.peek() == '/') {
                         state = INCOMMENT;
                         comments++;
                         break;
                }
                    
                
                else {
                    Token token = ERR;
                    switch (c) {
                        case '+':
                            token = PLUS; 
                            break;
                        case '-':
                            token = MINUS;
                            break;
                        case '*':
                            token = STAR;
                            break;
                        case '/':
                            token = SLASH;
                            break;
                        case '!':
                            token = BANG;
                            break;
                        case '(':
                            token = LPAREN;
                            break;
                        case ')':
                            token = RPAREN;
                            break;
                        case ';':
                            token = SC;
                            break; 
                    }
                    return Lex(token, lexeme, linenum);
                    
                }
               break;
            
                   case INSTRING:
                        
                        if (escape) {
                         escape = false;
                         if (c == 'n')
                             c = '\n';
                            lexeme += c;
                            break;
                        }
                        
            if (c == '\\') {
				escape = true;
				break;
			}
                
                        lexeme += c;
            
                        if (c == '\n') {
				return Lex(ERR, lexeme, linenum);
			}
            
			if (c == '"') {
				lexeme = lexeme.substr(1, lexeme.length() - 2);
				return Lex(STR, lexeme, linenum);
			}
			break;
            
                   case INID:
                       if (isalpha(c) || isdigit(c)) {
				lexeme += c;
			}
			else {
				in.putback(c);
				return checkToken(lexeme, linenum);
			}
			break;
       
                  case ININT:
                        if (isdigit(c)) {
				lexeme += c;
			}
			/*else if (isalpha(c)) {
				lexeme += c;
				return Lex(ERR, lexeme, linenum);
			}*/
			else {
				in.putback(c);
				return Lex(INT, lexeme, linenum);
			}
			break;
            
        case INCOMMENT:
            if (c == '\n') {
             ++linenum;
             //in.putback(c);
             state = BEGIN;
            
            }
        break;
    }
   
}
     if (in.eof())
		return Lex(DONE, "", linenum);
	return Lex(ERR, "", linenum);
}
