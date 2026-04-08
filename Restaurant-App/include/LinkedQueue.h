/** comment test This is a program that implements the queue abstract data type using a linked list.The queue is implemented as a chain of linked nodes that has two pointers, a frontPtr pointer for the front of the queue and a backPtr pointer for the back of the queue.*/
/*				The Node: item of type T and a "next" pointer					------------- 					| item| next | ---> 					-------------General Queue case:                 frontPtr																backPtr					\											   						/							 \											  					   /							------------- 	  ------------- 	  ------------- 	  ------------- 	  	  					| item| next |--->| item| next |--->  | item| next |--->  | item| next |---> NULL					------------- 	  ------------- 	  ------------- 	  -------------	  		Empty Case:                 frontptr	 backptr						\	 /										 \	/									---- NULL ------Single Node Case:                 frontPtr	 backPtr					\		/						 \	   /								----------- 						|item| next| -->NULL					-----------	*/

#ifndef LINKED_QUEUE_
#define LINKED_QUEUE_

#include "Node.h"
#include "QueueADT.h"
#include <iostream> // MY72: Added to support cout for the print function

template <typename T>
class LinkedQueue :public QueueADT<T>
{
protected:  //in case you need to inherit for the project
	Node<T>* backPtr;
	Node<T>* frontPtr;
	int count; // MY72: Variable to keep track of the number of items in the queue for O(1) access
public:
	LinkedQueue();
	bool isEmpty() const;
	bool enqueue(const T& newEntry);
	bool dequeue(T& frntEntry);
	bool peek(T& frntEntry)  const;
	int getCount() const; // MY72: Required function to return the total number of orders
	void print() const;   // MY72: Required function to display all order IDs
	~LinkedQueue();
};

/////////////////////////////////////////////////////////////////////////////////////////
/*Function: Queue()
The constructor of the Queue class.*/
template <typename T>
LinkedQueue<T>::LinkedQueue()
{
	backPtr = nullptr;
	frontPtr = nullptr;
	count = 0; // MY72: Initializing the counter to zero
}

/////////////////////////////////////////////////////////////////////////////////////////
/*Function: isEmpty
Sees whether this queue is empty.
Input: None.
Output: True if the queue is empty; otherwise false.*/
template <typename T>
bool LinkedQueue<T>::isEmpty() const
{
	return (frontPtr == nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////
/*Function:enqueue
Adds newEntry at the back of this queue.
Input: newEntry .
Output: True if the operation is successful; otherwise false.*/
template <typename T>
bool LinkedQueue<T>::enqueue(const T& newEntry)
{
	Node<T>* newNodePtr = new Node<T>(newEntry);
	// Insert the new node
	if (isEmpty())	//special case if this is the first node to insert
		frontPtr = newNodePtr; // The queue is empty
	else
		backPtr->setNext(newNodePtr); // The queue was not empty

	backPtr = newNodePtr; // New node is the last node now
	count++; // MY72: Incrementing count whenever a new order is enqueued
	return true;
} // end enqueue

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Function: dequeue
Removes the front of this queue. That is, removes the item that was added earliest.
Input: None.
Output: True if the operation is successful; otherwise false.*/
template <typename T>
bool LinkedQueue<T>::dequeue(T& frntEntry)
{
	if (isEmpty())
		return false;

	Node<T>* nodeToDeletePtr = frontPtr;
	frntEntry = frontPtr->getItem();
	frontPtr = frontPtr->getNext();
	// Queue is not empty; remove front
	if (nodeToDeletePtr == backPtr)	 // Special case: last node in the queue
		backPtr = nullptr;

	// Free memory reserved for the dequeued node
	delete nodeToDeletePtr;
	count--; // MY72: Decrementing count whenever an order is served or moved

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/*Function: peek
copies the front of this queue to the passed param. The operation does not modify the queue.
Input: None.
Output: The front of the queue.*/
template <typename T>
bool LinkedQueue<T>::peek(T& frntEntry) const
{
	if (isEmpty())
		return false;

	frntEntry = frontPtr->getItem();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/*Function: getCount
Returns the current number of elements in the queue.
MY72: Added for Phase 1.2 to provide O(1) complexity*/
template <typename T>
int LinkedQueue<T>::getCount() const
{
	return count;
}

/////////////////////////////////////////////////////////////////////////////////////////
/*Function: print
Prints all the items (IDs) currently in the queue.
MY72: Added to visualize the queue state as required in the project*/
template <typename T>
void LinkedQueue<T>::print() const
{
	Node<T>* p = frontPtr;
	while (p)
	{
		std::cout << p->getItem(); // MY72: This works because we overloaded the << operator in Order class
		p = p->getNext();
		if (p) std::cout << ", "; // MY72: Print a comma between IDs but not after the last one
	}
}

///////////////////////////////////////////////////////////////////////////////////
template <typename T>
LinkedQueue<T>::~LinkedQueue()
{
	//Note that the cout statements here is just for learning purpose
	//They should be normally removed from the destructor
	//std::cout << "\nStarting LinkedQueue destructor...";
	//std::cout << "\nFreeing all nodes in the queue...";

	//Free all nodes in the queue
	T temp;
	while (dequeue(temp));

	//std::cout << "\n Is LinkedQueue Empty now?? ==> " << std::boolalpha << isEmpty();
	//std::cout << "\nEnding LinkedQueue destructor..." << std::endl;
}
#endif