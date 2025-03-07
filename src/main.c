#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include <graphviz/gvcext.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define X_MASK 0b111111111
#define O_MASK (X_MASK << 9)

int table = 0b000000000000000000;
const int WINNING_MASKS[] = {
    0b111000000, // Red 1
    0b000111000, // Red 2
    0b000000111, // Red 3
    0b100100100, // Kolona 1
    0b010010010, // Kolona 2
    0b001001001, // Kolona 3
    0b100010001, // Dijagonala 1
    0b001010100 // Dijagonala 2
};

GVC_t* gvc;
Agraph_t* graph;

char* printTable(int depth)
{
    static char buffer[512];
    char temp[32];
    buffer[0] = '\0';

    snprintf(temp, sizeof(temp), "\n%*s+=+=+=+\n%*s", depth * 4, "", depth * 4, "");
    strcat(buffer, temp);

    for (int i = 0; i < 9; i++) {
        if (table & (1 << i)) {
            strcat(buffer, "|X");
        } else if (table & (1 << (i + 9))) {
            strcat(buffer, "|O");
        } else {
            snprintf(temp, sizeof(temp), "|%d", i);
            strcat(buffer, temp);
        }

        if (i % 3 == 2) {
            snprintf(temp, sizeof(temp), "|\n%*s+=+=+=+\n%*s", depth * 4, "", depth * 4, "");
            strcat(buffer, temp);
        }
    }
    printf("%s", buffer);
    return buffer;
}
int check_for_winner()
{
    for (int i = 0; i < 8; i++) {
        if ((table & WINNING_MASKS[i]) == WINNING_MASKS[i]) {
            printf("X win");
            return 0;
        } else if (((table >> 9) & WINNING_MASKS[i]) == WINNING_MASKS[i]) {
            printf("O win");
            return 0;
        }
    }
    if (((table | (table >> 9)) & 0x1FF) == 0x1FF) {
        printf("draw");
        return 0;
    }
    return 1;
}
int set_X(int position)
{
    if (table & (1 << position) || table & (1 << (position + 9))) {
        printf("position already taken try other");
        return 1;
    }
    table |= (1 << position);
    return 0;
}
int set_O(int position)
{
    if (table & (1 << position) || table & (1 << (position + 9))) {
        printf("position already taken try other");
        return 1;
    }
    table |= (1 << (position + 9));
    return 0;
}

int chosePosition()
{
    int position;
    do {
        position = -1;
        printf("You must choose positions 0-8: ");
        scanf("%d", &position);
    } while (position > 8 || position < 0);
    return position;
}
int score()
{
    for (int i = 0; i < 8; i++) {
        if ((table & WINNING_MASKS[i]) == WINNING_MASKS[i]) {
            return -10;
        } else if (((table >> 9) & WINNING_MASKS[i]) == WINNING_MASKS[i]) {
            return 10;
        }
    }
    if (((table | (table >> 9)) & 0x1FF) == 0x1FF) {
        return 0;
    }
    return -1;
}

void get_available_moves(int moves[], int* count)
{
    int movesBin = ~(table | (table >> 9)) & 0x1FF;
    *count = 0;

    for (int i = 0; i < 9; i++) {
        if (movesBin & (1 << i)) {
            moves[*count] = i;
            (*count)++;
        }
    }
}

int minmax(int depth, int* position, int alpha, int beta, int isMaximizing, Agnode_t* previos)
{
    int result = score();
    char str[600];
    sprintf(str, "%s depth: %d score: %d\n a:%d b:%d \n %s", isMaximizing ? "maximizing" : "minimizing", depth, result - depth, alpha, beta, printTable(0));

    Agnode_t* node = agnode(graph, str, 1);
    Agedge_t* edge = agedge(graph, previos, node, 0, 1);
    if (result == 10) {
        agsafeset(node, "style", "filled", "");
        agsafeset(node, "fillcolor", "green", "");
    } else if (result == -10) {
        agsafeset(node, "style", "filled", "");
        agsafeset(node, "fillcolor", "red", "");
    } else if (result == 0) {
        agsafeset(node, "style", "filled", "");
        agsafeset(node, "fillcolor", "blue", "");
    }
    if (result != -1) {
        return result;
    }

    int available[9], count = 0;
    get_available_moves(available, &count);
    int bestMove = -1;
    if (isMaximizing) {
        int bestScore = INT_MIN;

        for (int i = 0; i < count; i++) {
            int pos = available[i];
            set_O(pos);

            int score = minmax(depth - 1, position, alpha, beta, 0, node);
            table ^= (1 << (pos + 9));
            if (score > bestScore) {
                bestScore = score;
                bestMove = pos;
            }
            if (score >= alpha) {
                alpha = score;
            }
            if (beta <= alpha) {
                printf("Pruning branch at depth %d (alpha: %d, beta: %d)\n", depth, alpha, beta);
                agsafeset(node, "style", "filled", "");
                agsafeset(node, "fillcolor", "yellow", "");
                break; // Alpha-beta pruning
            }
        }
        *position = bestMove;
        return bestScore;
    } else {
        int bestScore = INT_MAX;

        for (int i = 0; i < count; i++) {
            int pos = available[i];
            set_X(pos);

            int score = minmax(depth - 1, position, alpha, beta, 1, node);
            table ^= (1 << pos);
            if (score < bestScore) {
                bestScore = score;
                bestMove = pos;
            }
            if (score <= beta) {
                beta = score;
            }
            if (beta <= alpha) {
                printf("Pruning branch at depth %d (alpha: %d, beta: %d)\n", depth, alpha, beta);
                agsafeset(node, "style", "filled", "");
                agsafeset(node, "fillcolor", "yellow", "");
                break; // Alpha-beta pruning
            }
        }
        *position = bestMove;
        return bestScore;
    }
}

int minmax_move()
{
    gvc = gvContext();
    graph = agopen("graph", Agdirected, 0);
    int available[9], count = 0;
    get_available_moves(available, &count);

    int bestScore = INT_MIN;
    int bestMove = -1;

    char str[600];
    sprintf(str, "node \n %s", printTable(0));
    Agnode_t* node = agnode(graph, str, 1);

    minmax(0, &bestMove, INT_MIN, INT_MAX, 1, node);

    gvLayout(gvc, graph, "dot");

    // gvRender(gvc, graph, "dot", stdout);
    gvRenderFilename(gvc, graph, "jpeg", "graph.jpeg");
    gvFreeLayout(gvc, graph);

    return bestMove;
}

int main(int argc, char** argv)
{
    table |= (1 << 31);
    int playing = 1;
    while (playing == 1) {
        int flag = 0;
        printTable(0);
        if (table & 1 << 31) {
            printf("X turn\n");
            int choice = chosePosition();
            flag = set_X(choice);
        } else {
            printf("O turn\n\n\n\n");
            int choice = minmax_move();
            flag = set_O(choice);
        }
        table = flag ? table : table ^ (1 << 31);
        playing = check_for_winner();
    }
    printTable(0);
    return 0;
}
