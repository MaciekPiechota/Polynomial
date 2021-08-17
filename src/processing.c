/** @file
  Implementacja funkcji umożliwających przetwarzenie ciagów znaków

  @author Maciej Piechota <m.piechota7@student.uw.edu.pl>
  @date 2021
*/
#include "processing.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

bool SmallerEqualThan(char *a, size_t beg, size_t end, char *b) {
    size_t len = strlen(b);
    if (end-beg < len)
        return true;
    if (end-beg > len)
        return false;

    size_t i = beg;
    size_t j = 0;
    while (j < len && b[j] == a[i]) {
        if (isalpha(b[i]) != 0 || isalpha(a[i]) != 0)
            return false;
        i++;
        j++;
    }
    if (j == len)
        return true;
    return a[i] < b[j];
}

bool IsNumber(char *a, size_t beg, size_t end) {
    if (end <= beg)
        return false;
    size_t i = beg;
    if (a[beg] == '-')
        i++;
    while (i < end) {
        if (a[i] < '0' || a[i] > '9')
            return false;
        i++;
    }
    return true;
}

/**
 * Zwraca prawdę jeśli ciaŋ znaków w przedziale [beg,end) jest poprawnym
 * exponentem wielomianu
 * @param a
 * @param beg
 * @param end
 * @return
 */
static bool IsExponent(char *a, size_t beg, size_t end) {
    return a[beg] != '-' && IsNumber(a,beg,end) &&
           SmallerEqualThan(a,beg,end,"2147483647");
}

/**
 * Zwraca wartość logiczna true/false w zaleznosci czy ciag znakow
 * reprezentuje poprawny wielomian będący współczynnikiem całkowitym
 * @param a
 * @param beg
 * @param end
 * @return
 */
static bool IsCoeff(char *a,size_t beg, size_t end) {
    if (!IsNumber(a,beg,end))
        return false;
    if (end-beg <= 0)
        return false;
    if (a[beg] == '-') {
        if (beg + 1 == end)
            return false;
        return SmallerEqualThan(a,beg+1,end,"9223372036854775808");
    }

    else
        return SmallerEqualThan(a,beg,end,"9223372036854775807");
}

/**
 * Zwraca prawdę jeśli ciąg znaków w przedziale [beg,end) jest poprawnym
 * jednomianem
 * @param input
 * @param beg
 * @param end
 * @return
 */
static bool IsMono(char *input, size_t beg, size_t end) {
    // input jest postaci (...) chcemy sprawdzic czy jest postaci (p,e)
    // gdzie p to validPolynomialExpression a e to liczba typu long long
    if (end - beg < 5)
        return false;
    size_t i = end-2;
    // szukamy poczatku e (p,e)
    while (i >= beg && input[i] != ',') {
        if (input[i] < '0' || input[i] > '9')
            return false;
        i--;
    }
    if (i <= beg+1) {
        return false;
    }
    return IsExponent(input,i+1,end-1) && IsPolynomialExpression(input,
                                                                 beg+1,i);
}

bool IsPolynomialExpression(char *input,size_t beg, size_t end) {
    if (IsCoeff(input,beg,end))
        return true;
    // puste wyrazenie nie jest poprawne
    if (beg == end)
        return false;
    if (input[beg] != '(')
        return false;
    size_t i = beg+1;
    int counter = 1;
    // szukamy pierwszego poprawnego segmentu nawiasowego
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
    if (counter != 0 || (i < end && input[i] != '+'))
        return false;
    // input jest jednym jednomianem (...)
    if (i == end)
        return IsMono(input,beg,end);
    // input jest postaci (...)+...
    return IsMono(input,beg,i) && IsPolynomialExpression(input,i+1,end);
}