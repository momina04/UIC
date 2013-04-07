/*
 * =====================================================================================
 *
 *       Filename:  string.h
 *
 *    Description:  Class for String
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

class String
{
    private:
        char *value;
        int size;

        void allocate_and_copy_from_cstring(const char * const);
        void allocate_and_copy_from_string(const String &);

    public:
        String();
        String(const String&);
        String(const char * const);
        String& operator=(const String&);
        String& operator=(const char * const);
        String operator+(const String&);
        String operator+(const char * const);
        String& operator+=(const String&); //a
        String& operator+=(const char * const); //a
        bool operator==(const String&);
        bool operator==(const char * const);
        bool operator!=(const String&); //a
        bool operator!=(const char * const); //a
        bool operator<(const String&); //a
        bool operator<(const char * const); //a
        bool operator<=(const String&); //a
        bool operator<=(const char * const); //a
        bool operator>(const String&); //a
        bool operator>(const char * const); //a
        bool operator>=(const String&); //a
        bool operator>=(const char * const); //a

        ~String();
        friend ostream& operator<<(ostream&, const String&);
        friend istream& operator>>(istream&, const String& );
};

ostream& operator<<(ostream&, const String&);
istream& operator>>(istream&, const String& );

#endif
