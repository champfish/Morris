// Please forgive code, I haven't coded in C++ since CS 2336

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int stringtoint(string line);
string inttostring(int input);

struct node  { 
    int data; 
    struct node *left; 
    struct node *right; 
}; 
   
struct node* newNode(int data) { 
    struct node* node = new struct node(); 
    node->data = data;
    node->left = NULL; 
    node->right = NULL; 
    return(node); 
} 

// 387420489
static int board[387420489][4];

int main(int argc, char** argv) {
    int targetDepth = argv[3];

    string line;
    ifstream myfile (argv[1]);
    getline (myfile,line);
    int startpos = stringtoint(line);
    cout << startpos << '\n';

    int i = 69;
    cout << "Board Position: " << i << endl;
    cout << "Positions evaluated by static estimation: " << i << endl;
    cout << "MINIMAX estimate: " << i << endl;

    ofstream outfile;
    outfile.open(argv[2]);
    outfile << inttostring(startpos);
    outfile.close();
}

// Todo fix
int[] GenerateAdd(int pos){
    int mutpos = pos;
    int moves[17];
    int index = 0;
    for(int i =17; i>=0; i--){
        int val = 0;
        if(line[i]=='W'){
            val = 1;
        }else if (line[i] == 'B'){
            val = 2;
        }
        sum += val * mult;
        mult *= 3;
    }
}

int stringtoint(string line){
    int sum = 0;
    int mult = 1;
    for(int i =17; i>=0; i--){
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

string inttostring(int input){
    string line(18,'x');
    for(int i =17; i>=0; i--){
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
