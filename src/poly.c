/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych
  @author Maciej Piechota <m.piechota7@student.uw.edu.pl>
  @date 2021
*/
#include "poly.h"
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

/**
 * Konczy program kodem awaryjnym 1 kiedy wskaznik p jest rowny null
 * @param[in] p
 */
static inline void CheckPointer(void *p) {
    if (p == NULL)
        exit(1);
}

/**
 * Zwraca maksimum z dwoch wartosci typu size_t
 * @param[in] a
 * @param[in] b
 * @return max(a,b)
 */
static size_t Max(size_t a, size_t b) {
    if (a > b)
        return a;
    return b;
}

/**
 * Zwraca wynik potegowania @f$x^y@f$ w typie poly_coeff_t
 * @param[in] x
 * @param[in] y
 * @return @f$x^y@f$
 */
static poly_coeff_t PowCcoeff(poly_coeff_t x, poly_coeff_t y) {
    assert(y >= 0);
    if ((x == 0 && y == 0) || y == 0)
        return 1;
    if (y % 2 != 0)
        return x* PowCcoeff(x,y-1);
    else{
        poly_coeff_t a = PowCcoeff(x,y/2);
        return a*a;
    }
}

/**
 * Usuwa wielomian @f$p@f$ typu const Poly z pamieci
 * @param[in] p
 */
static void PolyDestroyConst(const Poly *p) {
    assert(p != NULL);
    if (p->arr == NULL)
        return;
    for (size_t i = 0; i < p->size; i++)
        PolyDestroy(&p->arr[i].p);
    free(p->arr);
}

/**
 * Zwraca true jesli wielomian p jest zagnieżdzonym wielomianem stałym
 * @param[in] p
 * @return @f$x^y@f$
 */
static bool IsNestedCoeff(Poly *p) {
    assert(p != NULL);
    if (p->arr == NULL)
        return true;
    if (p->size != 1)
        return false;
    if (p->arr[0].exp == 0)
        return IsNestedCoeff(&p->arr[0].p);
    return false;
}

/**
 * Zwraca zagnieżdzony jednomian stały
 * @param[in] p
 */
static poly_coeff_t GetNestedCoeff(Poly *p) {
    assert(p != NULL);
    if (p->arr == NULL)
        return p->coeff;
    return GetNestedCoeff(&p->arr[0].p);
}

/**
 * Rekurencyjna funkcja pomocnicza zwracająca stopień wielomianu
 * @param[in] p
 * @return @f$deg(p)@f$
 */
static poly_exp_t PolyDegRec(const Poly *p) {
    assert(p != NULL);
    poly_exp_t deg = 0;
    if (p->arr == NULL)
        return 0;
    for (size_t i = 0; i < p->size; i++) {
        assert(deg >= 0);
        if (p->arr[i].exp != 0)
            deg = Max(deg, PolyDeg(&(p->arr[i].p)) + p->arr[i].exp);
        else
            deg = Max(deg, PolyDeg(&(p->arr[i].p)));
    }
    return deg;
}

bool PolyIsZeroDeep(const Poly *p) {
    assert(p != NULL);
    if (p->arr == NULL)
        return p->coeff == 0;
    bool isZeroDeep = true;
    for (size_t i = 0; i < p->size; i++) {
        assert(isZeroDeep == true);
        isZeroDeep &= PolyIsZeroDeep(&(p->arr[i].p));
        if (isZeroDeep == false)
            return false;
    }
    return isZeroDeep;
}

/**
 * Wypelnia wielomian res od indeksu k wartosciami z wielomianu p
 * z przedzialu @f$[i,p->size]@f$
 * @param[in] res
 * @param[in] p
 * @param[in] i
 * @param[in] k
 */
static void FillPoly(Poly *res, const Poly *p, size_t i, size_t *k) {
    assert(res != NULL && p != NULL);
    while (i < p->size) {
        assert(*k < res->size);
        res->arr[*k] = MonoClone(&p->arr[i]);
        *k = *k + 1;
        i++;
    }
}

