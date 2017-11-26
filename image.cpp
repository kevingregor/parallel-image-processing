
#include "image.h"
#include "image_proc.h"
#include <stdio.h>
#include <string.h> 


IMAGE_PROC *image_proc = NULL;

KERNEL *kernel = NULL;

void *create_image_instance(void *indata, int width, int height)
{
	// The following two values should be input args ideally
	// But hard-coding just for the time being
    FORMAT format = RGB_888;
    LAYOUT layout = STRIDED;

    IMAGE *img = new IMAGE(width, height, format, layout, indata);

	return img;
}


void perform_filtering(void *image_obj)
{
	IMAGE *image = (IMAGE *) (image_obj);
	
	// The following two lines should be moved inside the function that takes in filtering options
	kernel = new KERNEL(BOX_FILTER);

	assert(image_proc);

	image_proc->convolve(*image, *kernel);
}


void get_processed_image(void *image_obj, void *output_data)
{
	IMAGE *image = reinterpret_cast<IMAGE *>(image_obj);

	assert(image_proc);

	image_proc->convert_layout(*image, CONVERT_LAYOUT_OUT, output_data);
}


void destroy_image_instance(void *image_obj)
{
	IMAGE *image = reinterpret_cast<IMAGE *>(image_obj);
	
	delete image;
	delete kernel;
	delete image_proc;

	kernel = NULL;
	image_proc = NULL;
}


int bytes_per_pixel(FORMAT format)
{
	switch (format)
	{
		case L_8:        return 1;
		case L_16:       return 2;
		case L_32:       return 4;
		case RGB_888:    return 3;
		case RGBX_8888:  return 4;
		default:         return 0;
	}
}


int channels_per_pixel(FORMAT format)
{
	switch (format)
	{
		case L_8:        return 1;
		case L_16:       return 1;
		case L_32:       return 1;
		case RGB_888:    return 3;
		case RGBX_8888:  return 4;
		default:         return 0;
	}
}


void normalize_pixel_data(void *input, float *output, FORMAT format)
{
	uintptr_t input_addr = reinterpret_cast<uintptr_t>(input);

	switch (format)
	{
		case L_8:
		{
			*output = *(reinterpret_cast<unsigned char *>(input_addr)) / 255.0f;

			break;
		}
		case L_16:
		{
			*output = *(reinterpret_cast<unsigned short *>(input_addr)) / 65535.0f;

			break;
		}
		case L_32:
		{
			double value = *(reinterpret_cast<unsigned int *>(input_addr));

			value /= 4294967295.0;

			*output = static_cast<float>(value);

			break;
		}
		case RGB_888:
		{
			*output = *(reinterpret_cast<unsigned char *>(input_addr)) / 255.0f;
			*(output + 1) = *(reinterpret_cast<unsigned char *>(input_addr + 1)) / 255.0f;
			*(output + 2) = *(reinterpret_cast<unsigned char *>(input_addr + 2)) / 255.0f;

			break;
		}
		case RGBX_8888:
		{
			*output = *(reinterpret_cast<unsigned char *>(input_addr)) / 255.0f;
			*(output + 1) = *(reinterpret_cast<unsigned char *>(input_addr + 1)) / 255.0f;
			*(output + 2) = *(reinterpret_cast<unsigned char *>(input_addr + 2)) / 255.0f;
			*(output + 3) = 1.0f;

			break;
		}
		default:
		{
			break;
		}
	}
}


void unnormalize_pixel_data(void *output, float *input, FORMAT format)
{
	uintptr_t output_addr = reinterpret_cast<uintptr_t>(output);

	switch (format)
	{
		case L_8:
		{
			*(reinterpret_cast<unsigned char *>(output_addr)) = *input * 255.0f;

			break;
		}
		case L_16:
		{
			*(reinterpret_cast<unsigned short *>(output_addr)) = *input * 65535.0f;

			break;
		}
		case L_32:
		{
			double value = *input * 4294967295.0;

			*(reinterpret_cast<unsigned int *>(output_addr))= static_cast<unsigned int>(value);

			break;
		}
		case RGB_888:
		{
			*(reinterpret_cast<unsigned char *>(output_addr)) = *input * 255.0f;
			*(reinterpret_cast<unsigned char *>(output_addr + 1)) = *(input + 1) * 255.0f;
			*(reinterpret_cast<unsigned char *>(output_addr + 2)) = *(input + 2) * 255.0f;

			break;
		}
		case RGBX_8888:
		{
			*(reinterpret_cast<unsigned char *>(output_addr)) = *input * 255.0f;
			*(reinterpret_cast<unsigned char *>(output_addr + 1)) = *(input + 1) * 255.0f;
			*(reinterpret_cast<unsigned char *>(output_addr + 2)) = *(input + 2) * 255.0f;
			*(reinterpret_cast<unsigned char *>(output_addr + 3)) = 255;

			break;
		}
		default:
		{
			break;
		}
	}
}


IMAGE::IMAGE(int width, int height, FORMAT format, LAYOUT layout, void *raw_data)
{
	m_width = width;
	m_height = height;
	m_format = format;
	m_layout = layout;

	int size = 0;

	assert(!image_proc);

	image_proc = new IMAGE_PROC();

	// Validate the incoming parameters, if at all ...

	// Convert the incoming raw pixel data to the desired Layout
	// The incoming raw pixel data is always suppossed to be STRIDED
	switch (m_layout)
	{
		case STRIDED:
		{
			size = width * height * channels_per_pixel(format) * sizeof(float);

			m_pixel_data = malloc(size);
			m_processed_pixel_data = malloc(size);

			if (!m_pixel_data || !m_processed_pixel_data)
				return;

			image_proc->convert_layout(*this, CONVERT_LAYOUT_IN, raw_data);

			break;
		}

		case BLOCK_LINEAR_8:
		case BLOCK_LINEAR_16:
		case BLOCK_LINEAR_32:
		case TWIDDLED:
		{
			return;
		}
	}
}


int IMAGE::get_pixel_offset(int x_coord, int y_coord)
{
	switch (m_layout)
	{
		case STRIDED:
		{
			return (m_width * y_coord) + x_coord;
		}
		default:
		{
			assert(!"Not Implemented");
		}
	}

	return 0;
}

