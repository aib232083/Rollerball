#include <algorithm>
#include <random>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <vector>
#include <string>

#include "board.hpp"
#include "engine.hpp"

std::unordered_set<U16> tabuList;

using namespace std;


std::pair<int, int> getCoordinates(U8 position) {
    int x = position & 0x07; // Extract x coordinate (bottom 3 bits)
    int y = (position >> 3) & 0x07; // Extract y coordinate (next 3 bits)
    return std::make_pair(x, y);
}

int calculateDistance(const U8& piece1, const U8& piece2) {
    // Implement a function to calculate the Manhattan distance between two positions.
    // You can use this function to calculate the distance between pieces and the opponent's king.
    // int x1 = (piece1) & (0x3F00);
    // int y1 = (piece1) & (0x003F);
    // int x2 = (piece2) & (0x3F00);
    // int y2 = (piece2) & (0x003F);
    int x1 = getCoordinates(piece1).first;
    int y1 = getCoordinates(piece1).second;
    int x2 = getCoordinates(piece2).first;
    int y2 = getCoordinates(piece2).second;

    return abs(x1 - x2) + abs(y1 - y2);
}

double evaluatePosition(Board& b) {

    // Board* c = b.copy();

    double score = 0.0;
    double oppScore = 0.0;
    double myScore = 0.0;
    vector<U8> piece_pos (12);
    // Board* e = b.copy();
    // unordered_set <U16> new_moves = b.get_legal_moves();
    // for(auto move : new_moves){
    //     e->do_move(move);
    //     if(e->in_check()){
    //         myScore += 100;
    //     }

    //     delete e;
    // }
    
    if (b.data.player_to_play == WHITE){
    
        piece_pos = {b.data.b_rook_bs, b.data.b_rook_ws, b.data.b_king, b.data.b_bishop, b.data.b_pawn_bs, b.data.b_pawn_ws, b.data.w_rook_bs, b.data.w_rook_ws,
                                b.data.w_king, b.data.w_bishop, b.data.w_pawn_bs, b.data.w_pawn_ws};
        

    }

    else {
        piece_pos = {b.data.w_rook_bs, b.data.w_rook_ws, b.data.w_king, b.data.w_bishop, b.data.w_pawn_bs, b.data.w_pawn_ws, b.data.b_rook_bs, 
                                b.data.b_rook_ws, b.data.b_king, b.data.b_bishop, b.data.b_pawn_bs, b.data.b_pawn_ws};
    }
    

    // Piece values
    double pawnValue = 1.0;
    double bishopValue = 5.0;
    double rookValue = 3.0;

    // int rook_b_1 = (b.data.b_rook_bs) & (0x3F00);
    // int rook_b_2 = (b.data.b_pawn_ws) & (0x3F00);
    // int rook_w_1 = (b.data.w_rook_ws) & (0x3F00);
    // int rook_w_2 = (b.data.w_rook_bs) & (0x3F00);

    double kingValue = 100.0;

    // U8 myKingPos = piece_pos[8];
    U8 oppKingPos = piece_pos[2];
    

    // Initialize scores for both players
    

    // Count the number of pieces for each player
    int myPieceCount = 0;
    int oppPieceCount = 0;

    // Loop through piece_pos
    for (int i = 0; i < 6; i++) {

        if(piece_pos[i] == DEAD){
            myScore += 200;
        }
        if (piece_pos[2] == DEAD){
            myScore += 100;
        }
        if (piece_pos[3] == DEAD){
            myScore += 15;
        }
        if(piece_pos[0] == DEAD || piece_pos[1] == DEAD){
            myScore += 10;
        }
        // Check if the opponent's piece is not DEAD and add its value to oppScore
        if (piece_pos[i] != DEAD) {
                if (i == 2) {
                oppScore += kingValue;
            } else if (i == 3) {
                oppScore += bishopValue;
            } else if (i >= 0 && i <= 1) {
                oppScore += rookValue; // Add the rook value when i is 0 or 1
            } else {
                oppScore += pawnValue;
            }
            oppPieceCount++;
            
        }

        if(piece_pos[i + 6] == DEAD){
                myScore -= 200;
            }
        // Check if your piece is not DEAD and add its value to myScore
        if (piece_pos[i + 6] != DEAD) {
          
            // int distanceToOppKing = calculateDistance(piece_pos[i], oppKingPos);

            // Modify the value for capturing opponent's pieces
            
            if (i+6 == 8) {
                oppScore += kingValue;
            } else if (i+6  == 9) {
                oppScore += bishopValue;
            } else if (i + 6 >= 6 && i + 6 <= 7) {
                oppScore += rookValue; // Add the rook value when i is 0 or 1
            } else {
                oppScore += pawnValue;
            }
            oppPieceCount++;
            
                // Add a bonus for capturing opponent's piece
            // pieceValue += 100.0; // You can adjust this bonus value as needed
            

            // myScore += pieceValue;
            myPieceCount ++ ;
        }
    }

    // for (int i = 6; i < 12; i++){
    //     if((myPieceCount - oppPieceCount) >= 4){
    //         if (piece_pos[i] == DEAD){
    //             myScore += 1000;
    //         }
    //     }
    // }

    double kingRookDistance_1 = calculateDistance(oppKingPos, piece_pos[6]);
    double kingRookDistance_2 = calculateDistance(oppKingPos, piece_pos[7]);
    double kingBishopDistance = calculateDistance(oppKingPos, piece_pos[9]);
    double kingPawn1 = calculateDistance(oppKingPos, piece_pos[10]);
    double kingPawn2 = calculateDistance(oppKingPos, piece_pos[11]);

    myScore += 1000/(kingBishopDistance + kingRookDistance_1 + kingRookDistance_2 + kingPawn1 + kingPawn2);

    double kingRookDistance = min(kingRookDistance_1, kingRookDistance_2);
    
    if(piece_pos[6] != DEAD and piece_pos[7] != DEAD){
        myScore += 50/ (kingRookDistance + 1);
    }
    else{
        myScore -= 30;
    }

    // double rooks_distance_1 = calculateDistance(piece_pos[0], piece_pos[6]);
    // double rooks_distance_2 = calculateDistance(piece_pos[0], piece_pos[7]);
    // double rooks_distance_3 = calculateDistance(piece_pos[1], piece_pos[6]);
    // double rooks_distance_4 = calculateDistance(piece_pos[1], piece_pos[7]);

    // if(piece_pos[6] != DEAD || piece_pos[7]!=DEAD){
    //     myScore += 500/(rooks_distance_1 + rooks_distance_2 + rooks_distance_3 + rooks_distance_4);
    // }
    
    // if ((kingRookDistance == 0) & (piece_pos[6] != DEAD || piece_pos[7] != DEAD)){
    //     myScore += 100;
    // }

    double kingPawnDistance_1 = calculateDistance(oppKingPos, piece_pos[0]);
    double kingPawnDistance_2 = calculateDistance(oppKingPos, piece_pos[1]);

    double kingPawnDistance = min(kingPawnDistance_1, kingPawnDistance_2);
    if(piece_pos[0] != DEAD and piece_pos[1] != DEAD){
        myScore += 50 / (kingPawnDistance + 1);
    }
    else{
        myScore -= 30;
    }

    // if(piece_pos[0] != DEAD and piece_pos[1] != DEAD){
    //     if(piece_pos[0] == piece_pos[2] || piece_pos[1] == piece_pos[2])
    //     {
    //         myScore += 200;
    //     }
    // };




    
    // if(kingBishopDistance == 0){
    //     myScore += 50;
    // }
    if(piece_pos[9] != DEAD){
        myScore += 50/ (kingBishopDistance + 1);
    }
    else if(piece_pos[9] == DEAD){
        myScore -= 400;
    }

    
    

    int rook_1_xPos = getCoordinates(piece_pos[6]).first;
    int rook_2_xPos = getCoordinates(piece_pos[7]).first;
    int rook_1_yPos = getCoordinates(piece_pos[6]).second;
    int rook_2_yPos = getCoordinates(piece_pos[7]).second;

    int pawn_1_xPos = getCoordinates(piece_pos[10]).first;
    int pawn_2_xPos = getCoordinates(piece_pos[11]).first;
    int pawn_1_yPos = getCoordinates(piece_pos[10]).second;
    int pawn_2_yPos = getCoordinates(piece_pos[11]).second;

    
    if(b.data.player_to_play == WHITE)
    {   
        
        if(rook_1_yPos < 4 || rook_2_yPos < 4 ){
            if(rook_1_xPos > 4 || rook_2_xPos > 4){
                myScore -= -100;
            }
        }
        
        if(rook_1_xPos < 2 || rook_2_xPos <2 ){
        
            if(rook_1_yPos >=4 || rook_2_yPos >=4){
                if(piece_pos[6] != DEAD || piece_pos[7] != DEAD){
                myScore += 300;
                }
            }
        }
    

        if(((pawn_1_xPos == 1) || (pawn_2_xPos = 1)) & (pawn_1_yPos > 2 || pawn_2_yPos > 2) ){
            myScore +=100;
        }
        // if(((pawn_1_xPos==0) || (pawn_2_xPos ==0))  &  (pawn_1_yPos <= 1 || pawn_2_yPos <= 1)){
        //     myScore -= 100;
        // }/* code */
    }
   else{
        
            if(rook_1_xPos < 2  || rook_2_xPos < 2){
                myScore -= 60;
            }
        
        
        if(rook_1_xPos > 4 || rook_2_xPos > 4 ){
        
            if(rook_1_yPos < 2  || rook_2_yPos < 2){
                if(piece_pos[6] != DEAD || piece_pos[7] != DEAD){
                myScore += 60;
                }
            }
        }
    

        if(((pawn_1_xPos != 5) || (pawn_2_xPos != 5)) & (pawn_1_yPos > 5 || pawn_2_yPos > 5) ){
            myScore += 70 ;
        }
        if(((pawn_1_xPos==6) || (pawn_2_xPos ==6))  &  (pawn_1_yPos >= 5 || pawn_2_yPos >=5 )){
            oppScore -= 70;
        }/* code */
   }
    
    // Encourage the bot to protect its king
    // myScore += 0.1 * (64 - calculateDistance(myKingPos, oppKingPos));

    // Discourage trading pieces when behind in material
    if (myPieceCount > oppPieceCount) {
        myScore += 10.0; // Discourage trading pieces when behind
    }
    else{
        myScore -= 30.0;
        }

        // Calculate the score difference
        
        
        
        score = myScore - oppScore;
        
        
        
        return score;
    }








