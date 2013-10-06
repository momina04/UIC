/*
 * =====================================================================================
 *
 *       Filename:  a.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2013 07:30:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */


class testclass
{
 
  int x;
  public:
  testclass()
  {
    x=0;
  }
  void func4() const
  {
    x;
    //x++;
  }
};


void func3(int *c)
{
}

void func2(const int &b) //can take both const int and int
{
}

void func1(int &a)
{
}

int main (int argc, char *argv[])
{
    int x=1;
    const int y=2;

    func1(x);
//    func1(y); //not allowed

    func2(x);
    func2(y);

    func3(&x);
//    func3(&y);//not allowed

    testclass obj;
    const testclass constobj;
    obj.func4();
    constobj.func4();
    return 0;
}/* main */