/**
 * Zwraca suma @f$p+q@f$ gdzie p i q sa wielomianami
 * @param[in] p
 * @param[in] q
 * @return @f$p+q@f$
 */
static Poly PolyAddPolyToPoly(const Poly *p, const Poly *q) {
    /** Tworzymy wielomina o liczbie jednomianow rownej sumie liczb jednomianow
     *  wielomianow dodawanych, nastepnie przepisujemy poszeczegolne
     *  jednomiany w kolejnosci rosnych wykladnikow dodajac do siebie funkcja
     *  PolyAdd jednomiany o tych samych wykladnikach. Na koniec sprawdzamy
     *  warunki przegowe jakimi sa wielomin zerowy i zwracamy suma
     */
    assert(p != NULL || q != NULL);
    if (q->size > p->size)
        return PolyAddPolyToPoly(q, p);
    Poly res = {.size = p->size + q->size};
    res.arr = (Mono *) malloc(sizeof(Mono) * res.size);
    CheckPointer(&res.arr);

    size_t p_i = 0, q_i = 0;
    size_t k = 0;
    while (p_i < p->size && q_i < q->size) {
        assert(p_i == 0 || p->arr[p_i].exp >= p->arr[p_i - 1].exp);
        if (p->arr[p_i].exp == q->arr[q_i].exp) {
            res.arr[k].exp = p->arr[p_i].exp;
            res.arr[k].p = PolyAdd(&p->arr[p_i].p, &q->arr[q_i].p);
            if (!PolyIsZeroDeep(&res.arr[k].p))
                k++;
            else
                PolyDestroy(&res.arr[k].p);
            p_i++;
            q_i++;
        } else if (p->arr[p_i].exp < q->arr[q_i].exp) {
            res.arr[k] = MonoClone(&p->arr[p_i]);
            k++;
            p_i++;
        } else {
            res.arr[k] = MonoClone(&q->arr[q_i]);
            k++;
            q_i++;
        }
    }

    FillPoly(&res, p, p_i, &k);
    FillPoly(&res, q, q_i, &k);

    if (k == 0) {
        free(res.arr);
        res = PolyZero();
        return res;
    } else if (k == 1 && res.arr[0].exp == 0 && IsNestedCoeff(&res.arr[0].p)) {
        poly_coeff_t coeff = GetNestedCoeff(&res.arr[0].p);
        PolyDestroy(&res.arr[0].p);
        free(res.arr);
        if (coeff == 0)
            return PolyZero();
        return PolyFromCoeff(coeff);
    }
    res.arr = (Mono *) realloc(res.arr, sizeof(Mono) * k);
    CheckPointer(&res.arr);
    res.size = k;
    return res;
}

/**
 * Zwraca suma @f$p+x@f$ gdzie p to wielomian a x to stala
 * @param[in] p
 * @param[in] x
 * @return @f$p+x@f$
 */
