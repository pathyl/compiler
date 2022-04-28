#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string.h>
#include <fstream>
#include <deque>
#include <sstream>
#include <stack>
#include <vector>
using std::cout, std::cin, std::endl, std::string, std::fstream, std::stack, std::deque, std::vector, std::to_string;
void SyntaxMain();
const string OPERATORS[] = {"<semi>", "<assign>", "<addop>", "$LP", "$RP", "<mop>", "$IF", "$THEN", "$ELSE", "$ODD", "<relop>", "$LB", "$RB", "$CALL", "$WHILE", "$DO"};
const int OPERATORS_SIZE = 16;
const int PREC_TABLE_ROWS = 16;
const int PREC_TABLE_COLS = 16;
int Tvar = 1;
int Lvar = 1;
int Wvar = 1;
int Evar = 1;
fstream outputQuads;
fstream debugInfo;
class Token{
public:
    string tokenSymbol;
    string tokenClass;

    // takes a token and returns the row or col num it corresponds to in our operator precedence table.
    int tokenClassToNum()
    {
        string tmpClass = tokenClass;
        for (int i = 0; i < OPERATORS_SIZE; i++)
        {
            if (OPERATORS[i] == tmpClass)
            {
                return i;
            }
        }
        // return -1 if the class of the token is not an operator (We will ignore and just add to the stack).
        return -1;
    }
};

Token tempToken;
string labelNum;
char operatorPrecedenceData[] =
    {'0', '<', '0', '<', '0', '0', '<', '0', '0', '0', '0', '0', '>', '0', '<', '0',
     '>', '0', '<', '<', '0', '<', '0', '0', '>', '0', '0', '0', '0', '0', '0', '0',
     '>', '0', '>', '<', '>', '<', '0', '>', '>', '0', '>', '0', '0', '0', '0', '>',
     '0', '0', '<', '<', '=', '<', '0', '0', '0', '0', '<', '0', '0', '0', '0', '0',
     '>', '0', '>', '0', '>', '>', '0', '0', '>', '0', '0', '0', '0', '0', '0', '0',
     '>', '0', '>', '<', '>', '>', '0', '>', '>', '0', '>', '0', '0', '0', '0', '>',
     '0', '0', '<', '<', '0', '<', '0', '=', '0', '<', '<', '0', '0', '0', '0', '0',
     '>', '<', '0', '<', '0', '0', '<', '0', '=', '0', '0', '<', '>', '<', '0', '0',
     '>', '<', '0', '<', '0', '0', '<', '0', '>', '0', '0', '<', '>', '<', '0', '0',
     '0', '0', '<', '<', '0', '<', '0', '>', '>', '0', '0', '0', '0', '0', '0', '0',
     '0', '0', '<', '<', '>', '<', '0', '>', '>', '0', '0', '0', '0', '0', '0', '>',
     '0', '<', '0', '0', '0', '0', '<', '0', '0', '0', '0', '<', '=', '<', '<', '0',
     '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
     '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
     '0', '0', '<', '<', '0', '<', '0', '0', '0', '<', '<', '0', '0', '0', '0', '=',
     '>', '<', '0', '<', '0', '0', '0', '0', '0', '0', '0', '<', '>', '<', '<', '0'};
char operatorPrecedenceTable[PREC_TABLE_ROWS][PREC_TABLE_COLS];

