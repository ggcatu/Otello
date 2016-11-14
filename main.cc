// Game of Othello -- Example of main
// Universidad Simon Bolivar, 2012.
// Author: Blai Bonet
// Last Revision: 1/11/16
// Modified by: 

#include <iostream>
#include <limits>
#include "othello_cut.h" // won't work correctly until .h is fixed!
#include "utils.h"
#include <vector>
#include <algorithm>
#include <cmath>

#include <unordered_map>

using namespace std;

unsigned expanded = 0;
unsigned generated = 0;
int tt_threshold = 32; // threshold to save entries in TT

// Transposition table
struct stored_info_t {
    int value_;
    int type_;
    enum { EXACT, LOWER, UPPER };
    stored_info_t(int value = -100, int type = LOWER) : value_(value), type_(type) { }
};

struct hash_function_t {
    size_t operator()(const state_t &state) const {
        return state.hash();
    }
};

class hash_table_t : public unordered_map<state_t, stored_info_t, hash_function_t> {
};

hash_table_t TTable[2];

int maxmin(state_t state, int depth, bool use_tt);

int minmax(state_t state, int depth, bool use_tt = false){
    //if (state.terminal()) return state.value();
    if (state.terminal()) {
        //cout << "Valor: " << state.value() << endl;
        return state.value();
    }
    expanded++;

    int tmp;
    int score = 100000;
    state_t child;
    vector<int> valid_moves = state.get_valid_moves(false);
    if (valid_moves.size() != 0) {
        for (unsigned i = 0; i < valid_moves.size() ; i++) {
            child = state.move(false, valid_moves[i]);
            generated++;
            //cout << "Jugó Blanco" << endl;
            //cout << child << endl;
            score = score < (tmp = maxmin(child, depth - 1, use_tt)) ? score : tmp;
            //cout << "Valor: " << tmp << endl; 
            //score = std::min(score, maxmin(child, depth - 1, use_tt));
        }
    }
    return score;
};

int maxmin(state_t state, int depth, bool use_tt = false) {
    if (state.terminal()) {
        //cout << "Valor: " << state.value() << endl;
        return state.value();
    }
    expanded++;

    int tmp;
    int score = -100000;
    state_t child;
    vector<int> valid_moves = state.get_valid_moves(true);
    if (valid_moves.size() != 0) {
        for (unsigned i = 0; i < valid_moves.size() ; i++) {
            child = state.move(true, valid_moves[i]);
            generated++;
            //cout << "Jugó Negro" << endl;
            //cout << child << endl;
            
            score = score < (tmp = minmax(child, depth - 1, use_tt)) ? tmp : score;  
            //score = std::max(score, minmax(child, depth - 1, use_tt));
            //cout << "Valor: " << tmp << endl;
        }
    }
    return score;
};

int negamax(state_t state, int depth, int color, bool use_tt = false){
    if (state.terminal()) {
        //cout << "Valor: " << state.value() << endl;
        return color*state.value();
    }
    expanded++;

    int alpha = -100000;
    int tmp;
    bool mv = false;
    state_t child;
    if(color == 1){
        mv = true;
    }
    vector<int> valid_moves = state.get_valid_moves(mv);
    if(valid_moves.size() != 0){
        for(unsigned i = 0; i < valid_moves.size(); i++){
            child = state.move(mv,valid_moves[i]);
            generated++;
            alpha = alpha > (tmp = -negamax(child,depth-1,-color,use_tt)) ? alpha : tmp;
        }
    }
    return alpha;


};

int negamax(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false){
    if (state.terminal()) {
            //cout << "Valor: " << state.value() << endl;
            return color*state.value();
        }
    expanded++;
    
    int score = -100000;
    int val;
    bool mv = false;
    state_t child;
    if(color == 1){
        mv = true;
    }
    vector<int> valid_moves = state.get_valid_moves(mv);
    for(unsigned i = 0; i < valid_moves.size(); i++){
        child = state.move(mv,valid_moves[i]);
        generated++;
        val = -negamax(child,depth-1,-beta,-alpha,-color,use_tt);
        score = score > val ? score : val;
        alpha = alpha > val ? alpha : val;
        if(alpha >= beta){
            break;
        }
    }
    return score;
};


