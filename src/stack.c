/** @file
  Implementacja klasy stosu

  @author Maciej Piechota <m.piechota7@student.uw.edu.pl>
  @date 2021
*/
#include "poly.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "processing.h"


void StackPush(stack *s,char *in,size_t beg, size_t end) {
    if (s->bufferSize <= s->size) {
        s->bufferSize *= 2;
        s->arr = realloc(s->arr,sizeof (Poly)*s->bufferSize);
    }
    s->arr[s->size] = PolyFromString(in,beg,end);
    if (PolyIsZeroDeep(&s->arr[s->size])) {
        PolyDestroy(&s->arr[s->size]);
        s->arr[s->size] = PolyZero();
    }
    s->size++;
}

/**
 * Wypisuje jednomian na standardowe wyjście
 * @param m
 */
static void PrintMono(Mono *m);

/**
 * Wypisuje wielomian na standardowe wyjście
 * @param p
 */
static void PrintPoly(Poly *p) {
    if (p->arr == NULL) {
        printf("%ld",p->coeff);
        return;
    }
    for (size_t i = 0; i < p->size; i++) {
        printf("(");
        PrintMono(&p->arr[i]);
        printf(")");
        if (i != p->size-1)
            printf("+");
    }
}

static void PrintMono(Mono *m) {
    PrintPoly(&m->p);
    printf(",%d",m->exp);
}


void PrintTop(stack *s){
    PrintPoly(&s->arr[s->size-1]);
}

/**
 * Zwraca jednomian na podstawie ciągów znaków z przedziału [beg,end)
 * @param input
 * @param beg
 * @param end
 * @return
 */
static Mono MonoFromString(char *input, size_t beg, size_t end) {
    Mono result;
    size_t i = end-2;

    while (i >= beg && input[i] != ',') {
        i--;
    }

    char *endPtr;
    size_t len = i-beg;
    char *shiftedInput = input + len+1+beg;
    poly_exp_t x = strtol(shiftedInput,&endPtr,10);
    result.p = PolyFromString(input,beg+1,i);
    result.exp = x;
    return result;
}

Poly PolyFromString(char *input,size_t beg, size_t end) {
    if (input[beg] != '(') {
        // wielomian stały
        char *endPtr = NULL;
        poly_coeff_t x = strtol(input+beg,&endPtr,10);
        return PolyFromCoeff(x);
    }
    size_t i = beg+1;
    int counter = 1;
    size_t monosCount = 0;
    // szukamy pierwszego poprawnego segmentu nawiasowego
    while (i < end) {
        while (counter != 0 && i < end) {
            switch (input[i]) {
                case '(':
                    counter++;
                    break;
                case ')':
                    counter--;
                    break;
            }
            i++;
        }
        monosCount++;
        i+=2;
        counter = 1;
    }

    Mono *monos = (Mono*) malloc (sizeof(Mono)*monosCount);
    counter = 1;
    size_t lastIndex = beg;
    i = beg+1;
    size_t j = 0;
    while (i < end) {
        while (counter != 0 && i < end) {
            switch (input[i]) {
                case '(':
                    counter++;
                    break;
                case ')':
                    counter--;
                    break;
            }
            i++;
        }
        monos[j] = MonoFromString(input,lastIndex,i-1);
        lastIndex = i+1;
        j++;
        i+=2;
        counter = 1;
    }
    Poly res = PolyAddMonos(j,monos);
    free(monos);
    return res;
}

void StackPop(stack *s) {
    PolyDestroy(&s->arr[s->size-1]);
    s->size--;
}

void StackBinaryOperation(stack *s,Poly (*op)(const Poly *, const Poly *)){
    Poly p = op(&s->arr[s->size-1],&s->arr[s->size-2]);
    PolyDestroy(&s->arr[s->size-1]);
    PolyDestroy(&s->arr[s->size-2]);
    s->size -= 2;
    if (s->bufferSize <= s->size) {
        s->bufferSize *= 2;
        s->arr = (Poly*) realloc(s->arr,s->bufferSize*sizeof (Poly));
    }
    s->arr[s->size] = PolyClone(&p);
    PolyDestroy(&p);
    s->size++;
}

void StackCloneTop(stack *s) {
    if (s->bufferSize <= s->size) {
        s->bufferSize *= 2;
        s->arr = (Poly*) realloc(s->arr,s->bufferSize*sizeof (Poly));
    }
    s->arr[s->size] = PolyClone(&s->arr[s->size-1]);
    s->size++;
}

void StackUnaryBoolOperation(stack *s,bool (*op)(const Poly *)) {
    if (op(&s->arr[s->size-1])) {
        printf("1\n");
        return;
    }
    printf("0\n");
}

void StackAddZero(stack *s) {
    if (s->bufferSize <= s->size) {
        s->bufferSize *= 2;
        s->arr = (Poly*) realloc(s->arr,s->bufferSize*sizeof (Poly));
    }
    s->arr[s->size] = PolyZero();
    s->size++;
}

void StackNegTop(stack *s) {
    Poly temp = PolyNeg(&s->arr[s->size-1]);
    PolyDestroy(&s->arr[s->size-1]);
    s->arr[s->size-1] = PolyClone(&temp);
    PolyDestroy(&temp);
}

void StackDestroy(stack *s) {
    while(s->size > 0) {
        PolyDestroy(&s->arr[s->size-1]);
        s->size--;
    }
    free(s->arr);
}

void StackTopEqual(stack *s) {
    if (PolyIsEq(&s->arr[s->size-1],&s->arr[s->size-2]))
        printf("1\n");
    else
        printf("0\n");
}

void StackDegBy(stack *s,size_t idx) {
    printf("%d\n",PolyDegBy(&s->arr[s->size-1],idx));
}

void StackDeg(stack *s){
    printf("%d\n",PolyDeg(&s->arr[s->size-1]));
}

void StackAt(stack *s,poly_coeff_t x) {
    Poly temp = PolyAt(&s->arr[s->size-1],x);
    PolyDestroy(&s->arr[s->size-1]);
    s->arr[s->size-1] = PolyClone(&temp);
    PolyDestroy(&temp);
}