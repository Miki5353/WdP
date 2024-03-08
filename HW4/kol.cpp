#include <vector>
#include <malloc.h>

#include "kol.h"

using namespace std;

struct lqueue{
    interesant *head, *tail;
};

inline lqueue create_queue(){ // Tworzenie kolejki z dwoma atrapami
    lqueue q = {
        (interesant *)malloc(sizeof(interesant)),
        (interesant *)malloc(sizeof(interesant))};
    q.head->ptr1 = NULL;
    q.head->ptr2 = q.tail;
    q.tail->ptr1 = NULL;
    q.tail->ptr2 = q.head;
    return q;
}

inline bool empty(const lqueue q){
    return q.head->ptr2 == q.tail;
}

inline void link(interesant *i, interesant *o, interesant *n){ // Funkcja pomocnicza do linkowania interesantów
    if (i){
        if (i->ptr1 == o){
            i->ptr1 = n;
        }
        else{
            i->ptr2 = n;
        }
    }
}

inline void push_back(lqueue &q, interesant *i){ // Wstawienie interesanta do kolejki
    i->ptr1 = q.tail;
    i->ptr2 = q.tail->ptr2;
    link(q.tail->ptr2, q.tail, i);
    q.tail->ptr2 = i;
}

inline interesant *push_back(lqueue &q, int val){
    interesant *i = (interesant *)malloc(sizeof(interesant));
    i->val = val;
    push_back(q, i);
    return i;
}

inline interesant *erase(interesant *i){ // Usunięcie interesanta z kolejki
    link(i->ptr1, i, i->ptr2);
    link(i->ptr2, i, i->ptr1);
    return i;
}

inline interesant *pop_front(lqueue &q){
    if (!empty(q)){
        return erase(q.head->ptr2);
    }
    return NULL;
}

inline void reverse(lqueue &q){ // Odwrócenie kolejki
    swap(q.head, q.tail);
}

inline void append(lqueue &q1, lqueue &q2){ // Doklejenie q2 do q1, wyczyszczenie q2 do dwóch atrap
    if (!empty(q2)){
        interesant *b = q1.tail;
        interesant *f = q2.head;
        link(b->ptr2, b, f->ptr2);
        link(f->ptr2, f, b->ptr2);
        b->ptr2 = f;
        f->ptr2 = b;
        swap(q1.tail, q2.tail);
    }
}

inline void destroy(lqueue &q){ // Zwalnianie pamięci
    while (!empty(q)){
        pop_front(q);
    }
    free(q.head);
    free(q.tail);
}

static vector<lqueue> office;
static int counter;

void otwarcie_urzedu(int m){
    office.resize(m);
    for (int i = 0; i < m; i++){
        office[i] = create_queue();
    }
    counter = 0;
}

interesant *nowy_interesant(int k){
    return push_back(office[k], counter++);
}

int numerek(interesant *i){
    return i->val;
}

interesant *obsluz(int k){
    return pop_front(office[k]);
}

void zmiana_okienka(interesant *i, int k){
    erase(i);                // Usuwamy interesanta z kolejki, w której był
    push_back(office[k], i); // Wstawiamy go do nowej
}

void zamkniecie_okienka(int k1, int k2){
    append(office[k2], office[k1]);
}

inline interesant *get_next(interesant *i, interesant *prev){
    if (i->ptr1 != prev){ // Sprawdzamy który wskaźnik nie jest prev
        return i->ptr1;
    }
    else{
        return i->ptr2;
    }
}

vector<interesant *> fast_track(interesant *i1, interesant *i2){
    if (i1 == i2){
        return {erase(i1)};
    }

    vector<interesant *> paths[2]; // Będziemy szukać ścieżki od i1 do i2 w dwóch kierunkach jednocześnie
    paths[0].push_back(i1);
    paths[1].push_back(i1);

    interesant *ptrs[2];
    ptrs[0] = i1->ptr1;
    ptrs[1] = i1->ptr2;

    while (ptrs[0] != i2 && ptrs[1] != i2){ // Równoczeńsnie szukamy w dwóch kierunkach, de facto BFS
        for (int j = 0; j < 2; j++){
            if (ptrs[j]->ptr1 && ptrs[j]->ptr2){ // Sprawdzamy czy interesant nie jest atrapą
                paths[j].push_back(ptrs[j]);
                ptrs[j] = get_next(ptrs[j], paths[j][paths[j].size() - 2]); // Przechodzimy do interesanta połączonego z ptrs[j] i nie należącego do ścieżki
            }
        }
    }

    int num = ptrs[0] == i2 ? 0 : 1;                                    // Sprawdzamy w którym kierunku znaleźliśmy ścieżkę.
    interesant *prev = num ? i1->ptr1 : i1->ptr2;                       // Znajdujemy interesanta poprzedzającego i1 ...
    interesant *next = get_next(i2, paths[num][paths[num].size() - 1]); //... i następującego po i2, ...
    link(prev, i1, next);                                               //  ... łączymy ...
    link(next, i2, prev);                                               // ... ich.
    paths[num].push_back(i2);                                           // Dodajemy i2 do ścieżki
    return paths[num];
}

void naczelnik(int k){
    reverse(office[k]);
}

vector<interesant *> zamkniecie_urzedu(){
    vector<interesant *> result;
    for (auto q : office){
        while (!empty(q)){
            result.push_back(pop_front(q));
        }
        destroy(q);
    }
    return result;
}