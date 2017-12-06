
//
//
// HEADER FILE DEFINING THE IMAGE AND ITS COMPONENTS
//
//

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#if !defined(IMAGE_H)

#define IMAGE_H

extern "C" 
{
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
		GHOST_CELLS		   = 5,
		NUM_LAYOUTS        = 6,
		INVALID_LAYOUT     = -1
	};


	//
	// IMAGE
	//
	class IMAGE
	{
		private:

		// Image Width in terms of Pixels
		int           m_width;
		
		// Image Height in terms of Pixels
		int           m_height;
		
		// Pixel Format of the Image
		FORMAT        m_format;

		// Memory Layout of the Pixels
		LAYOUT        m_layout;

		// Image Width rounded off to the next power of 2
		int           m_width_pow2;

		// Image Height rounded off to the next power of 2
		int           m_height_pow2;

		// Number of Blocks along the width
		// Applicable for the Block_Linear Layout
		int           m_width_in_blocks;

		// Image Data
		void *        m_pixel_data;

		// Processed Image Data
		void *        m_processed_pixel_data;

		// Map of Pixel Offsets
		void *        m_pixel_offset_map;

		public:

		IMAGE(int width, int height, FORMAT format, LAYOUT layout, void *raw_data);

		~IMAGE()
		{
			if (m_pixel_data)
				free(m_pixel_data);

			if (m_processed_pixel_data)
				free(m_processed_pixel_data);

			if (m_pixel_offset_map)
				free(m_pixel_offset_map);
		}

		int get_image_width()	{ return m_width; }

		int get_image_height()	{ return m_height; }

		FORMAT get_image_format()	{ return m_format; }

		LAYOUT get_image_layout()	{ return m_layout; }

		void *get_image_pixel_data()	{ return m_pixel_data; }

		void *get_image_processed_pixel_data()	{ return m_processed_pixel_data; }

		void *get_pixel_offset_map()	{ return m_pixel_offset_map; }

		int get_pixel_offset(int x_coord, int y_coord);
	};

	int bytes_per_pixel(FORMAT format);

	int channels_per_pixel(FORMAT format);

	void normalize_pixel_data(void *input, float *output, FORMAT format);

	void unnormalize_pixel_data(void *output, float *input, FORMAT format);

	void *create_image_instance(void *indata,  int width, int height, int img_format, int img_layout);

	double perform_filtering(void *image_obj, int filter);

	void get_processed_image(void *image_obj, void *output_data);

	void destroy_image_instance(void *image_obj);
}

#endif  // !defined(IMAGE_H)

