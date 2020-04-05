#include "SmallHash.h"

#include <assert.h>
#include <iostream>
#include <unordered_map>
#include <sys/time.h>
#include <stdlib.h>

using namespace std;

int main() {

    // test 1
    SmallHash* hash = new SmallHash(10000);
    int idx = hash->find(100);
    assert(idx == -1);

    hash->put(100, 1000);
    idx = hash->find(100);
    assert(idx != -1);

    int val = hash->get_by_idx(idx);
    assert(val == 1000);

    for (int i = 0; i < 8; ++ i) {
        int k = i * 16381;
        hash->put(k, k);
        idx = hash->find(k);
        assert(idx != -1);

        val = hash->get_by_idx(idx);
        assert(val == k);
    }

    idx = hash->find(100);
    assert(idx != -1);
    val = hash->get_by_idx(idx);

    assert(val == 1000);

    idx = hash->find(101);
    assert(idx == -1);

    for (int i = 0; i < 15000; ++ i) {
        int k = rand();
        hash->put(k, k);
        idx = hash->find(k);
        assert(idx != -1);
        int val = hash->get_by_idx(idx);
        assert(val == k);
    }

    hash->put(100, 1001);
    idx = hash->find(100);
    assert(idx != -1);
    val = hash->get_by_idx(idx);
    assert(val == 1001);

    delete hash;

    // test 2
    timeval b, e;

    gettimeofday(&b, NULL);
    hash = new SmallHash(10000);
    for (int i = 0; i < 10000; ++ i) {
        int k = i * 1627;
        hash->put(k, k);
    }
    delete hash;
    gettimeofday(&e, NULL);
    uint32_t d1 = (e.tv_sec - b.tv_sec) * 1000000 + (e.tv_usec - b.tv_usec);

    unordered_map<int, int> m;
    gettimeofday(&b, NULL);
	for (int i = 0; i < 10000; ++ i) {
        int k = i * 1627;
        m[k] = k;
    }
    gettimeofday(&e, NULL);
    uint32_t d2 = (e.tv_sec - b.tv_sec) * 1000000 + (e.tv_usec - b.tv_usec);

    fprintf(stderr, "cost: SmallHash=%u us, std::unordered_map=%u us\n", d1, d2);

    return 0;
}
