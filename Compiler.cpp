#include "Java0DFSAdrv.cpp"
#include "SyntaxAnalyzer.cpp"
#include "CodeGenerator.cpp"
#include <stdio.h>
#include <iomanip>
using std::cout, std::endl;


int main(){
    Java0DFSAdrvMain();
    cout << "Java0DFSA (tokenizer) finished" << endl;
    SyntaxMain();
    cout << "Syntax finished - quads generated" << endl;
    CodeGeneratorMain();
    cout << "codegen finished - assembly generated" << endl;
    return 0;
}