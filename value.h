#ifndef VAL_H
#define VAL_H

#include <string>
#include <cstdlib>

using std::string;
using std::runtime_error;

class Val {
    int        i;
    string    s;
    enum ValType { ISINT, ISSTR, ISERR } vt;
    
public:
    Val() : i(0), vt(ISERR) {}
    Val(int i) : i(i), vt(ISINT) {}
    Val(string s) : i(0), s(s), vt(ISSTR) {}
    Val(ValType vt, string errmsg) : i(0), s(errmsg), vt(ISERR) {}
    
    ValType getVt() const { return vt; }
    
    bool isErr() const { return vt == ISERR; }
    bool isInt() const { return vt == ISINT; }
    bool isStr() const { return vt == ISSTR; }
    
    int ValInt() const {
        if( isInt() ) return i;
        throw runtime_error("This Val is not an Int");
    
    }
    string ValString() const {
        if( isStr() ) return s;
        throw runtime_error("This Val is not a Str");
       
    }
    
  
    
    friend ostream& operator<<(ostream& out, const Val& v) {
        // IMPLEMENT
        if(v.isErr())
            out << v.s;
        
        if(v.isInt())
            out << v.i;
        
        if(v.isStr())
            out << v.s;
		return out;
    }
    
    string GetErrMsg() const {
        if( isErr() ) return s;
        throw runtime_error("This Val is not an Error");
    }
   
    Val operator+(const Val& op) const {
        if( isInt() && op.isInt() )
            return ValInt() + op.ValInt();
        if( isStr() && op.isStr() )
            return ValString() + op.ValString();
        return Val(ISERR, "Type mismatch on operands of +");
    }
    
    Val operator-(const Val& op) const {
        // IMPLEMENT
    if( isInt() && op.isInt() )
            return ValInt() - op.ValInt();
        return Val(ISERR, "Type mismatch on operands of -");
    }
    
    Val operator*(const Val& op) const {
        // IMPLEMENT
    if( isInt() && op.isInt() )
            return ValInt() * op.ValInt();
        if( isStr() && op.isInt() ){
            if(op.ValInt() >=0){
            string str = "";
            if(op.ValInt() > 0)
            for(int i = 0; i < op.ValInt(); ++i) {
                str+=s;
              
            }
                return str;
            }
            else 
                throw runtime_error("Cannot multiply string by negative int");
            }
        if(isInt() && op.isStr()){
            if(ValInt() >=0) {
         string str = "";
                if(ValInt() > 0)
            for(int i = 0; i < ValInt(); ++i) {
                str+=op.ValString();
              
            }
                return str;
                
            }
            else
                throw runtime_error("Cannot multiply string by negative int");
        }
        
        return Val(ISERR, "Type mismatch on operands of *");
    }
    
    Val operator/(const Val& op) const {
        // IMPLEMENT
    if( isInt() && op.isInt() ){
        if(op.ValInt() != 0)
            return ValInt()/op.ValInt();
        if(op.ValInt() == 0) 
            throw runtime_error("Divide by zero error");
    }
        return Val(ISERR, "Type mismatch on operands of /");
    }
    
    Val operator!() const {
         // IMPLEMENT
        int rev;
        if(isInt()){
        rev = 0;
        int num = abs(i);
        while(num > 0) {
         rev = rev *10 + num%10;
         num/=10;
    }
        if(i < 0)
        return Val(-rev);
         
        return Val(rev);
        }
        if(isStr()){
            string str = ValString();
            int n = str.length();
            string s = "";
            for(int i = n; i > -1; --i)
            s += str[i];
            return s;
    
}         
        return Val();

}

    
    
};
#endif