static Poly PolyAddConstToPoly(const Poly *p, poly_coeff_t x) {
    /**
     *  Dodajemy do wielomianu wielomian staly poprzez rozpatrywanie trzech
     *  przypadkow,
     *  1 - wielomian staly jest rowny zero
     *  2 - wielomian ma wspoczynnik z wykladnikiem rownym zero
     *  3 - wielomian nie ma wspolczynnika z wykladnikiem rownym zero
     *      dodajemy wtedy taki wspolczynnik na poczatek wielomianu
     */
    assert(p != NULL);
    if (p->size == 0)
        return PolyZero();
    if (x == 0)
        return PolyClone(p);

    Poly result;
    // pierwszy skladnik wielomianu jest rowny stalej
    if (p->arr[0].exp == 0 && p->arr[0].p.arr == NULL) {
        if (p->arr[0].p.coeff + x != 0) {
            Poly result = PolyClone(p);
            result.arr[0].p.coeff += x;
            return result;
        }
            // po dodaniu stalej skladnik wielomianu zeruje sie musimy wtedy
            // usunac odpowiedni pierwszy jednomian lub zwrocic wielomian zerowy
            // w skrajenym przadku
        else {
            if (p->size == 1) {
                result = PolyFromCoeff(0);
                return result;
            } else {
                Poly result = {.size = p->size - 1};
                result.arr = (Mono *) malloc(sizeof(Mono) * result.size);
                CheckPointer(&result.arr);
                for (size_t i = 0; i < result.size; i++)
                    result.arr[i] = MonoClone(&p->arr[i + 1]);
                return result;
            }
        }
    }
    // dodajemy stala do wielomanu bedacego wspoczynnikiem przy wykladniku
    // zerowym
    if (p->arr[0].exp == 0) {
        // zmienne temp i xpoly zapobiegaja napisywaniu wartosci wielomianow
        Poly xpoly = PolyFromCoeff(x);
        Poly temp = PolyAdd(&xpoly, &p->arr[0].p);
        PolyDestroy(&xpoly);

        Poly result = PolyClone(p);
        PolyDestroy(&result.arr[0].p);
        result.arr[0].p = PolyClone(&temp);
        PolyDestroy(&temp);
        return result;
    }

    Poly sum;
    sum.size = p->size + 1;
    sum.arr = (Mono *) malloc(sizeof(Mono) * sum.size);
    CheckPointer(&sum.arr);
    sum.arr[0].exp = 0;
    sum.arr[0].p = PolyFromCoeff(x);

    for (size_t i = 0; i < p->size; i++)
        sum.arr[i + 1] = MonoClone(&p->arr[i]);

    return sum;
}

/**
 * Funkcja porównojąca structury Mono potrzebna do używania qsort, zwraca
 * liczbe dodatnio ujemna lub 0 zgodnie z wymaganiami qsort
 * @param[in] a
 * @param[in] b
 */
static int MonosCompare(const void *a, const void *b) {
    assert(a != NULL && b != NULL);
    Mono *_a = (Mono *) a;
    Mono *_b = (Mono *) b;

    if (_a->exp < _b->exp)
        return -1;
    else if (_a->exp == _b->exp)
        return 0;
    return 1;
}

/**
 * Usuwa structure Mono przekazaną jaką cons
 * @param[in] m
 */
static void MonoDestroyConst(const Mono *m) {
    assert(m != NULL);
    PolyDestroyConst(&m->p);
}

/**
 * Kopiuje tablice jednomianów przekazana jako const o rozmiarze count
 * @param[in] count
 * @param[in] monos[]
 */
static Mono *CopyMonos(size_t count, const Mono monos[]) {
    assert(count != 0);
    Mono *monosCopy = (Mono *) malloc(sizeof(Mono) * count);
    CheckPointer(&monosCopy);
    for (size_t i = 0; i < count; i++) {
        monosCopy[i].exp = monos[i].exp;
        monosCopy[i].p = PolyClone(&(monos[i].p));
    }
    return monosCopy;
}

/**
 * Usuwa tablice struktur Mono przekazana jako const o rozmiarze count
 * @param[in] count
 * @param[in] monos[]
 */
static void ConstMonosDestroy(size_t count, const Mono monos[]) {
    assert(monos != NULL);
    for (size_t i = 0; i < count; i++)
        MonoDestroyConst(&monos[i]);
}

/**
 * Zwraca iloczyn wielomianu p i stalej x
 * @param[in] p
 * @param[in] x
 * @return @f$xp@f$
 */
