#ifndef _INC_LINKEDLIST_
#define _INC_LINKEDLIST_

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#ifndef _INC_ASSERT
#include <assert.h>
#endif

template <class T> class nodeEx;

/**
  * Linked List ADT. Imported from Java (CS 151)
  *
  * @author Chad R. Hearn
  * @version 2006
  *
  */
template <class T>
class LinkedListEx
{
    public:
        LinkedListEx();
        ~LinkedListEx();

        BOOL add      (T*);
        BOOL add      (int, T*);
        T*   remove   (int);
        BOOL replace  (int, T*);

        int  getLength();
        int  getPosition(T);
        T*   getEntry (int);
        VOID clear    ();
        BOOL isEmpty  ();
        BOOL isFull   ();
        BOOL contains (T);
		BOOL getAutoDestroy() {return bAutoDestroyItems;}
		VOID setAutoDestroy(BOOL bNewAutoDestroy) 
		{
			bAutoDestroyItems = bNewAutoDestroy;
		}

        BOOL swap     (int);
        BOOL swap     (int, int);
        BOOL cycle    ();
        VOID reverse  ();
        VOID display  ();

		// added 08-08-2008 to asuage my stress
		T* GetItem() { return current->data; }
		BOOL EndOfList();
		void MoveToStart();
		void operator++();

    private:

        nodeEx<T>* getNodeAt(int);

    protected:
        int     iLength;
		BOOL	bAutoDestroyItems;
		nodeEx<T> *head;
		nodeEx<T> *current;
};

template <class T>
LinkedListEx<T>::LinkedListEx()
{
	head = NULL;
	current = head;
	iLength = 0;
	bAutoDestroyItems = TRUE;
}

template <class T>
LinkedListEx<T>::~LinkedListEx()
{
    clear();
	//while(iLength > 0)
	//	remove(iLength);
	return;
}

template <class T>
void LinkedListEx<T>::MoveToStart()
{
	current = head;
}

template <class T>
BOOL LinkedListEx<T>::EndOfList()
{
	return (current == NULL ? TRUE : FALSE);
}

template <class T>
void LinkedListEx<T>::operator++()
{
	current = current->next;
}

/**
 * Models a nodeEx, that will wrap around the data and make it part of a
 * linked list.
 *
 * @author Chad R. Hearn
 * @version
 */
template <class T>
class nodeEx
{
    public:
        /**
         * Constructor which accepts data for the nodeEx. This
         * constructor should be used to create a single-item
         * list
         *
         * @param data Object/data that is to be stored in
         * this nodeEx
         */
        nodeEx (T *newEntry)
        {
            data = newEntry;
            next = NULL;
        }

        /**
         * Constructor which accepts data for the nodeEx and a
         * "position." This constructor should be used to insert
         * a nodeEx into an existing list (basically).
         *
         * @param data Object/data that is to be stored in
         * this nodeEx
         * @param nextNode pointer to next nodeEx in the list
         */
        nodeEx (T *newEntry, nodeEx nextnode)
        {
            data = newEntry;
            next = &nextnode;
        }
		~nodeEx ()
		{
			data = NULL;
			next = NULL;
		}

    T *data;
    nodeEx *next; // Pointer to next nodeEx
};

/**
  * Removes all the items in the list. NOTE: modified from the
  * port because C++ doesn't do automatic garbage collection.
  */
template <class T>
void LinkedListEx<T>::clear()
{
	T* temp = NULL;

	while(iLength)
	{
		temp = remove(0);
		if(temp && bAutoDestroyItems)
        	delete temp;
	}
	head = NULL;
}

/**
 * Adds a new item to the end of a list.
 * Current elements in the list are unaffected, starting
 * position is 1
 * @param newEntry object to add to the list
 * @return true if the object was added, false is the list was
 * full
 */
template <class T>
BOOL LinkedListEx<T>::add(T *entry)
{
    // empty list is handled differently from a non-empty list
    if (isEmpty())
		head = new nodeEx<T>(entry);
    else
    {
        nodeEx<T> *lastNode = getNodeAt(iLength - 1);
        lastNode->next = new nodeEx<T>(entry);
    }
    iLength++;
    return TRUE;
}

