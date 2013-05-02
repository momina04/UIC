/*
 * =====================================================================================
 *
 *       Filename:  test_linked_list.cpp
 *
 *    Description:  Test Linked List
 *
 *        Version:  1.0
 *        Created:  04/07/2013 11:03:24 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include <iostream>
#include <assert.h>
#include "string.h"
#include "linked_list.h"
#include "linked_list.cpp"
#include "painting.h"
//#include "artist.h"

using namespace std;

int main (int argc, char *argv[])
{
//    linked_list_t<int>::node_t n; //compiler error node_t is private
    const int *x;
   linked_list_t<int> list;  //list
//   linked_list_t<string> list; 
    assert(list.empty());
    cout<<list<<endl;
    list.add_first(5);
    assert(*(x=list.search(5))==5);
    assert(!list.empty());
    assert(list.remove_first());
    assert(!list.remove_first());
    assert((list.search(5))==NULL);
    cout<<list<<endl;
    assert(list.empty());

    list.add_first(1);
    list.add_priority(1);
    list.add_first(0);
    cout<<list<<endl;

    linked_list_t<int> list2(list);  //list2
    assert(!list.empty());
    assert(!list2.empty());
    cout<<list2<<endl;
    list.add_priority(2);
    list.add_priority(3);
    list.add_priority(4);
    list.add_priority(7);
    list.add_priority(8);
    cout<<list<<endl;
    cout<<list2<<endl;
    assert((x=list2.search(5))==NULL);
    assert(*(x=list.search(1))==1);
    while(!list2.empty()){
        assert(list2.remove_first());
    }
    cout<<list<<endl;

    cout<<*list[0]<<","<<*list[1]<<","<<*list[2]<<","<<*list[3]<<","<<*list[4]<<","<<*list[5]<<","<<*list[6]<<","<<*list[7]<<endl;
    assert(list[7] != NULL);
    assert(list[8] == NULL);
    assert(list[9] == NULL);


    assert((x=list2.search(1))==NULL);
    cout<<list2<<endl;
    assert(*(x=list.search(0))==0);
    assert(*(x=list.search(1))==1);
    assert(*(x=list.search(2))==2);
    assert(*(x=list.search(3))==3);
    assert(*(x=list.search(4))==4);
    assert(*(x=list.search(7))==7);
    assert(*(x=list.search(8))==8);
    assert(list.search(9)==NULL);
    assert(list2.search(9)==NULL);

    assert(list2.search(1)==NULL);
    assert(!list2.remove(1));
    assert(list.remove(1));
    assert(*(x=list.search(0))==0);
    assert(list.remove(*x));
    cout<<list<<endl;
    assert(*(x=list.search(3))==3);
    assert(list.remove(*x));
    cout<<list<<endl;
    assert(*(x=list.search(4))==4);
    assert(list.remove(*x));
    cout<<list<<endl;
    assert(*(x=list.search(8))==8);
    assert(list.remove(*x));
    cout<<list<<endl;
    assert(*(x=list.search(2))==2);
    assert(list.remove(*x));
    cout<<list<<endl;
    assert(*(x=list.search(7))==7);
    assert(list.remove(*x));
    cout<<list<<endl;
    assert((x=list.search(7))==NULL);
    assert(!list.remove(7));
    cout<<list<<endl;
    assert(!list.empty());
    assert(list[0] != NULL);
    assert(*(x=list.search(1))==1);
    assert(list.remove(*x));
    cout<<list<<endl;
    assert(list.empty());
    assert(list[0] == NULL);

   cout<<endl<<endl<<"String test below"<<endl;
    const string_t *y;
    const string_t a="abc";
    const string_t b="pqr";
    const string_t c(a);
    linked_list_t<string_t> listA;  //listA
    assert(listA.empty());
    cout<<listA<<endl;
    listA.add_first(a);
    cout<<listA<<endl;
    assert(*(y=listA.search(a))==a);
    assert(!listA.empty());
    assert(listA.remove_first());
    assert(!listA.remove_first());
    assert((listA.search("non"))==NULL);
    cout<<listA<<endl;
    assert(listA.empty());
    listA.add_first(b);
    listA.add_priority(c);
    //listA.add_first("tor");
    cout<<listA<<endl; //fix mem read error
    linked_list_t<string_t> listB(listA);  //listB
    assert(!listA.empty());
    assert(!listB.empty());
    cout<<listB<<endl;
  //listA.add_priority("qwe");
    //listA.add_priority("rt");
    //listA.add_priority("dfg");
    //listA.add_priority("hj");
    //listA.add_priority("asd");
    cout<<listA<<endl;
    cout<<listB<<endl;
    assert((y=listB.search("non1"))==NULL);
    assert(*(y=listA.search(a))=="abc");
    while(!listB.empty()){
        assert(listB.remove_first());
    }
    cout<<listA<<endl;
    cout<<"mark1"<<endl;

    cout<<*listA[0]<<","<<*listA[1]<<endl;
    assert(listA[1] != NULL);
    assert(listA[8] == NULL);
    assert(listA[9] == NULL); 

    assert((y=listB.search("non23"))==NULL);
    listA.add_priority(a);
    cout<<listA<<endl;
    assert(listA.remove(a));
    cout<<listA<<endl;
    cout<<listB<<endl;
    
    assert(*(y=listA.search(a))=="abc");
    assert(*(y=listA.search(b))=="pqr");
    assert(listA.search("stz")==NULL);

    /*
    assert(listB.search(1)==NULL);
    assert(!listB.remove(1));
    assert(listA.remove(1));
    assert(*(y=listA.search(0))==0);
    assert(listA.remove(*y));
    cout<<listA<<endl;
    assert(*(y=listA.search(3))==3);
    assert(listA.remove(*y));
    cout<<listA<<endl;
    assert(*(y=listA.search(4))==4);
    assert(listA.remove(*y));
    cout<<listA<<endl;
    assert(*(y=listA.search(8))==8);
    assert(listA.remove(*y));
    cout<<listA<<endl;
    assert(*(y=listA.search(2))==2);
    assert(listA.remove(*y));
    cout<<listA<<endl;
    assert(*(y=listA.search(7))==7);
    assert(listA.remove(*y));
    cout<<listA<<endl;
    assert((y=listA.search(7))==NULL);
    assert(!listA.remove(7));
    cout<<listA<<endl;
    assert(!listA.empty());
    assert(listA[0] != NULL);
    assert(*(y=listA.search(1))==1);
    assert(listA.remove(*y));
    cout<<listA<<endl;
    assert(listA.empty());
    assert(listA[0] == NULL);

*/

