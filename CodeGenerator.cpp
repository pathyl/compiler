#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string.h>
#include <fstream>
#include <deque>
#include <sstream>
#include <stack>
#include <vector>
#include <sstream>
void CodeGeneratorMain();
using std::cout, std::cin, std::endl, std::string, std::fstream, std::stack, std::deque, std::vector, std::to_string;

fstream quads;
fstream myAsm;
fstream templates;
fstream symbolTable;
const string OPCODES[] = {"=", "+", "-", "*", "/", "ODD", ">", "<", ">=", "<=", "==", "!=", "GET", "PUT", "jmp", "<label>"};
int OPCODES_size = 16;

class Quad{
    public:
    string quad1;
    string quad2;
    string quad3;
    string quad4;
    Quad(string q1, string q2, string q3, string q4){
        quad1 = q1;
        quad2 = q2;
        quad3 = q3;
        quad4 = q4;
    }
    Quad(){
        
    }
    private:
    
};

class constHolder{
    public:
    string name;
    string value;
};

void CodeGeneratorMain(){
    string q;
    string line;
    deque<string> storeQuad;
    deque<Quad> quadDeque;
    Quad currQuad;
    constHolder newConst;
    stack<constHolder> constStack;
    stack<string> varStack;
    int k = 0;
    char ch;

    //read quads in
    quads.open("quads.txt", fstream::in);
    myAsm.open("myAssembly.asm", fstream::out);
    myAsm.close();
    myAsm.open("myAssembly.asm", fstream::app);
    templates.open("newassembly1.txt", fstream::in);
    while(std::getline(templates, line)){
        myAsm << line << "\n";
    }
    templates.close();

    //read vars into varStack and CONST's into constStack from symbol table
    symbolTable.open("symboltable.txt", fstream::in);

    //clear the header line
    std::getline(symbolTable, line);
    while(std::getline(symbolTable,line,'|')){
        std::getline(symbolTable,line,'|');
        //q now contains a name
        q = line;
        std::getline(symbolTable,line,'|');
        //line now contains the Class
        if(line == "$CONST"){
            newConst.name = q;
            std::getline(symbolTable,line,'|');
            //line now contains the value
            newConst.value = line;
            constStack.push(newConst);
        }else if(line == "<var>"){
            varStack.push(q);
        }else if(line == "$NUMLIT"){
            line = "lit" + q;
            newConst.name = line;
            newConst.value = q;
            constStack.push(newConst);
        }
        //clear the rest of the line
        std::getline(symbolTable,line);
    }
    symbolTable.close();
    //read constants and literals from symbol table and append them to myAsm here.
    myAsm << "\n;Begin adding constants\n";
    while(!constStack.empty()){
        myAsm << "\t" << constStack.top().name << "\tDW\t" << constStack.top().value << "\n";
        constStack.pop(); 
    }


    templates.open("newassembly2.txt", fstream::in);
    while(std::getline(templates, line)){
        myAsm << line << "\n";
    }
    templates.close();

    //read regular vars from symbol table and append them to myAsm here.
    myAsm << "\n;Begin adding vars\n";
    while(!varStack.empty()){
        myAsm << "\t" << varStack.top() << "\tRESW\t1\n";
        varStack.pop();
    }
    //read quads in from file
    while(std::getline(quads, line)){
        std::stringstream ss(line);
        while(std::getline(ss, q, ',')){
            storeQuad.push_back(q);
        }
    }
    //store the quads in a deque
    while(!storeQuad.empty()){
        currQuad.quad1 = storeQuad.front();
        storeQuad.pop_front();
        currQuad.quad2 = storeQuad.front();
        storeQuad.pop_front();
        currQuad.quad3 = storeQuad.front();
        storeQuad.pop_front();
        currQuad.quad4 = storeQuad.front();
        storeQuad.pop_front();
        quadDeque.push_back(currQuad);
    }

    myAsm << "section .text\n_start:\tnop\n";
    myAsm.close();

    while(!quadDeque.empty()){
        currQuad = quadDeque.front();
        quadDeque.pop_front();
        k = -1;
        for (int i = 0; i < OPCODES_size; i++)
            {
                if (OPCODES[i] == currQuad.quad1)
                {
                    k = i;
                }
            }
            if(isdigit(currQuad.quad2[0])){
                line = "lit" + currQuad.quad2;
                currQuad.quad2 = line;
            }
            if(isdigit(currQuad.quad3[0])){
                line = "lit" + currQuad.quad3;
                currQuad.quad3 = line;
            }

        switch(k){
            //= 

            case 0: line = "\tmov ax, [" + currQuad.quad3 + "]\n" + "\tmov [" + currQuad.quad2 + "], ax\n";
                        cout << "in assign quad3" << currQuad.quad3 << " quad2" << currQuad.quad2 << " k:" << to_string(k) << endl;
            break;
            //+
            case 1: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tadd [" + currQuad.quad3 + "], ax\n" + "\tmov [" + currQuad.quad4 + "], ax\n";
            break;
            //-
            case 2: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tsub ax, [" + currQuad.quad3 + "]\n" + "\tmov [" + currQuad.quad4 + "], ax\n";
            break;
            //* mul automatically multiplies against ax and stores result in ax
            case 3: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tmov bx, [" + currQuad.quad3 + "]\n" + "\tmul bx\n" + "\tmov [" + currQuad.quad4 + "], ax\n"; 
            break;
            // /
            case 4: line = "\tmov dx, 0\n\tmov ax, [" + currQuad.quad2 + "]\n" + "\tmov bx, [" + currQuad.quad3 + "]\n" + "\tdiv bx\n" + "\tmov [" + currQuad.quad4 + "], ax\n"; 
            break;
            //ODD - not listed in the lab at all, ignore and do not use ODD
            case 5: cout << "ERROR - ODD not implemented." << endl;
            break;
            //> the next quad in the deque will have the label L# in quad2, which we get here and pop
            case 6: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tcmp ax, [" + currQuad.quad3 + "]\n" + "\tjle " + quadDeque.front().quad2 + "\n";
                    quadDeque.pop_front();
            break;
            //<
            case 7: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tcmp ax, [" + currQuad.quad3 + "]\n" + "\tjge " + quadDeque.front().quad2 + "\n";
                    quadDeque.pop_front();
            break;
            //>=
            case 8: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tcmp ax, [" + currQuad.quad3 + "]\n" + "\tjl " + quadDeque.front().quad2 + "\n";
                    quadDeque.pop_front();
            break;
            //<=
            case 9: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tcmp ax, [" + currQuad.quad3 + "]\n" + "\tjg " + quadDeque.front().quad2 + "\n";
                    quadDeque.pop_front();
            break;
            //==
            case 10: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tcmp ax, [" + currQuad.quad3 + "]\n" + "\tjne " + quadDeque.front().quad2 + "\n";
                    quadDeque.pop_front();
            break;
            //!=
            case 11: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tcmp ax, [" + currQuad.quad3 + "]\n" + "\tje " + quadDeque.front().quad2 + "\n";
                    quadDeque.pop_front();
            break;
            //GET
            case 12: line = "\tcall PrintString\n\tcall GetAnInteger\n\tmov ax,[ReadInt]\n\tmov [" + currQuad.quad2 + "], ax\n";
            break;
            //PUT need answer in ax register, call ConvertIntegerToString, call PutInteger
            case 13: line = "\tmov ax, [" + currQuad.quad2 + "]\n" + "\tcall ConvertIntegerToString\n" + "\tcall PutInteger\n";
            break;
            //jmp
            case 14: line = "\tjmp " + currQuad.quad2 + "\n";
            break;
            //<label>
            case 15: line = currQuad.quad2 + ":\tnop\n";
            break;
            default: cout << "ERROR while generating code for quad:" << currQuad.quad1 << "," << currQuad.quad2 << "," << currQuad.quad3 << "," << currQuad.quad4 << "\n";
            break;
        }
        myAsm.open("myAssembly.asm", fstream::app);
        myAsm << line;
        myAsm.close();
    }

    //append the IO routines
    myAsm.open("myAssembly.asm", fstream::app);
    templates.open("newassembly3.txt", fstream::in);
    while(std::getline(templates, line)){
        myAsm << line << "\n";
    }
    templates.close();
    myAsm.close();

}