#include "ary.h"
#include <math.h>
#include <stdbool.h>

#define swap(x, y) do { __typeof__(x) temp = x; x = y; y = temp;} while (0)

#define empty (wartosc){NAN,NAN}
#define full (wartosc){-HUGE_VAL, HUGE_VAL}


inline bool is_zero(double x) { return fabs(x) < 1e-10; }
inline bool is_less_equal(double x, double y) { return x < y || is_zero(x-y); }

inline bool is_only_zero(wartosc w) { return is_zero(w.first) && is_zero(w.second); }   //(0,0) -> {0}
inline bool is_empty(wartosc w) { return isnan(w.first) || isnan(w.second); }       //(NAN,NAN) -> pusty
inline bool is_full(wartosc w) { return isinf(w.first) && isinf(w.second) && w.first < 0 && w.second > 0; } //(-inf,inf) -> pełny
inline bool is_norm(wartosc w) { return is_less_equal(w.first,w.second); }      //<a,b> -> [a,b] 

wartosc wartosc_dokladna(double x){
    return (wartosc){x,x};
}

wartosc wartosc_od_do(double x, double y){
    if(is_zero(x)){
        x = 0.0;
    }

    if(is_zero(y)){
        y = -0.0;
    }

    return (wartosc){x, y};
}

wartosc wartosc_dokladnosc(double x, double p){
    double first = x*(1-p/100);
    double second = x*(1+p/100);
    if(x < 0){
        swap(first, second);
    }
    return wartosc_od_do(first, second);
}

bool in_wartosc(wartosc w, double x){
    if(is_norm(w)){
        return (is_less_equal(w.first,x) && is_less_equal(x,w.second));
    }
    else{
        return ((w.second < x) ^ (x < w.first) || is_zero(w.first-x)|| is_zero(w.second-x));
    }
}

double min_wartosc(wartosc w){
    if(is_empty(w)){
        return NAN;
    }
    if(is_norm(w)){
        return w.first;
    }
    return -HUGE_VAL;
}

double max_wartosc(wartosc w){
    if(is_empty(w)){
        return NAN;
    }
    if(is_norm(w)){
        return w.second;
    }
    return HUGE_VAL;
}

double sr_wartosc(wartosc w){
    if(is_norm(w)){
        return (w.first + w.second)/2;
    }
    return NAN;
}

wartosc plus(wartosc a, wartosc b){
    if(is_empty(a) || is_empty(b)){
        return empty;
    }
    if(is_full(a) || is_full(b)){
        return full;
    }

    if(is_norm(a) && is_norm(b)){
        return wartosc_od_do(a.first + b.first, a.second + b.second);
    }

    if(is_norm(a)){
        swap(b,a);
    }

    if(is_norm(b)){
        if(is_less_equal(a.first + b.first, a.second + b.second)){               //Min i max się przękręciły, czyli (-inf,a2]U[a1,inf) + [b1,b2] = (-inf,inf)
            return full;
        }
        else{
            return wartosc_od_do(a.first + b.first, a.second + b.second);       //Min i max się nie przękręciły
        }
    }

    return full; //(-inf,a2]U[a1,inf) + (-inf,b2]U[b1,inf) = (-inf,inf)
}

wartosc razy(wartosc a, wartosc b) {
    if (is_empty(a) || is_empty(b)) {
        return empty;
    }

    if (is_only_zero(a) || is_only_zero(b)) {
        return wartosc_dokladna(0);
    }

    if (is_full(a) || is_full(b)) {
        return full;
    }

    
    if(is_norm(a) && is_norm(b)){
        // gdy mamy przedziały [a1,a2] i [b1,b2] to mamy 4 kandydatów na min i max:
        double a1b1 = a.first * b.first;
        double a1b2 = a.first * b.second;
        double a2b1 = a.second * b.first;
        double a2b2 = a.second * b.second;

        // szukamy min i max spośród 4 kandydatów:
        double min_val = fmin(fmin(a1b1, a1b2), fmin(a2b1, a2b2));
        double max_val = fmax(fmax(a1b1, a1b2), fmax(a2b1, a2b2));

        // [min_val, max_val] to przedział reprezentujący wynik mnożenia
        return wartosc_od_do(min_val, max_val);
    }

    if(is_norm(a)){
        swap(b,a);
    }

    if(is_norm(b)){
        double first;
        double second;

        //Gdy mamy przedziały (-inf;a2] U [a1;inf] i [b1;b2] to mamy dwa przypadki: b2 < 0 i b2 >= 0
        //W zależności od znaku b2 wybieramy odpowiednich kandydatów na min i max
        if(b.second < 0){
            first = fmin(a.second*b.first,a.second*b.second);
            second = fmax(a.first * b.first, a.first * b.second);

        }
        else{
            first = fmin(a.first * b.first, a.first * b.second);
            second = fmax(a.second * b.first, a.second * b.second);
        }

        if(is_less_equal(first, second)){   //Min i max się przękręciły, czyli (-inf,a2]U[a1,inf) * [b1,b2] = (-inf,inf)
            return full;
        }       
        
        //Min i max się nie przękręciły         
        return wartosc_od_do(first, second);
    }

    //Gdy mamy przedziały (-inf;a2]U[a1;inf) i (-inf;b2]U[b1;inf) to mamy kandydatów na min i max:
    double first = fmin(a.first * b.first, a.second * b.second);
    double second = fmax(a.first * b.second, a.second * b.first);
    if(is_less_equal(first,second)){   //Min i max się przękręciły, czyli (-inf,a2]U[a1,inf) * (-inf,b2]U[b1;inf) = (-inf,inf)
        return full;
    }
    else{                 //Min i max się nie przękręciły        
        return wartosc_od_do(first, second);
    }
}

wartosc minus(wartosc a, wartosc b){
   return plus(a, razy(b, wartosc_dokladna(-1)));
}

wartosc inverse(wartosc a){
    if(is_empty(a)||is_only_zero(a)){
        return empty;
    }
    
    if(is_full(a)){
        return full;
    }

    return wartosc_od_do(1/a.second, 1/a.first);
}

wartosc podzielic(wartosc a, wartosc b){
    return razy(a, inverse(b));
}