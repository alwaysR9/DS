#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <stack>

//==============================================//
//                  Trie Tree                   //
//               not thread safe                //
//==============================================//
const int WIDTH = 26;

struct TrieNode {
    long _count;
    int _p; // position for iteration
    TrieNode* _next[WIDTH];
};

TrieNode* CreateTrieNode() {
    TrieNode* p = (TrieNode*) malloc(sizeof(TrieNode));
    memset(p, 0, sizeof(TrieNode));
    return p;
}

void ReleaseTrieNode(TrieNode* p) {
    for (int i = 0; i < WIDTH; ++ i) {
        if (p->_next[i]) {
            ReleaseTrieNode(p->_next[i]);
        }
    }
    delete p;
}

struct TrieTree {
    long long _mem_size;
    TrieNode* _root;

    // iteration
    std::stack<TrieNode*> _stack;
    char _prefix[129];
    int _prefix_len;

    int NodeSize;

    TrieTree() {
        _root = CreateTrieNode();
        NodeSize = sizeof(TrieNode);
        _mem_size = NodeSize;

        _stack.push(_root);
        _prefix_len = 0;
    }
    ~TrieTree() {
        ReleaseTrieNode(_root);
    }

    /*
    * buff must contain [a-z] or '\n'
    * insert buff[b: '\n') into Trie Tree
    * update b to '\n' + 1 position
    * */
    void Insert(const char* buff, int & b) {
        TrieNode* cur = _root;
        while (1) {
            if (buff[b] == '\n') {
                cur->_count ++;
                break;
            }
            int pos = buff[b] - 'a';
            if (cur->_next[pos] == NULL) {
                cur->_next[pos] = CreateTrieNode();
                _mem_size += NodeSize;
            }
            cur = cur->_next[pos];
            b ++;
        }
        ++ b; // skip '\n'
    }

    /*
    * extract next word from current iterator to buff[b:]
    * update b to '\n' + 1 position
    * buff have to have enough space.
    * This method will destruct Trie Tree.
    * 
    * return:
    *   true if iteration finish
    *   othervise return false
    * */
    bool Next(char* buff, int & b) {
        while (!_stack.empty()) {
            TrieNode* cur = _stack.top();
            assert (cur != NULL);
            if (cur->_count > 0) {
                memcpy(buff + b, _prefix, _prefix_len);
                b += _prefix_len;
                buff[b] = '\n';
                b ++;
                cur->_count --;
                return false;
            }
            bool find_child = false;
            while (cur->_p < WIDTH) {
                if (cur->_next[cur->_p] != NULL) {
                    //printf("path: %c\n", cur->_p + 'a');
                    _stack.push(cur->_next[cur->_p]);
                    _prefix[_prefix_len++] = cur->_p + 'a';
                    cur->_p ++;
                    find_child = true;
                    break;
                }
                cur->_p ++;
            }
            if (!find_child) {
                _stack.pop();
                _prefix_len --;
            }
        }
        return true;
    }
};

int main() {

    TrieTree* tree = new TrieTree;
    
    char* buff = (char*) malloc(sizeof(char) * 10000);

    int b = 0;
    int s_len = 38;
    sprintf(buff, "zxy\na\nwk\nzxy\nabdf\nabc\na\nwk\nzxyznaocna\n");
    //printf("len = %d\n", (int)strlen(buff));
    while (b < s_len) {
        tree->Insert(buff, b);
        printf("b after insert: %d\n", b);
    }

    b = 0;
    int bb = b;
    bool ret;
    while ( (ret = tree->Next(buff, b)) == false ) {
        printf("after Next(): b=%d\n", b);
        for (int i = bb; i < b; ++ i) {
            printf("%c", buff[i]);
        }
        printf("\n");
        bb = b;
    }
    printf("finish\n");

    delete tree;

    return 0;
}