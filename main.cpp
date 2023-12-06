#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <ctime>
#include <map>
#include <algorithm>
#include <limits>
#include <list>
using namespace std;

void waitForEnter() {
    cout << "Press Enter to continue...";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    system("cls");
}


// ===================================================== Scoreboard ======================================================================================
const int MAX_PLAYERS = 10;

struct PlayerScore {
    string name;
    int score;
};

void readScoreboardFromFile(PlayerScore scoreboard[], int& numPlayers, const string& filename) {
    ifstream file(filename);

    numPlayers = 0;
    while (file >> scoreboard[numPlayers].name >> scoreboard[numPlayers].score) {
        numPlayers++;
        if (numPlayers >= MAX_PLAYERS) {
            break;
        }
    }
}

void writeScoreboardToFile(const PlayerScore scoreboard[], int numPlayers, const string& filename) {
    ofstream file(filename);

    for (int i = 0; i < numPlayers; ++i) {
        file << scoreboard[i].name << " " << scoreboard[i].score << "\n";
    }
}

void sortScoreboard(PlayerScore scoreboard[], int numPlayers) {
    for (int i = 0; i < numPlayers - 1; ++i) {
        for (int j = 0; j < numPlayers - i - 1; ++j) {
            if (scoreboard[j].score < scoreboard[j + 1].score) {
                swap(scoreboard[j], scoreboard[j + 1]);
            }
        }
    }                                                               // I'll be honest, this part would be impossible without Chat-GPT, bless microsoft
}
// ================================================================ Preferences =======================================================================================

const int EMPTY = 0;
const int MISS = 1;
const int HIT = 2;
const int SHIP = 5;

void printBoard(int board[7][7]) {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if (board[i][j] == EMPTY) {
                cout << "- ";                                       // 0 - an empty block
            } else if (board[i][j] == MISS) {                       // 1 - shotten block (miss)
                cout << "* ";                                       // 2 - hitten
            } else if (board[i][j] == HIT) {                        // 5 - ship part
                cout << "x ";                                       
            } else if (board[i][j] == SHIP) {
                cout << "S ";
            }
        }
        cout << endl;
    }
}
//   =============================================================== Random ship generator =============================================================================
bool canPlaceShip(int board[7][7], int start_x, int end_x, int start_y, int end_y) {
    if (start_x < 0 || end_x >= 7 || start_y < 0 || end_y >= 7) {
        return false;
    }

    for (int i = start_x - 1; i <= end_x + 1; i++) {
        for (int j = start_y - 1; j <= end_y + 1; j++) {
            if (i >= 0 && i < 7 && j >= 0 && j < 7 && board[i][j] != 0) {
                return false;
            }
        }
    }

    for (int i = start_x; i <= end_x; i++) {
        for (int j = start_y; j <= end_y; j++) {
            if (board[i][j] != 0) {
                return false;
            }
        }
    }

    for (int i = start_x; i <= end_x; i++) {
        for (int j = start_y; j <= end_y; j++) {
            if ((i > 0 && board[i - 1][j] != 0) ||
                (i < 6 && board[i + 1][j] != 0) ||
                (j > 0 && board[i][j - 1] != 0) ||
                (j < 6 && board[i][j + 1] != 0)) {
                return false;
            }
        }
    }
    return true;
}


void placeShip(int board[7][7], int length) {

    while (true) {
        bool isHorizontal = rand() % 2 == 0;
        int start_x, end_x, start_y, end_y;

        if (isHorizontal) {
            start_y = (rand() % 7) - length + 1;
            start_x = rand() % 7;
            end_x = start_x;
            end_y = start_y + length - 1;
        } else {
            start_x = (rand() % 7) - length + 1;
            start_y = rand() % 7;
            end_x = start_x + length - 1;
            end_y = start_y;
        }

        if (canPlaceShip(board, start_x, end_x, start_y, end_y)) {
            for (int i = start_x; i <= end_x; i++) {
                for (int j = start_y; j <= end_y; j++) {
                    board[i][j] = SHIP;
                }
            }
            break;
        }
    }
}
// ================================================================ Game functions ==============================================================================================

int noShip(int board[7][7], int x, int y) {
    int res = 0;
    for (int i = x-1; i <= x+1; i++) {
        for (int j = y-1; j <= y+1; j++) {
            if (board[i][j] == SHIP) {
                res++;
            }
        }
    }
    return res;
}


