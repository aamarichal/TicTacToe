// TicTacToe.cpp : Defines the entry point for the console application.
//

#include<iostream>
#include<sstream>
#include<vector>
#include<algorithm>

#include<string>

using namespace std;

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
	for(int i=0; i<3;i++)
		for(int j=0; j<3; j++)
			squares[i][j] = 0;
}

Board::Board(const Board& b){
	for(int i=0; i<3;i++)
		for(int j=0; j<3; j++)
			squares[i][j] = b.squares[i][j];
}

string Board::toString() {
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
	squares[row-1][col-1] = val;
}

bool Board::full_board() {
	for(int i=0; i<3;i++)
		for(int j=0; j<3; j++)
			if(squares[i][j]==0)
				return false;
	return true;
}

//returns winner or zero if none
int Board::winner() {
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
	return 0;
}

int Board::get_square(int row, int col) {
	return squares[row-1][col-1];
}

bool Board::isEmpty(int row, int col){
	if(get_square(row, col)==0) return true;
	else return false; 
}

/*******************************************************
* This is the minimax tree and its methods
********************************************************/

class node {
private: 
	std::vector<node*> children; 
	node* parent;
	float utility;
	bool minimax; // 1 if max node, 0 if min node
	Board* state; 
public: 
	node(Board* state, bool minormax);
	void addChild(Board* newState, bool minormax);
	void printChildren();
	std::vector<node*> getChildren() {return children;};
	float getUtility() {return utility;};
	Board* getState() {return state;};
	void setUtility(float u) {utility = u;};
};

node::node(Board* state, bool minormax) {
	minimax = minormax;
	parent = NULL;
	utility = 0;
	state = NULL;
}

void node::addChild(Board* newState, bool minormax) {
	/* 
	 * Pushes a new child node into the children vector 
	 * given the next availble state
	*/
	node* n = new node(newState, minormax);
	children.push_back(n);
}



void node::printChildren() {
	/* Test to see if child nodes are added to vector 
	 * Mainly for debugging purposes
	 */
	cout << "Child nodes: \n";
	for (int i=0; i<children.size();i++) {
		if (children.at(i)->minimax == true) {
			cout << "Maximum\n";
		} else {
			cout << "Minimum\n";
		}
	}
}

class tree {
	// This class may not be necessary
private: 
	node* root; 
	float alpha;
	float beta; 
public: 
	tree();
	float alphaBetaPruning(node* currentNode, int depth, float alpha, float beta, bool maximizingPlayer, Board* state);
};

tree::tree() {
	root = new node(new Board(), 1); // New max node at top
	alpha = -99999999;
	beta = 99999999;
}

float tree::alphaBetaPruning(node* currentNode, int depth, float alpha, float beta, bool maximizingPlayer, Board* state) {
	// First set all states for child nodes
	Board* temp = currentNode->getState(); 
	for (int i=0; i<depth; i++) {
		// try all possible moves 
		for (int j=0; j<3; j++) {
			for (int k=0; k<3; k++) {
				if (temp->isEmpty(j, k)) {
					node* child = new node(temp, !maximizingPlayer);
					child->getState()->play_square(j+1, k+1, 0);
					currentNode->addChild(child->getState(), !maximizingPlayer);
				}
			}
		}
	}

	if (depth == 0 || currentNode->getChildren().size() == 0) {
		/* Set utility value and then return it
		 * If node is a terminal node, return its utility 
		 * value 
		 */
		int winner = state->winner();
		if (winner == 1) {
			currentNode->setUtility(-1);
		} else if (winner == -1) {
			currentNode->setUtility(1);
		} else {
			currentNode->setUtility(0);
		}
		return currentNode->getUtility(); 
	}

	/* Maximizing player will set a new alpha value if it is higher than the previous*/
	if (maximizingPlayer) { // Now at max node
		for (int i=0; i<currentNode->getChildren().size(); i++) {
			alpha = std::max(alpha, alphaBetaPruning(currentNode->getChildren().at(i), depth-1, alpha, beta, false, state));
			if (beta <= alpha) {
				break;
			}
		}
		return alpha;
	} else {	// At min node
		/* Will set a new beta value if it is less than the current one */
		for (int j=0; j<currentNode->getChildren().size(); j++) {
			beta = std::min(beta, alphaBetaPruning(currentNode->getChildren().at(j), depth-1, alpha, beta, true, state));
			if (beta <= alpha) {
				break;
			}
		}
		return beta;
	}
}

/********************************************************
* Declare all functions not in Board, node, or tree class here
********************************************************/

int minValue(Board*, int);
int maxValue(Board*, int);
void play();
bool make_simple_cpu_move(Board*, int);

bool make_simple_cpu_move(Board * b, int cpuval) {
	for(int i=1; i<4; i++)
		for(int j=1; j<4; j++)
			if(b->get_square(i, j)==0) {
				b->play_square(i, j, cpuval);
				return true;
			}
	return false;
}


int minValue(Board* b, int cpuval){
	//if terminal case, return utility (1 is win, -1 is lose, 0 is tie)
	if(b->full_board() || b->winner()!=0){
		if(cpuval == 1) return b->winner();
		else return (b->winner() * -1);
	}
	//otherwise, recursive call to maxValue for all successors
	else
	{
		Board* test[9];
		int count = 0;
		int t;
		int min = 1;
		int oppoval = (cpuval * -1);
		for(int i=1; i<4; i++){
			for(int j=1; j<4; j++){
				if(b->isEmpty(i, j)){
					test[count] = new Board(*b);
					test[count]->play_square(i, j, oppoval);
					cout << test[count]->toString();
					t = maxValue(test[count], cpuval);
					if(t < min) min = t;
					count++;
				}
			}
		}
		for(int k=0; k<count; k++)
		{
			delete test[k];
		}
		return min;
	}
}

int maxValue(Board* b, int cpuval){
	//if terminal case, return utility (1 is win, -1 is lose, 0 is tie)
	if(b->full_board() || b->winner()!=0){
		if(cpuval == 1) return b->winner();
		else return (b->winner() * -1);
	}
	//otherwise, recursive call to minValue for all successors
	else{
		Board* test[9];
		int count = 0;
		int t;
		int max = -1;
		for(int i=1; i<4; i++){
			for(int j=1; j<4; j++){
				if(b->isEmpty(i, j)){
					test[count] = new Board(*b);
					test[count]->play_square(i, j, cpuval);
					cout << test[count]->toString();
					t = minValue(test[count], cpuval);
					if(t > max) max = t;
					count++;
				}
			}
		}
		for(int k=0; k<count; k++)
		{
			delete test[k];
		}
		return max;
	}
}


void play() {
	Board * b = new Board();
	int humanPlayer = 1;
	int cpuPlayer = -1;

	cout << b->toString();
	while(!b->full_board()&&b->winner()==0) {
		int row, col;
		cout << "Your move row (1-3): ";
		cin >> row;
		cout << "Your move col (1-3): ";
		cin >> col;

		if(b->get_square(row, col)!=0) {
			cout << "Square already taken." << endl;
			continue;
		}
		else {
			b->play_square(row, col, humanPlayer);
			if(b->full_board() || b->winner()!=0)
				break;
			else {
				cout << b->toString() << "..." << endl;
				make_simple_cpu_move(b, cpuPlayer);
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
	char a;
	cin >> a;
}


int main(int argc, char * argv[])
{
	play();
	return 0;
}

