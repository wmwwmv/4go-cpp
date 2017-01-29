#ifndef STRATEGY1_CPP
#define STRATEGY1_CPP

//
#include "def.h"
#include "strategy1.h"
#include "route.h"
//


//
int score(rank_type rank)
{
    switch(rank)
    {
        case 10: return 500;
        case 40: return 40;
        case 39: return 30;
        case 38: return 20;
        case 37: return 10;
        case 36: return 4;
        case 35: return 3;
        case 0: return 23;
        case 30: return 7;
        case 100: return 3;
        default: return 2;
    }
}

//
int position_score(rank_type rank)
{
    switch(rank)
    {
        case 40: return 30;
        case 39: return 25;
        case 38: return 20;
        case 37: return 15;
        case 36: return 10;
        case 35: return 8;
        case 0: return 22;
        default: return 5;
    }
}

//
int under_attack(board & b, chess_type chess)
{
    int result = 0;

    position p(chess.code);

    if ((chess.state != empty) && !p.is_camp())
    {
        pos_list enemy_chesses = b.find_allies(right_country(chess.belong_to));

        for_int(i, enemy_chesses.size())
        {
            int_type e_code = enemy_chesses[i];
            position e_pos(e_code);
            chess_type e_chess = b.find_chess(e_code);

            if (e_chess.movable())
            {
                int beat = beat_it(e_chess.rank, chess.rank);

                if (beat == 1)
                {
                    pos_list move_list = route_list(b, e_chess, chess.code);
                    bool accessible = (move_list.size() > 1);

                    if (accessible)
                    {
                        return -score(chess.rank);
                    }
                }

                if (beat == 0)
                {
                    pos_list move_list = route_list(b, e_chess, chess.code);
                    bool accessible = (move_list.size() > 1);

                    if (accessible)
                    {
                        result = score(e_chess.rank) - score(chess.rank);
                        if (result > 0) result = 0;
                        return result;
                    }

                }
              }
            }

        }

        return result;

}

//
int position_value(row_type row, col_type col, col_type flag_col)
{
    if (flag_col == 3)

        return position_value(row, 4 - col, 1);

    else // flag_col == 1

        if (row == 5)
            switch (col)
            {
                case 0: return 5;
                case 1: return 0;
                case 2: return 5;
                case 3: return 0;
                default: return 1;
            }
        else if (row == 4)
            switch (col)
            {
                case 0: return 5;
                case 1: return 5;
                case 2: return 5;
                case 3: return 2;
                default: return 1;
            }
        else if (row == 3)
            switch (col)
            {
                case 1: return 5;
                case 2: return 2;
                case 3: return 2;
                default: return 1;
            }
        else if (row == 2)
            switch (col)
            {
                case 2: return 4;
                default: return 1;
            }
        else
            return 1;

}

const float ratio = 0.35;


float calculate_value(board & b, country_type belong_to)
{
    pos_list all_chess = b.find(belong_to);
    float sum = 0;

    if (b.is_empty(belong_to)) return 0;

    for_int(i, all_chess.size())
    {
        int_type pos = all_chess[i];

        chess_type chess = b.find_chess(pos);

        sum += score(chess.rank);

        sum += ratio * under_attack(b, chess);
    }

    return sum;
}

float calculate_position_value(board & b, country_type belong_to)
{
    pos_list all_chess = b.find(belong_to);
    float sum = 0;

    if (b.is_empty(belong_to)) return 0;

    pos_list flag_list = b.find_rank(10, all_chess);

    if (flag_list.size() == 0) return sum;

    int_type flag = flag_list[0];

    col_type flag_col = position(flag).col;

    pos_list my_all_chess = b.find_country(belong_to);

    for_int(i, my_all_chess.size())
    {
        int_type my_pos = my_all_chess[i];

        chess_type my_chess = b.find_chess(my_pos);

        if (my_chess.state != empty)
        {
            float sign = (is_enemy(my_chess.belong_to, belong_to))?-1:2/5;

            sum += sign/10 * position_score(my_chess.rank) *
                             position_value(position(my_pos).row, position(my_pos).col, flag_col);

            if (is_enemy(my_chess.belong_to, belong_to))
            {
                for(row_type row = 0; row < row_num(belong_to); row ++)
                for(col_type col = 0; col < col_num(belong_to); col ++)
                {
                    position pp(belong_to, row, col);
                    pos_list move_list = route_list(b, my_chess, pp.encode());
                    bool accessible = (move_list.size() > 1);

                    if (accessible)
                        if ((!b.is_occupied(pp)) ||
                            ((b.is_occupied(pp)) &&
                             (is_enemy(my_chess.belong_to, b.find_chess(pp).belong_to)) &&
                             (beat_it(my_chess.rank, b.find_chess(pp).rank) == 1)
                            ))

                        sum -= -2.0/8 * position_score(my_chess.rank) *
                                        position_value(row, col, flag_col);
                }
            }
        }

    }


    return sum;

}

//
//
move_type run_strategy1(board &b, country_type belong_to)
{
    pos_list whole_list = b.find(belong_to);
    move_type one_move;
    float max_value = -10000;

    for_int(i, whole_list.size())
    {
        int_type s_code = whole_list[i];
        position s_pos(s_code);
        chess_type s_chess = b.find_chess(s_code);

        if (s_chess.movable() && !s_pos.is_base())

            loop(d_code)
            {
                position d_pos(d_code);
                chess_type d_chess = b.find_chess(d_pos);

                pos_list move_list = route_list(b, s_chess, d_code);
                bool accessible = (move_list.size() > 1);

                bool go_able = (!b.is_occupied(d_code)) ||
                        (b.is_occupied(d_code) && is_enemy(s_chess.belong_to, d_chess.belong_to) &&
                         (!d_pos.is_camp()) && (!(d_pos.is_base() && (d_chess.rank != 10))))
                        ;

                if (accessible && go_able)
                {
                    board b2 = b;

                    b2.remove_position(s_code);

                    if (!b2.is_occupied(d_code))

                        b2.occupy(d_pos, s_chess.rank, s_chess.belong_to, normal);

                    else
                    {
                        switch(beat_it(s_chess.rank, d_chess.rank))
                        {
                            case 1:
                                    b2.remove_position(d_code);
                                    b2.occupy(d_pos, s_chess.rank, s_chess.belong_to, normal);
                                    break;
                            case 0:
                                    b2.remove_position(d_code);
                                    break;
                            default: ;
                        }
                     }

                     float value = calculate_value(b2, belong_to)
                                 + calculate_position_value(b2, belong_to)
                                 + calculate_value(b2, ally_country(belong_to))
                                 - calculate_value(b2, right_country(belong_to))
                                 - calculate_value(b2, left_country(belong_to));

                     value += qrand() % 5;

                     if (value >= max_value)
                     {
                         max_value = value;

                         one_move.from = s_code;
                         one_move.to = d_code;
                     }

                   }
                }

    }

    return one_move;

}

#endif // STRATEGY1_CPP