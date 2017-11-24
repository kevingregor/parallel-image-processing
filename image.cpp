#include "image.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <assert.h>

void * CreateInstanceOfImage(double * indata, int r, int c, double * output)
{
    IMAGE *img = new IMAGE();
    FORMAT format = L_8; // Dummy Value. Not really sure what format is
    LAYOUT layout = STRIDED;
    img->image(r, c, format, layout, indata);
    return img;
}

void IMAGE::image(int width, int height, FORMAT format, LAYOUT layout, void *raw_data)
{
	m_width = width;
	m_height = height;
	m_format = format;
	m_layout = layout;

	int size = 0;

	// Validate the incoming parameters, if at all ...

	// Convert the incoming raw pixel data to the desired Layout
	// The incoming raw pixel data is always suppossed to be STRIDED
	switch (m_layout)
	{
		case STRIDED:
		{
			size = width * height * bytes_per_pixel(format);

			m_pixel_data = malloc(size);

			if (!m_pixel_data)
				return;

			// TODO: Normalize the Pixel Values and Store in Float instead of just memcpy
			memcpy(m_pixel_data, raw_data, size);

			break;
		}

		// YET TO BE IMPLEMENTED (Layout Conversions should be implemented in separate file)
		case BLOCK_LINEAR_8:
		case BLOCK_LINEAR_16:
		case BLOCK_LINEAR_32:
		case TWIDDLED:
		{
			return;
		}
	}
}


// Need to move this into a separate file for handling memory_layouts
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


// Need to move this into a separate file for handling memory_layouts
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


// Need to move this into a separate file for handling memory_layouts
int get_pixel_offset(IMAGE& image, int x_coord, int y_coord)
{
	switch (image.m_layout)
	{
		case STRIDED:
		{
			return (image.m_width * y_coord) + x_coord;
		}
		default:
		{
			assert(!"Not Implemented");
		}
	}

	return 0;
}


