/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
using std::vector;
using std::map;
using std::string;
using std::runtime_error;
using std::logic_error;

#include "parse.h"
#include "lex.h"
#include "value.h"

extern map<string, Val> symbols;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

// a "forward declaration" for a class to hold values
class Val;

class ParseTree {
protected:
	int			linenum;
	ParseTree	*left;
	ParseTree	*right;

public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}
    /*int Traverse(int input, int (ParseTree::*func)(void)) {
        if (left) 
            input+= left->Traverse(input, func);
        if(right)
            input+= right->Traverse(input, func);
        return Visit(input, func);   
    }
    
    virtual int Visit(int input, int (ParseTree::*func)(void)){
         return input;   
    }*/

	int GetLineNumber() const { return linenum; }
    
   virtual NodeType GetType() const {return ERRTYPE;}

	int MaxDepth() const {
        int ld = 0, rd =0;
        
        if(left)
            ld += left->MaxDepth();
        if(right) 
            rd += right->MaxDepth();
        
        int depth = (ld > rd ? ld : rd) + 1;
        
        return depth;
	}

	virtual bool IsIdent() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual string GetId() const { return ""; }
    virtual int IsBang() const { return 1; }
    virtual bool IsLet() const { return false; }
    

	int BangCount() const {
        int count = 0;
        
        if(left) 
            count += left->BangCount();
        
        if(right)
            count += right->BangCount();
        
        count += IsBang();
        
        return count;
	}

	virtual int CheckLetBeforeUse(map<string, bool>& var) {
        int count = 0;
            if(left)
                count += left->CheckLetBeforeUse(var);
            if(right)
                count += right->CheckLetBeforeUse(var);
            if(IsLet())
                var[this->GetId()] = true;
            
            if(IsIdent() && var.find(GetId()) == var.end()){
               throw logic_error("UNDECLARED VARIABLE " + GetId());
                count++;   
            }
        return count;
	}
    
   virtual Val Eval() const = 0; 

};

class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
    
    Val Eval() const override {
     left->Eval();
     if(right)
         right->Eval();
        
      return Val();
        
    }
};

class Let : public ParseTree {
	string id;

public:
	Let(Lex& t, ParseTree *e) : ParseTree(t.GetLinenum(), e), id(t.GetLexeme()) {}

	string GetId() const { return id; }
	bool IsLet() const { return true; }
    
    Val Eval() const override {
            cout << this->GetLineNumber() << endl;
            Val l = left->Eval();
                if(symbols.count(id) && !symbols.empty())
                 symbols[id] = l ;
        
    else 
        symbols.insert(pair<string, Val>(id, l));
       
    return Val();
    }
};

class Print : public ParseTree {
    public:
    Print(int line, ParseTree *ex) : ParseTree(line, ex) {}
    
    Val Eval() const override {
    cout << this->GetLineNumber() << endl;
     Val l = left->Eval();
        if(l.isErr())
         cout << "RUNTIME ERROR at " << this->GetLineNumber() << ": " << l << endl;
        else 
            cout << l;
        return Val();
        
    }
    
    
};

class Loop : public ParseTree {
    public:
    Loop(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
    
     Val Eval() const override {
     Val result = left->Eval();
     if(result.isInt())
         while(result.ValInt() != 0){
          right->Eval();
          result = left->Eval();
             
         }
         //if(result.isStr())
           //  throw string("Expression is not an integer");
        return Val();
    }
};

class If : public ParseTree {
    public:
    If(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
    Val Eval() const override{
     Val result = left->Eval();
        
        if(result.isInt() == true)
            if(result.ValInt() != 0)
                return right->Eval();
        if(result.isStr())
            throw runtime_error("Expression is not an integer");
        
       return Val();
    }
};


class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    bool isOp() const {return true;}
    
    Val Eval() const override{
        return left->Eval() + right->Eval();    
    }
};

class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    bool isOp() const {return true;}
    Val Eval() const override {
        return left->Eval() - right->Eval();    
    }
};

class TimesExpr : public ParseTree {
    public:
    TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    bool isOp() const {return true;}
    Val Eval() const override{
        return left->Eval() * right->Eval();    
    }
};

class DivideExpr : public ParseTree {
    public:
    DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    bool isOp() const {return true;}
    Val Eval() const override{
        return left->Eval() / right->Eval();    
    }
};

class BangExpr : public ParseTree {
    public:
    BangExpr(int line, ParseTree *l) : ParseTree(line,l) {}
    int IsBang() const {return 1;}
    
    virtual Val Eval() const override {
     Val rev = !(left->Eval());
     return rev;   
    }
};

class IConst : public ParseTree {
	int val;

public:
    IConst(int l, int i) : ParseTree(l), val(i){}
	IConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
   Val Eval() const override{
        return Val(val);    
    }
    NodeType GetType() const { return INTTYPE;}
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Lex& t ) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}
    bool isString() const { return true;}
    NodeType GetType() const { return STRTYPE;}
    Val Eval() const override {
        return Val(val);    
    }
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Lex& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
    
    virtual Val Eval() const override {
        if(symbols.count(id) && !symbols.empty())
            return symbols[id];
        else
            throw runtime_error("Symbol " + id + " is not defined");
        
    }
};
#endif /* PARSETREE_H_ */
