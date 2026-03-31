#pragma once
#include "priNode.h"
#include <iostream> // MY72: Added to support std::cout for the print function

//This class impelements the priority queue as a sorted list (Linked List)
//The item with highest priority is at the front of the queue
template <typename T>
class priQueue
{
protected:  //in case you need to inherit for the project
    priNode<T>* head;
    int count; // MY72: Variable added to keep track of the queue size in O(1) time

public:
    priQueue() : head(nullptr), count(0) {} // MY72: Initialized count to 0 in the constructor

    ~priQueue() {
        T tmp;
        int p;
        while (dequeue(tmp, p));
    }

    //insert the new node in its correct position according to its priority
    void enqueue(const T& data, int priority) {
        priNode<T>* newNode = new priNode<T>(data, priority);

        count++; // MY72: Increment count whenever a new item is added to the queue

        if (head == nullptr || priority > head->getPri()) {

            newNode->setNext(head);
            head = newNode;
            return;
        }

        priNode<T>* current = head;
        while (current->getNext() && priority <= current->getNext()->getPri()) {
            current = current->getNext();
        }
        newNode->setNext(current->getNext());
        current->setNext(newNode);
    }

    bool dequeue(T& topEntry, int& pri) {
        if (isEmpty())
            return false;

        topEntry = head->getItem(pri);
        priNode<T>* temp = head;
        head = head->getNext();
        delete temp;

        count--; // MY72: Decrement count whenever an item is removed from the queue

        return true;
    }

    bool peek(T& topEntry, int& pri) {
        if (isEmpty())
            return false;

        topEntry = head->getItem(pri);
        return true;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    // MY72: getCount function added to return the current number of elements
    int getCount() const {
        return count;
    }

    // MY72: print function added to display all IDs currently in the priority queue
    void print() const {
        priNode<T>* p = head;
        int dummyPri;
        while (p) {
            std::cout << p->getItem(dummyPri); // MY72: Relies on the overloaded << operator in the entity class
            p = p->getNext();
            if (p) std::cout << ", "; // MY72: Print a comma between items
        }
    }
};