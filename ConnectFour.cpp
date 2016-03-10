//============================================================================
// Name        : ConnectFour.cpp
// Author      : Brad McGovern
// Description : Player vs Computer Connect 4 game
//============================================================================

#include <iostream>
#include <stdlib.h>

using namespace std;

void initializeGame();  //sets up a new game
void displayBoard();    //shows the current game board
int playerMove();       //gets move the player wants to make
int computerMove();     //choose move the computer makes
int getMoveScore (int, int, char);  //evaluates the total score of all lines for a potential move
int getLineScore (int, int, int, char); //evaluates the score of a move for a single direction (i.e. horizontal, down diagonal, etc...)
int getNumberInARow (int, int, int, int, char, bool); //returns how many in a row from a given space for a given a piece in a given direction

char gameBoard[8][8];   //array representing spaces on board; first is row, second is column
int columnPieces[8];    //count of how many pieces are in each column
int turns;              //number of turns that have played; game ends when this reaches 64


int main() {

	int turnResult;     //indicates if the game should end as a result of the turn
	bool gameOn = true; //shows if the game should continue or end
    char choice;        //choice entered by player

    initializeGame();   //sets up a new game

	while (gameOn) {
        turnResult = playerMove(); //function processes player move and returns 0 if game should end
        if (turnResult == 0){
            gameOn = false;
        }

        if (gameOn) {
            turnResult = computerMove(); //function evaluates potential moves and chooses one, and returns 0 if game should end
            if (turnResult == 0)
                gameOn = false;
        }
        if (!gameOn) {
            cout << "Play Again? (enter 'y' to play again, anything else to quit): ";
            cin >> choice;
            if (choice == 'y') {
                gameOn = true;
                initializeGame();
            } else {
                cout << "So long!!";
            }
        }

	}

}//end main()

void initializeGame() {
    cout << "\nLet's Play!" << endl;

	// clear the game board
	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++) {
			gameBoard[i][j] = '_';
			columnPieces[i] = 0;
		}
	}
	displayBoard();
	turns = 0;
}//end initializeGame()

void displayBoard() {
	for (int row=7; row > -1; row--){
			for (int col=0; col<8; col++) {
				cout << gameBoard[row][col] << " ";
			}
			cout << endl;
		}
	cout << "1 2 3 4 5 6 7 8 \n";

}//end displayBoard()

int playerMove () {

    int row;            //variable for the row being evaluated
    int column;         //variable for the column being evaluated
    int moveScore;      //variable for the score of the move played

    while (true) { //get choice from player
        cout << "\nPick column to play (0 to quit): ";
            cin >> column;
            if (!cin) {
                cout << "Please enter a number from 0 to 8\n";
                cin.clear();
                cin.ignore(10000,'\n');
            }
            else if (column == 0) {     //exit if player enters '0'
                cout << "Quitting game\n";
                return 0;
            }
            else if (column < 1 || column > 8) {
                cout << "Column must be between 1 and 8\n";
            }
            else if (columnPieces[column-1] == 8 ) {
                cout << "That column is full!";
            }
            else {  //if player enters a valid column to play, evaluate that move
                column -= 1;
                row = columnPieces [column];
                columnPieces[column] += 1;
                gameBoard[row][column] = 'X';
                if (getMoveScore(row, column, 'X') > 9999) { //if this gives player 4 in a row, display victory and return 0 to end game
                    displayBoard();
                    cout << "You win!!\n";
                    return 0;
                }
                else //if not, return 1 to keep game going
                    return 1;
            }
    }

}//end playerMove()

