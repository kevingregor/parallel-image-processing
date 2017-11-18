
//
//
// HEADER FILE THAT DEFINES FILTER KERNEL AND THE RELATED DETAILS
//
//


enum FILTER_TYPE
{
	BOX_FILTER          = 0,
	GAUSSIAN_BLUR       = 1,
	EDGE_DETECT         = 2,
	NUM_FILTER_TYPES    = 3,
	INVALID_FILTER_TYPE = -1
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
	public:

	IMAGE_PROC(){}

	convolve(IMAGE& image, KERNEL& kernel);
};


