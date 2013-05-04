/*
 * =====================================================================================
 *
 *       Filename:  string.cpp
 *
 *    Description:  Implementation for string class
 *
 *        Version:  1.0
 *        Created:  04/05/2013 02:30:14 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include "string.h"
#include <string.h>
void string_t::allocate_and_copy_from_cstring(const char * const str)
{
    size = strlen(str);
    value = new char[size + 1];
    strncpy(value, str, size); /* Does not make sense to use strncpy if str is not NULL terminated. since size will be detected using a NULL character */
    value[size] = '\0';
}

void string_t::allocate_and_copy_from_string(const string_t &str)
{
    size = str.size;
    value = new char[size + 1];
    strncpy(value, str.value, size);
    value[size] = '\0';
}

string_t::string_t()
{
    value = new char[1];
    size = 0;
    value[0] = '\0';
}

string_t::string_t(const string_t& str)
{
    allocate_and_copy_from_string(str);
}

string_t::string_t(const char * const str)
{
    allocate_and_copy_from_cstring(str);
}


string_t& string_t::operator=(const string_t& str)
{
    delete[] value;
    allocate_and_copy_from_string(str);
    return *this;
}


string_t string_t::operator+(const string_t& str) const
{
    string_t result;
    delete[] result.value;
    result.size = size + str.size;
    result.value = new char [result.size + 1];
    strncpy(result.value, value, size);
    result.value[size] = '\0';
    strncat(result.value, str.value, str.size); 
    return result;
}


#define PLUS_EQUALS(dest,src) dest = dest + src

string_t& string_t::operator+=(const string_t& str)
{
    PLUS_EQUALS(*this, str);
    return *this;
}


bool string_t::operator==(const string_t& str) const
{
    if(size == str.size && strncmp(value,str.value,size)==0)
    {
        return true;
    }
    return false;
}

#define NOT_EQUAL(a,b) !(a == b)

bool string_t::operator!=(const string_t& str) const
{
    return NOT_EQUAL(*this,str);
}


bool string_t::operator<(const string_t& str) const
{
    if(strncmp(value,str.value,(size>str.size?size:str.size)) < 0)
    {
        return true;
    }
    return false;

}


#define LESS_EQUAL(a,b) (a<b || a ==b)

bool string_t::operator<=(const string_t& str) const
{
    return LESS_EQUAL(*this,str);
}

#define GREATER(a,b) !(a<=b)
bool string_t::operator>(const string_t& str) const
{
    return GREATER(*this,str);
}


#define GREATER_EQUAL(a,b) (a>b || a==b)
bool string_t::operator>=(const string_t& str) const
{
    return GREATER_EQUAL(*this, str);
}

const string_t & string_t::vcopy() const
{
    //return *(new string_t(*this));
    return *this;
}

//string_t::operator char*()
//{
//    char *ret_str;
//    ret_str = new char[size + 1];
//    strncpy(ret_str, value, size); /* Does not make sense to use strncpy if str is not NULL terminated. since size will be detected using a NULL character */
// ret_str[size] = '\0';
//    return ret_str; /* warning: the client should deallocate the memory to avoid leaks. */
//}

string_t::~string_t()
{
    delete[] value;
}

ostream& operator<<(ostream& cout, const string_t& str)
{
    cout<<str.value;
    return cout;
}

istream& operator>>(istream& cin, string_t& str)
{
    char tmp[256];
    cin>>tmp;
    str=tmp;
    return cin;
}
