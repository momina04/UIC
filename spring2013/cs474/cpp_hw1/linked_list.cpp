/*
 * =====================================================================================
 *
 *       Filename:  linked_list.cpp
 *
 *    Description:  Implementation of Linked List that stores generic object
 *
 *        Version:  1.0
 *        Created:  04/07/2013 05:25:06 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#ifndef LINKED_LIST_CPP
#define LINKED_LIST_CPP

#include "linked_list.h"
#include <cstddef>
#include <assert.h>

using namespace std;

template <class item_t>
unsigned int linked_list_t<item_t>::get_size()
{
    return size;
}

template <class item_t>
linked_list_t<item_t>::linked_list_t()
{
    first_node = NULL;
    size = 0;
}

template <class item_t>
linked_list_t<item_t>::linked_list_t(const linked_list_t &list)
{
    node_t *curr = NULL;
    first_node = NULL;
    size = 0;
    curr = list.first_node;
    while(curr){
        this->add_priority(curr->val());
        //this->add_first(curr->val());
        curr = curr -> next();
    }
    size = list.size;
}

template <class item_t>
const item_t* linked_list_t<item_t>::search(const item_t &item) const
{
    node_t *curr = NULL;
    curr = first_node;
    while(curr){
        if(item == curr->val())
            return &(curr->val());
        curr = curr -> next();
    }
    return NULL;
}

template <class item_t>
void linked_list_t<item_t>::deleteAll()
{
    const item_t *item;
    while(!empty())
    {
        item = (*this)[0];
        remove_first();
        delete item;
    }
}

template <class item_t>
inline bool linked_list_t<item_t>::empty()
{
    return (size == 0);
}


template <class item_t>
void linked_list_t<item_t>::add_first(const item_t &item)
{
    node_t *new_node;
    new_node = new node_t(item);
    new_node->next(first_node);
    first_node = new_node;
    size++;
}

template <class item_t>
void linked_list_t<item_t>::add_priority(const item_t &item)
{
    node_t *new_node;
    node_t *curr = NULL;
    node_t *prev = NULL;
    
    if(size==0 || item < (first_node->val())){
        add_first(item);
        return;
    }

    new_node = new node_t(item);

    prev = first_node;
    curr = first_node->next();

    while(curr && curr->val() <= item){
        prev = curr;
        curr = curr -> next();
    }

    new_node->next(prev->next());
    prev->next(new_node);
    size++;
}

template <class item_t>
bool linked_list_t<item_t>::remove_first()
{
    node_t *tmp;
    if(empty()) return false;
    tmp = first_node;
    first_node = first_node->next();
    delete tmp;
    size--;
    return true;
}

template <class item_t>
bool linked_list_t<item_t>::remove(const item_t &item)
{
    node_t *curr = NULL;
    node_t *prev = NULL;

    if(empty()) return false;

    if(item == first_node->val()){
        remove_first();
        return true;
    }

    prev = first_node;
    curr = first_node->next();

    while(curr){
        if(item == curr->val())
        {
            prev->next(curr->next());
            delete curr;
            size--;
            return true;
        }
        prev = curr;
        curr = curr -> next();
    }
    return false;
}

template <class item_t>
linked_list_t<item_t>::~linked_list_t()
{
    while(!empty())
    {
        remove_first();
    }
}

template <class item_t>
const item_t* linked_list_t<item_t>::operator[](unsigned int idx)
{
    node_t *curr = NULL;
    if(idx>=size) return NULL;
    curr = first_node;
    for(unsigned int i=0; i<idx; i++){
        assert(curr!=NULL);
        curr=curr->next();
    }
    return &curr->val();
}

template <class item_t>
ostream& operator<<(ostream& cout, const linked_list_t<item_t>& list)
{
    typename linked_list_t<item_t>::node_t *curr;
    curr = list.first_node;
    cout<<"size = "<<list.size<<" : ";
    while(curr){
        cout<<curr->val()<<" -> ";
        curr = curr->next();
    }
    cout<<"NULL.";
    return cout;
}

#endif