double min_max(Board& board, int depth, double alpha, double beta, bool maximizing_player) {
    if (depth == 0) {
        return evaluatePosition(board);
    }

    if (maximizing_player) {
        double max_eval = -std::numeric_limits<double>::infinity();
        std::unordered_set<U16> legal_moves = board.get_legal_moves();

        for (U16 move : legal_moves) {

            Board* copy = board.copy();
            copy->do_move(move);
            double eval = min_max(*copy, depth - 1, alpha, beta, false);
            max_eval = std::max(max_eval, eval);

            // Alpha-beta pruning
            alpha = std::max(alpha, eval);

            // Updating beta (missing in the previous response)
            beta = std::min(beta, eval);

            if (beta <= alpha) {
                break; // Beta cut-off
            }

            // Deleting the dynamically allocated board copy
            delete copy;
        }

        return max_eval;
    } else {
        double min_eval = std::numeric_limits<double>::infinity();
        std::unordered_set<U16> legal_moves = board.get_legal_moves();

        for (U16 move : legal_moves) {
            
                
                Board* copy = board.copy();
                copy->do_move(move);
                double eval = min_max(*copy, depth - 1, alpha, beta, true);
                min_eval = std::min(min_eval, eval);

                // Alpha-beta pruning
                beta = std::min(beta, eval);

                if (beta <= alpha) {
                    break; // Alpha cut-off
                }

                // Delete the dynamically allocated board copy
             delete copy;
        }   
        
        

        return min_eval;
    }
}


