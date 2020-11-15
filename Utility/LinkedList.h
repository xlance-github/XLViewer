#ifndef _LINKEDLIST_
#define _LINKEDLIST_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor     
// Purpose:   LinkedList object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <string>

/**
 * Models a node, that will wrap around the data and make it part of a
 * linked list.
 *
 * @author Chad R. Hearn
 * @version
 */
template <class T>
class node
{
    public:
        /**
         * Constructor which accepts data for the node. This
         * constructor should be used to create a single-item
         * list
         *
         * @param data Object/data that is to be stored in
         * this node
         */
        node (T *newEntry)
        {
            data = newEntry;
            next = NULL;
        }

        /**
         * Constructor which accepts data for the node and a
         * "position." This constructor should be used to insert
         * a node into an existing list (basically).
         *
         * @param data Object/data that is to be stored in
         * this node
         * @param nextNode pointer to next node in the list
         */
        node (T *newEntry, node nextnode)
        {
            data = newEntry;
            next = &nextnode;
        }
		~node ()
		{
			data = NULL;
			next = NULL;
		}

    T *data;
    node *next; // Pointer to next node
};

// LinkedList object definition
template <class T>
class LinkedList
{
protected:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	int iLength;
	node<T> *head;
		
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////
    
	/**
	 *
	 */
	node<T>* getNodeAt(int);
		
public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	LinkedList();

	/**
	 * Destructor, performs clean-up.
	 */
	~LinkedList();
	
	///////////////////////////////////////////////////////////////////////////
	// Accessor Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 *
	 */
	int  getLength();

	/**
	 *
	 */
	int  getPosition(T);

	/**
	 *
	 */
	T*   getEntry(int);

	/**
	 *
	 */
	BOOL isEmpty();

	/**
	 *
	 */
	BOOL isFull();

	/**
	 *
	 */
	BOOL contains (T);
		
	///////////////////////////////////////////////////////////////////////////
	// Mutator Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 *
	 */
	BOOL add(T*);

	/**
	 *
	 */
	BOOL add(int, T*);

	/**
	 *
	 */
	VOID clear();

	/**
	 *
	 */
	T*   remove(int);

	/**
	 *
	 */
	BOOL replace(int, T*);	

	/**
	 *
	 */
	BOOL swap(int);

	/**
	 *
	 */
	BOOL swap(int, int);

	/**
	 *
	 */
	BOOL cycle();

	/**
	 *
	 */
	VOID reverse();	

	///////////////////////////////////////////////////////////////////////////
	// Display Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 *
	 */
    VOID display();		
};

#endif // End _LINKEDLIST_
