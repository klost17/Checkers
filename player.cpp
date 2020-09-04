#include "player.hpp"
#include <algorithm>
#include <map>
#include <cstdlib>

// Define infinity as the maximum (signed) INT16 allowed in C++
#define INF INT16_MAX

using namespace std;

struct states{
    checkers::GameState state;
    int pts;
};

//std::map<string,int> Hash;

namespace checkers{

// Declare functions
GameState chooseMove(vector<states> &nextStates,int player,int depth,const Deadline &pDue);
int minimax(const GameState &pState,int depth,int alpha,int beta,int player,const Deadline &pDue);
int heuristics(const GameState &pState);

// Function 1 - compareMin
bool compareMin(struct states s1,struct states s2){
    return (s1.pts<s2.pts);
}

// Function 2 - compareMax
bool compareMax(struct states s1,struct states s2){
    return (s1.pts>s2.pts);
}

// Function 3 - play
GameState Player::play(const GameState &pState,const Deadline &pDue){
    std::vector<GameState> lNextStates;
    pState.findPossibleMoves(lNextStates);
    if (lNextStates.size()==0) return GameState(pState, Move());
    if (lNextStates.size()==1) return lNextStates[0];
    vector<states> nextStates(lNextStates.size());
    for(int i=0; i<(int)lNextStates.size(); i++){
        nextStates[i].state = lNextStates[i];
        nextStates[i].pts = 0;
    }
    // Start with a depth of 1, which may be increased on the fly if there is enough time
    int depth=1;
    GameState returnState = lNextStates[rand() % lNextStates.size()];
    while(pDue.getSeconds()-pDue.now().getSeconds()>0.0001 && depth<10){
        returnState = chooseMove(nextStates,pState.getNextPlayer(),depth,pDue);
        depth++;
    }
    return returnState;
}

// Function 4 - chooseMove
GameState chooseMove(vector<states> &nextStates,int player,int depth,const Deadline &pDue){
    int alpha=-INF,beta=INF,max=-INF,min=INF,ind=0;
    // Maximizing player (WHITE)...
    if(player==CELL_WHITE){
        // For efficiency, nextStates will be ordered from the highest "pts" to the lowest.
        std::sort(nextStates.begin(), nextStates.end(), compareMax);
        for(int i=0; i<(int)nextStates.size(); i++){
            // The minimax function yields the "pts" for a given state
            nextStates[i].pts = minimax(nextStates[i].state,depth,alpha,beta,CELL_RED,pDue);
            // Update the value of "alpha" if "pts" is larger
            if(max<nextStates[i].pts){
                max = nextStates[i].pts;
                ind = i;
            }
            alpha = std::max(alpha,nextStates[i].pts);
            // Pruning
            if(beta<=alpha) break;
        }
    // Minimizing player (RED)...
    }else{
        // For efficiency, nextStates will be ordered from the lowest "pts" to the highest.
        std::sort(nextStates.begin(), nextStates.end(), compareMin);
        for(int i=0; i<(int)nextStates.size(); i++){
            // The minimax function yields the "pts" for a given state
            nextStates[i].pts = minimax(nextStates[i].state,depth,alpha,beta,CELL_WHITE,pDue);
            // Update the value of "beta" if "pts" is lower
            if(min>nextStates[i].pts){
                min = nextStates[i].pts;
                ind = i;
            }
            beta = std::min(beta,nextStates[i].pts);
            // Pruning
            if(beta<=alpha) break;
        }
    }
    return nextStates[ind].state;
}

// Function 5 - minimax (includes alpha-beta pruning)
int minimax(const GameState &pState,int depth,int alpha,int beta,int player,const Deadline &pDue){
    int vMax=0, vMin=0, IDS=0;
    std::vector<GameState> nextStates;
    // Here note that depth 0 means the last layer of the tree
    if(depth==0 || pState.isEOG()){
        if(pState.isWhiteWin()) return INF;
        else if(pState.isRedWin()) return -INF;
        else if(pState.isDraw()) return 0;
        else return heuristics(pState);
    }
    // Recall there is a time limitation!
    if(pDue.getSeconds()-pDue.now().getSeconds()<0.0001){
        return 0;
    }
    pState.findPossibleMoves(nextStates);
    // If maximizing player (WHITE)
    if(player==CELL_WHITE){
        vMax=-INF;
        // Iterative Deepening Search with Hash table
        for(int i=0;i<(int)nextStates.size();i++){
            IDS=minimax(nextStates[i],depth-1,alpha,beta,CELL_RED,pDue);
/*            string s = nextStates[i].toMessage().substr(0,32)+to_string(depth);
            if(Hash.count(s)==0){
                IDS=minimax(nextStates[i],depth-1,alpha,beta,CELL_RED,pDue);
                Hash[s]=IDS;
            }else{
                IDS=Hash[s];
            }*/
            vMax=max(vMax,IDS);
            alpha=max(alpha,IDS);
            // Pruning
            if(beta<=alpha) break;
        }
        return vMax;
    // If minimizing player (RED)
    }else{
        vMin=INF;
        // Iterative Deepening Search with Hash table
        for(int i=0;i<(int)nextStates.size();i++){
            IDS=minimax(nextStates[i],depth-1,alpha,beta,CELL_WHITE,pDue);
/*            string s = nextStates[i].toMessage().substr(0,32)+to_string(depth);
            if(Hash.count(s)==0){
                IDS=minimax(nextStates[i],depth-1,alpha,beta,CELL_WHITE,pDue);
                Hash[s]=IDS;
            }else{
                IDS=Hash[s];
            }*/
            vMin=min(vMin,IDS);
            beta=min(beta,IDS);
            // Pruning
            if(beta<=alpha) break;
        }
        return vMin;
    }
}

// Function 6 - heuristics
int heuristics(const GameState &pState){
    int h=0;
    for(int i=0;i<=31;i++){
        if(pState.at(i)==CELL_WHITE && pState.at(i)==CELL_KING){
            h+=2; // A white king is given a value of +2
        }else if(pState.at(i)==CELL_WHITE){
            h+=1; // A white man is given a value of +1
        }else if(pState.at(i)==CELL_RED && pState.at(i)==CELL_KING){
            h-=2; // A red king is given a value of -2
        }else if(pState.at(i)==CELL_RED){
            h-=1; // A red man is given a value of -1
        }
    }
    return h;
}
}