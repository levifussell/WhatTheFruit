#include <Windows.h>

#pragma once
template <class T> class List
{
public:
	int length;

	//Constructor that sets first node to NULL and length to zero
	List()
	{
		first = NULL;
		length = 0;
	}
	//Deconstructor that deletes all memory from list (includes nodes and data)
	~List()
	{
		Node *q;

		if(first == NULL)
			return;

		while(first != NULL)
		{
			q = first->next;
			//delete first->data;
			delete first;
			first = q;
		}
	}
	//Adds a node and data to the back of the list, pointing to NULL
	void AddBack(T dataIn)
	{
		//Make two node
		Node *q, *t;

		//If first node is NULL, set the data to the first node
		if(first == NULL)
		{
			first = new Node;
			first->data = dataIn;
			first->next = NULL;
		}
		//Else, itterate over list until the last node is found 
		// and place a new node at end
		else
		{
			q = first;
			//Itterate over list to last node
			while(q->next != NULL)
				q = q->next;

			//Place new node at end
			t = new Node;
			t->data = dataIn;
			t->next = NULL;
			q->next = t;
		}
		//Extend length by 1
		length++;
	}
	//Returns the data of a specified index in the list
	T Get(int index)
	{
		Node *q;
		int count = 0;
		//Itterate over list to last node
		for(q = first; q != NULL; q = q->next)
		{
			//If specified index and count match, return data of that node, 
			// otherwise leave scope if count exceed list length
			if(count == index)
				return q->data;
			//Add to count index
			count++;
		}
	}
	//Remove node based on specified index location
	void RemoveAt(int index)
	{
		Node *q, *t;
	
		//Special case if deleted value is 0
		q = first;
		if(index == 0)
		{
			//Link nodes adjacent to deleted node
			first = q->next;
			delete q;
			//Decrease length by 1
			length--;
			return;
		}

		//Else look through other values
		t = q;
		int count = 0;
		//Itterate over list until at end
		for(q = first; q != NULL; q = q->next)
		{
			//If count is equal to specified index, return node's data
			if(count == index)
			{
				//Link nodes adjacent to deleted node
				t->next = q->next;
				//delete q->data;
				delete q;
				//deccrease length by 1
				length--;
				return;
			}
			t = q;
			count++;
		}
	}
	//Remove  node based on given data
	void Remove(T dataIn)
	{
		//Create 2 temporary nodes
		Node *q, *t;
	
		//Special case if deleted value is 0
		q = first;
		if(q->data == dataIn)
		{
			//Link nodes adjacent to deleted node
			first = q->next;
			delete q;
			//Decrease length by 1
			length--;
			return;
		}

		t = q;
		//Itterate over nodes
		for(q = first; q != NULL; q = q->next)
		{
			//If data equals node, select node and delete
			if(q->data == dataIn)
			{
				//Link nodes adjacent to deleted node
				t->next = q->next;
				//delete q->data;
				delete q;
				//Decrease length by 1
				length--;
				return;
			}
			t = q;
		}
	}

private:
	//Node struct contains the node it links to (equals NULL if at end)
	// and specified data
	struct Node
	{
		T data;
		Node *next;
	};

	Node *first;
};

