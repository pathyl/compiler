/*	   In file FSAdrv.cpp
Table drive deterministic finite state automaton.
*/

#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string.h>

using namespace std;


//int is another name for Boolean.
typedef int Boolean;
const Boolean TRUE = 1;
const Boolean FALSE = 0;

class TableDrivenDFSA{
private:
	int stateTable[3][3]; //rows => states, cols => response to question.
public:
//pg52 table
	//table constructor
	TableDrivenDFSA(){
	   stateTable[0][0] = 4; stateTable[0][1] = 1; stateTable[0][2] = 8;
	   stateTable[1][0] = 2; stateTable[1][1] = 5; stateTable[1][2] = 8;
	   stateTable[2][0] = 3; stateTable[2][1] = 6; stateTable[2][2] = 7;
	}


	//Table Constructor if given the answers in a list. E.g int tabData[]={4,1,8,2,5,8,3,6,7} would produce the table above. Fills in the table left to right, line by line.
	TableDrivenDFSA(int tabData[]){
		for(int row = 0; row < 3; row++)
			for(int col = 0; col < 3; col++)
				stateTable[row][col] = tabData[row*3+col];
    }

	//resp[3] holds the 3 answers in order.
	void ProcessRequest(int resp[3]){
        Boolean finished = FALSE;  
		int nextState = 0; //Start state.
	    while( !finished){
			switch (nextState){
			case 0: nextState = stateTable[0][resp[0]]; break;
			case 1: nextState = stateTable[1][resp[1]]; break;
			case 2: nextState = stateTable[2][resp[2]]; break;
			case 3: cout << "Approve the order ."<< endl;
				    finished = TRUE; break;
			case 4: cout << "Approve the order and notify parent."
						 << endl;  finished = TRUE; break;
			case 5: cout << "Reject order and kick bum out!";
				    finished = TRUE; break;
			case 6: cout << "Reject the order." << endl;
				    finished = TRUE; break;
			case 7: cout << "Approve the order and notify management."
						 << endl; 
				    finished = TRUE; break;
			case 8: cout << "A user error has occured." << endl;
				    finished = TRUE; break;
			default: cout << "Illegal integer input!!" << endl;
				     finished = TRUE;
			}
		}
	}
};

 //LegalResponce is another name for int
 //typedef existingType newAliasForExistingType
typedef int LegalResponce;
const LegalResponce y = 0;
const LegalResponce n = 1;
const LegalResponce f = 2;

int AnswerToNumeric(char ch){ //Convert char responce to int.
	if(ch == 'y') return 0;
	else if(ch == 'n') return 1;
	else if(ch == 'f') return 2;
	else return 9;  // Illegal character input.
}

void main(){

	int quesResp [3];
	char again = 'y';
	char ans;
	int table2[] = {4, 1, 8, 2, 5, 8, 3, 6, 5};

    TableDrivenDFSA FSA1; //Declare a FSA.
	TableDrivenDFSA FSA2(table2);

	while (again == 'y' | again == 'Y'){
		cout << "Respond with 'y' for yes, 'n' for no, and 'f' for fair!\n"
			 << endl;
		cout << "Do you have special clearance: ";
            cin >> ans;
            quesResp[0] = AnswerToNumeric(ans);
		cout << "Is your credit okay?: ";
		cin >> ans;
		quesResp[1] = AnswerToNumeric(ans);
		cout << "Enter pay experience (make payments on time): ";
	      cin >> ans;
            quesResp[2] = AnswerToNumeric(ans);

		cout << "\nPolicy determined by FSA1: " << endl;
		FSA1.ProcessRequest(quesResp);  //Determine policy.

		cout << "\nPolicy determined by FSA2: " << endl;
		FSA2.ProcessRequest(quesResp);  //Determine policy.

            cout << "\nEnter another expression {y or n}: ";
	      cin >> again;
	}

}