Token generateQuads(vector<Token> popVec)
{
    // take a vector of tokens containing the terminals and non-terminals in reverse order. Return a token with T <var> to be placed in the stack.
    Token opQuad;
    Token quad2;
    Token quad3;
    Token quad4;
    // opQuad,quad2,quad3,quad4
    // quad 4 is usually a T var that must be returned to enter the PDA
    debugInfo.open("debugInfo.txt", fstream::app);
    debugInfo << "Entering generateQuads, popVec contains:";
    for (auto i : popVec)
    {
        debugInfo << i.tokenSymbol << ",";
    }
    debugInfo << endl;
    // determine the operator
    for (auto i : popVec)
    {
        int opCode = i.tokenClassToNum();
        if (opCode != -1)
        {
            // found an operator.
            switch (opCode)
            {
            case 0: // semi - error
                break;
            case 1: // assign vector should be [input] = [receive]. quad is =, [receive], [input], -
                opQuad = popVec.at(1);
                quad2 = popVec.at(2);
                quad3 = popVec.at(0);
                quad4.tokenClass = "null";
                quad4.tokenSymbol = "null";
                outputQuads.open("quads.txt", fstream::app);
                outputQuads << opQuad.tokenSymbol << "," << quad2.tokenSymbol << "," << quad3.tokenSymbol << "," << quad4.tokenSymbol << endl;
                outputQuads.close();
                break;
            case 2: // addop - check for unary + and unary -
                // binary + for A + B
                if (popVec.size() == 3)
                {
                    // quad +, A, B, T1, vector should contain B, +, A. Similar for -
                    opQuad = popVec.at(1);
                    quad2 = popVec.at(2);
                    quad3 = popVec.at(0);
                    quad4.tokenClass = "<var>";
                    quad4.tokenSymbol = "T" + to_string(Tvar);
                    outputQuads.open("quads.txt", fstream::app);
                    outputQuads << opQuad.tokenSymbol << "," << quad2.tokenSymbol << "," << quad3.tokenSymbol << "," << quad4.tokenSymbol << endl;
                    outputQuads.close();
                }
                else
                {
                    // unary + or -, vector should contain A, -
                    opQuad = popVec.at(1);
                    if (opQuad.tokenSymbol == "+")
                    {
                        // throw quad away, return the var to be pushed back onto stack instead of T
                        return popVec.at(0);
                    }
                    else
                    {
                        // unary -.    -,0,A,T1, so that 0-A gives us -A
                        opQuad = popVec.at(1);
                        quad2.tokenClass = "<var>";
                        quad2.tokenSymbol = "0";
                        quad3 = popVec.at(0);
                        quad4.tokenClass = "<var>";
                        quad4.tokenSymbol = "T" + to_string(Tvar);
                        outputQuads.open("quads.txt", fstream::app);
                        outputQuads << opQuad.tokenSymbol << "," << quad2.tokenSymbol << "," << quad3.tokenSymbol << "," << quad4.tokenSymbol << endl;
                        outputQuads.close();
                    }
                }
                break;
            case 3: // LP handled when ( = )
                break;
            case 4: // RP handled when { = }
                break;
            case 5: // mop quad *,A,B,T1 when vec is B, *, A,
                opQuad = popVec.at(1);
                quad2 = popVec.at(2);
                quad3 = popVec.at(0);
                quad4.tokenClass = "<var>";
                quad4.tokenSymbol = "T" + to_string(Tvar);
                outputQuads.open("quads.txt", fstream::app);
                outputQuads << opQuad.tokenSymbol << "," << quad2.tokenSymbol << "," << quad3.tokenSymbol << "," << quad4.tokenSymbol << endl;
                outputQuads.close();
                break;
            case 6: // IF handled at IF = THEN, or IF = THEN = ELSE
                break;
            case 7: // THEN
                break;
            case 8: // ELSE
                break;
            case 9: // ODD quad ODD, A, null, null. vec is A, ODD
                opQuad = popVec.at(1);
                quad2 = popVec.at(0);
                quad3.tokenSymbol = "null";
                quad4.tokenSymbol = "null";
                outputQuads.open("quads.txt", fstream::app);
                outputQuads << opQuad.tokenSymbol << "," << quad2.tokenSymbol << "," << quad3.tokenSymbol << "," << quad4.tokenSymbol << endl;
                outputQuads.close();
                break;
            case 10: // relop quad >=, A, B, null. vec is B , >= , A
                opQuad = popVec.at(1);
                quad2 = popVec.at(2);
                quad3 = popVec.at(0);
                quad4.tokenClass = "null";
                quad4.tokenSymbol = "null";
                outputQuads.open("quads.txt", fstream::app);
                outputQuads << opQuad.tokenSymbol << "," << quad2.tokenSymbol << "," << quad3.tokenSymbol << "," << quad4.tokenSymbol << endl;
                outputQuads.close();
                break;
            case 11: // LB handled when {=}
                break;
            case 12: // RB
                break;
            case 13: // CALL A option, probably won't implement
                break;
            case 14: // WHILE handled when WHEN = DO
                break;
            case 15: // DO
                break;
            default:
                break;

            }
        }
    }
    // We only want to use 8 Temp variables.
    Tvar++;
    if (Tvar > 9)
    {
        Tvar = 1;
    }
    debugInfo.close();
    return quad4;
}

