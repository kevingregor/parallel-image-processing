
//
//
// HEADER FILE DEFINING THE IMAGE AND ITS COMPONENTS
//
//
extern "C" 
{


	#if !defined(IMAGE_H)

	#define IMAGE_H


	//
	// IMAGE FORMATS
	//
	enum FORMAT
	{
		L_8               = 0,    // unsigned 8-bit Luminance
		L_16              = 1,    // unsigned 16-bit Luminance
		L_32              = 2,    // unsigned 32-bit Luminance
		RGB_888           = 3,    // unsigned 8-bit per Channel (RGB)
		RGBX_8888         = 4,    // similar to RGB888 but with an additional undefined Channel
		NUM_FORMATS       = 5,
		INVALID_FORMAT    = -1
	};


	//
	// IMAGE MEMORY LAYOUTS
	//
	enum LAYOUT
	{
		STRIDED            = 0,    // Linear, Row-Major Layout
		BLOCK_LINEAR_8     = 1,    // Blocked Layout with Block Size 8 x 8
		BLOCK_LINEAR_16    = 2,    // Blocked Layout with Block Size 16 x 16
		BLOCK_LINEAR_32    = 3,    // Blocked Layout with Block Size 32 x 32
		TWIDDLED           = 4,    // Z-Morton Layout
		NUM_LAYOUTS        = 5,
		INVALID_LAYOUT     = -1
	};


	//
	// IMAGE
	//
	class IMAGE
	{
		public:

		// Image Width in terms of Pixels
		int           m_width;
		
		// Image Height in terms of Pixels
		int           m_height;
		
		// Pixel Format of the Image
		FORMAT        m_format;

		// Memory Layout of the Pixels
		LAYOUT        m_layout;

		// Image Data
		void *        m_pixel_data;

		// Processed Image Data
		void *        m_processed_pixel_data;

		// public:

		void image(int width, int height, FORMAT format, LAYOUT layout, void *raw_data);
	};

	int bytes_per_pixel(FORMAT format);
	int channels_per_pixel(FORMAT format);
	int get_pixel_offset(IMAGE& image, int x_coord, int y_coord);
	void * CreateInstanceOfImage( double * indata,  int r, int c, double * output );

	#endif  // !defined(IMAGE_H)
}