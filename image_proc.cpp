
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
			h_00 = 1.0f / 4.0f;
			h_01 = 1.0f / 2.0f;
			h_02 = 1.0f / 4.0f;
			h_10 = 1.0f / 2.0f;
			h_11 = -3.0f;
			h_12 = 1.0f / 2.0f;
			h_20 = 1.0f / 4.0f;
			h_21 = 1.0f / 2.0f;
			h_22 = 1.0f / 4.0f;

			break;
		}
		default:
		{
			assert(!"Invalid Filter Type !");

			break;
		}
	}
}


void IMAGE_PROC::process_chunk(IMAGE &image, int chunk_x, int chunk_y, KERNEL& kernel)
{
	int image_width = image.get_image_width();
	int image_height = image.get_image_height();
	FORMAT image_format = image.get_image_format();
	LAYOUT image_layout = image.get_image_layout();
	void *image_pixel_data = image.get_image_pixel_data();
	void *image_processed_pixel_data = image.get_image_processed_pixel_data();

	int x_start = chunk_x * chunk_width;
	int y_start = chunk_y * chunk_height;

	int pixel_offsets[9];
	int *pixel_offset_map = (int *)(image.get_pixel_offset_map());

	//#pragma omp parallel for
	for (int y_coord = y_start; y_coord < (y_start + chunk_height); y_coord++)	
	{
		for (int x_coord = x_start; x_coord < (x_start + chunk_width); x_coord++)
		{
			if ((x_coord < 1) || (x_coord > (image_width - 2)) ||
				(y_coord < 1) || (y_coord > (image_height - 2)))
			{
				continue;
			}

			int bpp = channels_per_pixel(image_format) * sizeof(float);
			uintptr_t pixel_data = reinterpret_cast<uintptr_t>(image_pixel_data);
			uintptr_t output_pixel_data = reinterpret_cast<uintptr_t>(image_processed_pixel_data);
			
			for (int i = -1; i<=1; i++) {
				for (int j = -1; j<=1; j++) {
					pixel_offsets[3*(i+1) + (j+1)] = bpp * (*(pixel_offset_map + ((y_coord + i) * image_width) + x_coord + j));
				}
			}

			// int pixel_offset_00 = bpp * image.get_pixel_offset(x_coord - 1, y_coord - 1);
			// int pixel_offset_01 = bpp * image.get_pixel_offset(x_coord, y_coord - 1);
			// int pixel_offset_02 = bpp * image.get_pixel_offset(x_coord + 1, y_coord - 1);
			// int pixel_offset_10 = bpp * image.get_pixel_offset(x_coord - 1, y_coord);
			// int pixel_offset_11 = bpp * image.get_pixel_offset(x_coord, y_coord);
			// int pixel_offset_12 = bpp * image.get_pixel_offset(x_coord + 1, y_coord);
			// int pixel_offset_20 = bpp * image.get_pixel_offset(x_coord - 1, y_coord + 1);
			// int pixel_offset_21 = bpp * image.get_pixel_offset(x_coord, y_coord + 1);
			// int pixel_offset_22 = bpp * image.get_pixel_offset(x_coord + 1, y_coord + 1);

			for (int channel = 0; channel < channels_per_pixel(image_format); channel++)
			{
				float result = 0.0f;

				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[0] + (channel * sizeof(float)))) * kernel.h_00;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[1] + (channel * sizeof(float)))) * kernel.h_01;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[2] + (channel * sizeof(float)))) * kernel.h_02;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[3] + (channel * sizeof(float)))) * kernel.h_10;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[4] + (channel * sizeof(float)))) * kernel.h_11;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[5] + (channel * sizeof(float)))) * kernel.h_12;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[6] + (channel * sizeof(float)))) * kernel.h_20;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[7] + (channel * sizeof(float)))) * kernel.h_21;
				result += *(reinterpret_cast<float *>(pixel_data + pixel_offsets[8] + (channel * sizeof(float)))) * kernel.h_22;

				// Clamp the result in the range (0.0, 1.0)
				if (result < 0.0f)
					result = 0.0f;
				else if (result > 1.0f)
					result = 1.0f;

				//*(reinterpret_cast<float *>(output_pixel_data + pixel_offset_11 + (channel * sizeof(float)))) = result;
			
				*(reinterpret_cast<float *>(output_pixel_data + pixel_offsets[4] + (channel * sizeof(float)))) = result;
			}
		}
	}
}


double IMAGE_PROC::convolve(IMAGE& image, KERNEL& kernel)
{
	num_chunks_x = 4;
	num_chunks_y = 4;

	int image_width = image.get_image_width();
	int image_height = image.get_image_height();

	chunk_width = (image_width + (num_chunks_x - 1)) / num_chunks_x;
	chunk_height = (image_height + (num_chunks_y - 1)) / num_chunks_y;

	double start_time = omp_get_wtime();

	#pragma omp parallel
	{
		for (int chunk_y = 0; chunk_y < num_chunks_y; chunk_y++)
		{
			for (int chunk_x = 0; chunk_x < num_chunks_x; chunk_x++)
			{
				#pragma omp task
				process_chunk(image, chunk_x, chunk_y, kernel);
			}
		}
	}

	double time = omp_get_wtime() - start_time;

	return time;
}


bool IMAGE_PROC::generate_pixel_offset_map(IMAGE& image)
{
	int x_coord, y_coord;
	int image_width = image.get_image_width();
	int image_height = image.get_image_height();
	LAYOUT image_layout = image.get_image_layout();
	int *pixel_offset_map = (int *)(image.get_pixel_offset_map());

	for (y_coord = 0; y_coord < image_height; y_coord++)
	{
		for (x_coord = 0; x_coord < image_width; x_coord++)
		{
			*(pixel_offset_map + (y_coord * image_width) + x_coord) = image.get_pixel_offset(x_coord, y_coord);
		}
	}

	return true;
}


bool IMAGE_PROC::convert_layout(IMAGE& image, LAYOUT_CONVERSION_DIRECTION direction, void *raw_data)
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