void SyntaxMain()
{
    // read in the tokenlist produced by the scanner and put all tokens and their class in order inside a vector so we can iterate through them easily.
    fstream tokenList;
    tokenList.open("tokenlist.txt", fstream::in);
    outputQuads.open("quads.txt", fstream::out);
    outputQuads.close();
    debugInfo.open("debugInfo.txt", fstream::out);
    debugInfo.close();


    // deque containing all constructed Tokens from tokenlist in the order they appeared.
    deque<Token> tokenDeque(1);
    string tmp = "    ";

    // currentToken stores the latest token grabbed, possibly an operator or non-terminal.
    Token currentToken;

    // prevToken stores the last operator in the stack.
    Token prevOperator = Token();
    prevOperator.tokenClass = "$LB";
    prevOperator.tokenSymbol = "{";
    stack<Token> tokenStack;
    vector<Token> popVec;
    Token popToken;
    Token saveToken;
    stack<string> fixUpStack;
    stack<string> endStack;
    stack<string> startWhileStack;
    // initialize the 2d operator precedence matrix
    for (int row = 0; row < PREC_TABLE_ROWS; row++)
    {
        for (int col = 0; col < PREC_TABLE_COLS; col++)
        {
            operatorPrecedenceTable[row][col] = operatorPrecedenceData[row * PREC_TABLE_COLS + col];
        }
    }

    // read in tokens and put them into a deque
    //pop header
    std::getline(tokenList, tmp);


    while (std::getline(tokenList, tmp, '|'))
    {
        Token newToken;
        newToken.tokenSymbol = tmp;
        std::getline(tokenList, tmp, '\n');
        newToken.tokenClass = tmp;
        tokenDeque.push_back(newToken);
    }
    //linux adds this empty token for no reason at all!!
    if(tokenDeque.front().tokenClass == ""){
        tokenDeque.pop_front();
    }
    debugInfo.open("debugInfo.txt", fstream::app);
    // clear out all CONST & VAR declarations and the initial CLASS PGM{ and final } from PGM
    if (tokenDeque.front().tokenSymbol == "CLASS")
    {
        debugInfo << "Popping Class:" << tokenDeque.front().tokenClass << endl;
        tokenDeque.pop_front(); // pop CLASS
        debugInfo << "Popping Class2:" << tokenDeque.front().tokenSymbol << endl;
        tokenDeque.pop_front(); // pop pgmName
    }


    // erase CONST ---------; and VAR ---------; these are already put into the symbol table during lexical analysis.
    for (int i = 0; i < tokenDeque.size(); i++)
    {

        if (tokenDeque.at(i).tokenClass == "$CONST" || tokenDeque.at(i).tokenClass == "$VAR")
        {
            debugInfo << "Found CONST/VAR line:" << tokenDeque.at(i).tokenClass << " ";
            bool foundSemi = false;
            int k = i;
            while (!foundSemi)
            {
                k++;
                debugInfo << tokenDeque.at(k).tokenClass << " ";
                if (tokenDeque.at(k).tokenClass == "<semi>")
                {
                    foundSemi = true;
                }
            }
            debugInfo << "\n"
                      << "Deleting from:" << tokenDeque.at(i).tokenClass << " to " << tokenDeque.at(k).tokenClass << endl;
            debugInfo << "Full line to erase:";
            for (int j = i; j <= k; j++)
            {
                debugInfo << tokenDeque.at(j).tokenSymbol << " ";
            }
            debugInfo << endl;
            std::deque<Token>::iterator iterBegin = tokenDeque.begin() + i;
            std::deque<Token>::iterator iterEnd = tokenDeque.begin() + k;
            tokenDeque.erase(iterBegin, iterEnd);
        }
    }
    debugInfo.close();
    int currentNum;
    int prevOperatorNum;
    while (!tokenDeque.empty())
    {
    SkipWhile:
        debugInfo.open("debugInfo.txt", fstream::app);
        if(tokenDeque.size() > 0){
            currentToken = tokenDeque.front();
            tokenDeque.pop_front();
        }else{
            if(tokenStack.empty()){
                return;
            }
        }

        debugInfo << "Got front & popped:" << currentToken.tokenSymbol << endl;

    // we jump here if we popped quads and need to check against the new prevOp
    checkAfterQuadGen:
        currentNum = currentToken.tokenClassToNum();
        prevOperatorNum = prevOperator.tokenClassToNum();

        if (tokenStack.empty())
        {
            // stack is empty, automatically add a token and set the currentToken to the next one.
            debugInfo << "Found empty stack, pushing:" << currentToken.tokenSymbol << endl;
            tokenStack.push(currentToken);
            while (tokenDeque.front().tokenClass == "<semi>")
            {
                tokenDeque.pop_front();
            }
            debugInfo << "New currentToken after empty push:" << tokenDeque.front().tokenSymbol << endl;
            currentToken = tokenDeque.front();
            tokenDeque.pop_front();

            // if the current token was an operator, set prevOperatorNum to currentNum before recalculating currentNum
            if (currentNum != -1)
            {
                // the current token is an operator.
                prevOperatorNum = currentNum;
            }

            currentNum = currentToken.tokenClassToNum();
        }

        // GET/PUT handlers
        if (currentToken.tokenSymbol == "GET")
        {
            debugInfo << "Found GET, popping:" << tokenDeque.front().tokenSymbol;
            tokenDeque.pop_front(); // pop (
            outputQuads.open("quads.txt", fstream::app);
            outputQuads << "GET"
                        << "," << tokenDeque.front().tokenSymbol << ",null,null" << endl;
            debugInfo << " " << tokenDeque.front().tokenSymbol;
            tokenDeque.pop_front(); // pop var
            debugInfo << " " << tokenDeque.front().tokenSymbol;
            tokenDeque.pop_front(); // pop )
            debugInfo << " " << tokenDeque.front().tokenSymbol;
            tokenDeque.pop_front(); // pop ;
            outputQuads.close();
            debugInfo.close();
            goto SkipWhile;
        }
        if (currentToken.tokenSymbol == "PUT")
        {
            debugInfo << "Found PUT, popping:" << tokenDeque.front().tokenSymbol;
            tokenDeque.pop_front(); // pop (
            outputQuads.open("quads.txt", fstream::app);
            outputQuads << "PUT"
                        << "," << tokenDeque.front().tokenSymbol << ",null,null" << endl;
            debugInfo << " " << tokenDeque.front().tokenSymbol;
            tokenDeque.pop_front(); // pop var
            debugInfo << " " << tokenDeque.front().tokenSymbol;
            tokenDeque.pop_front(); // pop )
            debugInfo << " " << tokenDeque.front().tokenSymbol;
            tokenDeque.pop_front(); // pop ;
            outputQuads.close();
            debugInfo.close();
            goto SkipWhile;
        }
        if (currentNum == -1)
        { 
            //-1 means it is not an operator, push into stack and do not bother checking precedence.
            debugInfo << "Inside NotOperator if, pushing nonoperator into PDA:" << currentToken.tokenSymbol << endl;
            tokenStack.push(currentToken);
        }
        else if (operatorPrecedenceTable[prevOperatorNum][currentNum] == '>')
        {
            // compare the previous operator to the next(current) and use the precedence table to determine what to do.
            debugInfo << "Inside >, prevOperatorNum:" << to_string(prevOperatorNum) << ", currentNum:" << to_string(currentNum) << endl;
            // found the end of the prime phrase. Back up and pop, then send to our quad generator.
            // read down until we find an operator and pop.
            debugInfo << "topStack:" << tokenStack.top().tokenSymbol << endl;
            popToken = tokenStack.top();
            int opCount = 0;
            // while the popToken is not an operator, push it into the vector. We expect to encounter 1 operator, our prevOperator.
            if(currentToken.tokenClass == "$ELSE" && prevOperator.tokenClass == "$RB"){
                //need to handle this in a different way than normal operators.
            }else if(currentToken.tokenClass == "$THEN" && prevOperator.tokenClass == "$RB"){
                //set this up to handle IFTHEN instead of IFTHENELSE, currently need full IFTHENELSE
            }else{
                //handler for normal arithmetic & relational operators.
                while (popToken.tokenClassToNum() == -1 || opCount < 1)
                {
                    debugInfo << "Building popVector for quads" << endl;
                    if (popToken.tokenClassToNum() != -1)
                    {// We've encountered our prevOperator. This is the only operator we must allow in.
                        debugInfo << "Found operator:" << popToken.tokenSymbol << endl;
                        opCount++;
                    }
                    debugInfo << "Pushing to popVec:" << popToken.tokenSymbol << endl;
                    popVec.push_back(popToken);
                    debugInfo << "Popping from stack" << tokenStack.top().tokenSymbol << endl;
                    tokenStack.pop();
                    debugInfo << "New popToken:" << tokenStack.top().tokenSymbol << endl;
                    popToken = tokenStack.top();
                }
                // We should have a vector containing the non-terminals + operator in reverse order. e.g. Tom / Mary should become Mary / Tom in the vector.
                opCount = 0;
                // generateQuads will return a T var if we need to push it into the stack, otherwise return a Token with tokenClass = "null".
                debugInfo.close();
                tempToken = generateQuads(popVec);
                popVec.clear();
                debugInfo.open("debugInfo.txt", fstream::app);
                while (tokenStack.top().tokenSymbol == ";")
                {
                    tokenStack.pop();
                }
                debugInfo << "Finished generateQuads, prevOperator set:" << tokenStack.top().tokenSymbol << endl;
                prevOperator = tokenStack.top();
                prevOperatorNum = prevOperator.tokenClassToNum();
                if (tempToken.tokenClass != "null")
                {
                    debugInfo << "genQuads returned a Temp," + tempToken.tokenSymbol + " pushing into stack" << endl;
                    tokenStack.push(tempToken);
                    goto checkAfterQuadGen;
                }
                if(currentToken.tokenSymbol == ";" && prevOperator.tokenSymbol == "{"){
                    //we have fully completed a statement and need to get rid of the lingering ;
                    goto SkipWhile;
                }
                /*
                if(currentToken.tokenSymbol == ";" && prevOperator.tokenClass == "$THEN"){
                    
                    currentToken = tokenDeque.front();
                    tokenDeque.pop_front();
                    debugInfo << "Popped the ; after generating quads." << endl;
                    goto SkipWhile;
                }*/
                goto checkAfterQuadGen;
            }

            
        }
        else if (operatorPrecedenceTable[prevOperatorNum][currentNum] == '=')
        {
            // We've completed a (---) or {---} or IF THEN ELSE or WHILE DO
            //  ( = )
            if (prevOperator.tokenClass == "$LP" && currentToken.tokenClass == "$RP")
            {
                tokenStack.pop();
                prevOperator = tokenStack.top();
                prevOperatorNum = prevOperator.tokenClassToNum();
            }
            // { = }
            if (prevOperator.tokenClass == "$LB" && currentToken.tokenClass == "$RB")
            {
                tokenStack.pop();
                //currentToken = tokenDeque.front();
                //tokenDeque.pop_front();
                if(tokenStack.size() != 0){
                    prevOperator = tokenStack.top();
                }else{
                    tempToken.tokenClass = "$LB";
                    tempToken.tokenSymbol = "{";
                }
                //prevOperator = tempToken;
                prevOperatorNum = prevOperator.tokenClassToNum();
                if(prevOperator.tokenClass == "$ELSE"){
                    //We have popped the {---} after ELSE and need to pop a completed IF-THEN-ELSE from the stack.
                    tokenStack.pop();
                    tokenStack.pop();
                    tokenStack.pop();
                    prevOperator = tokenStack.top();
                    prevOperatorNum = prevOperator.tokenClassToNum();
                    if(tokenStack.size() < 2){
                        //We have popped all IFTHENELSE and need to fully pop the end stack and place labels
                        outputQuads.open("quads.txt", fstream::app);
                        while(endStack.size() != 0){
                            outputQuads << "<label>" << ",_" << endStack.top() << ",null,null" << endl;
                            endStack.pop();
                        }
                        outputQuads.close();
                    }

                    // Do this after code generation is working. Otherwise just force an empty ELSE{} on every IFTHEN
                if(prevOperator.tokenClass == "$THEN" && tokenDeque.front().tokenClass != "$ELSE"){
                    //We have popped the {---} after THEN and there is no ELSE lined up.
                    outputQuads.open("quads.txt", fstream::app);
                    string x = fixUpStack.top();
                    outputQuads << "<label>"
                            << ",_"
                            << x
                            << ",null,null"
                            << endl;
                    fixUpStack.pop();
                    outputQuads.close();
                }
                    goto SkipWhile;                                                                                                 
                }

                

                if(currentToken.tokenClass == "$RB" && prevOperator.tokenClass == "$DO"){
                    //We have popped the {---} after DO and need to place jmp W# then jmp E#
                    //We also need to pop the WHILE DO 
                    outputQuads.open("quads.txt", fstream::app);
                        outputQuads << "jmp,_" << startWhileStack.top() << ",null,null" << endl;
                        startWhileStack.pop();
                        outputQuads << "<label>,_" << endStack.top() << ",null,null" << endl;
                        endStack.pop();
                    outputQuads.close();
                    tokenStack.pop();
                    tokenStack.pop();
                    goto SkipWhile;
                }
            }
            //   IF = THEN
            if (prevOperator.tokenClass == "$IF" && currentToken.tokenClass == "$THEN")
            {
                // when this happens we have popped the boolean expression and need to generate the label.
                //this label will go into the fix-up stack. this will be directly after a relop quad >=, A, B, null
                //we need to come back later and find relop quad >=, A, B, null followed by <label>,L#,null,null and make it jne,jle,jge L# when we generate code from quads
                tokenStack.push(currentToken);
                prevOperator = tokenStack.top();
                prevOperatorNum = prevOperator.tokenClassToNum();

                outputQuads.open("quads.txt", fstream::app);
                labelNum = to_string(Lvar);
                // label quad form will be <label>, L1, null, null

                outputQuads << "<fixlabel>"
                            << ",_L" << labelNum << ","
                            << "null"
                            << ","
                            << "null" << endl;
                outputQuads.close();
                tmp = "L" + labelNum;
                fixUpStack.push(tmp);
                Lvar++;
            }
            //   THEN = ELSE
            if (prevOperator.tokenClass == "$THEN" && currentToken.tokenClass == "$ELSE")
            {
                // when this happens we have popped the {---} after THEN and need to generate the label for the ELSE.
                //ELSE is being pushed into the stack
      
                tokenStack.push(currentToken);
                prevOperator = tokenStack.top();
                prevOperatorNum = prevOperator.tokenClassToNum();

                outputQuads.open("quads.txt", fstream::app);
                labelNum = to_string(Evar);
                // label quad form will be <label>, E1, null, null
                outputQuads << "jmp"
                            << ",_E" << labelNum << ","
                            << "null"
                            << ","
                            << "null" << endl;
                tmp = "E" + labelNum;
                endStack.push(tmp);
                Evar++;

                //pop fix-up stack and make L#: nop
                //Do this as we push ELSE into the stack.
                 tmp = fixUpStack.top();
                outputQuads << "<label>"
                            << ",_"
                            << tmp
                            << ",null,null"
                            << endl;
                fixUpStack.pop();
                outputQuads.close();
            }
            //   WHILE = DO
            if (prevOperator.tokenClass == "$WHILE" && currentToken.tokenClass == "$DO")
            {
                // when this happens we have popped the boolean expression for WHILE and need to generate the label to exit WHILE.
                // jmp E# as DO is placed into the stack
                tokenStack.push(currentToken);
                prevOperator = tokenStack.top();
                prevOperatorNum = prevOperator.tokenClassToNum();

                outputQuads.open("quads.txt", fstream::app);
                labelNum = to_string(Evar);
                // label quad form will be <label>, L1, null, null
                outputQuads << "jmp,"
                            << "_E" << labelNum << ","
                            << "null"
                            << ","
                            << "null" << endl;
                outputQuads.close();
                tmp = "E" + labelNum;
                endStack.push(tmp);
                Evar++;
            }
            goto SkipWhile;
        }
        else if (operatorPrecedenceTable[prevOperatorNum][currentNum] == '<')
        {
            // prevOperator yields to the operator in currentToken, push into stack and continue
            if (currentToken.tokenClass == "$WHILE")
            {
                // generate the label for the start WHILE
                outputQuads.open("quads.txt", fstream::app);
                labelNum = to_string(Wvar);
                // label quad form will be <label>, W1, null, null
                outputQuads << "<label>"
                            << ",_W" << labelNum << ","
                            << "null"
                            << ","
                            << "null" << endl;
                outputQuads.close();
                tmp = "W" + labelNum;
                startWhileStack.push(tmp);
                Wvar++;
            }
            tokenStack.push(currentToken);
            prevOperator = tokenStack.top();
            prevOperatorNum = prevOperator.tokenClassToNum();
            goto SkipWhile;
        }else{
            //no precedence between operators, error
            if(currentToken.tokenSymbol == ";" && prevOperator.tokenClass == "$LB"){
                currentToken = tokenDeque.front();
                tokenDeque.pop_front();
            }
            debugInfo << "Entered the no precedence else. This should be an error." << endl;
            debugInfo << "No precedence between CurrentToken:" << currentToken.tokenSymbol << " prevOperator:" << prevOperator.tokenSymbol << endl;
        }

        if (currentNum != -1)
        {
            // if the current token was an operator, place it in prevOperator before while loop clears it next round
            prevOperator = currentToken;
        }
    }
    debugInfo.close();
}