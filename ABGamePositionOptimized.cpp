// Christian Duffee (cbd170000)
// CS 6364.0U1
// Please forgive code, I haven't coded in C++ since CS 2336

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <math.h>
#include <climits>

using namespace std;

// max size of children to consider, a value of 18^2 was chosen
static const int ARRAY_SIZE = 324;

// static const int INT_MAX = 2147483647;
// static const int INT_MIN = -2147483648;

// an adjacency matrix representing whether a connection exists between any two positions
const static bool adj[18][18] = {0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0};
static short startboard[387420489];
static short board[387420489];

static const int EMPTY = 0;
static const int WHITE = 1;
static const int BLACK = 2;

static int color = 0;
static int targetdepth = 0;
static int placedepth = 16;

static int positionsEvaluated = 0; 

static int bestChildVal = INT_MIN;
static int bestChildPos = 0;

int stringtoint(string line);
string inttostring(int input);
int staticestartingstimate(int pos);
int staticestimate(int pos);
bool closeMill(int pos, int index, int c);
int get(int pos, int index);
int getPos(int pos, int index, int val);
int getbeststartmove(int pos, int depth, int alpha, int beta);
int getbestmove(int pos, int depth, int alpha, int beta);
array<int, ARRAY_SIZE> GenerateAdd(int pos, int c);
array<int, ARRAY_SIZE> GenerateMovesMidgameEndgame(int pos, int c);
array<int, ARRAY_SIZE> GenerateRemove(int pos, int c);
array<int, ARRAY_SIZE> GenerateHopping(int pos, int c);
array<int, ARRAY_SIZE> GenerateMove(int pos, int c);

// Main method which takes 3 arguments: string of input file name, string of output file name, depth to search
int main(int argc, char** argv) {
    color = strtol(argv[3], NULL, 10);
    targetdepth = strtol(argv[4], NULL, 10);
    placedepth = strtol(argv[5], NULL, 10);

    string line;
    ifstream myfile (argv[1]);
    getline (myfile,line);
    int startpos = stringtoint(line);

    int rootVal = getbeststartmove(startpos, 0, INT_MIN, INT_MAX);

    cout << "Board Position: " << inttostring(bestChildPos) << endl;
    cout << "Positions evaluated by static estimation: " << positionsEvaluated << endl;
    cout << "MINIMAX estimate: " << rootVal << endl;

    ofstream outfile;
    outfile.open(argv[2]);
    outfile << inttostring(bestChildPos);
    outfile.close();
}

// Performs the minimax search with limits alpha and beta, setting the minimax value to root, and returning it
int getbeststartmove(int pos, int depth, int alpha, int beta) {
    int localMin = INT_MAX;
    int localMax = INT_MIN;
    if(depth==placedepth){
        return getbestmove(pos,depth,alpha,beta);
    }
    if(depth==targetdepth){
        if(startboard[pos]==0){
            startboard[pos] = staticestartingstimate(pos);
        }
        return startboard[pos];
    }else{
        array<int, ARRAY_SIZE> moves = GenerateAdd(pos, depth%2+1); // WHITE for even, BLACK for odd
        int n = 0;
        int move = moves[n];
        while(move != 0){
            int childVal = getbeststartmove(move,depth+1,alpha,beta);
            if(depth==0 && childVal>bestChildVal){
                bestChildPos = move;
                bestChildVal = childVal;
            }
            localMax = max(localMax, childVal);
            localMin = min(localMin, childVal);
            
            if(depth%2+1==WHITE){
                alpha = max(alpha, localMax);
                if(beta <= alpha){
                    return INT_MAX;
                }
            }else{
                beta = min(beta, localMin);
                if(beta <= alpha){
                    return INT_MIN;
                }
            }
            move = moves[++n];         
        }
    }
    if(depth%2==0){
        return localMax;
    }else{
        return localMin;
    }
}

// Performs the minimax search with limits alpha and beta, setting the minimax value to root, and returning it
int getbestmove(int pos, int depth, int alpha, int beta) {
    int localMin = INT_MAX;
    int localMax = INT_MIN;
    if(depth==targetdepth){
        if(board[pos]==0){
            board[pos] = staticestimate(pos);
        }
        return board[pos];
    }else{
        array<int, ARRAY_SIZE> moves = GenerateMovesMidgameEndgame(pos, depth%2+1); // WHITE for even, BLACK for odd
        int n = 0;
        int move = moves[n];
        while(move != 0){
            int childVal = getbestmove(move,depth+1,alpha,beta);
            if(depth==0 && childVal>bestChildVal){
                bestChildPos = move;
            }

            localMax = max(localMax, childVal);
            localMin = min(localMin, childVal);
            
            if(depth%2+1==WHITE){
                alpha = max(alpha, localMax);
                if(beta <= alpha){
                    return INT_MAX;
                }
            }else{
                beta = min(beta, localMin);
                if(beta <= alpha){
                    return INT_MIN;
                }
            }
            move = moves[++n];         
        }
    }
    if(depth%2==0){
        return localMax;
    }else{
        return localMin;
    }
}

