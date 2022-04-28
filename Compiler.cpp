#include "Java0DFSAdrv.cpp"
#include "SyntaxAnalyzer.cpp"
#include "CodeGenerator.cpp"
#include <stdio.h>
#include <iomanip>
using std::cout, std::endl;


int main(){
    Java0DFSAdrvMain();
    cout << "Java0DFSA finished" << endl;
    SyntaxMain();
    cout << "Syntax finished" << endl;
    CodeGeneratorMain();
    cout << "codegen finished" << endl;
    return 0;
}