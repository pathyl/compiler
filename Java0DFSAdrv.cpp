#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
using std::cout, std::cin, std::endl, std::string, std::fstream, std::vector, std::to_string;
void Java0DFSAdrvMain();
const int TOKENIZER_NUM_ROWS = 10;
const int TOKENIZER_NUM_COLS = 18;
const int FIRSTPASS_NUM_ROWS = 9;
const int FIRSTPASS_NUM_COLS = 12;
const string RESERVED_WORDS[] = {"CONST","IF","VAR","THEN","PROCEDURE","WHILE","CALL","DO","ODD","CLASS","ELSE"};
const string FIRST_PASS_CLASS_LIST[] = {"$CLASS","<var>","$LB","$RB","$CONST","$VAR","<assign>","<integer>", "<semi>", "<comma>", "$PROCEDURE"};
const int FIRST_PASS_SIZE = 11;
vector<string> numLitTracker(1);
bool x;
class TableDrivenDFSA{
private:
	int TokenizerTable[TOKENIZER_NUM_ROWS][TOKENIZER_NUM_COLS]; //rows => states, cols => token received(transition).
    int FirstPassTable[FIRSTPASS_NUM_ROWS][FIRSTPASS_NUM_COLS];
    int FirstPassState = 0;
    int SymbolTableCounter = 0;
    int codeSegmentAddr = 0;
    int dataSegmentAddr = 0;
    string savedToken;
    fstream sourceFile;
    fstream outputTokenList;
    fstream outputSymbolTable;
    fstream outputDiagnostic;
    string sourceFileName, outputFileName;

public:
    //Given 1 dimensional int arrays representing the FSA, build the 2 dimensional array to make the decision tables.
	TableDrivenDFSA(){
        /*0L 1D	2/	3*	4=	5!	6<	7>	8(	9)	10{	11}	12+	13-	14,	15;	16whitespace 17Unrecognized*/
        int tokenizerData[]=
        {5, 2,	6,	12,	1,	9,	3,	4,	17,	18,	19,	20,	21,	22,	23,	24,	0,  0,
        16,	16,	16,	16,	15,	16,	16,	16,	16,	16,	16,	16,	16,	16,	16,	16,	16, 16,
        13,	2,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13, 13,
        28,	28,	28,	28,	26,	28,	28,	28,	28,	28,	28,	28,	28,	28,	28,	28,	28, 28,
        27,	27,	27,	27,	25,	27,	27,	27,	27,	27,	27,	27,	27,	27,	27,	27,	27, 27,
        5,	5,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14, 14,
        11,	11,	11,	7,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11, 11,
        7,	7,	7,	8,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,  7,
        7,	7,	10,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,  7,
        -1,	-1,	-1,	-1,	29,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1, -1,
        0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,   0
        };
        /*$CLASS,<var>,$LB,$RB,$CONST,$VAR,<assign>,<int>,<semi>,<comma>,$PROCEDURE,othertoken*/
        int firstPassData[]=
        {1,	0,	0,	0,	3,	7,	0,	0,	0,	0,	1,	0,
        -1,	2,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	0,
         0,	0,	0,   0,	0,	0,	0,	0,	0,	0,	0,	0,
        -1,	4,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	0,
        -1,	-1,	-1,	-1,	-1,	-1,	5,	-1,	-1,	-1,	-1,	0,
        -1,	-1,	-1,	-1,	-1,	-1,	-1,	6,	-1,	-1,	-1,	0,
        -1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	0,	3,	-1,	6,
        -1,	8,-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	0,
         0,	0,	0,	0,	0,	0,	0,	0,	0,	7,	0,	0
        };
        //create table for initial tokenizer FSA
		for(int row = 0; row < TOKENIZER_NUM_ROWS; row++)
			for(int col = 0; col < TOKENIZER_NUM_COLS; col++)
				TokenizerTable[row][col] = tokenizerData[row*TOKENIZER_NUM_COLS+col];

        //create table for first pass FSA
        for(int row = 0; row < FIRSTPASS_NUM_ROWS; row++)
			for(int col = 0; col < FIRSTPASS_NUM_COLS; col++)
				FirstPassTable[row][col] = firstPassData[row*FIRSTPASS_NUM_COLS+col];
    }
    //take a single character passed to it and output the corresponding column number in TokenizerTable.
    int CharToColNum(char c){
       /* COLS 0Letter 1Digit 2/ 3*	4= 5! 6< 7>	8(	9) 10{ 11} 12+ 13- 14, 15; 16whitespace 17Unrecognized
       ASCII Codes 65-90=A-Z    97-122=a-z  48-57=0-9 Digits */
        if(isspace(c)){
          //whitespace
          return 16;
        }else if(int(c) >= 65 && int(c) <= 90){
            //upperchar
            return 0;
        }else if(int(c) >= 97 && int(c) <= 122){
            //lowerchar
            return 0;
        }else if(int(c) >= 48 && int(c) <= 57){
            //digit
            return 1;
        }
      switch(c){
          case('/'): return 2; break;
          case('*'): return 3; break;
          case('='): return 4; break;
          case('!'): return 5; break;
          case('<'): return 6; break;
          case('>'): return 7; break;
          case('('): return 8; break;
          case(')'): return 9; break;
          case('{'): return 10; break;
          case('}'): return 11; break;
          case('+'): return 12; break;
          case('-'): return 13; break;
          case(','): return 14; break;
          case(';'): return 15; break;
          default: return 17; break;
      }
      //error state
      return -1;
    }
    //determine if the token passed is contained in the list of reserved words.
    bool CompareToReservedWords(string token){
        for(auto word : RESERVED_WORDS){
            if(token == word){
                return true;
            }
        }
        return false;
    }
    //As ProcessTokens() builds tokens one char at a time, whenever a full token is created, call with the token value and class.
    //Uses a table driven FSA to build the symbol table. Modified version of Pg64 BurrisCompiler
    void SymbolTableBuilder(string tokenVal, string tokenClass){
        //catchall state for token classes not found in the class list.
        int col = 11;
        outputSymbolTable.close();
        outputSymbolTable.open("symboltable.txt", fstream::app);
        //Determine the column number in FirstPassTable to goto. Possible tokens are in FIRST_CLASS_LIST in order of their columns in FirstPassTable.
        //FIRST_PASS_CLASS_LIST[] = {"$CLASS","<var>","$LB","$RB","$CONST","$VAR","<assign>","<integer>", "<semi>", "<comma>", "$PROCEDURE"};
        for(int i = 0; i < FIRST_PASS_SIZE; i++){
            if(FIRST_PASS_CLASS_LIST[i] == tokenClass){
                col = i;
            }
        }
        outputDiagnostic << "In SymbolTableBuilder, tokenVal:" << tokenVal << " tokenClass:" << tokenClass << " col:" << col << endl;
        switch(FirstPassState){
            case 0: FirstPassState = FirstPassTable[0][col];
                    outputDiagnostic <<"In 0, nextState=" << FirstPassState <<" col:" << col << endl;
                        if(col == 7){
                        //add numeric literal to symbol table
                        bool exists;
                        for(auto i : numLitTracker){
                            if(i == tokenVal){
                                exists = true;
                            }
                        }
                        if(!exists){
                            //only add the new numlit if it does not already exist in the symbol table
                            numLitTracker.push_back(tokenVal);
                            outputSymbolTable << SymbolTableCounter << "|" << tokenVal << "|" << "$NUMLIT" << "|" << tokenVal << "|" << dataSegmentAddr << "|" << "DS" << "\n";
                            SymbolTableCounter++;
                            dataSegmentAddr += 2; //assuming 2 bytes per int/var
                        }
                        exists = false;
                    }       
                    break;
            case 1: FirstPassState = FirstPassTable[1][col]; 
                    //add class/procedure pgm name to symbol table. No code segment address yet, leave as ?
                    if(FirstPassState == 2){
                        if(SymbolTableCounter == 0){
                            outputSymbolTable << SymbolTableCounter << "|" << tokenVal << "|" << "<program name>" << "|" << "?|" << "?" << "|" << "CS" << "\n";
                        }else{
                            outputSymbolTable << SymbolTableCounter << "|" << tokenVal << "|" << "<procedure>" << "|" << "?|" << "?" << "|" << "CS" << "\n";
                        }
                        
                        SymbolTableCounter++;
                    }
                    outputDiagnostic <<"In 1, nextState=" << FirstPassState << endl;
                    break;
            case 2: FirstPassState = FirstPassTable[2][col];
                    outputDiagnostic <<"In 2, nextState=" << FirstPassState << endl;
                        break;
            case 3: FirstPassState = FirstPassTable[3][col];
                    outputDiagnostic <<"In 3, nextState=" << FirstPassState << endl;
                    //have a const varname, save it out.
                    savedToken = tokenVal; 
                    break;
            case 4: FirstPassState = FirstPassTable[4][col];
                    outputDiagnostic <<"In 4, nextState=" << FirstPassState << endl;
                    break;
            case 5: FirstPassState = FirstPassTable[5][col];
                    outputDiagnostic <<"In 5, nextState=" << FirstPassState << endl;
                    if(FirstPassState == 6){
                            //add const int to symbol table
                            outputSymbolTable << SymbolTableCounter << "|" << savedToken << "|" << "$CONST" << "|" << tokenVal << "|" << dataSegmentAddr << "|" << "DS" << "\n";
                            SymbolTableCounter++;
                            dataSegmentAddr += 2; //assuming 2 bytes per int/var
                        }
                    break;
            case 6: FirstPassState = FirstPassTable[6][col];
                    outputDiagnostic <<"In 6, nextState=" << FirstPassState << endl; break;
            case 7: FirstPassState = FirstPassTable[7][col];
                    outputDiagnostic <<"In 7, nextState=" << FirstPassState << endl;
                    if(FirstPassState == 8){
                        //add varname to symbol table
                        outputSymbolTable << SymbolTableCounter << "|" << tokenVal << "|" << "<var>" << "|" << "?" << "|" << dataSegmentAddr << "|" << "DS" << "\n";
                        SymbolTableCounter++;
                        dataSegmentAddr += 2; //assuming 2 bytes per int/var
                    }
                    break;
            case 8: FirstPassState = FirstPassTable[8][col];
                    outputDiagnostic <<"In 8, nextState=" << FirstPassState << endl; break;

        }
        outputSymbolTable.close();
    }

