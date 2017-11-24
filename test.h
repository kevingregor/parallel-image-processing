extern "C" 
{
	class MyClass
	{
		public:
		double * data; 
		double * output;
		int rows;
		int cols;
		
		void set_values (int,int);
	    int Test() {return rows*cols;}
	};


    void * CreateInstanceOfClass( double * indatav,  int r, int c, double * output );

    void DeleteInstanceOfClass (void *ptr);

    int CallMemberTest(void *ptr);

}