//Painting below here
    linked_list_t<painting_t> listP;
    artist_t a1("agarwal","ritesh");
    string_t sp1("p1");
    string_t sp2("p2");
    painting_t p1(sp1,a1,10,20);
    painting_t p2(sp2,a1,12,30);
    painting_t p3(sp1,a1,20,11);
    painting_t p4(p1);
   assert(!(p1==p2));
   assert(p1==p3);
   assert(p1==p4);
   assert(p1<=p4);
   assert(p1<=p2);
   assert(p1<p2);
   assert(!(p2<=p1));
   assert(!(p2<p1));
   cout<<p1<<endl;
   cout<<p2<<endl;
   cout<<p3<<endl;
   cout<<p4<<endl;
   listP.add_priority(p1);
   listP.add_priority(p4);
   listP.add_priority(p2);
   listP.add_priority(p3);
   cout<<listP<<endl;

    cout<<endl<<endl<<"Artist"<<endl;
//    cout<<a1<<endl;
    a1.add_painting(p1);
    a1.add_painting(p2);
    a1.add_painting(p3);
    a1.add_painting(p4);
    a1.add_painting(p1);
    cout<<a1<<endl;
    const painting_t *psearch;
    assert(*(psearch=a1.search_painting(p2))==p2);
    a1.remove_painting(p2);
    assert((psearch=a1.search_painting(p2))==NULL);
    cout<<a1<<endl;
    a1.remove_painting(p1);
    cout<<a1<<endl;
    a1.remove_painting(p1);
    cout<<a1<<endl;
    a1.remove_painting(p1);
    cout<<a1<<endl;
    a1.remove_painting(p1);
    cout<<a1<<endl;
    assert(!(a1.remove_painting(p1)));
    artist_t a2("agarwal","vinay");
    assert(a1==a1);
    assert(!(a1==a2));
    assert(a1<a2);
    assert(!(a2<a1));
    assert(a1<=a2);
    assert(!(a2<=a1));
    return 0;
}/* main */

