/** @file
  Plik zawierający wszystkie potrzebne deklaracje umozliwiające włączenie
  kalkulatora wielomianów

  @author Maciej Piechota <m.piechota7@student.uw.edu.pl>
  @date 2021
*/
#define  _GNU_SOURCE
#include "poly.h"
#include "stack.h"
#include "processing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/**
 * Parsuje ciąg znaków i w wykonuje odpwiednie polecenie zgodnie z
 * dokumentacją w mainpage
 * @param in
 * @param s
 * @param lineNum
 */
static void ParseInputAndExecute(char *in,stack *s,size_t lineNum) {
    size_t len = strlen(in);
    if (in[len-1] == '\n')
        len--;
    if (len == 0)
        return;
    if (strncmp(in,"ZERO",len) == 0 && len == 4)
        StackAddZero(s);
    else if (strncmp(in,"IS_COEFF",len) == 0 && len == 8) {
        if (s->size < 1)
            fprintf(stderr,"ERROR %zu STACK UNDERFLOW\n",lineNum);
        else
            StackUnaryBoolOperation(s,PolyIsCoeff);
    }
    else if (strncmp(in,"IS_ZERO",len) == 0 && len == 7) {
        if (s->size < 1)
            fprintf(stderr,"ERROR %zu STACK UNDERFLOW\n",lineNum);
        else
            StackUnaryBoolOperation(s,PolyIsZero);
    }
    else if (strncmp(in,"CLONE",len) == 0 && len == 5) {
        if (s->size < 1)
            fprintf(stderr,"ERROR %zu STACK UNDERFLOW\n",lineNum);
        else
            StackCloneTop(s);
    }
    else if (strncmp(in,"ADD",len) == 0 && len == 3) {
        if (s->size < 2)
            fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", lineNum);
        else
            StackBinaryOperation(s, PolyAdd);
    }
    else if (strncmp(in,"NEG",len) == 0 && len == 3) {
        if (s->size < 1)
            fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", lineNum);
        else
            StackNegTop(s);
    }
    else if (strncmp(in,"MUL",len) == 0 && len == 3) {
        if (s->size < 2)
            fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", lineNum);
        else
            StackBinaryOperation(s,PolyMul);
    }
    else if (strncmp(in,"SUB",len) == 0 && len == 3) {
        if (s->size < 2)
            fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", lineNum);
        else
            StackBinaryOperation(s,PolySub);
    }
    else if (strncmp(in,"IS_EQ",len) == 0 && len == 5) {
        if (s->size < 2)
            fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", lineNum);
        else
            StackTopEqual(s);
    }
    else if (strncmp(in,"DEG",len) == 0 && len == 3) {
        if (s->size < 1)
            fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", lineNum);
        else
            StackDeg(s);
    }
    else if (strncmp(in,"DEG_BY",6) == 0) {
        if (len < 7 || in[6] != ' '  || !IsNumber(in,7,len) || in[7] =='-')
            fprintf(stderr,"ERROR %zu DEG BY WRONG VARIABLE\n",lineNum);
        else if (s->size < 1)
            fprintf(stderr,"ERROR %zu STACK UNDERFLOW\n",lineNum);
        else {
            char *endPtr;
            errno = 0;
            long x = strtol(in+7,&endPtr,10);
            if (errno == ERANGE)
                fprintf(stderr,"ERROR %zu DEG BY WRONG VARIABLE\n",lineNum);
            else
                StackDegBy(s,x);
        }
    }
    else if (strncmp(in,"AT",2) == 0 && len >= 2) {
        if (in[2] != ' '  || !IsNumber(in, 3,len) || (in[3] == '-' && len == 4))
            fprintf(stderr,"ERROR %zu AT WRONG VALUE\n",lineNum);
        else if (s->size < 1)
            fprintf(stderr,"ERROR %zu STACK UNDERFLOW\n",lineNum);
        else {
            char *endPtr;
            errno = 0;
            long x = strtol(in+2,&endPtr,10);
            if (errno == ERANGE)
                fprintf(stderr,"ERROR %zu AT WRONG VALUE\n",lineNum);
            else
                StackAt(s, x);
        }
    }
    else if (strncmp(in,"PRINT",len) == 0 && len == 5) {
        if (s->size < 1)
            fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", lineNum);
        else {
            PrintTop(s);
            printf("\n");
        }
    }
    else if (strncmp(in,"POP",len) == 0 && len == 3) {
        if (s->size < 1)
            fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", lineNum);
        else
            StackPop(s);
    }
    else if ((in[0] >= 'a' && in[0] <= 'z') || (in[0] >= 'A' && in[0] <= 'Z')) {
        fprintf(stderr,"ERROR %zu WRONG COMMAND\n",lineNum);
    }
    else{
        if (IsPolynomialExpression(in,0,len)) {
            StackPush(s, in,0,len);
            //PrintTop(s);
        }
        else
            fprintf(stderr,"ERROR %zu WRONG POLY\n",lineNum);
    }
}

/**
 * Główna funkcja main rozpoczynająca działanie kalkulatora
 * @return
 */
int main() {

    size_t bufferSize = 0;
    char *input = NULL;
    int len = getline(&input,&bufferSize,stdin);
    size_t lineNumber = 1;

    stack polynomialStack;
    polynomialStack.size = 0;
    polynomialStack.bufferSize = 8;
    polynomialStack.arr = malloc(sizeof (Poly)*polynomialStack.bufferSize);
    while(len != -1) {
        if (len != (int) strlen(input)) {
            if (isalpha(input[0]) != 0)
                fprintf(stderr,"ERROR %zu WRONG COMMAND\n",lineNumber);
            else
                fprintf(stderr,"ERROR %zu WRONG POLY\n",lineNumber);
        }
        else if (len > 0 && input[0] != '#') {
            ParseInputAndExecute(input,&polynomialStack,lineNumber);
        }
        len = getline(&input,&bufferSize,stdin);
        lineNumber++;
    }

    free(input);
    StackDestroy(&polynomialStack);
}