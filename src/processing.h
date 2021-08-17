/** @file
  Deklaracja funkcji umożliwających przetwarzenie ciagów znaków. Wszystkie
  funkcje działają zgodnie z ustaleniami z mainpage

  @author Maciej Piechota <m.piechota7@student.uw.edu.pl>
  @date 2021
*/
#ifndef __PROCESSING__H__
#define __PROCESSING__H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "poly.h"

/**
 * Zwraca prawdę jeśli ciąg znaków w przedziale [beg,end) reprezentuje
 * poprawny wielomian
 * @param input
 * @param beg
 * @param end
 * @return
 */
bool IsPolynomialExpression(char *input,size_t beg, size_t end);

/**
 * Zwraca prawdę jeśli ciąg znaków w przedziale [beg,end) reprezentuje liczbę
 * mniejszą bądź równą liczbie zapisanej jako ciąg znaków b
 * @param a
 * @param beg
 * @param end
 * @param b
 * @return
 */
bool SmallerEqualThan(char *a, size_t beg, size_t end, char *b);

/**
 * Zwraca prawdę jeśli ciąg znaków w przedziale [beg,end) reprezentuje liczbe
 * całkowitą
 * @param a
 * @param beg
 * @param end
 * @return
 */
bool IsNumber(char *a, size_t beg, size_t end);

#endif