/**
 * Gets the nodeEx at the specified position. Remember, the list
 * is zero based while positions specified by the user are
 * assumed to be 1 based.
 *
 * @param iPosition ordinal value of the item in the list
 * being requested
 * @return the nodeEx object which resides at the specified
 * position in the list if successful, otherwise null is
 * returned
 */
template <class T>
nodeEx<T>* LinkedListEx<T>::getNodeAt(int position)
{
    if (isEmpty() || position < 0 || position > iLength - 1)
		return NULL;

    nodeEx<T> *cNode = head;

    for (int i = 0; i < position; i++)
        cNode = cNode->next;

    assert(cNode != NULL);
    return cNode;
}

/**
 * Purpose: Add a new entry to the list at a specified position
 * in the list.
 * Entries currently in list at specified position (and higher)
 * are all shifted one position higher in the list.
 * @param newPosition position to insert new entry
 * @param newEntry entry to add to the list
 * @return true if the add was successful, false if the list was
 * full OR newPosition < 1 || newPosition > getLength() + 1
 */
template <class T>
BOOL LinkedListEx<T>::add(int newPosition, T *newEntry)
{
    if (newPosition < 0 || newPosition > iLength - 1)
		return FALSE;

    nodeEx<T> *tempNode = new nodeEx<T>(newEntry);

    // adding to the beginning of the list a special affair
    if(isEmpty() || newPosition == 0)
    {
        tempNode->next = this->head;
        this->head = tempNode;
    }
    else
    {
        nodeEx<T> *before = getNodeAt(newPosition - 1);
        nodeEx<T> *after = before->next;
        tempNode->next = after;
        before->next = tempNode;
    }
    iLength++;
    return TRUE;
}

/**
  * Purpose: Determine if a list contains a specified entry
  * @param entry the object thatis the desired entry
  * @return TRUE if the list contains the entry of interest or FALSE if not
  * Precondition: The object T contains a valid <i>equals</i> method.
  */
template <class T>
BOOL LinkedListEx<T>::contains(T entry)
{
    for (nodeEx<T> cNode = head; cNode != NULL; cNode = cNode.next)
        if (cNode.data.equals(entry))
            return TRUE;

    return FALSE;
}

/**
 * Display all entries in the list (in order in which they
 * occur).
 */
template <class T>
void LinkedListEx<T>::display()
{

    nodeEx<T> *cNode = head;
    for (int i = 0; i <= iLength; i++)
    {
        if (cNode != NULL)
        {
            printf("\t%s\n", cNode->data->toString());
			delete cNode->data->toString();
            cNode = cNode->next;
        }
    }
}

/**
 * Purpose: Retrieves the entry at the specified position from the list.
 * The entry is NOT removed from the list.
 * @param position position of entry of interest
 * @return  a reference to the desired entry, or null if the list is empty
 * or position < 1 or > getLength() + 1
 */
template <class T>
T* LinkedListEx<T>::getEntry(int position)
{
    if (isEmpty() || position < 0 || position > iLength - 1)
        return NULL;

    return getNodeAt(position)->data;
}

/**
 * Returns the current number of items in the list
 *
 * @return The number of items in the list
 */
template <class T>
int LinkedListEx<T>::getLength()
{
    return iLength;
}

/**
 * Returns whether list is empty or not
 *
 * @return TRUE if the list is empty, FALSE otherwise
 */
template <class T>
BOOL LinkedListEx<T>::isEmpty()
{
    if (iLength == 0)
    {
        // this has better be TRUE, orsomething funky has happened
        assert(head == NULL);
        return TRUE;
    }
    assert(head != NULL);
    return FALSE;
}

/**
 * Returns whether the list is full or not
 *
 * @return TRUE if the list is full, FALSE otherwise
 */
template <class T>
BOOL LinkedListEx<T>::isFull()
{
    return FALSE;
}

/**
 * Removes an entry from the list. Entries Higher in the list are
 * all shifted one position lower in the list
 * @param position position of entry in the list to remove
 * @return a reference to the removed entry (object) or null if:
 * the list was empty or position < 1 or
 * position > getLength() + 1
 */
