
#include "image_proc.h"
#include <omp.h>


KERNEL::KERNEL(FILTER_TYPE filter_type)
{
	switch (filter_type)
	{
		case BOX_FILTER:
		{
			h_00 = 1.0f / 9.0f;
			h_01 = 1.0f / 9.0f;
			h_02 = 1.0f / 9.0f;
			h_10 = 1.0f / 9.0f;
			h_11 = 1.0f / 9.0f;
			h_12 = 1.0f / 9.0f;
			h_20 = 1.0f / 9.0f;
			h_21 = 1.0f / 9.0f;
			h_22 = 1.0f / 9.0f;

			break;
		}
		case GAUSSIAN_BLUR:
		{
			h_00 = 1.0f / 16.0f;
			h_01 = 1.0f / 8.0f;
			h_02 = 1.0f / 16.0f;
			h_10 = 1.0f / 8.0f;
			h_11 = 1.0f / 4.0f;
			h_12 = 1.0f / 8.0f;
			h_20 = 1.0f / 16.0f;
			h_21 = 1.0f / 8.0f;
			h_22 = 1.0f / 16.0f;

			break;
		}
		case EDGE_DETECT:
		{
			assert(!"Not Yet Implemented");

			break;
		}
		default:
		{
			assert(!"Invalid Filter Type !");

			break;
		}
	}
}


double IMAGE_PROC::convolve(IMAGE& image, KERNEL& kernel)
{
	double start_time = omp_get_wtime();

	int image_width = image.get_image_width();
	int image_height = image.get_image_height();
	FORMAT image_format = image.get_image_format();
	LAYOUT image_layout = image.get_image_layout();
	void *image_pixel_data = image.get_image_pixel_data();
	void *image_processed_pixel_data = image.get_image_processed_pixel_data();

	for (int y_coord = 1; y_coord < (image_height - 1); y_coord++)
	{
		for (int x_coord = 1; x_coord < (image_width - 1); x_coord++)
		{
			int bpp = channels_per_pixel(image_format) * sizeof(float);
			uintptr_t pixel_data = reinterpret_cast<uintptr_t>(image_pixel_data);
			uintptr_t output_pixel_data = reinterpret_cast<uintptr_t>(image_processed_pixel_data);

			int pixel_offset_00 = bpp * image.get_pixel_offset(x_coord - 1, y_coord - 1);
			int pixel_offset_01 = bpp * image.get_pixel_offset(x_coord, y_coord - 1);
			int pixel_offset_02 = bpp * image.get_pixel_offset(x_coord + 1, y_coord - 1);
			int pixel_offset_10 = bpp * image.get_pixel_offset(x_coord - 1, y_coord);
			int pixel_offset_11 = bpp * image.get_pixel_offset(x_coord, y_coord);
			int pixel_offset_12 = bpp * image.get_pixel_offset(x_coord + 1, y_coord);
			int pixel_offset_20 = bpp * image.get_pixel_offset(x_coord - 1, y_coord + 1);
			int pixel_offset_21 = bpp * image.get_pixel_offset(x_coord, y_coord + 1);
			int pixel_offset_22 = bpp * image.get_pixel_offset(x_coord + 1, y_coord + 1);

			for (int channel = 0; channel < channels_per_pixel(image_format); channel++)
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

	double time = omp_get_wtime() - start_time;
	return time;
}


bool IMAGE_PROC::convert_layout(IMAGE &image, LAYOUT_CONVERSION_DIRECTION direction, void *raw_data)
{
	int x_coord, y_coord;
	int image_width = image.get_image_width();
	int image_height = image.get_image_height();
	FORMAT image_format = image.get_image_format();
	LAYOUT image_layout = image.get_image_layout();
	void *image_pixel_data = image.get_image_pixel_data();
	void *image_processed_pixel_data = image.get_image_processed_pixel_data();
	float normalized_pixel_data[4];

	switch (direction)
	{
		case CONVERT_LAYOUT_IN:
		{
			// The input raw_data is always supposed to be strided
			for (y_coord = 0; y_coord < image_height; y_coord++)
			{	
				for (x_coord = 0; x_coord < image_width; x_coord++)
				{
					int bpp_input = bytes_per_pixel(image_format);

					uintptr_t input_pixel_addr = (reinterpret_cast<uintptr_t>(raw_data) + ((image_width * y_coord) + x_coord) * bpp_input);

					normalize_pixel_data(reinterpret_cast<void *>(input_pixel_addr), &normalized_pixel_data[0], image_format);

					uintptr_t pixel_addr = reinterpret_cast<uintptr_t>(image_pixel_data)
										 + (image.get_pixel_offset(x_coord, y_coord) * channels_per_pixel(image_format) * sizeof(float));

					float *pixel_data_ptr = reinterpret_cast<float *>(pixel_addr);

					for (int channel = 0; channel < channels_per_pixel(image_format); channel++)
					{
						*(pixel_data_ptr + channel) = normalized_pixel_data[channel];
					}
				}
			}

			break;
		}
		case CONVERT_LAYOUT_OUT:
		{
			// The output raw_data is always supposed to be strided
			for (y_coord = 1; y_coord < (image_height - 1); y_coord++)
			{	
				for (x_coord = 1; x_coord < (image_width - 1); x_coord++)
				{
					int bpp_output = bytes_per_pixel(image_format);

					uintptr_t output_pixel_addr = reinterpret_cast<uintptr_t>(raw_data)
												+ (((image_width * y_coord) + x_coord) * bpp_output);

					uintptr_t pixel_addr = reinterpret_cast<uintptr_t>(image_processed_pixel_data)
										 + (image.get_pixel_offset(x_coord, y_coord) * channels_per_pixel(image_format) * sizeof(float));

					float *pixel_data_ptr = reinterpret_cast<float *>(pixel_addr);

					for (int channel = 0; channel < channels_per_pixel(image_format); channel++)
					{
						normalized_pixel_data[channel] = *(pixel_data_ptr + channel);
					}

					unnormalize_pixel_data(reinterpret_cast<void *>(output_pixel_addr), &normalized_pixel_data[0], image_format);
				}
			}

			break;
		}
		default:
		{
			break;
		}
	}

	return true;
}

