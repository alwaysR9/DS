#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//===========================================//
//                 ring queue                //
//                thread safe                //
//===========================================//
struct Queue {
    int _size;
    void** _buff;
    int _head;
    int _tail;

    pthread_mutex_t _mu;
    pthread_cond_t _nonfull;
    pthread_cond_t _nonempty;

    Queue(){}
    Queue(int size) {
        _size = size;
        _buff = new void*[_size];
        _head = _tail = 0;
        pthread_mutex_init(&_mu, NULL);
        pthread_cond_init(&_nonfull, NULL);
        pthread_cond_init(&_nonempty, NULL);
    }
    ~Queue() {
        delete [] _buff;
        pthread_mutex_destroy(&_mu);
        pthread_cond_destroy(&_nonfull);
        pthread_cond_destroy(&_nonempty);
    }

    bool isEmpty() {
        return _tail == _head;
    }

    bool isFull() {
        return (_tail + 1) % _size == _head;
    }

    void Put(void* pItem) {
        pthread_mutex_lock(&_mu);

        while(isFull()) {
            //printf("Queue is full...\n");
            pthread_cond_wait(&_nonfull, &_mu);
        }

        _buff[_tail] = pItem;
        _tail = (_tail + 1) % _size;        

        pthread_cond_signal(&_nonempty);
        pthread_mutex_unlock(&_mu);
    }

    void* Get() {
        pthread_mutex_lock(&_mu);

        while(isEmpty()) {
            //printf("Queue is empty...\n");
            pthread_cond_wait(&_nonempty, &_mu);
        }

        void* pItem = _buff[_head];
        _head = (_head + 1) % _size;

        pthread_cond_signal(&_nonfull);
        pthread_mutex_unlock(&_mu);

        return pItem;
    }
};

void* Produce(void* argv) {
    Queue* q = (Queue*) argv;
    
    int pid = rand() % 10000;
    for (int i = 1; i <= 20; ++ i) {
        char* buff = new char[10];
        sprintf(buff, "%d: %d", pid, i);
        printf("[Put]: %s\n", buff);
        q->Put((void*) buff);
    }
    char* buff = new char[10];
    buff[0] = 0;
    q->Put((void*) buff);
    printf("Producer finish\n");
}

void* Consume(void* argv) {
    Queue* q = (Queue*) argv;
    while (1) {
        char* v = (char*) q->Get();
        if (v[0] == 0) {
            delete v;
            break;
        }
        printf("[Receive]: %s\n", v);
        delete v;
    }
    printf("Consumer finish\n");
}

int main() {

    Queue q(2);

    pthread_t pidProducer, pidConsumer;
    pthread_create(&pidProducer, NULL, Produce, &q);
    pthread_create(&pidConsumer, NULL, Consume, &q);
    pthread_join(pidProducer, NULL);
    pthread_join(pidConsumer, NULL);

    return 0;
}