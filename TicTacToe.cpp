// TicTacToe.cpp : Defines the entry point for the console application.
//

#include<iostream>
#include<sstream>

#include<string>

using namespace std;

/********************************************************
* Board Class
********************************************************/

class Board {
	int squares[3][3];

public:
	Board();
	Board(const Board&);
	string toString();
	void play_square(int, int, int);
	int get_square(int, int);
	int winner();
	bool full_board();
	bool isEmpty(int, int);
};

Board::Board() {
	/* Initializes empty board */
	for(int i=0; i<3;i++)
		for(int j=0; j<3; j++)
			squares[i][j] = 0;
}

Board::Board(const Board& b){
	/* Fills new board with contents of board b */
	for(int i=0; i<3;i++)
		for(int j=0; j<3; j++)
			squares[i][j] = b.squares[i][j];
}

string Board::toString() {
	/* Formats board into a printable string */
	stringstream s;
	char cforvalplusone[] = {'O', '_', 'X'};
	s << " _ _ _" << endl;
	for(int i=0; i<3;i++) {
		s << '|';
		for(int j=0; j<3; j++)
			s << cforvalplusone[squares[i][j]+1] << '|';
		s << endl;
	}
	return s.str();
}

void Board::play_square(int row, int col, int val) {
	/* Enters a val into the board at (row, col) */
	squares[row-1][col-1] = val;
}

bool Board::full_board() {
	/* Returns True if a board is filled with nonzero values */
	for(int i=0; i<3;i++)
		for(int j=0; j<3; j++)
			if(squares[i][j]==0)
				return false;
	return true;
}

int Board::winner() {
	/* returns winner's value or zero if there is no winner */
	//check rows:
	for(int row=0; row<3; row++)
		if(squares[row][0]!=0 && squares[row][0]==squares[row][1] && squares[row][0]==squares[row][2])
			return squares[row][0];
	//check cols:
	for(int col=0; col<3; col++)
		if(squares[0][col]!=0 && squares[0][col]==squares[1][col] && squares[0][col]==squares[2][col])
			return squares[0][col];
	//check diagonals:
	if(squares[0][0]!=0 && squares[0][0]==squares[1][1] && squares[0][0]==squares[2][2])
		return squares[0][0];
	if(squares[2][0]!=0 && squares[2][0]==squares[1][1] && squares[2][0]==squares[0][2])
		return squares[2][0];
	//There is no winner
	return 0;
}

int Board::get_square(int row, int col) {
	return squares[row-1][col-1];
}

bool Board::isEmpty(int row, int col){
	if(get_square(row, col)==0)
		return true;
	else
		return false; 
}

/********************************************************
* Function Declarations
********************************************************/

int minValue(Board*, int);
int maxValue(Board*, int);
void play();
bool make_simple_cpu_move(Board*, int);

/********************************************************
* Play Function
********************************************************/

void play() {
	/* Create Blank Board  */
	Board * b = new Board();

	/* Human = O & CPU = X */
	int humanPlayer = 1;
	int cpuPlayer = -1;

	/* Print initial, empty board */
	cout << b->toString();

	/* While there is no winner and the board isn't filled, loop */
	while(!b->full_board()&&b->winner()==0) {
		/* Get human's move */
		int row, col;
		cout << "Your move row (1-3): ";
		cin >> row;
		cout << endl;
		cout << "Your move col (1-3): ";
		cin >> col;
		cout << endl;

		/* If the input is invalid, start from the top of the loop */
		if (row > 3 || row < 1 || col > 3 || col < 1)
		{
			cout << "Input out of range." << endl;
			continue;
		}
		/* If the human's desired square is already filled,
		 * start from the top of the loop */
		if(b->get_square(row, col)!=0) {
			cout << "Square already taken." << endl;
			continue;
		}
		else
		{
			/* Fill square with human's value */
			b->play_square(row, col, humanPlayer);
			/* Print the new board after the human's move */
			cout << b->toString();
			/* If the board is full or the human has won, exit loop */
			if(b->full_board() || b->winner()!=0)
				break;
			/* The CPU makes a move */
			else
			{
				cout << "..." << endl;
				make_simple_cpu_move(b, cpuPlayer);
				/* Print the new board after the CPU's move */
				cout << b->toString();
			}
		}
	}
	if(b->winner()==0)
		cout << "Cat game." << endl;
	else if(b->winner()==cpuPlayer)
		cout << "Computer wins." << endl;
	else if(b->winner()==humanPlayer)
		cout << "You win." << endl;
}

