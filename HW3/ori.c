#include <stdio.h>
#include <math.h>
#include <malloc.h>

typedef struct point{
    double x;
    double y;
}point;

typedef struct origami{
    int prev;   //Poprzednia kartka, -1 gdy takiej nie ma
    point p1;   //Prawy dolny róg prostokąta / środek koła / pierwszy punkt prostej złożenia
    point p2;   //Lewy górny róg prostokąta / (typ:okrąg, promień koła) / drugi punkt prostej złożenia
}origami;

inline int is_zero(double x) {return fabs(x) < 1e-10;}
inline int less_equal(double x, double y) {return x < y || is_zero(x - y);}
inline double sq(double x) {return x * x;}
inline double x_product(point l1, point l2, point p) {return (l2.x - l1.x) * (p.y - l1.y) - (l2.y - l1.y) * (p.x - l1.x);}

inline int inside_circle(point center, double r, point p2){
    return less_equal(sq(center.x - p2.x) + sq(center.y - p2.y), sq(r));
}

inline int inside_orth_rect(point rect1, point rect2, point p3){
    return less_equal(rect1.x, p3.x) && less_equal(p3.x, rect2.x) && less_equal(rect1.y, p3.y) && less_equal(p3.y, rect2.y);
}

inline point reflect_point(point l1, point l2, point p) {
    double dx = l2.x - l1.x;
    double dy = l2.y - l1.y;

    double a = (sq(dx) - sq(dy)) / (sq(dx) + sq(dy));
    double b = 2 * dx * dy / (sq(dx) + sq(dy));

    double x = a * (p.x - l1.x) + b * (p.y - l1.y) + l1.x;
    double y = b * (p.x - l1.x) - a * (p.y - l1.y) + l1.y;

    point result = (point){x, y};
    return result;
}

int count(origami* orgs, int it, point p){
    origami org = orgs[it];
    if(org.prev < 0){ //Kartka nie powstała przez złożenie
        if(less_equal(org.p1.x,org.p2.x)) // Prostokąt
            return inside_orth_rect(org.p1, org.p2, p);
        return inside_circle(org.p1, org.p2.y, p); // Koło
    }

    double x_prod = x_product(org.p1, org.p2, p);
    if(is_zero(x_prod)){ 
        //Punkt na prostej złożenia: Liczba przebić po złożeniu się nie zmieni. 
        return count(orgs, org.prev, p);  
    }
    else if(x_prod > 0){ //Punkt po lewej stronie prostej złożenia: 
        //Liczba przebić po złożeniu jest równa sumie przebić przed złożeniem dla punktu i jego odbicia względem prostej złożenia.
        return count(orgs, org.prev, reflect_point(org.p1,org.p2,p)) + count(orgs, org.prev, p);
    }
    return 0; //Punkt po prawej: Szpilka trafia w pustkę, nie ma żadnego przebicia.
}

int main(void){
    int n,q;
    scanf("%d %d", &n, &q);
    origami* orgs = (origami*)malloc((size_t)n * sizeof(origami));

    char type;
    for(int i = 0; i < n; i++){
        scanf(" %c", &type);
        if(type == 'P'){
            scanf("%lf %lf %lf %lf", &orgs[i].p1.x, &orgs[i].p1.y, &orgs[i].p2.x, &orgs[i].p2.y);
            orgs[i].prev = -1;
        }
        else if(type == 'K'){
            scanf("%lf %lf %lf", &orgs[i].p1.x, &orgs[i].p1.y, &orgs[i].p2.y);
            orgs[i].p2.x = orgs[i].p1.x-1; //"Psujemy" prostokąt by kodować koło
            orgs[i].prev = -1;
        }
        else if(type == 'Z'){
            scanf("%d %lf %lf %lf %lf",&orgs[i].prev, &orgs[i].p1.x, &orgs[i].p1.y, &orgs[i].p2.x, &orgs[i].p2.y);
            orgs[i].prev--;
        }
        else{
            printf("Incorrect input!");
            return 1;
        }
    }
    for (int i = 0; i < q; i++) {
        int it;
        point p;
        scanf("%d %lf %lf",&it, &p.x, &p.y);
        printf("%d\n", count(orgs, it-1, p));
    }

    free(orgs);
    return 0;
}