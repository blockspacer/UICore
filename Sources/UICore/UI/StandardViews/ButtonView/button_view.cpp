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
**    Mark Page
*/

#include "UICore/precomp.h"
#include "UICore/UI/StandardViews/button_view.h"
#include "UICore/UI/StandardViews/image_view.h"
#include "UICore/UI/Events/pointer_event.h"
#include "button_view_impl.h"

namespace uicore
{
#if 0
	ButtonBaseView::ButtonBaseView() : impl(new ButtonBaseViewImpl())
	{
		impl->button = this;
		style()->set("flex-direction: row");

		impl->image_view = std::make_shared<ImageBaseView>();
		impl->image_view->style()->set("flex: 0 0 auto");
		add_child(impl->image_view);

		impl->label = std::make_shared<LabelBaseView>();
		impl->label->style()->set("margin: auto");
		impl->label->style()->set("flex: 1 1 auto");
		add_child(impl->label);

		slots.connect(sig_pointer_press(), [this](PointerEvent *e) { impl->on_pointer_press(*e); });
		slots.connect(sig_pointer_release(), [this](PointerEvent *e) { impl->on_pointer_release(*e); });

		slots.connect(sig_pointer_enter(), [&](PointerEvent *e) { impl->_state_hot = true; impl->update_state(); });
		slots.connect(sig_pointer_leave(), [&](PointerEvent *e) { impl->_state_hot = false; impl->update_state(); });
	}

	ButtonBaseView::~ButtonBaseView()
	{
	}

	bool ButtonBaseView::disabled() const
	{
		return impl->_state_disabled;
	}

	void ButtonBaseView::set_disabled()
	{
		if (!impl->_state_disabled)
		{
			impl->_state_disabled = true;
			impl->update_state();
		}
	}
	void ButtonBaseView::set_enabled()
	{
		if (impl->_state_disabled)
		{
			impl->_state_disabled = false;
			impl->update_state();
		}
	}

	std::shared_ptr<LabelBaseView> ButtonBaseView::label()
	{
		return impl->label;
	}

	std::shared_ptr<ImageBaseView> ButtonBaseView::image_view()
	{
		return impl->image_view;
	}

	void ButtonBaseView::move_label_before_image()
	{
		impl->label->remove_from_parent();
		impl->image_view->remove_from_parent();

		add_child(impl->label);
		add_child(impl->image_view);
	}

	void ButtonBaseView::move_label_after_image()
	{
		impl->label->remove_from_parent();
		impl->image_view->remove_from_parent();

		add_child(impl->image_view);
		add_child(impl->label);
	}

	Signal<void()> &ButtonBaseView::sig_clicked()
	{
		return impl->sig_clicked;
	}
#endif
}