int computerMove(){

    int moveScores [8];
    int bestScore = -2;
    int totalBestMoves = 1;
    int columnToPlay = 0;
    int bestMoveChoice;
    int testScore;
    int bestTestScore;
    bool newBestScore;
    int i;
    int j;

    // get scores for all potential moves
    for (i = 0; i < 8; i++){
        if (columnPieces[i] < 8){ //if there is space in the column to make this move

            //score this move by checking what it will create for computer and what it will block for player
            moveScores[i] = getMoveScore(columnPieces[i], i, 'O');
            moveScores[i] += getMoveScore(columnPieces[i], i, 'X');

             //if a winning move is found, play it immediately, then return 0 to end the game
            if (moveScores[i] > 49999) {
                gameBoard[columnPieces[i]][i] = 'O';
                displayBoard();
                cout << "Computer plays column " << i + 1 << endl;
                cout << "Computer wins!\n";
                return 0;
            }

            //check if playing this move will give opponent a possibility of a 4-in-a-row or an open-ended 3-in-a-row next turn, and flag as "don't play" if it does
            if (moveScores[i] < 10000 && columnPieces[i] < 7){ //don't bother to check if this move already blocks an existing potential 4IR or fills the column
                //temporarily put piece in this column
                gameBoard[columnPieces[i]][i] = 'O';
                columnPieces[i] +=1;
                for (j = 0; j < 8; j++){
                    testScore = getMoveScore(columnPieces[j], j, 'X');
                    if (testScore > 9999 || (testScore > 999 && moveScores[i] < 1000)){ //if this move gives the player an opportunity for a 4IR next turn, OR an OE3IR AND this move won't block a currently available 4IR or OE3IR
                        moveScores[i] = -1; //flag as don't play
                        break; //end loop right away
                    }
                } //end loop through moves
                //remove temporary piece
                columnPieces[i] -=1;
                gameBoard[columnPieces[i]][i] = '_';
            }

            //check against best score, and if necessary update bestScore, totalBestMoves and columnToPlay
            if (moveScores[i] > bestScore) {
                bestScore = moveScores[i];
                totalBestMoves = 1;
                columnToPlay = i;
            } else if (moveScores[i] == bestScore){
                totalBestMoves += 1;
            }

        } else { //there isn't space in the column to make this move
            moveScores[i] = -2; //flag as "can't be played"
        }
    } //end loop evaluating move score for all columns

    //check what potential moves this could give the computer on its next turn
   if (bestScore < 1000){ //only check if current best score won't block and opponent OE3IR or better

        newBestScore = false; // reset variable to default value

        for (i = 0; i < 8; i++){ //loop thorough all already evaluated current moves

            //if currently this move can be played and is not flagged as don't play, and it doesn't already create a 3IR then check if playing here might create a good opportunity the following turn
            if (moveScores[i] > -1 && moveScores[i] < 100) {

                //temporarily put piece in this column
                gameBoard[columnPieces[i]][i] = 'O';
                columnPieces[i] +=1;

                bestTestScore = moveScores[i];

                //loop through all potential moves the following turn and find best score if it will at least block an opponent OE3IR
                for (j = 0; j < 8; j++){
                    testScore = getMoveScore(columnPieces[j], j, 'O');
                    if (testScore > 1000 && testScore > bestTestScore){
                        bestTestScore = testScore;
                        newBestScore = true; //this will alert later to re-evaluate best score(s)
                    }
                }

                moveScores[i] = bestTestScore;

                //remove temporary piece
                columnPieces[i] -=1;
                gameBoard[columnPieces[i]][i] = '_';
            }

        } //end loop though evaluated current moves
   } //finish checking potential future moves

   //if a new best score was found while evaluating future moves, re-examine all scores to find new best score(s)
    if (newBestScore){
        for (i = 0; i < 8; i++){
            if (moveScores[i] > bestScore) {
                bestScore = moveScores[i];
                totalBestMoves = 1;
                columnToPlay = i;
            } else if (moveScores[i] == bestScore){
                totalBestMoves += 1;
            }
        }
    }

    //if there are multiple moves that produce the same best score, choose one at random
    if (totalBestMoves > 1) {
        bestMoveChoice = (rand() % totalBestMoves);
        int i = 0;
        while (true){
            if (moveScores[i] == bestScore) {
                if (bestMoveChoice == 0) {
                    columnToPlay = i;
                    break;
                } else {
                    bestMoveChoice -= 1;
                }
            }

            i += 1;
        }

    }

    //make the chosen move
    gameBoard[columnPieces[columnToPlay]][columnToPlay] = 'O';
    columnPieces[columnToPlay] += 1;
    turns += 2;

    /*********
    displays the scores calculated for each move--used for testing purposes
    for (i=0; i < 8; i++){
        cout << moveScores[i] << " ";
    }
    cout << endl;
    ***********/

    displayBoard();
    cout << "\nComputer plays column " << columnToPlay + 1 << endl;

    if (turns == 64) {  //after 64 turns board is full, so display tie and return 0 to end game
        cout << "\nGame ends in a tie!\n";
        return 0;
    } else { //if computer doesn't win and board isn't full, return 1 to keep game going
        return 1;
    }

}//end computerMove

int getMoveScore (int row, int column, char piece) {

    int moveScore = 0;

    moveScore = getLineScore (row, column, 1, piece); //check horizontal line
    moveScore += getLineScore (row, column, 10, piece); //check vertical line
    moveScore += getLineScore (row, column, 11, piece); //check up diagonal line
    moveScore += getLineScore (row, column, -11, piece); //check down diagonal line

    return moveScore;

}

