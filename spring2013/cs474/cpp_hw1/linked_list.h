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

using namespace std;

template <class item_t>
class linked_list_t{
    public:
        class node_t
        {
            private:
                const item_t &_val;
                node_t *_next;
            public:
                node_t(const item_t& item) { _val = item; _next = NULL; }
                item_t& val() { return _val;}
                node_t * next() { return _next;}
                void next(const node_t * const n) {_next = n;}
                ~node_t() {delete &val;}
        };
    private:
        unsigned int size;
        node_t *first_node;

    public:
        linked_list_t();
        linked_list_t(const linked_list_t &);
        item_t* search(const item_t &);

        bool empty();
        void add_first(const item_t &);
        void add_priority(const item_t &);
        bool remove_first();
        bool remove(const item_t &);
        ~linked_list_t();

        friend ostream& operator<<(ostream&, const linked_list_t&);
};


template <class item_t>
ostream& operator<<(ostream&, const linked_list_t<item_t>&);

#endif