bool mayor(int a, int b){
    return a>b;
}
bool mayorigual(int a, int b){
    return a>=b;
}


// COMO SE SI ES MAX O ES MIN? color == 1 ? NOP
/*
bool test(state_t state, int depth, int color, int score, bool func(int,int), int max){
    if(state.terminal()){
        return func(color*state.value(),score) ? true : false;
    }
    bool mv = false;
    state_t child;
    if(color == 1){
        mv = true;
    }
    vector<int> valid_moves = state.get_valid_moves(mv);
    for(unsigned i = 0; i < valid_moves.size(); i++){
        child = state.move(mv,valid_moves[i]);
        if (max == 1 && test(child,depth-1,-color, score, func, -max)){
            return true;
        }
        if (max == -1 && !test(child,depth-1,-color, score, func, -max)){
            return false;
        }
    }
    return max == 1 ? false : true;
}


int scout_m(state_t state, int depth, int color, int max, bool use_tt = false){
    if(state.terminal()){
        return color*state.value();
    }
    expanded++;
    int score = 0;
    bool mv = false;
    state_t child;
    if(color == 1){
        mv = true;
    }
    vector<int> valid_moves = state.get_valid_moves(mv);
    for(unsigned i = 0; i < valid_moves.size(); i++){
        child = state.move(mv,valid_moves[i]);
        generated++;
        if (i == 0) {
            score = scout_m(child,depth-1,-color, -max,use_tt);
        } 
        else {
            if (max == 1 && test(child,depth,-color, score,mayor,-max)) {
                score = scout_m(child, depth-1, -color, -max, use_tt);
            }
            if (max == -1 && !test(child,depth,-color, score,mayorigual,-max)){
                score = scout_m(child, depth-1, -color, -max, use_tt);
            }
        }
    }
    return score;
};

int scout(state_t state, int depth, int color, bool use_tt = false){
    return scout_m(state, depth, color, 1, use_tt);
};
*/

bool test(state_t state, int depth, int color, int score, bool func(int,int)) {
    if (state.terminal()) {
        //return func(state.value(),score) ? true : false;
        return func(state.value(),score) ? true : false;
    }
    state_t child;
    vector<int> valid_moves = state.get_valid_moves(color == 1);
    for(unsigned i = 0; i < valid_moves.size(); i++){
        child = state.move(color == 1,valid_moves[i]);
        if (color == 1 && test(child, depth-1, -color, score, func)){
            return true;
        }
        if (color == -1 && !test(child, depth-1, -color, score, func)){
            return false;
        }
    }
    return color == 1 ? false : true;
}

int scout(state_t state, int depth, int color, bool use_tt = false) {
    if (state.terminal()) {
        //cout << state.value() << endl; 
        //return state.value();
        return state.value();
    }
    expanded++;
    int score = 0;
    state_t child;
    vector<int> valid_moves = state.get_valid_moves(color == 1);
    for (unsigned i = 0; i < valid_moves.size(); i++) {
        //color == 1 ? cout << "Juega Negro" << endl : cout << "Juega Blanco" << endl; 
        child = state.move(color == 1,valid_moves[i]);
        //cout << child;
        generated++;
        if (i == 0) {
            score = scout(child, depth-1, -color, use_tt);
        } 
        else {
            if (color == 1 && test(child, depth, -color, score, mayor)) {
                score = scout(child, depth-1, -color, use_tt);
            }
            if (color == -1 && !test(child, depth, -color, score, mayorigual)){
                score = scout(child, depth-1, -color, use_tt);
            }
        }
    }
    return score;
};

