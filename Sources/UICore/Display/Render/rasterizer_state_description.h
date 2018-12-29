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

#include "graphic_context.h"

namespace uicore
{
	class RasterizerStateDescription_Impl;

	/// \brief Rasterizer state description.
	class RasterizerStateDescription
	{
	public:
		RasterizerStateDescription();
		RasterizerStateDescription clone() const;

		bool culled() const;
		bool enable_line_antialiasing() const;
		CullMode face_cull_mode() const;
		FillMode face_fill_mode() const;
		FaceSide front_face() const;
		bool enable_scissor() const;

		bool antialiased() const;
		bool offset_point() const;
		bool offset_line() const;
		bool offset_fill() const;
		void polygon_offset(float &out_factor, float &out_units) const;
		float point_size() const;
		float point_fade_treshold_size() const;
		bool is_point_size() const;
		PointSpriteOrigin point_sprite_origin() const;

		/// \brief Enables/disables polygon cull clipping.
		void set_culled(bool value);

		/// \brief Setting to true enables line antialiasing.
		/** <p>Initially antialiasing of lines is disabled.</p>*/
		void enable_line_antialiasing(bool enabled);

		/// \brief Sets the polygon cull clipping mode.
		void set_face_cull_mode(CullMode value);

		/// \brief Sets the filling mode for polygons.
		void set_face_fill_mode(FillMode value);

		/// \brief Sets which side is the front side of a face.
		void set_front_face(FaceSide value);

		/// \brief Enables/disables if clipping rects are used
		void enable_scissor(bool enabled);

		/// \brief Enables/disables anti-aliasing.	(uicoreGL only)
		void enable_antialiased(bool value);

		/// \brief Enables/disables point offsetting.
		void enable_offset_point(bool value);

		/// \brief Enables/disables line offsetting.
		void enable_offset_line(bool value);

		/// \brief Enables/disables polygon offsetting.
		void enable_offset_fill(bool value);

		/// \brief Sets the offset factor.
		void set_polygon_offset(float factor, float units);

		/// \brief The default value is 1.0	(uicoreGL only)
		void set_point_size(float);

		/// \brief Alpha fade point once minimum size treshold reached. Requires multisampling to be enabled.	(uicoreGL only)
		void set_point_fade_treshold_size(float);

		/// \brief Enables if points sizes is set by the vertex shader.	(uicoreGL only)
		void enable_point_size(bool enable);

		/// \brief Sets the origin of texture point sprites.	(uicoreGL only)
		void set_point_sprite_origin(PointSpriteOrigin origin);

		bool operator==(const RasterizerStateDescription &other) const;
		bool operator<(const RasterizerStateDescription &other) const;

	private:
		std::shared_ptr<RasterizerStateDescription_Impl> impl;
	};
}