    /*  Process one character at a time using a table-driven DFSA to form tokens.
        Outputs tokens to the symbol list and passes the tokens to SymbolTableBuilder
        to build the symbol table using fully formed tokens. */
	void ProcessTokens(){
        char ch;
        string token = "";
        int col;
        cout << "\n" << "Type the source filename (with extension): ";
        cin >> sourceFileName;
        outputDiagnostic << endl;
        cout << "\n";

        sourceFile.open(sourceFileName, fstream::in);
        outputTokenList.open("tokenlist.txt", fstream::out);
        outputTokenList << "Symbol|Class\n";
        outputSymbolTable.open("symboltable.txt", fstream::out);
        outputSymbolTable << "Num|Symbol|Class|Value|Address|Segment\n";
        outputSymbolTable.close();
        outputDiagnostic.open("outputDiagnostic.txt", fstream::out);
		int nextState = 0;
     /* 0Start
        1eq
        2int
        3less
        4greater
        5var
        6comment
        7comment1
        8comment2
        9exclamation all rows after this are end states that will output the token into the list. End states need to goto SpareChar label to avoid the next char being overwritten
        10(discard & start) instead of outputting the token, discard the entire token containing the comment and begin at the start state at the next char.
        11(/,<mop)
        12(*,<mop>)
        13(int,<integer>)
        14(ident,<ident>)  ident can be a var or reserved word, need to check against array of reserved words: RESERVED_WORDS[]
        15(==,<relop>)
        16(=,<assign>)
        17((,$LP)
        18(),$RP)
        19({,$LB)
        20(},$RB)
        21(+,<addop>)
        22(-,<addop>)
        23(,,<comma>)
        24(;,<semi>)
        25(>=,<relop>)
        26(<=,<relop>)
        27(>,<relop>)
        28(<,<relop>)
        29(!=,<relop>) */

        //while not EOF, continue and get next character without skipping whitespace.
	    while(sourceFile >> std::noskipws >> ch){
            //Jump here whenever we need to avoid the while loop overwriting the current char.
            //For example if we have a full token that's been written and the next char is some space or delimiter, we need to spare it from being overwritten.
            SpareChar:
			switch (nextState){
            //set the next state based on the token recieved in the current state.
			case 0: token = ch;
                    nextState = TokenizerTable[0][CharToColNum(ch)];
                    outputDiagnostic << "0Start char:" << token << " nextState:" << to_string(nextState) << "\n"; 
                    break;
			case 1: outputDiagnostic << "1eq token:" << token << " char:" << ch << "\n";
                    nextState = TokenizerTable[1][CharToColNum(ch)]; break;
			case 2: outputDiagnostic << "2int token:" << token << " char:" << ch << "\n";
                    nextState = TokenizerTable[2][CharToColNum(ch)];
                    if(nextState == 2){
                        //only accumulate digits
                        token += ch;
                        outputDiagnostic <<"2int builder:"<< token << endl;
                    }else{
                        goto SpareChar;
                    } break;
			case 3: outputDiagnostic << "3less token:" << token << " char:" << ch << "\n";
                    nextState = TokenizerTable[3][CharToColNum(ch)];
                    //Only pick up = to make <=. Otherwise spare the next char.
                    if(nextState == 26){
                        token += ch;
                    }else{
                        goto SpareChar;
                    } break;
			case 4: outputDiagnostic << "4greater token:" << token << " char:" << ch << "\n";
                    nextState = TokenizerTable[4][CharToColNum(ch)];
                    //Only pick up = to make >=. Otherwise spare the next char.
                    if(nextState == 25){
                        token += ch;
                    }else{
                        goto SpareChar;
                    } break;
			case 5: outputDiagnostic << "5var token:" << token << " char: " << ch << " nextState:" << nextState << "\n";
                    nextState = TokenizerTable[5][CharToColNum(ch)];
                    //Only pick up letters and digits. Otherwise spare the next char
                    if(nextState == 5){
                        token += ch;
                    }else{
                        goto SpareChar;
                    } break;
			case 6: outputDiagnostic << "6comment token:" << token << " char:" << ch << " nextState:" << nextState << "\n";
                    nextState = TokenizerTable[6][CharToColNum(ch)]; 
                    //if the next state is 7 we need to accumulate more of the comment.
                    if(nextState == 7){
                        token += ch;
                        break;
                    }else{
                        //not a comment, we will be printing /|<mop>
                        goto SpareChar;
                    } break;
			case 7: token += ch;
                    outputDiagnostic << "7comment1 token:" << token << " char:" << ch << " nextState:" << nextState << "\n";
                    nextState = TokenizerTable[7][CharToColNum(ch)];
                    break;
			case 8: token += ch;
                    outputDiagnostic << "8comment2 token:" << token << " char:" << ch << " nextState:" << nextState << "\n";
                    nextState = TokenizerTable[8][CharToColNum(ch)]; break;
            case 9: token += ch;
                    outputDiagnostic << "9exclamation token:" << token << " char:" << ch << " nextState:" << nextState << "\n";
                    nextState = TokenizerTable[9][CharToColNum(ch)]; break;
			case 10:outputDiagnostic << "Got full comment, discard & begin at 0\n";
                    token = "";
                    ch = ' ';
                    nextState = 0; break;
			case 11:outputTokenList << token << "|" << "<mop>" << "\n";
                    SymbolTableBuilder(token,"<mop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
			case 12:outputTokenList << token << "|" << "<mop>" << "\n";
                    SymbolTableBuilder(token,"<mop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 13:outputTokenList << token << "|" << "<integer>" << "\n";
                    SymbolTableBuilder(token,"<integer>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 14://varname or reserved word? must check now that token is complete.
                    if(CompareToReservedWords(token)){
                        outputTokenList << token << "|" << "$" << token << "\n";
                        string y = "$" + token;
                        SymbolTableBuilder(token,y);
                        outputDiagnostic << "14ident token:" << token << " Found reserved word" << "\n";
                    }else{
                        outputTokenList << token << "|" << "<var>" << "\n";
                        SymbolTableBuilder(token,"<var>");
                        outputDiagnostic << "14ident token:" << token << " Found <var>" << "\n";
                    }
                    token = "";
                    nextState = 0;
                    goto SpareChar; break;
            case 15:outputTokenList << "==" << "|" << "<relop>" << "\n";
                    SymbolTableBuilder(token,"<relop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 16:outputTokenList << token << "|" << "<assign>" << "\n";
                    SymbolTableBuilder(token,"<assign>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 17:outputTokenList << token << "|" << "$LP" << "\n";
                    SymbolTableBuilder(token,"$LP");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 18:outputTokenList << token << "|" << "$RP" << "\n";
                    SymbolTableBuilder(token,"$RP");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 19:outputTokenList << token << "|" << "$LB" << "\n";
                    SymbolTableBuilder(token,"$LB");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 20:outputTokenList << token << "|" << "$RB" << "\n";
                    SymbolTableBuilder(token,"$RB");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 21:outputTokenList << token << "|" << "<addop>" << "\n";
                    SymbolTableBuilder(token,"<addop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 22:outputTokenList << token << "|" << "<addop>" << "\n";
                    SymbolTableBuilder(token,"<addop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 23:outputTokenList << token << "|" << "<comma>" << "\n";
                    SymbolTableBuilder(token,"<comma>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 24:outputTokenList << token << "|" << "<semi>" << "\n";
                    SymbolTableBuilder(token,"<semi>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 25:outputTokenList << token << "|" << "<relop>" << "\n";
                    SymbolTableBuilder(token,"<relop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 26:outputTokenList << token << "|" << "<relop>" << "\n";
                    SymbolTableBuilder(token,"<relop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 27:outputTokenList << token << "|" << "<relop>" << "\n";
                    SymbolTableBuilder(token,"<relop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 28:outputTokenList << token << "|" << "<relop>" << "\n";
                    SymbolTableBuilder(token,"<relop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case 29:outputTokenList << token << "|" << "<relop>" << "\n";
                    SymbolTableBuilder(token,"<relop>");
                    nextState = 0;
                    token = "";
                    goto SpareChar; break;
            case -1: outputDiagnostic << "Error" << endl; break;
			default: outputDiagnostic << "Illegal input!!" << endl; break;
			}
		}
        sourceFile.close();
        outputTokenList.close();
        outputDiagnostic.close();
	}
};

void Java0DFSAdrvMain(){
	TableDrivenDFSA FSA1 = TableDrivenDFSA();
	FSA1.ProcessTokens();
}
