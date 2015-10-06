/*
**  UICore
**  Copyright (c) 1997-2015 The UICore Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    Mark Page
*/

#include "UICore/precomp.h"
#include "cpu_pixel_buffer_provider.h"
#include "UICore/Core/System/system.h"

namespace uicore
{
	CPUPixelBufferProvider::CPUPixelBufferProvider(TextureFormat new_format, const Size &size, const void *data_ptr, bool only_reference_data)
	{
		_width = size.width;
		_height = size.height;
		texture_format = new_format;

		unsigned int alloc_size = data_size(size, new_format);

		if (only_reference_data)
		{
			if (!data_ptr)
				throw Exception("PixelBuffer only_reference_data set without data");

			_data = (unsigned char *)data_ptr;
			delete_data = false;
		}
		else
		{
			if (data_ptr)
			{
				delete_data = true;
				_data = (unsigned char *)System::aligned_alloc(alloc_size, 16);
				memcpy(_data, data_ptr, alloc_size);
			}
			else
			{
				delete_data = true;
				_data = (unsigned char *)System::aligned_alloc(alloc_size, 16);
			}
		}
	}

	CPUPixelBufferProvider::~CPUPixelBufferProvider()
	{
		if (delete_data)
			System::aligned_free(_data);
	}

	void CPUPixelBufferProvider::upload_data(GraphicContext &gc, const Rect &dest_rect, const void *data)
	{
		// Handle the simple base
		if (dest_rect.left == 0 && dest_rect.get_width() == _width)
		{
			unsigned int data_size = PixelBuffer::data_size(Size(_width, dest_rect.get_height()), texture_format);
			memcpy(_data, data, data_size);
		}
		else
		{
			// Need to upload in blocks here
			throw Exception("CPUPixelBufferProvider::upload_data() Implement me for this situation");
		}
	}
}
