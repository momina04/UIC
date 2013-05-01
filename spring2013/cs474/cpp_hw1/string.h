/*
 * =====================================================================================
 *
 *       Filename:  string.h
 *
 *    Description:  Class for string_t
 *
 *        Version:  1.0
 *        Created:  04/05/2013 01:41:10 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#ifndef STRING_H
#define STRING_H

#include <iostream>

using namespace std;

class string_t
{
    private:
        char *value;
        unsigned int size;

        void allocate_and_copy_from_cstring(const char * const);
        void allocate_and_copy_from_string(const string_t &);

    public:
        string_t();
        string_t(const string_t&);
        string_t(const char * const);
        string_t& operator=(const string_t&);
        string_t operator+(const string_t&) const;
        string_t& operator+=(const string_t&);
        bool operator==(const string_t&) const;
        bool operator!=(const string_t&) const;
        bool operator<(const string_t&) const; 
        bool operator<=(const string_t&) const;
        bool operator>(const string_t&) const; 
        bool operator>=(const string_t&) const; 

        operator char*();

        ~string_t();
        friend ostream& operator<<(ostream&, const string_t&);
        friend istream& operator>>(istream&, string_t& );
};

ostream& operator<<(ostream&, const string_t&);
istream& operator>>(istream&, string_t& );

#endif
