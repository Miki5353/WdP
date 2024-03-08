#include<stdio.h>
#include<stdlib.h>

typedef struct pair{
    int x;
    int y;
}pair;

inline int in_arr(const pair* arr, int n,int x){
    for(int i = 0; i < n; i++){
        if(arr[i].x == x){
            return 1;
        }
    }
    return 0;
}

inline int max(int a, int b){
    return a > b ? a : b;
}

inline int min(int a, int b){
    return a < b ? a : b;
}

int solve_min(const pair* arr, int n){
    pair* prev = (pair*)malloc(sizeof(pair) *(size_t)n); //prev[i] ostatnie dwa motele parami różnych typów ze sobą i z i-tym
    prev[0] = (pair){-1,-1};
    for(int i = 1; i < n; i++){
        if(arr[i].x == arr[i-1].x){
            prev[i] = prev[i-1];
        }
        else{
            prev[i].x = i-1;
            if(arr[i].x != arr[prev[i-1].x].x){
                prev[i].y = prev[i-1].x;
            }
            else{
                prev[i].y = prev[i-1].y;
            }
        }
    }

    pair* next = (pair*)malloc(sizeof(pair) *(size_t)n); //next[i] następne dwa motele parami różnych typów ze sobą i z i-tym
    next[n-1] = (pair){-1,-1};
    for(int i = n-2; i >= 0; i--){
        if(arr[i].x == arr[i+1].x){
            next[i] = next[i+1];
        }
        else{
            next[i].x = i+1;
            if(arr[i].x != arr[next[i+1].x].x){
                next[i].y = next[i+1].x;
            }
            else{
                next[i].y = next[i+1].y;
            }
        }
    }

    int res = __INT_MAX__;
    for(int i = 0; i < n; i++){
        if(prev[i].x == -1 || next[i].x == -1){
            continue;
        }
        if(arr[prev[i].x].x != arr[next[i].x].x){
            res = min(res, max(arr[i].y - arr[prev[i].x].y, arr[next[i].x].y - arr[i].y));
        }
        else{
            if(prev[i].y != -1){
                res = min(res, max(arr[i].y - arr[prev[i].y].y, arr[next[i].x].y - arr[i].y));
            }
            if(next[i].y != -1){
                res = min(res, max(arr[i].y - arr[prev[i].x].y, arr[next[i].y].y - arr[i].y));
            }
        } 
    }

    free(next);
    free(prev);
    return res;
}

int solve_max(const pair* arr, int n){

    pair* first = (pair*)malloc(sizeof(pair) * (size_t)3);   //Pierwsze trzy motele różnych typów {typ, index}
    
    int size1 = 0;
    for(int i = 0; i < n && size1 < 3; i++){
        if(!in_arr(first, size1, arr[i].x)){
            first[size1++] = (pair){arr[i].x, i};
        }
    }
    

    pair* last = (pair*)malloc(sizeof(pair) * (size_t)3);    //Ostatnie trzy motele różnych typów {typ, index}
    int size2 = 0;
    for(int i = n-1; i >= 0 && size2 < 3; i--){
        if(!in_arr(last, size2, arr[i].x)){
            last[size2++] = (pair){arr[i].x, i};
        }
    }

    int res = -1;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < size1; j++){
            for(int k = 0; k < size2; k++){
                if(arr[i].x == first[j].x || arr[i].x == last[k].x || first[j].x == last[k].x){
                    continue;
                }
                res = max(res, min(arr[i].y - arr[first[j].y].y, arr[last[k].y].y - arr[i].y));
            }
        }
    }

    free(last);
    free(first);
    return res;
}

int main(){
    int n;
    scanf("%d", &n);
    pair* arr = (pair*)malloc(sizeof(pair) *(size_t)n);
    for(int i = 0; i < n; i++){
        scanf("%d %d", &arr[i].x, &arr[i].y);
    }

    int mn = solve_min(arr, n);
    int mx = solve_max(arr, n);

    if(mx < 0){
        printf("0 0\n");
    }
    else{
        printf("%d %d\n", mn, mx);
    }

    free(arr);
    return 0;
}