static Poly PolyMulCoeff(const Poly *p, poly_coeff_t x) {
    /** Mnozymy wielomian p rekurencyjnie przez x. Warunek koncowy rekurencji
     *  jest wtedy gdy p jest wielomianem stalym
     */
    assert(p != NULL);
    switch (x) {
        case 0:
            return PolyFromCoeff(0);
            break;
        case 1:
            return PolyClone(p);
            break;
        default: {
            Poly answer = PolyClone(p);
            for (size_t i = 0; i < p->size; i++) {
                if (p->arr[i].p.arr == NULL)
                    answer.arr[i].p.coeff *= x;
                else {
                    Poly temp = PolyMulCoeff(&(answer.arr[i].p), x);
                    PolyDestroy(&answer.arr[i].p);
                    answer.arr[i].p = PolyClone(&temp);
                    PolyDestroy(&temp);
                }
            }
            return answer;
            break;
        }
    }
}

/**
 * Zwraca iloczyn wielomianow p i q
 * @param[in] p
 * @param[in] q
 * @return @f$pq@f$
 */
static Poly PolyMulPoly(const Poly *p, const Poly *q) {
    /**
     *  Do tablicy o rozmiarze rownym iloczynowi rozmiarow p i q wpisujemy
     *  wymnozone odpowiednie jednomiany a nastepnie upraszczamy je uzywajac
     *  funkcji PolyAddMonos
     */
    assert(p != NULL && q != NULL);
    Mono *monos = (Mono *) malloc(sizeof(Mono) * p->size * q->size);
    CheckPointer(&monos);
    size_t k = 0;
    for (size_t i = 0; i < p->size; i++) {
        for (size_t j = 0; j < q->size; j++) {
            monos[k].exp = p->arr[i].exp + q->arr[j].exp;
            monos[k].p = PolyMul(&p->arr[i].p, &q->arr[j].p);
            k++;
        }
    }
    Poly res = PolyAddMonos(p->size * q->size, monos);
    free(monos);
    return res;
}

/**
 * Funckja pomocznicza rekurencyjnie negujaca wielomian p
 * @param[in] p
 */
static void PolyNegHelp(Poly *p) {
    /**
     *  Rekurencja się kończy gdy p jest jednomianem stalym
     */
    assert(p != NULL);
    if (p->arr == NULL) {
        p->coeff *= (-1);
    } else {
        for (size_t i = 0; i < p->size; i++)
            PolyNegHelp(&(p->arr[i].p));
    }
}

void PolyDestroy(Poly *p) {
    assert(p != NULL);
    if (p->arr == NULL)
        return;
    for (size_t i = 0; i < p->size; i++)
        PolyDestroy(&p->arr[i].p);
    free(p->arr);
}

Poly PolyClone(const Poly *p) {
    /**
     * Wykonuje rekurencyjnie głęboką kopię wielomianu konczać rekurencje gdy
     * p to wielomian staly
     */
    assert(p != NULL);
    if (p->arr == NULL) {
        return PolyFromCoeff(p->coeff);
    }
    Poly clone = {.size = p->size};
    clone.arr = (Mono *) malloc(sizeof(Mono) * clone.size);
    CheckPointer(&clone.arr);
    for (size_t i = 0; i < clone.size; i++) {
        assert(i < clone.size);
        clone.arr[i].exp = p->arr[i].exp;
        // arr[i] to jednomian
        if (p->arr[i].p.arr == NULL) {
            clone.arr[i].p.arr = NULL;
            clone.arr[i].p.coeff = p->arr[i].p.coeff;
        } else {
            clone.arr[i].p.size = p->arr[i].p.size;
            clone.arr[i].p = PolyClone(&(p->arr[i].p));
        }
    }
    return clone;
}

poly_exp_t PolyDeg(const Poly *p) {
    /**
     *  Wywołuje funkcje pomocnicze PolyDegRec
     */
    if (!PolyIsZero(p))
        return PolyDegRec(p);
    return -1;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    /**
     *  Dodajemy wielomiany rozpatrujac trzy przypadki, nastepnie wywolujemy
     *  odpowiednie funkcje PolyFromCoeff PolyAdddConstToPoly lub PolyAddPoly
     */
    assert(p != NULL && q != NULL);
    if (p->size == 0 && q->size == 0)
        return PolyFromCoeff(0);
    else if (p->size == 0)
        return PolyClone(q);
    else if (q->size == 0)
        return PolyClone(p);
    else if (p->arr == NULL && q->arr == NULL)
        return PolyFromCoeff(p->coeff + q->coeff);
    else if (q->arr == NULL && p->arr != NULL)
        return PolyAddConstToPoly(p, q->coeff);
    else if (q->arr != NULL && p->arr == NULL)
        return PolyAddConstToPoly(q, p->coeff);
    else
        return PolyAddPolyToPoly(p, q);
}


