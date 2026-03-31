#ifndef _LINKEDLIST
#define _LINKEDLIST

#include "Node.h"
#include <iostream> // MY72: Added to support cout for print functions

template <typename T>
class LinkedList
{
private:
	Node<T>* Head;	//Pointer to the head of the list
	//You can add tail pointer too (depending on your problem)
	int count; // MY72: Added to keep track of the number of elements in O(1)

public:

	LinkedList()
	{
		Head = nullptr;
		count = 0; // MY72: Initialize count for Phase 1.2
	}

	//List is being desturcted ==> delete all items in the list
	~LinkedList()
	{
		DeleteAll();
	}

	////////////////////////////////////////////////////////////////////////
	/*
	* Function: PrintList.
	* prints the values of all nodes in a linked list.
	*/
	void PrintList()	const
	{
		std::cout << "\nprinting list contents:\n\n";
		Node<T>* p = Head;

		while (p)
		{
			std::cout << "[ " << p->getItem() << " ]";
			std::cout << "--->";
			p = p->getNext();
		}
		std::cout << "NULL\n";
	}

	// MY72: Standard print function as required for all DS in Phase 1.2
	void print() const
	{
		Node<T>* p = Head;
		while (p) {
			std::cout << p->getItem();
			p = p->getNext();
			if (p) std::cout << ", ";
		}
	}

	// MY72: Returns the count of nodes in O(1)
	int getCount() const
	{
		return count;
	}

	////////////////////////////////////////////////////////////////////////
	/*
	* Function: InsertBeg.
	* Creates a new node and adds it to the beginning of a linked list.
	* * Parameters:
	*	- data : The value to be stored in the new node.
	*/
	void InsertBeg(const T& data)
	{
		Node<T>* R = new Node<T>(data);
		R->setNext(Head);
		Head = R;
		count++; // MY72: Increment count on insertion
	}

	////////////////////////////////////////////////////////////////////////
	/*
	* Function: DeleteAll.
	* Deletes all nodes of the list.
	*/
	void DeleteAll()
	{
		Node<T>* P = Head;
		while (Head)
		{
			P = Head->getNext();
			delete Head;
			Head = P;
		}
		count = 0; // MY72: Reset count when list is cleared
	}

	////////////////     Requirements   ///////////////////

	//[1]InsertEnd 
	void InsertEnd(const T& data) {
		Node<T>* newNode = new Node<T>(data);
		if (!Head) {
			Head = newNode;
		}
		else {
			Node<T>* p = Head;
			while (p->getNext()) p = p->getNext();
			p->setNext(newNode);
		}
		count++; // MY72: Increment count
	}

	//[2]Find 
	bool Find(int Key) {
		Node<T>* p = Head;
		while (p) {
			if (p->getItem() == Key) return true; // MY72: Assumes T can be compared to int
			p = p->getNext();
		}
		return false;
	}

	//[3]CountOccurance
	int CountOccurance(const T& value) {
		int occ = 0;
		Node<T>* p = Head;
		while (p) {
			if (p->getItem() == value) occ++;
			p = p->getNext();
		}
		return occ;
	}

	//[4] DeleteFirst
	void DeleteFirst() {
		if (Head) {
			Node<T>* p = Head;
			Head = Head->getNext();
			delete p;
			count--; // MY72: Decrement count
		}
	}

	//[5] DeleteLast
	void DeleteLast() {
		if (!Head) return;
		if (!Head->getNext()) {
			delete Head;
			Head = nullptr;
		}
		else {
			Node<T>* p = Head;
			while (p->getNext()->getNext()) p = p->getNext();
			delete p->getNext();
			p->setNext(nullptr);
		}
		count--; // MY72: Decrement count
	}

	//[6] DeleteNode
	bool DeleteNode(const T& value) {
		if (!Head) return false;
		if (Head->getItem() == value) {
			DeleteFirst();
			return true;
		}
		Node<T>* p = Head;
		while (p->getNext()) {
			if (p->getNext()->getItem() == value) {
				Node<T>* toDelete = p->getNext();
				p->setNext(toDelete->getNext());
				delete toDelete;
				count--; // MY72: Decrement count
				return true;
			}
			p = p->getNext();
		}
		return false;
	}

	//[7] DeleteNodes (Deletes ALL occurrences)
	bool DeleteNodes(const T& value) {
		bool deleted = false;
		while (Head && Head->getItem() == value) {
			DeleteFirst();
			deleted = true;
		}
		Node<T>* p = Head;
		while (p && p->getNext()) {
			if (p->getNext()->getItem() == value) {
				Node<T>* toDelete = p->getNext();
				p->setNext(toDelete->getNext());
				delete toDelete;
				count--; // MY72: Decrement count
				deleted = true;
			}
			else {
				p = p->getNext();
			}
		}
		return deleted;
	}

	//[8]Merge
	void Merge(const LinkedList& L) {
		if (!L.Head) return;
		if (!Head) {
			Head = L.Head; // MY72: Basic merge logic
		}
		else {
			Node<T>* p = Head;
			while (p->getNext()) p = p->getNext();
			p->setNext(L.Head);
		}
		count += L.count; // MY72: Update total count
	}

	//[9] Reverse
	void Reverse() {
		Node<T>* prev = nullptr, * curr = Head, * next = nullptr;
		while (curr) {
			next = curr->getNext();
			curr->setNext(prev);
			prev = curr;
			curr = next;
		}
		Head = prev;
	}

};

#endif