// return a list of all possible moves when adding a piece of color c from position pos
array<int, ARRAY_SIZE> GenerateAdd(int pos, int c){
    array<int, ARRAY_SIZE> moves = {0};
    int n = 0;
    for(int i =0; i<18; i++){
        if(get(pos,i)==EMPTY){
            if(closeMill(pos,i,c)){
                array<int, ARRAY_SIZE> reMoves = GenerateRemove(getPos(pos,i,c),c);
                for(int j = 0; j < ARRAY_SIZE; j ++ ){
                    if(reMoves[j]!=EMPTY){
                        moves[n++] = reMoves[j];
                    }
                }
            }else{
                moves[n++] = getPos(pos,i,c);
            }
        }
    }
    return moves;
}
// return a list of all possible moves during the midgame or endgame for color c from position pos
array<int, ARRAY_SIZE> GenerateMovesMidgameEndgame(int pos, int c){
    int cCount = 0;
    for(int i =0; i<18; i++){
        if(get(pos,i)==c){
            cCount++;
            if(cCount>3){
                i = 18;
            }
        }
    }
    if(cCount>3){
        return GenerateMove(pos, c);            
    }else if(cCount==3){
        return GenerateHopping(pos, c);  
    }else{
        array<int, ARRAY_SIZE> moves = {0};
        moves[0] = pos;
        return moves;
    }   
}

// return a list of all possible moves during the endgame for color c from position pos
array<int, ARRAY_SIZE> GenerateHopping(int pos, int c){
    array<int, ARRAY_SIZE> moves = {0};
    int n = 0;
    for(int i =0; i<18; i++){
        if(get(pos,i)==c){
            for(int k=0; k<18; k++){
                if(get(pos,k)==EMPTY){
                    int emptyPos = getPos(pos,i,EMPTY);
                    int newPos = getPos(emptyPos,k,c);
                    if(closeMill(emptyPos,k,c)){
                        array<int, ARRAY_SIZE> reMoves = GenerateRemove(newPos,c);
                        for(int j = 0; j < ARRAY_SIZE; j ++ ){
                            if(reMoves[j]!=EMPTY){
                                moves[n++] = reMoves[j];
                            }
                        }
                    }else{
                        moves[n++] = newPos;
                    }
                }
            }            
        }
    }
    return moves;
}

// return a list of all possible moves during the midgame for color c from position pos
array<int, ARRAY_SIZE> GenerateMove(int pos, int c){
    array<int, ARRAY_SIZE> moves = {0};
    int n = 0;
    for(int i =0; i<18; i++){
        if(get(pos,i)==c){
            for(int k=0; k<18; k++){
                if(adj[i][k] && get(pos,k)==EMPTY){
                    int emptyPos = getPos(pos,i,EMPTY);
                    int newPos = getPos(emptyPos,k,c);
                    if(closeMill(emptyPos,k,c)){
                        array<int, ARRAY_SIZE> reMoves = GenerateRemove(newPos,c);
                        for(int j = 0; j < ARRAY_SIZE; j ++ ){
                            if(reMoves[j]!=EMPTY){
                                moves[n++] = reMoves[j];
                            }
                        }
                    }else{
                        moves[n++] = newPos;
                    }
                }
            }            
        }
    }
    return moves;
}

// return a list of all possible moves when removing a piece for color !c from position pos
array<int, ARRAY_SIZE> GenerateRemove(int pos, int c){
    int d = WHITE;
    if(c==WHITE){
        d = BLACK;
    }
    array<int, ARRAY_SIZE> moves = {0};
    int n = 0;
    for(int i =0; i<18; i++){
        if(get(pos,i)==d && !closeMill(pos,i,d)){
            moves[n++] = getPos(pos,i,EMPTY);
        }
    }
    if(n==0){
        moves[0] = pos;
    }
    return moves;
}

// returns the int representation of position line
int stringtoint(string line){
    int sum = 0;
    int mult = 1;
    for(int i =0; i<19; i++){
        int val = 0;
        if((color==WHITE && line[i]=='W') || (color==BLACK && line[i]=='B')){
            val = 1;
        }else if ((color==WHITE && line[i]=='B') || (color==BLACK && line[i]=='W')){
            val = 2;
        }
        sum += val * mult;
        mult *= 3;
    }
    return sum;
}