Poly PolyAddMonos(size_t count, const Mono monos[]) {
    /**
     *  Dodaje do siebie tablice jednomianow monos zwracajac wielomian. Funkcja
     *  kopiuje zawartosc monos sortuje ją a nastepnie dodaje jednomiany do
     *  siebie na biezaca sprawdzajac warunki brzegowe takie jak np: czy dany
     *  wielomian jest rowny 0
     */
    assert(monos != NULL);
    if (count == 0)
        return PolyFromCoeff(0);

    Mono *monosCopy = CopyMonos(count, monos);
    ConstMonosDestroy(count, monos);

    Poly res;

    qsort(monosCopy, count, sizeof(Mono), MonosCompare);

    res.arr = (Mono *) malloc(sizeof(Mono) * count);
    res.size = count;
    size_t k = 0;
    size_t i = 1;
    res.arr[0] = MonoClone(&monosCopy[0]);

    while (i < count) {
        if (res.arr[k].exp == monosCopy[i].exp) {
            //  zmienne temp zapewniaja nie nadpisywanie pamieci
            Poly temp_sum = PolyAdd(&res.arr[k].p, &monosCopy[i].p);
            //  dodane jednomiany sa rowne 0
            if (PolyIsZeroDeep(&temp_sum)) {
                //  sa to ostanie jednomiany dodatkowo nie ma zadnych innych
                //  w wyniki
                if (k == 0 && i == count - 1) {
                    PolyDestroy(&res.arr[0].p);
                    res.arr[0].p = PolyZero();
                    res.arr[0].exp = 0;
                }
                    //  byly to ostatnie jednomiany ale sa dodatkowe w wyniku
                else if (k > 0 && i == count - 1) {
                    PolyDestroy(&res.arr[k].p);
                    k--;
                }
                    // nie sa to ani ostatnie wielomiany dodatkowo nie sa to
                    // jedyne w wyniku
                else {
                    PolyDestroy(&res.arr[k].p);
                    res.arr[k] = MonoClone(&monosCopy[i + 1]);
                    i++;
                }
                PolyDestroy(&temp_sum);
            } else {
                PolyDestroy(&res.arr[k].p);
                res.arr[k].p = PolyClone(&temp_sum);
                PolyDestroy(&temp_sum);
            }
        } else {
            k++;
            res.arr[k] = MonoClone(&monosCopy[i]);
        }
        i++;
    }
    //  sprawdzenie czy wynik nie jest zagniezdzonym jednomianem stalym
    if (k == 0 && res.arr[0].exp == 0 && IsNestedCoeff(&res.arr[0].p)) {
        poly_coeff_t x = GetNestedCoeff(&res.arr[0].p);
        Poly temp;
        if (x == 0)
            temp = PolyZero();
        else
            temp = PolyFromCoeff(GetNestedCoeff(&res.arr[0].p));
        PolyDestroy(&res.arr[0].p);
        free(res.arr);
        res = temp;
    } else {
        res.arr = (Mono *) realloc(res.arr, sizeof(Mono) * (k + 1));
        res.size = k + 1;
    }

    for (size_t i = 0; i < count; i++)
        MonoDestroy(&monosCopy[i]);
    free(monosCopy);

    return res;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);
    if (p->arr == NULL && q->arr == NULL)
        return PolyFromCoeff(p->coeff * q->coeff);
    if (p->arr == NULL || q->arr == NULL) {
        //p jest stałym wielomianem
        if (q->arr == NULL)
            return PolyMulCoeff(p, q->coeff);
        else
            return PolyMulCoeff(q, p->coeff);
    } else
        return PolyMulPoly(p, q);
}

