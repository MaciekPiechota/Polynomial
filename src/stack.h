/** @file
  Interfejs klasy stosu przechowującego wielomiany

  @author Maciej Piechota <m.piechota7@student.uw.edu.pl>
  @date 2021
*/

#ifndef __STACK_H__
#define __STACK_H__

#include "poly.h"
#include "stdio.h"

/**
 *  To jest struktura przechowująca stos, jest to implementacja tablicowa
 */
typedef struct stack {
    Poly *arr;         ///< tablica przechowujaca stos
    size_t size;       ///< rozmiar stosu zajetych elementow na stosie
    size_t bufferSize; ///< rozmiar calego stosu
}stack;

/**
 * Dodaje na wierzch stosu wielomian zapisany jako ciąg znaków z
 * zakresu [beg,end) zgodnie z dokumentacja z mainpage
 * @param s
 * @param in
 * @param beg
 * @param end
 */
void StackPush(stack *s,char *in,size_t beg, size_t end);

/**
 * Zwraca wielomian typu Poly na podstawie ciągu znaków w zakresie [beg,end)
 * zgodnie z dokumentacja z mainpage
 * @param input
 * @param beg
 * @param end
 * @return
 */
Poly PolyFromString(char *input,size_t beg, size_t end);

/**
 * Usuwa stos z pamieci
 * @param s
 */
void StackDestroy(stack *s);

/**
 * Wypisuje wielomian z wierzchu stosu na standardowe wyjscie
 * @param s
 */
void PrintTop(stack *s);

/**
 * Usuwa wielomian z wierzchu stosu
 * @param s
 */
void StackPop(stack *s);

/**
 * Wypisuje na standardowe wyjscie 1 lub 0 w zaleznosci od wyniku operacji op
 * na dwoch wielomianach z wierzchu stosu
 * @param s
 * @param op
 */
void StackBinaryOperation(stack *s,Poly (*op)(const Poly *, const Poly *));

/**
 * Tworzy kopie wielomianu z wierzchu stosu i wstawia go na wierzch
 * @param s
 */
void StackCloneTop(stack *s);

/**
 * Wypisuje na standardowe wyjscie 1 lub 0 w zaleznosci od wyniku operacji op
 * na wielomianie na wierzchu stosu
 * @param s
 * @param op
 */
void StackUnaryBoolOperation(stack *s,bool (*op)(const Poly *));

/**
 * Dodaje wielomian torzsamościowo równy zero na wierzch stosu
 * @param s
 */
void StackAddZero(stack *s);

/**
 * Neguje wielomian na wierzchu stosu
 * @param s
 */
void StackNegTop(stack *s);

/**
 * Wypisuje na standardowe wyjscie 1 lub 0 w zaleznosci czy dwa wielomiany na
 * wierzchu stosu są sobie równe
 * @param s
 */
void StackTopEqual(stack *s);

/**
 * Wypisuje na standardowe wyjście stopień wielomianu na wierzchu stosu
 * @param s
 */
void StackDeg(stack *s);

/**
 * Wypisuje na standardowe wyjście wynik funkcji Deg_By z poly.h na
 * wielomianie z wierzchu stosu
 * @param s
 * @param idx
 */
void StackDegBy(stack *s,size_t idx);

/**
 * Zamienia wielomian na wierzchu stosu na ów wielomian po wywołaniu na nim
 * funkcji PolyAt z poly.h
 * @param s
 * @param x
 */
void StackAt(stack *s,poly_coeff_t x);
#endif /*__STACK_H */