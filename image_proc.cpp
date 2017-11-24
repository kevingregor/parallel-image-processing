#include "image.h"
#include "image_proc.h"
#include <stdint.h>

// Compile with: gcc -shared -o image_proc.so *.cpp -lstdc++

bool IMAGE_PROC::convolve(IMAGE& image, KERNEL& kernel)
{
	for (int y_coord = 1; y_coord < (image.m_height - 1); y_coord++)
	{
		for (int x_coord = 1; x_coord < (image.m_width - 1); x_coord++)
		{
			int bpp = channels_per_pixel(image.m_format) * sizeof(float);
			uintptr_t pixel_data = reinterpret_cast<uintptr_t>(image.m_pixel_data);
			uintptr_t output_pixel_data = reinterpret_cast<uintptr_t>(image.m_processed_pixel_data);

			int pixel_offset_00 = bpp * get_pixel_offset(image, x_coord - 1, y_coord - 1);
			int pixel_offset_01 = bpp * get_pixel_offset(image, x_coord, y_coord - 1);
			int pixel_offset_02 = bpp * get_pixel_offset(image, x_coord + 1, y_coord - 1);
			int pixel_offset_10 = bpp * get_pixel_offset(image, x_coord - 1, y_coord);
			int pixel_offset_11 = bpp * get_pixel_offset(image, x_coord, y_coord);
			int pixel_offset_12 = bpp * get_pixel_offset(image, x_coord + 1, y_coord);
			int pixel_offset_20 = bpp * get_pixel_offset(image, x_coord - 1, y_coord + 1);
			int pixel_offset_21 = bpp * get_pixel_offset(image, x_coord, y_coord + 1);
			int pixel_offset_22 = bpp * get_pixel_offset(image, x_coord + 1, y_coord + 1);

			for (int channel = 0; channel < channels_per_pixel(image.m_format); channel++)
			{
				float result = 0.0f;

				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_00 + (channel * sizeof(float)))) * kernel.h_00;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_01 + (channel * sizeof(float)))) * kernel.h_01;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_02 + (channel * sizeof(float)))) * kernel.h_02;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_10 + (channel * sizeof(float)))) * kernel.h_10;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_11 + (channel * sizeof(float)))) * kernel.h_11;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_12 + (channel * sizeof(float)))) * kernel.h_12;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_20 + (channel * sizeof(float)))) * kernel.h_20;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_21 + (channel * sizeof(float)))) * kernel.h_21;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offset_22 + (channel * sizeof(float)))) * kernel.h_22;

				*(reinterpret_cast<float *>(output_pixel_data + pixel_offset_11 + (channel * sizeof(float)))) = result;
			}
		}
	}
}

