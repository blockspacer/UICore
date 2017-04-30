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

#include "UICore/Display/Render/graphic_context.h"
#include "UICore/Display/Render/program_object.h"
#include "UICore/Display/Render/shader_object.h"

namespace uicore
{
	class StandardPrograms_Impl;

	class StandardPrograms
	{
	public:
		StandardPrograms();
		StandardPrograms(const std::shared_ptr<GraphicContext> &gc);

		std::shared_ptr<ProgramObject> get_program_object(StandardProgram standard_program) const;

	private:
		std::shared_ptr<ProgramObject> compile(const std::shared_ptr<GraphicContext> &gc, const void *vertex_code, int vertex_code_size, const void *fragment_code, int fragment_code_size);
		void link(std::shared_ptr<ProgramObject> &program, const std::string &error_message);

		std::shared_ptr<StandardPrograms_Impl> impl;

		static const BYTE color_only_vertex[];
		static const BYTE color_only_fragment[];
		static const BYTE single_texture_vertex[];
		static const BYTE single_texture_fragment[];
		static const BYTE sprite_vertex[];
		static const BYTE sprite_fragment[];
		static const BYTE path_vertex[];
		static const BYTE path_fragment[];
	};
}
