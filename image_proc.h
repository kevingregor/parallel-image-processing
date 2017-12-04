
//
//
// HEADER FILE THAT DEFINES FILTER KERNEL AND THE RELATED DETAILS
//
//

#include "image.h"

# if !defined(IMAGE_PROC_H)

#define IMAGE_PROC_H

extern "C" 
{
	enum FILTER_TYPE
	{
		BOX_FILTER          = 0,
		GAUSSIAN_BLUR       = 1,
		EDGE_DETECT         = 2,
		NUM_FILTER_TYPES    = 3,
		INVALID_FILTER_TYPE = -1
	};


	enum LAYOUT_CONVERSION_DIRECTION
	{
		CONVERT_LAYOUT_IN      = 0,
		CONVERT_LAYOUT_OUT     = 1,
		CONVERT_LAYOUT_INVALID = -1
	};


	class KERNEL
	{
	//  +------+------+------+
	//  | h_00 | h_01 | h_02 |
	//  +------+------+------+
	//  | h_10 | h_11 | h_12 |
	//  +------+------+------+
	//  | h_20 | h_21 | h_22 |
	//  +------+------+------+

		public:

		float h_00, h_01, h_02;
		float h_10, h_11, h_12;
		float h_20, h_21, h_22;

		KERNEL(FILTER_TYPE filter_type);
	};


	class IMAGE_PROC
	{
		private:

		int chunk_width;
		int chunk_height;
		int num_chunks_x;
		int num_chunks_y;

		public:

		IMAGE_PROC(){}

		double convolve(IMAGE& image, KERNEL& kernel);

		bool convert_layout(IMAGE& image, LAYOUT_CONVERSION_DIRECTION direction, void *raw_data);

		void process_chunk(IMAGE& image, int chunk_x, int chunk_y, KERNEL& kernel);

		bool generate_pixel_offset_map(IMAGE& image);
	};
}

#endif  // !defined(IMAGE_PROC_H)

