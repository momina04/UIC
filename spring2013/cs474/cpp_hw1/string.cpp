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
    strncpy(value, str, size + 1);
}

void String::allocate_and_copy_from_string(const String &str)
{
    size = str.size;
    value = new char[size + 1];
    strncpy(value, str.value, size + 1);
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
    strncpy(result.value, value, size + 1);
    strncat(result.value, str.value, str.size + 1);
    return result;
}

String String::operator+(const char * const str)
{
    String result;
    delete[] result.value;
    result.size = size + strlen(str);
    result.value = new char [result.size + 1];
    strncpy(result.value, value, size + 1);
    strncat(result.value, str, strlen(str) + 1);
    return result;
}

String& String::operator+=(const String& str)
{
    String result = *this + str;
    *this = result;
    return *this;
}

String& String::operator+=(const char * const str)
{
    String result = *this + str;
    *this = result;
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

bool String::operator!=(const String& str)
{
    return !(*this == str);
}

bool String::operator!=(const char * const str)
{
    return !(*this ==  str);
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

bool String::operator<=(const String& str)
{
    if(*this < str || *this == str)
        return true;
    else
        return false;
}

bool String::operator<=(const char * const str)
{
    if(*this < str || *this == str)
        return true;
    else
        return false;
}

bool String::operator>(const String& str)
{
    return !(*this <= str);
}


bool String::operator>(const char * const str)
{
    return !(*this <= str);
}

bool String::operator>=(const String& str)
{
    if(*this > str || *this == str)
        return true;
    else
        return false;
}

bool String::operator>=(const char * const str)
{
    if(*this > str || *this == str)
        return true;
    else
        return false;
}



String::~String()
{
    delete[] value;
}

ostream& operator<<(ostream& cout, const String& str)
{
    cout<<str.value<<str.size;
}

istream& operator>>(istream& cin, const String& str)
{
    cin>>str.value;
}