bool isMoveTabu(U16 move) {
    return tabuList.find(move) == tabuList.end();
}
// Entry point for the Min-Max search
U16 min_max_search(Board& board) {
    constexpr int MAX_DEPTH = 3;
    double best_eval = -std::numeric_limits<double>::infinity();
    U16 best_move = 0;
    std::unordered_set<U16> legal_moves = board.get_legal_moves();
    double alpha = -std::numeric_limits<double>::infinity();
    double beta = std::numeric_limits<double>::infinity();

    for (U16 move : legal_moves) {
    
            tabuList.insert(move);
            Board* copy = board.copy();
            copy->do_move(move);
            double eval = min_max(*copy, MAX_DEPTH, alpha, beta, false);

            if (eval > best_eval) {
                best_eval = eval;
                best_move = move;
            }

            // Update alpha
            alpha = std::max(alpha, eval);

            delete copy;
    
    }
    

    return best_move;

}



void Engine::find_best_move(const Board& b) {

    

    // pick a random move
    Board* temp = b.copy();
    auto moveset = temp->get_legal_moves();
    if (moveset.size() == 0) {
        this->best_move = 0;
    }

    else
    {
        std::vector<U16> moves;
        std::cout << all_boards_to_str(*temp) << std::endl;
        for (auto m : moveset) {

            std :: cout << "printing moves : " << "\n";
            std::cout << move_to_str(m) << " ";
        }
        vector<pair < U16, U16>> v;
        // std::cout << std::endl;
        // std::sample(
        //     moveset.begin(),
        //     moveset.end(),
        //     std::back_inserter(moves),
        //      1,
        //     std::mt19937{std::{}()}
        // ); 
        // this->best_move = moves[0];
        // int c = 100 ;
        // auto it = moveset.begin();
        // U16 fi = *it;
        // for(auto m : moveset){
        //     U16 current_loaction =((0x003f) & (m));
        //     U16 king_location = temp->data.b_king;
        //     U16 diff = abs(current_loaction - king_location);
        //     v.push_back({diff, m});
        // //     if(diff <= c){
        //         c = diff;
        //         fi = m;
        // }
    

    
    std::cout << move_to_str(temp->data.last_killed_piece) << "\n";
    while(search){
        this->best_move = min_max_search(*temp);
    }
    }
}   


