#include "test.h"
#include <new>

// Compile with: gcc -shared -o test.so test.cpp -lstdc++

class MyClass
{
    public: 
    int Test() {
        return 42;
    }
};

//Note: The interface this linkage region needs to use C only.  
void * CreateInstanceOfClass( void )
{
    // Note: Inside the function body, I can use C++. 
    return new MyClass;
}

int CallMemberTest(void *ptr)
{
    MyClass * ref = reinterpret_cast<MyClass *>(ptr);
    return ref->Test();
}