/********************************************************
* Min / Max Functions
********************************************************/

int minValue(Board* b, int cpuval){
	//if terminal case, return utility (1 is win, -1 is lose, 0 is tie)
	if(b->full_board() || b->winner()!=0)
	{
		/* IF CPU is X, return result of winner function */
		if(cpuval == 1) return b->winner();
		/* If CPU is O, return opposite of winner function */
		else return (b->winner() * -1);
	}
	//otherwise, recursive call to maxValue for all successors
	else
	{
		/* Create an array of possible boards */
		Board* test[9];
		/* Create a counter for the number of boards attempted */
		int count = 0;
		/* t is the value returned after trying a test move */
		int t;
		/* Min is the current minimum value returned */
		int min = 1;
		for(int i=1; i<4; i++){
			for(int j=1; j<4; j++){
				/* If a square is empty, try to play there */
				if(b->isEmpty(i, j)){
					/* Coppy current board into the array */
					test[count] = new Board(*b);
					/* Play in the free square */
					test[count]->play_square(i, j, cpuval);
					/* Print current board */
					cout << test[count]->toString();
					/* Find the maximum value from playing in the given square */
					t = maxValue(test[count], cpuval);
					/* If the current value t is less than the minimum value
					 * t is the new minimum value */
					if(t < min) min = t;
					count++;
				}
			}
		}
		/* Delete all of the test boards */
		for(int k=0; k<count; k++)
		{
			delete test[k];
		}
		/* Return the minimum value */
		return min;
	}
}

int maxValue(Board* b, int cpuval){
	//if terminal case, return utility (1 is win, -1 is lose, 0 is tie)
	if(b->full_board() || b->winner()!=0)
	{
		/* IF CPU is X, return result of winner function */
		if(cpuval == 1) return b->winner();
		/* If CPU is O, return opposite of winner function */
		else return (b->winner() * -1);
	}
	//otherwise, recursive call to minValue for all successors
	else
	{
		/* Create an array of possible boards */
		Board* test[9];
		/* Create a counter for the number of boards attempted */
		int count = 0;
		/* t is the value returned after trying a test move */
		int t;
		/* Max is the current maximum value returned */
		int max = -1;
		/* Set opponent to have the opposite value of the CPU
		 * e.g. if CPU is X, opponent is O */
		int oppoval = (cpuval * -1);
		for(int i=1; i<4; i++){
			for(int j=1; j<4; j++){
				/* If a square is empty, try to play there */
				if(b->isEmpty(i, j)){
					/* Copy current board into the array */
					test[count] = new Board(*b);
					/* Play in the free square */
					test[count]->play_square(i, j, oppoval);
					/* Print current board */
					cout << test[count]->toString();
					/* Find the minimum value from playing in the given square */
					t = minValue(test[count], cpuval);
					/* If the current value t is greater than the maximum value
					 * t is the new maximum value */
					if(t > max) max = t;
					count++;
				}
			}
		}
		/* Delete all of the test boards */
		for(int k=0; k<count; k++)
		{
			delete test[k];
		}
		/* Return the maximum value */
		return max;
	}
}

/********************************************************
* CPU Move Function
********************************************************/

bool make_simple_cpu_move(Board * b, int cpuval) {
	for(int i=1; i<4; i++)
		for(int j=1; j<4; j++)
			if(b->get_square(i, j)==0) {
				b->play_square(i, j, cpuval);
				return true;
			}
	return false;
}

/********************************************************
* Main Function
********************************************************/

int main(){
	Board* b = new Board();
	b->play_square(1, 1, 1);
	b->play_square(1, 2, -1);
	b->play_square(1, 3, 1);
	b->play_square(2, 1, -1);

	cout << maxValue(b, 1) << endl;
return
	 0;
}
/*
int main(int argc, char * argv[])
{
	char willPlay = 'Y';
	while (true)
	{
		if (willPlay == 'Y')
		{
			play();
			cout << "Play again? (Y/N) ";
			cin >> willPlay;
		}
		else if (willPlay == 'N')
		{
			return 0;
		}
		else
		{
			cout << "Invalid input"  << endl;
			cout << "Play again? (Y/N) ";
			cin >> willPlay;
		}
	}
}
*/
