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
**    Mark Page
*/

#pragma once

#include "UICore/Display/Font/font.h"
#include "UICore/Display/Font/glyph_metrics.h"
#include "UICore/Display/Font/font_metrics.h"
#include "UICore/Display/Render/texture.h"
#include "UICore/Display/2D/path.h"
#include <list>
#include <map>

namespace uicore
{
	class Colorf;
	class TextureGroup;
	class FontEngine;
	class TextureGroupImage;
	class FontPixelBuffer;
	class Path;
	class RenderBatchTriangle;

	class Font_PathGlyph
	{
	public:
		Font_PathGlyph() : glyph(0) { };

		/// \brief Glyph this buffer refers to.
		unsigned int glyph;

		std::shared_ptr<Path> path;
		GlyphMetrics metrics;
	};

	class PathCache
	{
	public:
		PathCache();
		virtual ~PathCache();

		/// \brief Get a glyph. Returns NULL if the glyph was not found
		Font_PathGlyph *get_glyph(const std::shared_ptr<Canvas> &canvas, FontEngine *font_engine, unsigned int glyph);

		GlyphMetrics get_metrics(FontEngine *font_engine, const std::shared_ptr<Canvas> &canvas, unsigned int glyph);

	private:
		std::vector<Font_PathGlyph* > glyph_list;
	};
}
