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
void String::allocate_and_copy_from_cstring(const char * const str)
{
    size = strlen(str);
    value = new char[size + 1];
    strncpy(value, str, size); /* Does not make sense to use strncpy if str is not NULL terminated. since size will be detected using a NULL character */
    value[size] = '\0';
}

void String::allocate_and_copy_from_string(const String &str)
{
    size = str.size;
    value = new char[size + 1];
    strncpy(value, str.value, size);
    value[size] = '\0';
}

String::String()
{
    value = new char[1];
    size = 0;
    value[0] = '\0';
}

String::String(const String& str)
{
    allocate_and_copy_from_string(str);
}

String::String(const char * const str)
{
    allocate_and_copy_from_cstring(str);
}


String& String::operator=(const String& str)
{
    delete[] value;
    allocate_and_copy_from_string(str);
    return *this;
}

String& String::operator=(const char * const str)
{
    delete[] value;
    allocate_and_copy_from_cstring(str);
    return *this;
}

String String::operator+(const String& str)
{
    String result;
    delete[] result.value;
    result.size = size + str.size;
    result.value = new char [result.size + 1];
    strncpy(result.value, value, size);
    result.value[size] = '\0';
    strncat(result.value, str.value, str.size); 
    return result;
}

String String::operator+(const char * const str)
{
    String result;
    delete[] result.value;
    result.size = size + strlen(str);
    result.value = new char [result.size + 1];
    strncpy(result.value, value, size);
    result.value[size] = '\0';
    strncat(result.value, str, strlen(str)); /* Does not make sense to use strncat if str is not NULL terminated. since size will be detected using a NULL character */
    return result;
}

#define PLUS_EQUALS(dest,src) dest = dest + src

String& String::operator+=(const String& str)
{
    PLUS_EQUALS(*this, str);
    return *this;
}

String& String::operator+=(const char * const str)
{
    PLUS_EQUALS(*this, str);
    return *this;
}

bool String::operator==(const String& str)
{
    if(size == str.size && strncmp(value,str.value,size)==0)
    {
        return true;
    }
    return false;
}

bool String::operator==(const char * const str)
{
    if(size == strlen(str) && strncmp(value,str,size)==0)
    {
        return true;
    }
    return false;
}

#define NOT_EQUAL(a,b) !(a == b)

bool String::operator!=(const String& str)
{
    return NOT_EQUAL(*this,str);
}

bool String::operator!=(const char * const str)
{
    return NOT_EQUAL(*this,str);
}

bool String::operator<(const String& str)
{
    if(strncmp(value,str.value,(size>str.size?size:str.size)) < 0)
    {
        return true;
    }
    return false;

}

bool String::operator<(const char * const str)
{
    if(strncmp(value,str,(size>strlen(str)?size:strlen(str))) < 0)
    {
        return true;
    }
    return false;
}

#define LESS_EQUAL(a,b) (a<b || a ==b)

bool String::operator<=(const String& str)
{
    return LESS_EQUAL(*this,str);
}

bool String::operator<=(const char * const str)
{
    return LESS_EQUAL(*this,str);
}

#define GREATER(a,b) !(a<=b)
bool String::operator>(const String& str)
{
    return GREATER(*this,str);
}

bool String::operator>(const char * const str)
{
    return GREATER(*this,str);
}

#define GREATER_EQUAL(a,b) (a>b || a==b)
bool String::operator>=(const String& str)
{
    return GREATER_EQUAL(*this, str);
}

bool String::operator>=(const char * const str)
{
    return GREATER_EQUAL(*this, str);
}


String::~String()
{
    delete[] value;
}

ostream& operator<<(ostream& cout, const String& str)
{
    cout<<str.value<<str.size;
    return cout;
}

istream& operator>>(istream& cin, String& str)
{
    cin>>str.value;
    str.size = strlen(str.value);
    return cin;
}
