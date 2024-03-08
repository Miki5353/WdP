#include <unordered_map>

#include "wys.h"

#define ull unsigned long long

using namespace std;

struct node {
    int question; //Optymalne pytanie
    int max_depth; //Minimalna pesymistyczna liczba zapytań do osiągnięcia stanu końcowego
    ull next[2]; //Stany osiągalne po odpowiedziach 0 i 1 na question
};

int n, k, g;

ull mask[5]; //Mask[i] maska bitowa, która ANDowana z liczbą stanu daje możliwe liczby przy i kłamstwach

void gen_masks() {
    mask[0] = (1ull << n) - 1;
    for(int i = 1; i <= k; i++) {
        mask[i] = (mask[i - 1] << n);
    }
}

ull get_state(const ull& state, int i, int sgn) { //Zwraca stan po zapytaniu o i przy odpowiedzi sgn
    ull shift = (1ull << (i - 1)) - 1; //Zapalamy lamplki od 1 do i - 1
    if (!sgn) shift = ~shift & mask[0]; //Jeśli Ala odpowiadała 1 to zapalamy wszystkie lampki od i do n

    ull res1 = 0; //OR z przypadkiem gdy Ala nie kłamała
    ull res2 = 0; //OR z przypadkiem gdy Ala kłamała
    for(int j = 0; j < k; j++) {
        res1 |= (state & shift);
        res2 |= (state & ~shift & mask[j]);
        shift = (shift << n);
    }
    res1 |= (state & shift);
    return res1 | (res2 << n);
}

int answer (ull state) { //Sprawdza, czy da się jednoznacznie określić liczbę Ali
    ull res = 0;
    for (int i = 0; i <= k; i++) {
        res |= (state & mask[i]) >> (i * n);
    }

    if(__builtin_popcountll(res) == 1) { //Niezależnie jak Ala kłamała mamy tylko jedną możliwość
        return __builtin_ctzll(res) + 1;
    }
    return 0;
}

unordered_map<ull, node> mp; //Hashmapa przechowująca wygenerowane stany

void gen_tree(ull state) {
    if(mp.find(state) != mp.end()) return; //Taki stan był już generowany

    if (!state) { //Stan nieosiągalny w grze
        mp[state] = {0, -1, {-1ull, -1ull}};
        return;
    }  

    int x = answer(state);
    if(x) {
        mp[state] = {-x, 0, {-1ull, -1ull}};
        return;
    }

    mp[state] = {0, 256, {-1ull, -1ull}};
    for(int i = 2; i <= n; i++) {   //Sprawdzamy wszystkie możliwe pytania, wybieramy optymalne

        node candidate = {-1, 256, {-1ull, -1ull}};
        for (int j = 0; j < 2; j++) {       //Gdy Ala odpowiada j na pytanie x < i
            ull new_state = get_state(state, i, j);
            if(state != new_state){         //Sprawdzamy czy zapytanie zmienia stan 
                gen_tree(new_state);
                candidate.next[j] = new_state;   
            }
        }

        if(candidate.next[0] == -1ull || candidate.next[1] == -1ull){   
            continue;
        }

        candidate.max_depth = max(mp[candidate.next[0]].max_depth, mp[candidate.next[1]].max_depth) + 1;
        candidate.question = i;

        if (candidate.max_depth < mp[state].max_depth) {
            mp[state] = candidate;
        }
    }
}

int main() {

    dajParametry(n, k, g);

    gen_masks();
    gen_tree(mask[0]);
    
    for(int i = 0; i < g; i++){
        ull st = mask[0];
        while(mp[st].question >= 0) {
            st = mp[st].next[mniejszaNiz(mp[st].question)];
        }
        odpowiedz(-mp[st].question);
    }

    return 0;
}