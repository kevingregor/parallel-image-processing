#include "test.h"
#include <new>
#include "stdio.h"

// Compile with: gcc -shared -o test.so test.cpp -lstdc++

void MyClass::set_values (int r, int c) {
	rows = r;
	cols = c;
}

//Note: The interface this linkage region needs to use C only.  
void * CreateInstanceOfClass(double * indatav, int r, int c, double * output)
{

    // Note: Inside the function body, I can use C++. 
    MyClass *clss = new MyClass();
    clss->data = indatav;
    clss->output = output;
    clss->set_values(r, c);
    return clss;
}

int CallMemberTest(void *ptr)
{
    MyClass * ref = (MyClass *) (ptr);
    for (int i = 0; i < ref->rows * ref->cols; ++i) {
        ref->output[i] = ref->data[i] * 2;
    }
    return 1;
}