int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false){
    if (state.terminal()){
        return color*state.value();
    }
    expanded++;
    int score;
    state_t child;
    bool mv = false;
    if(color == 1){
        mv = true;
    }
    vector<int> valid_moves = state.get_valid_moves(mv);
    // cout << "Color : " << color << " Valid moves: " << valid_moves.size() << endl;
    for(unsigned i = 0; i < valid_moves.size(); i++){
        child = state.move(mv,valid_moves[i]);
        generated++;
        if (i == 0){
            score = -negascout(child,depth-1,-beta,-alpha,-color,use_tt);
        } else {
            score = -negascout(child,depth-1,-alpha-1,-alpha,-color,use_tt);
            if (alpha < score && score < beta){
                score = -negascout(child,depth-1,-beta,-score,-color,use_tt);
            }
        }
        alpha = alpha > score ? alpha : score;
        if (alpha >= beta) break;
    }
    // cout << "Alpha : " << alpha << " Score: " << score << endl;
    return alpha;
};

int main(int argc, const char **argv) {
    state_t pv[128];
    int npv = 0;
    for( int i = 0; PV[i] != -1; ++i ) ++npv;

    int algorithm = 0;
    if( argc > 1 ) algorithm = atoi(argv[1]);
    bool use_tt = argc > 2;

    // Extract principal variation of the game
    state_t state;
    cout << "Extracting principal variation (PV) with " << npv << " plays ... " << flush;
    for( int i = 0; PV[i] != -1; ++i ) {
        bool player = i % 2 == 0; // black moves first!
        int pos = PV[i];
        pv[npv - i] = state;
        state = state.move(player, pos);
    }
    pv[0] = state;
    cout << "done!" << endl;

#if 0
    // print principal variation
    for( int i = 0; i <= npv; ++i )
        cout << pv[npv - i];
#endif

    // Print name of algorithm
    cout << "Algorithm: ";
    if( algorithm == 0 ) {
        cout << "Minmax-Maxmin";
    } else if( algorithm == 1 ) {
        cout << "Negamax (minmax version)";
    } else if( algorithm == 2 ) {
        cout << "Negamax (alpha-beta version)";
    } else if( algorithm == 3 ) {
        cout << "Scout";
    } else if( algorithm == 4 ) {
        cout << "Negascout";
    }
    cout << (use_tt ? " w/ transposition table" : "") << endl;

    // Run algorithm along PV (backwards)
    cout << "Moving along PV:" << endl;
    for( int i = 0; i <= npv; ++i ) {
        //cout << "Tablero inicial: " << endl;
        //cout << pv[i];
        int value = 0;
        TTable[0].clear();
        TTable[1].clear();
        float start_time = Utils::read_time_in_seconds();
        expanded = 0;
        generated = 0;
        int color = i % 2 == 1 ? 1 : -1;

        try {
            if( algorithm == 0 ) {
                value = color * (color == 1 ? maxmin(pv[i], 0, use_tt) : minmax(pv[i], 0, use_tt));
            } else if( algorithm == 1 ) {
                value = negamax(pv[i], 0, color, use_tt);
            } else if( algorithm == 2 ) {
                value = negamax(pv[i], 0, -200, 200, color, use_tt);
            } else if( algorithm == 3 ) {
                value = color * scout(pv[i], 0, color, use_tt);
            } else if( algorithm == 4 ) {
                value = negascout(pv[i], 0, -200, 200, color, use_tt);
            }
        } catch( const bad_alloc &e ) {
            cout << "size TT[0]: size=" << TTable[0].size() << ", #buckets=" << TTable[0].bucket_count() << endl;
            cout << "size TT[1]: size=" << TTable[1].size() << ", #buckets=" << TTable[1].bucket_count() << endl;
            use_tt = false;
        }

        float elapsed_time = Utils::read_time_in_seconds() - start_time;

        cout << npv + 1 - i << ". " << (color == 1 ? "Black" : "White") << " moves: "
             << "value=" << color * value
             << ", #expanded=" << expanded
             << ", #generated=" << generated
             << ", seconds=" << elapsed_time
             << ", #generated/second=" << generated/elapsed_time
             << endl;
        /*
        if (i == 1) {
            break;
        }
        */
    }

    return 0;
}