template <class T>
T* LinkedListEx<T>::remove(int position)
{
    if (isEmpty() || position < 0 || position > iLength - 1)
		return NULL;

    T* result = NULL;

    if(position == 0)
	{
		nodeEx<T> *ndNext = NULL;

		result = head->data;
		ndNext = head->next;

		delete head;
		head = ndNext;
    }
    else
    {
        nodeEx<T> *before = getNodeAt(position - 1);
        nodeEx<T> *remove = before->next;
        nodeEx<T> *after = remove->next;

        result = remove->data;
		before->next = after;

    	// garbage collect
		if(remove)
			delete remove;
    }
    iLength--;
	return result;
}

/**
 * Purpose: replace the entry at the given position in the list.
 * @param position position of the entry to replace
 * @param newEntry object to replace enrty at specified position
 * with
 * @return true if the replacement was succesful, false is the
 * list was empty or position < 1 or position > getLength() + 1
 */
template <class T>
BOOL LinkedListEx<T>::replace(int position, T *newEntry)
{
    if(isEmpty() || position < 0 || position > iLength - 1)
		return FALSE;

    getNodeAt(position)->data = newEntry;
    return TRUE;
}

/**
 * Swaps the item at the specified position with the list item
 * preceeding it
 *
 * @param second list item being swapped with preceeding item
 * @return TRUE if the swap is successful, otherwise FALSE
 */
template <class T>
BOOL LinkedListEx<T>::swap(int second)
{
    nodeEx<T> *before;
    nodeEx<T> *one;
    nodeEx<T> *two;

    if (isEmpty() || second < 0 || second > iLength - 1)
        return FALSE;
    else
    {
        two = getNodeAt(second);
        one = getNodeAt(second - 1);

        if (head == one)
        {
            one->next = two->next;
            two->next = one;
            head = two;
        }
        else
        {
            before = getNodeAt(second - 2);
            before->next = two;
            one->next = two->next;
            two->next = one;
        }
    }
    return TRUE;
}

/**
 * Swaps the item at position "one" with the item at position
 * "two"
 *
 * @param one first item of the pair being swapped
 * @param two second item of the pair being swapped
 * @return TRUE if the items are successfully swapped, otherwise
 * FALSE
 */
template <class T>
BOOL LinkedListEx<T>::swap(int one, int two)
{
    if (isEmpty() || one < 0 || one > iLength - 1 || two < 0 || two > iLength - 1)
        return FALSE;

    nodeEx<T> *tempOne = getNodeAt(one);
    nodeEx<T> *tempTwo = getNodeAt(two);
    T *temp = tempOne->data;

    tempOne->data = tempTwo->data;
    tempTwo->data = temp;
    return TRUE;
}

/**
 * Moves the first item in the list to the end (cycles
 * the list)
 *
 * @return TRUE if successful, otherwise FALSE
 */
template <class T>
BOOL LinkedListEx<T>::cycle()
{
    if (isEmpty())
        return FALSE;

    nodeEx<T> *last = getNodeAt(iLength);
    nodeEx<T> *first = head;

    head = head->next;
    last->next = first;
    first->next = NULL;
    return TRUE;
}

/**
 * Reverses the order of the list.
 */
template <class T>
VOID LinkedListEx<T>::reverse()
{
    nodeEx<T> *start = head;
    nodeEx<T> *next = NULL;
    nodeEx<T> *end = NULL;

    while (start != NULL)
    {
        next = start->next;
        start->next = end;
        end = start;
        start = next;
    }
    head = end;
}

/**
 * Returns the ordinal position of the item specified within the
 * list.
 *
 * @param Item Object which is to be located within the list
 * @return The ordinal position of the specified object within
 * the list if successful, otherwise 0 (zero) is returned
 */
template <class T>
int LinkedListEx<T>::getPosition (T Item)
{
    nodeEx<T> *nLoop = head;  // item being searched for,
    int iPosition;

    // loop through list
    for(iPosition = 0; nLoop != NULL;
        nLoop = nLoop->next,
        iPosition++)
    {
		printf("Begin loop\n");
        if(nLoop->data->equals(Item))
            return iPosition;
		printf("End loop\n");
    }

    return 0;
}

#endif