void bigShipDestroyed(int board[7][7], int x, int y) {
    int new_x = 99, new_y = 99;
    for (int i = max(0, x - 1); i <= min(6, x + 1); i++) {
        for (int j = max(0, y - 1); j <= min(6, y + 1); j++) {
            if (board[i][j] == HIT) {
                new_x = i;
                new_y = j;
                board[i][j] = MISS;
            }
            board[i][j] = MISS;
        }
    }
    if (new_x != 99) {
        bigShipDestroyed(board, new_x, new_y);
    }
}

// ================================================================ Main game ==================================================================================================

void mainGame(int ships[7][7], int you[7][7], int& score) {
    int win = 0, main_count = 0;
    cout << "\nWelcome to the Sea Battle\n\n";
    while (win == 0) {
        int a, b, pop = 0, count = 0;
        printBoard(you);
        cout  << "\nEnter the coordinates of your attack!: ";
        cin >> a >> b;
        if (b < 0 || b > 6 || a < 0 || a > 6) {
            while (b < 0 || b > 6 || a < 0 || a > 6) {
                cout << "Your coordinates are incorrect! write another ones!: ";
                cin >> a >> b;
            }
        }
        if (ships[a][b] == 1 || ships[a][b] == 2 || ships[a][b] == 3) {
            while (ships[a][b] == 1 || ships[a][b] == 2 || ships[a][b] == 3) {
                cout << "You already have hitten this point write another!: ";
                cin >> a >> b;
            }
        }
        if (ships[a][b] == 0) {
            cout << "\nMiss!\n" << endl;
            ships[a][b] = MISS;
            you[a][b] = MISS;
            printBoard(you);
        } else if (ships[a][b] == SHIP and noShip(ships, a, b) == 1) {
            cout << "\nSunk!\n";
            bigShipDestroyed(ships, a, b);
            bigShipDestroyed(you, a, b);
            printBoard(you);
        } else if (ships[a][b] == SHIP and noShip(ships, a, b) > 1) {
            cout << "\nHit!\n";
            ships[a][b] = HIT;
            you[a][b] = HIT;
            printBoard(you);
        }
        main_count++;

        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                if (ships[i][j] == 5) {
                    pop++;
                }
            }
        }
        if (pop != 0) {
            waitForEnter();
        } else {
            win = 1;
            if (main_count == 0) {
                main_count = count;
            } else {
                if (count > main_count) {
                    main_count = count;
                }
            }
        }
    }
    string answer;
    cout << endl << "amount of your attempts: " << main_count <<"\n\nDo you want to continue? [y/n]: ";
    cin >> answer;
    if (answer == "y") {
        mainGame(ships, you, score);
    } else {
        score = main_count;
    }
}


int main() {
    int me[7][7], game[7][7], win = 0, score = 0;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            me[i][j] = 0;
            game[i][j] = 0;
        }
    }

    srand(time(0));
    int ships[] = {1, 1, 1, 1, 2, 2, 3};
    for (int i = 0; i < 7; i++) {
        placeShip(me, ships[i]);
    }

    PlayerScore scoreboard[MAX_PLAYERS];
    int numPlayers = 0;

    string filename = "scoreboard.txt";
    readScoreboardFromFile(scoreboard, numPlayers, filename);   

    string name;
    cout << "Hi there! Input your name so we can record your score: ";
    cin >> name;

    mainGame(me, game, score);

    if (numPlayers < MAX_PLAYERS) {
        scoreboard[numPlayers].name = name;
        scoreboard[numPlayers].score = score;
        numPlayers++;
    } else {
        if (score > scoreboard[MAX_PLAYERS - 1].score) {
            scoreboard[MAX_PLAYERS - 1].name = name;
            scoreboard[MAX_PLAYERS - 1].score = score;
        }
    }

    sortScoreboard(scoreboard, numPlayers);
    writeScoreboardToFile(scoreboard, numPlayers, filename);

    cout << "\nScoreboard (from the best to the worst):\n";
    for (int i = 0; i < numPlayers; ++i) {
        cout << scoreboard[i].name << ": " << scoreboard[i].score << endl;
    }
    
}


