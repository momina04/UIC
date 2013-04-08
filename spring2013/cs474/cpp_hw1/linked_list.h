/*
 * =====================================================================================
 *
 *       Filename:  linked_list.h
 *
 *    Description:  Linked List that stores generic objects
 *
 *        Version:  1.0
 *        Created:  04/07/2013 05:24:44 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstddef>
#include <iostream>
#include "string.h"

using namespace std;

template <class item_t>
class linked_list_t{
        class node_t
        {
            private:
                const item_t &_val;
                node_t *_next;
            public:
                node_t(const item_t& item):_val(item) { _next = NULL; }
                const item_t& val() { return _val;}
                node_t * next() { return _next;}
                void next(node_t *node) {_next = node;}
                ~node_t() {}
        }*first_node;
        unsigned int size;

    public:
        linked_list_t();
        linked_list_t(const linked_list_t &);
        const item_t* search(const item_t &);

        bool empty();
        void add_first(const item_t &);
        void add_priority(const item_t &);
        bool remove_first();
        bool remove(const item_t &);
        ~linked_list_t();

        const item_t* operator[](unsigned int idx);

        template<class T>
        friend ostream& operator<< (ostream&, const linked_list_t <T>&);
};


template <class item_t>
ostream& operator<<(ostream&, const linked_list_t<item_t>&);

template class linked_list_t <int>;
//template class linked_list_t <string_t>;

#endif
