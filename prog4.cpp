#include "parse.h"
#include "lex.h"
#include <iostream>
#include <fstream>
#include <map>
#include <cctype>
#include <bits/stdc++.h>
#include "value.h"
#include "parsetree.h"


using namespace std;

map<string, Val> symbols;

int main(int argc, char* argv[]) {
    //stats
    int linenum = 0;

    //initialize pointer to cin
    istream *in = &cin;
    ifstream file;
    
    if(argc == 2){
        file.open(argv[1]);
        if(file.is_open() == false) {
         cout << "COULD NOT OPEN " << argv[1] << endl;
         return -1;
        }
        
        in = &file;
    }
    
    else if (argc > 2) {
     cout << "TOO MANY FILENAMES" << endl;
        return 1;
    }
    
    ParseTree *prog = Prog(*in, linenum);
    if(prog == 0) {
     return 0;   
    }
    map<string, bool> declaredId;
    

    try
    {
        prog->CheckLetBeforeUse(declaredId);
        prog->Eval();
     
    }
    catch(runtime_error& e)
    {
        //cout << e << endl;
        cout << "RUNTIME ERROR at " << linenum-1 << ": " << e.what() << endl;
    }
    catch(std::string& er){
     cout << er <<endl;   
    }
    catch(logic_error& err){
     cout << err.what() << endl;   
    }
    
    return 0;
}