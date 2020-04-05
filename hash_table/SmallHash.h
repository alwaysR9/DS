// A Simple Hash Table
// Which speed is 6 times faster than std::unordered_map

// key:   uint64_t
// value: uint64_t

#ifndef __SMALL_HASH__
#define __SMALL_HASH__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

struct SmallHashElem {
    uint64_t key;
    uint64_t val;
};

class SmallHash {
public:
    SmallHash() {
        m_bucket_size = 0;
        m_bucket = NULL;
    }
    SmallHash(size_t real_size) {
        assert(real_size <= 500000);
        int PRIME_NUM = 8;
        int PRIME[] = {10, 281, 1217, 9209, 38833, 163729, 388211, 690163};
        int i;
        for (i = 0; i < PRIME_NUM; ++ i) {
            if (real_size * 2 < PRIME[i]) {
                m_bucket_size = PRIME[i];
                break;
            }
        }
        if (i == PRIME_NUM)
            m_bucket_size = PRIME[PRIME_NUM-1];
        m_bucket = new SmallHashElem[m_bucket_size];
        memset(m_bucket, -1, sizeof(SmallHashElem) * m_bucket_size);
        m_used_size = 0;
        fprintf(stderr, "bucked_size: %u\n", m_bucket_size);
    }

     ~SmallHash() {
        if (m_bucket) {
            delete [] m_bucket;
            m_bucket = NULL;
        }
    }

    void clear() {
        memset(m_bucket, -1, sizeof(SmallHashElem) * m_bucket_size);
        m_used_size = 0;
    }

    // true: success
    // false: fail, hash table is full
    bool put(uint64_t k, uint64_t v) {
        if (m_used_size >= m_bucket_size * 0.8)
            return false;

        int64_t key = static_cast<int64_t>(k / m_bucket_size);
        int idx = k % m_bucket_size;

        int64_t _key = static_cast<int64_t>(m_bucket[idx].key);
        while (_key != -1) {
            if (_key == key) {
                //fprintf(stderr, "put exist: original_key=%llu, key=%lld, _key:%lld idx:%d\n", k, key, _key, idx);
                break; // exist
            }
            //fprintf(stderr, "put collsion: original_key=%llu, key=%lld, _key:%lld idx:%d\n", k, key, _key, idx);
            idx += 1;
            if (idx >= m_bucket_size)
                idx = 0;
            _key = static_cast<int64_t>(m_bucket[idx].key);
        }
        m_bucket[idx].val = v;
        if (_key == -1) {// put a new item
            m_bucket[idx].key = static_cast<uint64_t>(key);
            m_used_size += 1;
        }
        //fprintf(stderr, "put succ: origin_key=%llu, key=%llu, idx=%d\n", k, m_bucket[idx].key, idx);
        return true;
    }

    int find(uint64_t k) {
        int64_t key = static_cast<int64_t>(k / m_bucket_size);
        int idx = k % m_bucket_size;

        int64_t _key = static_cast<int64_t>(m_bucket[idx].key);
        while (_key != -1) {
            if (_key == key) {
                //fprintf(stderr, "find succ: origin_key=%llu, key:%lld, _key=%lld, idx:%d\n", k, key, _key, idx);
                return idx;
            }
            //fprintf(stderr, "find collsion: origin_key=%llu, key:%lld, _key=%lld, idx:%d\n", k, key, _key, idx);
            idx += 1;
            if (idx >= m_bucket_size)
                idx = 0;
            _key = static_cast<int64_t>(m_bucket[idx].key);
        }
        //fprintf(stderr, "find fail: origin_key=%llu, key:%lld, _key:%lld, idx:%d\n", k, key, _key, idx);
        return -1;
    }

    uint64_t get_by_idx(int idx) {
        assert (idx >= 0 && idx < m_bucket_size);
        return m_bucket[idx].val;
    }

private:
    size_t m_bucket_size;
    SmallHashElem* m_bucket;
    size_t m_used_size;
};


#endif
