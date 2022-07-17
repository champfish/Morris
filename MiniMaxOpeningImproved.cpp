// Christian Duffee (cbd170000)
// CS 6364.0U1
// Please forgive code, I haven't coded in C++ since CS 2336

#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

// max size of children to consider, a value of 18^2 was chosen
static const int ARRAY_SIZE = 324;

// an adjacency matrix representing whether a connection exists between any two positions
const static bool adj[18][18] = {0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0};

static const int EMPTY = 0;
static const int WHITE = 1;
static const int BLACK = 2;

static int targetdepth = 0;
static int positionsEvaluated = 0;

// Node object 
struct Node {
    int val;
    int pos;
    int depth;
    vector<Node *>children;
};
  
 // Node constructor
Node *newNode(int pos, int depth) {
    Node *temp = new Node;
    temp->val = 0;
    temp->pos = pos;
    temp->depth = depth;
    return temp;
}

int stringtoint(string line);
string inttostring(int input);
int staticestimate(int pos);
bool closeMill(int pos, int index, int c);
int get(int pos, int index);
int getPos(int pos, int index, int val);
int getbestmove(Node * root);
array<int, ARRAY_SIZE> GenerateMovesOpening(int pos, int c);
array<int, ARRAY_SIZE> GenerateAdd(int pos, int c);
array<int, ARRAY_SIZE> GenerateRemove(int pos, int c);

// Main method which takes 3 arguments: string of input file name, string of output file name, depth to search
int main(int argc, char** argv) {
    targetdepth = strtol(argv[3], NULL, 10);

    string line;
    ifstream myfile (argv[1]);
    getline (myfile,line);
    int startpos = stringtoint(line);

    Node* root = newNode(startpos,0);

    int rootVal = getbestmove(root);
    int bestChildPos = 0;
    int bestChildVal = INT_MIN;
    for(unsigned long int i = 0; i < root->children.size(); i++){
        if(root->children[i]->val>bestChildVal){
            bestChildVal = root->children[i]->val;
            bestChildPos = root->children[i]->pos;
        }
    }
    cout << "Board Position: " << inttostring(bestChildPos) << endl;
    cout << "Positions evaluated by static estimation: " << positionsEvaluated << endl;
    cout << "MINIMAX estimate: " << rootVal << endl;

    ofstream outfile;
    outfile.open(argv[2]);
    outfile << inttostring(bestChildPos);
    outfile.close();
}

// Performs the minimax search, setting the minimax value to root, and returning it
int getbestmove(Node * root) {
    int min = INT_MAX;
    int max = INT_MIN;
    if(root->depth==targetdepth){
        root->val = staticestimate(root->pos);
        return root -> val;
    }else{
        array<int, ARRAY_SIZE> moves = GenerateMovesOpening(root->pos, root->depth%2+1); // WHITE for even, BLACK for odd
        int n = 0;
        int move = moves[n];
        while(move != 0){
            Node * child = newNode(move,root->depth+1);
            (root->children).push_back(child);
            int childVal = getbestmove(child);
            if(childVal<min){
                min = childVal;
            }
            if(childVal>max){
                max = childVal;
            }
            move = moves[++n];         
        }
    }
    if(root->depth%2==0){
        root->val = max;
        return max;
    }else{
        root->val = min;
        return min;
    }
}

// return a list of all possible moves during the opening stage for color c from position pos
array<int, ARRAY_SIZE> GenerateMovesOpening(int pos, int c){
    return GenerateAdd(pos, c);
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
    return moves;
}

// returns the int representation of position line
int stringtoint(string line){
    int sum = 0;
    int mult = 1;
    for(int i =0; i<19; i++){
        int val = 0;
        if(line[i]=='W'){
            val = 1;
        }else if (line[i] == 'B'){
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
            line[i] = 'W';
        }else if (val == 2){
            line[i] = 'B';
        }
        input /= 3;
    }
    return line;
}

// returns the static estimate of position pos
int staticestimate(int pos){
    positionsEvaluated++;
    int estimate=0;
    for(int i =0; i<18; i++){
        int val = get(pos,i);
        if(val == WHITE){
            estimate+=2;
        }else if (val == BLACK){
            estimate-=2;
        }else if(closeMill(pos,i,WHITE)){
            estimate++;
        }else if(closeMill(pos,i,BLACK)){
            estimate--;
        }
    }
    return estimate;
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
