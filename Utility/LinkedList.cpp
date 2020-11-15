///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor     
// Purpose:   LinkedList object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <TChar.h>
#include "LinkedList.h"

template <class T>
LinkedList<T>::LinkedList()
{
	head = NULL;
	iLength = 0;
}

template <class T>
LinkedList<T>::~LinkedList()
{
    clear();
	//while(iLength > 0)
	//	remove(iLength);
	return;
}

/**
  * Removes all the items in the list. NOTE: modified from the
  * port because C++ doesn't do automatic garbage collection.
  */
template <class T>
void LinkedList<T>::clear()
{
    while(iLength)
        remove(iLength);
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
BOOL LinkedList<T>::add(T *entry)
{
    // empty list is handled differently from a non-empty list
    if (isEmpty())
		head = new node<T>(entry);
    else
    {
        node<T> *lastNode = getNodeAt(iLength);
        lastNode->next = new node<T>(entry);
    }
    iLength++;
    return TRUE;
}

/**
 * Gets the node at the specified position. Remember, the list
 * is zero based while positions specified by the user are
 * assumed to be 1 based.
 *
 * @param iPosition ordinal value of the item in the list
 * being requested
 * @return the Node object which resides at the specified
 * position in the list if successful, otherwise null is
 * returned
 */
template <class T>
node<T>* LinkedList<T>::getNodeAt(int position)
{
    if (isEmpty() || position < 1 || position > iLength)
		return NULL;

    node<T> *cNode = head;

    for (int i = 1; i < position; i++)
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
BOOL LinkedList<T>::add(int newPosition, T *newEntry)
{
    if (newPosition < 1 || newPosition > iLength + 1)
		return FALSE;

    node<T> *tempNode = new node<T>(newEntry);

    // adding to the beginning of the list a special affair
    if(isEmpty() || newPosition == 1)
    {
        tempNode->next = this->head;
        this->head = tempNode;
    }
    else
    {
        node<T> *before = getNodeAt(newPosition - 1);
        node<T> *after = before->next;
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
BOOL LinkedList<T>::contains(T entry)
{
    for (node<T> cNode = head; cNode != NULL; cNode = cNode.next)
        if (cNode.data.equals(entry))
            return TRUE;

    return FALSE;
}

/**
 * Display all entries in the list (in order in which they
 * occur).
 */
template <class T>
void LinkedList<T>::display()
{

    node<T> *cNode = head;
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
T* LinkedList<T>::getEntry(int position)
{
    if (isEmpty() || position < 1 || position > iLength)
        return NULL;

    return getNodeAt(position)->data;
}

/**
 * Returns the current number of items in the list
 *
 * @return The number of items in the list
 */
template <class T>
int LinkedList<T>::getLength()
{
    return iLength;
}

/**
 * Returns whether list is empty or not
 *
 * @return TRUE if the list is empty, FALSE otherwise
 */
template <class T>
BOOL LinkedList<T>::isEmpty()
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
BOOL LinkedList<T>::isFull()
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
T* LinkedList<T>::remove(int position)
{
    if (isEmpty() || position < 1 || position > iLength)
        return NULL;

    T* result = NULL;

    if (position == 1)
    {
        result = head->data;
        head = head->next;
    }
    else
    {
        node<T> *before = getNodeAt(position - 1);
        node<T> *remove = before->next;
        node<T> *after = remove->next;

        result = remove->data;
        before->next = after;
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
BOOL LinkedList<T>::replace(int position, T *newEntry)
{
    if(isEmpty() || position < 1 || position > iLength)
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
BOOL LinkedList<T>::swap(int second)
{
    node<T> *before;
    node<T> *one;
    node<T> *two;

    if (isEmpty() || second < 1 || second > iLength)
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
BOOL LinkedList<T>::swap(int one, int two)
{
    if (isEmpty() || one < 1 || one > iLength || two < 1 || two > iLength)
        return FALSE;

    node<T> *tempOne = getNodeAt(one);
    node<T> *tempTwo = getNodeAt(two);
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
BOOL LinkedList<T>::cycle()
{
    if (isEmpty())
        return FALSE;

    node<T> *last = getNodeAt(iLength);
    node<T> *first = head;

    head = head->next;
    last->next = first;
    first->next = NULL;
    return TRUE;
}

/**
 * Reverses the order of the list.
 */
template <class T>
VOID LinkedList<T>::reverse()
{
    node<T> *start = head;
    node<T> *next = NULL;
    node<T> *end = NULL;

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
int LinkedList<T>::getPosition (T Item)
{
    node<T> *nLoop = head;  // item being searched for,
    int iPosition;

    // loop through list
    for(iPosition = 1; nLoop != NULL;
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