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
**  Note: Some of the libraries UICore may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include "../System/databuffer.h"

namespace uicore
{
	/// \brief Deflate compressor
	class ZLibCompression
	{
	public:
		enum CompressionMode
		{
			default_strategy,
			filtered,
			huffman_only,
			rle,
			fixed
		};

		// \brief Compress data
		// \param data Data to compress
		// \param raw Skips header if true
		// \param compression_level Compression level in range 0-9. 0 = no compression, 1 = best speed, 6 = default, 9 = best compression.
		// \param mode Compression strategy
		static std::shared_ptr<DataBuffer> compress(const std::shared_ptr<DataBuffer> &data, bool raw = true, int compression_level = 6, CompressionMode mode = default_strategy);

		// \brief Decompress data
		// \param data Data to compress
		// \param raw Skips header if true
		static std::shared_ptr<DataBuffer> decompress(const std::shared_ptr<DataBuffer> &data, bool raw = true);
	};
}