int getLineScore (int row, int column, int direction, char piece) {
    int rightPieces;    //adjacent matching pieces to the right
    int rightSpaces;    //adjacent empty spaces to the right that are open to be played
    int leftPieces;     //adjacent matching pieces to the left
    int leftSpaces;     //adjacent empty spaces to the left that are open to be played
    int lineScore;      //score for the line
    int totalPieces;    //total number of pieces in a row there are
    int validSpaces;    //spaces in a line that have been already played by this player(computer or human) or are open to be played later

    bool openEnded = false;

    //set row and column direction from 1 to -1, depending on which way you will move to evaluate the line; reverse direction is the opposite way
    int rowDirection = direction / 10;
    int columnDirection = abs(direction % 10);
    int rowReverseDirection = rowDirection * -1;
    int columnReverseDirection = columnDirection * -1;

    //get pieces in a row to the left or the space to be played, or down in the case of checking vertically
    leftPieces = getNumberInARow(row, column, rowReverseDirection, columnReverseDirection, piece, false);

    if (columnDirection == 0) { //this evaluates a vertical line, so you don't need to check anything else
        totalPieces = leftPieces + 1; //add one to include the space to be played
        validSpaces = 7 - columnPieces[column] + totalPieces;
    }
    else {
        //get pieces in a row and then playable spaces past the pieces to the right and left
        rightPieces = getNumberInARow(row, column, rowDirection, columnDirection, piece, false);
        rightSpaces = getNumberInARow((row + (rightPieces * rowDirection)), (column + (rightPieces * columnDirection)), rowDirection, columnDirection, '_', true);
        leftSpaces = getNumberInARow((row + (leftPieces * rowReverseDirection)), (column + (leftPieces * columnReverseDirection)), rowReverseDirection, columnReverseDirection, '_', true);

        totalPieces = rightPieces + leftPieces + 1; //add one to include the space to be played
        validSpaces = totalPieces + rightSpaces + leftSpaces; //add up pieces in a row plus immediately playable spaces on each side

        if (validSpaces < 4) //if above calculation doesn't show a potential for a 4IR, check if there are more spaces to the left past the immediately playable spaces
            validSpaces += getNumberInARow((row + ((leftPieces + leftSpaces) * rowReverseDirection)), (column + ((leftPieces + leftSpaces) * columnReverseDirection)), rowReverseDirection, columnReverseDirection, '_', false);
        if (validSpaces < 4) //if still not at least 4, check if there are spaces to the right
            validSpaces += getNumberInARow((row + ((rightPieces + rightSpaces) * rowDirection)), (column + ((rightPieces + rightSpaces) * columnDirection)), rowDirection, columnDirection, '_', false);
        if (rightSpaces > 0 && leftSpaces > 0) //check if there are immediately playable spaces to both the left and right
            openEnded = true;
    }

    //determine score for this line
    if (validSpaces < 4) //score this line as 0 if there isn't any potential for ever getting a 4IR from this line
        return 0;

    if (totalPieces > 3) { //score for 4IR
        if (piece == 'O')
            return 50000; //piece == 'O' means this is the score for the computer to make this move
        else
            return 10000; //else piece == 'X' so this is score for player, or score for computer to block player from making this move

    } else if (totalPieces == 3 && openEnded) { //score for OE3IR
        if (piece == 'O')
            return 5000;
        else
            return 1000;

    } else if (totalPieces == 3) { //score for 3IR
        return 100;

    } else if (totalPieces == 2 && openEnded) { //score for OE2IR
        return 10;

    } else if (totalPieces == 2) { //score for 2IR
        return 1;
    } else //score 0 if there aren't any pieces in a row
        return 0;
}

int getNumberInARow (int row, int column, int rowDirection, int columnDirection, char piece, bool checkPlayable) {
    bool checkContinue = true; //indication if it should keep checking or stop there
    int total = 0; //total pieces in a row found

    while (checkContinue) {
        row += rowDirection;
        column += columnDirection;

        if (row < 0 || column < 0 || row > 7 || column > 7)
            checkContinue = false;  //off the board, stop checking here
        else if (gameBoard[row][column] != piece)
            checkContinue = false;  //piece doesn't match the piece being evaluated, stop checking here
        else { //pieces on this space matches piece being evaluated
            if (!checkPlayable) //add one to total if you're not checking if this space can be played next turn
                total +=1;
            else if (row == 0 || gameBoard[row - 1][column] != '_' ) //add one to total if you're checking if this space can be played next turn and it can be played (there isn't a space under it)
                total +=1;
            else //you are checking if it's playable but it's not, so stop checking here
                checkContinue = false;
        }
    }

    return total;
}



