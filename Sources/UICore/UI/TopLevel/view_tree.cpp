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

#include "UICore/precomp.h"
#include "UICore/UI/TopLevel/view_tree.h"
#include "UICore/UI/Events/event.h"
#include "UICore/UI/Events/focus_change_event.h"
#include "../View/view_impl.h"
#include <algorithm>

namespace uicore
{
	class ViewTreeImpl
	{
	public:
		static void dispatch_activation_change(View *view, ActivationChangeType type)
		{
			ActivationChangeEvent change(type);
			view->dispatch_event(&change, true);
			/*for (const auto &child : view->children())
			{
				dispatch_activation_change(child.get(), type);
			}*/
		}

		View *focus_view = nullptr;
		std::shared_ptr<View> root;
	};

	ViewTree::ViewTree() : impl(new ViewTreeImpl)
	{
		set_root_view(std::make_shared<View>());
	}

	ViewTree::~ViewTree()
	{
	}

	View *ViewTree::focus_view() const
	{
		return impl->focus_view;
	}

	const std::shared_ptr<View> &ViewTree::root_view() const
	{
		return impl->root;
	}

	void ViewTree::set_root_view(std::shared_ptr<View> view)
	{
		if (impl->root)
			impl->root->impl->view_tree = nullptr;
		impl->root = view;
		if (impl->root)
			impl->root->impl->view_tree = this;
	}

	void ViewTree::set_focus_view(View *new_focus_view)
	{
		View *old_focus_view = impl->focus_view;

		if (old_focus_view == new_focus_view)
			return;

		if (old_focus_view)
		{
			FocusChangeEvent focus_loss(FocusChangeType::lost);
			old_focus_view->dispatch_event(&focus_loss, true);
		}

		impl->focus_view = new_focus_view;

		if (new_focus_view)
		{
			FocusChangeEvent focus_gain(FocusChangeType::gained);
			new_focus_view->dispatch_event(&focus_gain, true);
		}
	}

	void ViewTree::removing_view(View *view)
	{
		if (impl->focus_view)
		{
			if (impl->focus_view == view || view->has_child(impl->focus_view))
				set_focus_view(nullptr);
		}
	}

	void ViewTree::render(const std::shared_ptr<Canvas> &canvas, const Rectf &margin_box)
	{
		View *view = impl->root.get();

		ViewGeometry g;
		g.content_x = margin_box.left;
		g.content_y = margin_box.top;
		g.content_width = margin_box.right - margin_box.left;
		g.content_height = margin_box.bottom - margin_box.top;
		view->set_geometry(g);
		//view->set_geometry(ViewGeometry::from_margin_box(view->style_cascade(), margin_box));

		view->impl->render(view, canvas);
	}

	void ViewTree::dispatch_activation_change(ActivationChangeType type)
	{
		ViewTreeImpl::dispatch_activation_change(impl->root.get(), type);
	}
}