Poly PolyNeg(const Poly *p) {
    assert(p != NULL);
    Poly answer = PolyClone(p);
    PolyNegHelp(&answer);
    return answer;
}

Poly PolySub(const Poly *p, const Poly *q) {
    /**
     *  Usuwa od siebie jednomiany korzystajać z zależnosći że @f$a-b=a+(-b)@f$
     */
    assert(p != NULL && q != NULL);
    Poly qNeg = PolyNeg(q);
    Poly res = PolyAdd(p, &qNeg);
    PolyDestroy(&qNeg);
    return res;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    /**
     *  Na poczatku zaglebiamy sie rekurencyjnie na dany poziom a pozniej
     *  wywołujemy funkcje PolyDeg
     */
    assert(p != NULL);
    if (PolyIsZero(p))
        return -1;
    if (var_idx == 0) {
        if (p->arr != NULL) {
            size_t deg = 0;
            for (size_t i = 0; i < p->size; i++) {
                deg = Max(deg, p->arr[i].exp);
            }
            return deg;
        }
    }
    if (p->arr != NULL) {
        size_t deg = 0;
        for (size_t i = 0; i < p->size; i++) {
            deg = Max(deg, PolyDegBy(&p->arr[i].p, var_idx - 1));
        }
        return deg;
    }
    return 0;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    /**
     *  Wiemy że wielomiany spelniaja ta zaleznosc ze sa posortowane rosnaca
     *  wzgledem wykladnikow. Jest tak dlatego ponieważ wszystkie inne
     *  funkcje spełniają to założenie a wielomiany sa tworzone przez te
     *  funkcje. Dzieki temu mozemy po prostu przejsc wszystkie jednomiany po
     *  kolei i porownywac je ze soba
     */
    assert(p != NULL && q != NULL);
    if (PolyIsZeroDeep(p) && PolyIsZeroDeep(q))
        return true;
    else if (p->arr == q->arr)
        return p->coeff == q->coeff;
    else if (p->arr == NULL || q->arr == NULL)
        return false;
    else if (p->size == q->size) {
        bool isEq = true;
        for (size_t i = 0; i < p->size; i++) {
            assert(i == 0 || p->arr[i].exp >= p->arr[i - 1].exp);
            if (p->arr[i].exp == q->arr[i].exp) {
                isEq &= PolyIsEq(&(p->arr[i].p), &(q->arr[i].p));
                if (!isEq)
                    return false;
            } else
                return false;
        }
        return isEq;
    }
    return false;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    /**
     *  Przechodzimy jednomiany wielomiany ktore sa postaci $px^n$ obliczamy
     *  $x^n$ a nastepnie do wyniku dodajemy $x^n * p$ uzywajac funkcji PolyMul
     */
    assert(p != NULL);
    if (p->arr == NULL)
        return PolyClone(p);

    Poly res = PolyFromCoeff(0);

    for (size_t i = 0; i < p->size; i++) {
        Poly temp;
        //  zmienee temp zapobiegaja przeciekom powstalym podczas
        //  nadpisania pamieci
        if (p->arr[i].exp == 0) {
            temp = PolyAdd(&res, &p->arr[i].p);
            PolyDestroy(&res);
            res = PolyClone(&temp);
        } else {
            Poly coeff = PolyFromCoeff(PowCcoeff(x, p->arr[i].exp));
            Poly mul = PolyMul(&p->arr[i].p, &coeff);
            temp = PolyAdd(&res, &mul);
            PolyDestroy(&res);
            res = PolyClone(&temp);
            PolyDestroy(&mul);
            PolyDestroy(&coeff);
        }
        PolyDestroy(&temp);
    }
    return res;
}
