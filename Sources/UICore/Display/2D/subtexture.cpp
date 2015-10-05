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
*/

#include "UICore/precomp.h"
#include "UICore/Display/2D/subtexture.h"
#include "UICore/Display/Render/texture_2d.h"
#include "UICore/Core/Math/rect.h"

namespace uicore
{
	class Subtexture_Impl
	{
	public:
		Subtexture_Impl()
		{
		}

		~Subtexture_Impl()
		{
		}

		Texture2DPtr texture;

		Rect geometry;
	};

	Subtexture::Subtexture()
	{
	}

	Subtexture::Subtexture(Texture2DPtr texture, const Rect &geometry)
		: impl(std::make_shared<Subtexture_Impl>())
	{
		impl->texture = texture;
		impl->geometry = geometry;
	}

	Subtexture::~Subtexture()
	{
	}

	void Subtexture::throw_if_null() const
	{
		if (!impl)
			throw Exception("Subtexture is null");
	}

	Texture2DPtr Subtexture::get_texture()
	{
		return impl->texture;
	}

	Rect Subtexture::get_geometry() const
	{
		return impl->geometry;
	}
}
