#include <iostream>
#include <cassert>
#include "LinkedQueue.h"

void testEmptyQueueDequeue() {
    LinkedQueue<int> q;
    int item = 0;

    // Explicitly test the issue: dequeue on empty should return false
    assert(q.dequeue(item) == false);

    // Also test peek on empty
    assert(q.peek(item) == false);
}

void testQueueOperations() {
    LinkedQueue<int> q;

    assert(q.isEmpty() == true);
    assert(q.getCount() == 0);

    int item;

    // Test Enqueue
    assert(q.enqueue(10) == true);
    assert(q.isEmpty() == false);
    assert(q.getCount() == 1);

    assert(q.peek(item) == true);
    assert(item == 10);

    assert(q.enqueue(20) == true);
    assert(q.enqueue(30) == true);
    assert(q.getCount() == 3);

    // Test Dequeue
    assert(q.dequeue(item) == true);
    assert(item == 10);
    assert(q.getCount() == 2);

    assert(q.dequeue(item) == true);
    assert(item == 20);
    assert(q.getCount() == 1);

    assert(q.dequeue(item) == true);
    assert(item == 30);
    assert(q.getCount() == 0);
    assert(q.isEmpty() == true);

    // Dequeue on empty again
    assert(q.dequeue(item) == false);
}

int main() {
    testEmptyQueueDequeue();
    testQueueOperations();

    std::cout << "All LinkedQueue tests passed.\n";
    return 0;
}