// returns the string representation of position input
string inttostring(int input){
    string line(18,'x');
    for(int i =0; i<18; i++){
        int val = input % 3;
        if(val==1){
            if(color==WHITE){
                line[i] = 'W';
            }else{
                line[i] = 'B';
            }
        }else if (val == 2){
            if(color==WHITE){
                line[i] = 'B';
            }else{
                line[i] = 'W';
            }
        }
        input /= 3;
    }
    return line;
}

int staticestartingstimate(int pos){
    positionsEvaluated++;
    int estimate=0;
    for(int i =0; i<18; i++){
        int val = get(pos,i);
        if(val==1){
            estimate++;
        }else if (val == 2){
            estimate--;
        }
    }
    return estimate;
}

// returns the static estimate of position pos
int staticestimate(int pos){
    positionsEvaluated++;

    array<int, ARRAY_SIZE> moves = GenerateMovesMidgameEndgame(pos, 2); // WHITE for even, BLACK for odd
    int moveCount = 0;
    while(moves[moveCount]!=0){moveCount++;}
    int whiteCount=0;
    int blackCount=0;
    for(int i =0; i<18; i++){
        int val = get(pos,i);
        if(val==WHITE){
            whiteCount++;
        }else if (val == BLACK){
            blackCount++;
        }
    }
    if(whiteCount<=2){
        return -10000;
    }
    if(blackCount<=2 || moveCount==0){
        return 10000;
    }
    return 1000*(whiteCount-blackCount)-moveCount;
}

// returns the value of the piece at index, 0 null, 1 white, 2 black
int get(int pos, int index){
    return pos / lround(pow(3,index)) % 3;
}

// return the new position if you set index to val
int getPos(int pos, int index, int val){
    return (pos / lround(pow(3,index+1)) * lround(pow(3,index+1))) + (val * pow(3,index)) + (pos % lround(pow(3,index)));
}

// return if color c at index of position pos would, or does, complete a mill
bool closeMill(int pos, int index, int c){
    switch(index){
        case 0:
            return get(pos,2)==c && get(pos,4)==c;
        break;
        case 1:
            return (get(pos,3)==c && get(pos,5)==c) || (get(pos,8)==c && get(pos,17)==c);
        break;
        case 2:
            return get(pos,0)==c && get(pos,4)==c;
        break;
        case 3:
            return (get(pos,1)==c && get(pos,5)==c) || (get(pos,7)==c && get(pos,14)==c);
        break;
        case 4:
            return get(pos,0)==c && get(pos,2)==c;
        break;
        case 5:
            return (get(pos,1)==c && get(pos,3)==c) || (get(pos,6)==c && get(pos,11)==c);
        break;
        case 6:
            return (get(pos,5)==c && get(pos,11)==c) || (get(pos,7)==c && get(pos,8)==c);
        break;
        case 7:
            return (get(pos,6)==c && get(pos,8)==c) || (get(pos,3)==c && get(pos,14)==c);
        break;
        case 8:
            return (get(pos,6)==c && get(pos,7)==c) || (get(pos,1)==c && get(pos,17)==c);
        break;
        case 9:
            return (get(pos,10)==c && get(pos,11)==c) || (get(pos,12)==c && get(pos,15)==c);
        break;
        case 10:
            return (get(pos,9)==c && get(pos,11)==c) || (get(pos,13)==c && get(pos,16)==c);
        break;
        case 11:
            return (get(pos,9)==c && get(pos,10)==c) || (get(pos,5)==c && get(pos,6)==c) || (get(pos,14)==c && get(pos,17)==c);
        break;
        case 12:
            return (get(pos,9)==c && get(pos,15)==c) || (get(pos,13)==c && get(pos,14)==c);
        break;
        case 13:
            return (get(pos,10)==c && get(pos,16)==c) || (get(pos,12)==c && get(pos,14)==c);
        break;
        case 14:
            return (get(pos,3)==c && get(pos,7)==c) || (get(pos,12)==c && get(pos,13)==c);
        break;
        case 15:
            return (get(pos,9)==c && get(pos,12)==c) || (get(pos,16)==c && get(pos,17)==c);
        break;
        case 16:
            return (get(pos,15)==c && get(pos,17)==c) || (get(pos,10)==c && get(pos,13)==c);
        break;
        case 17:
            return (get(pos,15)==c && get(pos,16)==c) || (get(pos,11)==c && get(pos,14)==c) || (get(pos,1)==c && get(pos,8)==c);
        break;        
    }
    return false;
}
