
#include "image.h"
#include "image_proc.h"


bool IMAGE_PROC::convolve(IMAGE& image, KERNEL& kernel)
{
	for (int y_coord = 1; y_coord < (image.m_height - 1); y_coord++)
	{
		for (int x_coord = 1; x_coord < (image.m_width - 1); x_coord++)
		{
			int pixel_offset = image.get_pixel_offset(image, x_coord, y_coord);
			int bpp = channel_per_pixel(image.m_format) * sizeof(float);
			int offset_in_bytes = pixel_offset * bpp;

			for (int channel = 0; channel < channels_per_pixel(image.m_format); channel++)
			{
				// COMPUTE THE CONVOLUTION
			}
		}
	}
}

