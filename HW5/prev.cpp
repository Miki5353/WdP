#include <vector>
#include "prev.h"

#define INT_MIN -2147483648
#define INT_MAX 2147483647

using namespace std;

inline int get_mid(int a, int b) {
    long long sum = 0ll + a + b;
    return int(sum / 2 - (sum % 2 != 0 && sum < 0 ? 1 : 0));
}

inline bool exist(int x) { return x > -1; }

struct node {
    int ls, rs;
    int val;
};

vector<node> tree;
vector<int> roots;

int query(int n, int n_lo, int n_hi, int lo, int hi) {
    if (!exist(n))
        return -1;

    if (n_lo == lo && n_hi == hi)
        return tree[n].val;

    int mid = get_mid(n_lo, n_hi);
    if (lo <= mid && mid <= hi)
        return max(query(tree[n].ls, n_lo, mid, lo, mid), query(tree[n].rs, mid + 1, n_hi, mid + 1, hi));
    if (lo <= mid)
        return query(tree[n].ls, n_lo, mid, lo, hi);
    return query(tree[n].rs, mid + 1, n_hi, lo, hi);
}

int update(int n, int n_lo, int n_hi, int idx, int val){
    
    if (n_lo == n_hi){
        return tree[n].val = max(tree[n].val, val);
    }

    int mid = get_mid(n_lo, n_hi);
    if (idx <= mid){
        if (exist(tree[n].ls))
            tree.push_back(tree[tree[n].ls]);
        else
            tree.push_back({-1, -1, 0});
        tree[n].ls = int(tree.size() - 1);
        return tree[n].val = max(tree[n].val, update(tree[n].ls, n_lo, mid, idx, val));
    }
    else{
        if (exist(tree[n].rs))
            tree.push_back(tree[tree[n].rs]);
        else
            tree.push_back({-1, -1, 0});
        tree[n].rs = int(tree.size() - 1);
        return tree[n].val = max(tree[n].val, update(tree[n].rs, mid + 1, n_hi, idx, val));
    }
}

void init(const vector<int> &seq) {
    roots.push_back(0);
    tree.push_back({-1, -1, seq[0]});
    for (auto val : seq)
        pushBack(val);
}

void pushBack(int value) {
    tree.push_back(tree[roots.back()]);
    roots.push_back(int(tree.size()-1));
    update(roots.back(), INT_MIN, INT_MAX, value, int(roots.size()-2));
}

int prevInRange(int i, int lo, int hi) { return query(roots[i+1], INT_MIN, INT_MAX, lo, hi); }

void done() { tree.clear(), roots.clear(); }
