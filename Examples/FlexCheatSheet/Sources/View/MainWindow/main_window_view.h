
#pragma once

class FlexRedBoxView : public uicore::View
{
public:
	FlexRedBoxView(const std::string &styles = std::string())
	{
		style()->set(R"(
			width: 100px;
			height: 100px;
			background-color: #e46119;
			border: 1px solid #626262;
			margin: 3px;
			)");
		style()->set(styles);
	}
};

class FlexContainerView : public uicore::View
{
public:
	FlexContainerView(const std::string &styles = std::string(), int num_boxes = 4)
	{
		style()->set(R"(
			min-height: 300px;
			max-height: 450px;
			background-color: #dce7f2;
			border: 1px solid #2a4f73;
			width: 750px;
			margin: 15px auto;
			)");
		style()->set(styles);

		for (int i = 0; i < num_boxes; i++)
			boxes.push_back(add_child<FlexRedBoxView>());
	}

	std::vector<std::shared_ptr<FlexRedBoxView>> boxes;
};

class FlexHeadline : public uicore::TextBlockBaseView
{
public:
	FlexHeadline(const std::string &text)
	{
		style()->set(R"(
			font-weight: 400;
			font-style: italic;
			font-size: 24px;
			line-height: 32px;
			width: 750px;
			margin: 15px auto;
			flex: none;
			)");

		add_text(text);
	}
};

class FlexParagraph : public uicore::TextBlockBaseView
{
public:
	FlexParagraph(const std::string &text)
	{
		style()->set(R"(
			width: 750px;
			margin: 8px auto;
			flex: none;
			)");

		add_text(text);
	}
};

class FlexPanelButton : public uicore::ButtonBaseView
{
public:
	FlexPanelButton(const std::string &text)
	{
		style()->set("margin: 5px 0; padding: 2px 5px");
		label()->set_text(text);
	}
};

class FlexExampleView : public uicore::ColumnView
{
public:
	FlexExampleView()
	{
		set_hidden();
	}
};

class MainWindowView : public uicore::RowView
{
public:
	MainWindowView()
	{
		style()->set(R"(
			background: rgb(250,250,250);
			font: 11px/15px 'Segoe UI'; color: black
			)");

		panel = add_child<uicore::ColumnView>();
		panel->style()->set("width: 300px; background: rgb(240,240,240); padding: 15px");

		examples = add_child<uicore::ColumnView>();
		examples->style()->set("flex: 1 1");

		add_example1();
		add_example2();
		add_example3();
		add_example4();
		add_example5();
		add_example6();
		add_example7();
		add_example8();
		add_example9();
		add_example10();
		add_example11();
	}

	void add_example1()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Put flex items into a row");
		example->add_child<FlexParagraph>("Flex items are the children of a flex container. They are positioned along a main axis and a cross axis. The main axis is horizontal by default, so the items flow into a row. You can flip the main axis by setting flex-direction to column, it's set to row by default.");

		auto container = example->add_child<FlexContainerView>(R"(
			flex-direction: row
			)");

		auto button = panel->add_child<FlexPanelButton>("Put flex items into a row");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });

		example->set_hidden(false);
	}

	void add_example2()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Put flex items into a column");
		example->add_child<FlexParagraph>("Flex items are the children of a flex container. They are positioned along a main axis and a cross axis. The main axis is horizontal by default, so the items flow into a row. You can flip the main axis by setting flex-direction to column, it's set to row by default.");

		auto container = example->add_child<FlexContainerView>(R"(
			flex-direction: column
			)");

		auto button = panel->add_child<FlexPanelButton>("Put flex items into a column");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example3()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Move flex items to the top");
		example->add_child<FlexParagraph>("How you move flex items to the top depends on the orientation of the main axis. If it's vertical, you set align-items. If it's horizontal, you set justify-content.");

		auto container1 = example->add_child<FlexContainerView>(R"(
			flex-direction: column;
			justify-content: flex-start;
			height: 500px;
			)");

		auto container2 = example->add_child<FlexContainerView>(R"(
			flex-direction: row;
			align-items: flex-start
			)");

		auto button = panel->add_child<FlexPanelButton>("Move flex items to the top");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example4()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Move flex items to the left");
		example->add_child<FlexParagraph>("Moving items to the left or right also depends on the orientation of the main axis. If flex-direction is set to row, then you set justify-content, otherwise if it's set to column you set align-items.");

		auto container1 = example->add_child<FlexContainerView>(R"(
			flex-direction: row;
			justify-content: flex-start
			)");

		auto container2 = example->add_child<FlexContainerView>(R"(
			flex-direction: column;
			align-items: flex-start
			)");

		auto button = panel->add_child<FlexPanelButton>("Move flex items to the left");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example5()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Move flex items to the right");
		example->add_child<FlexParagraph>("Moving items to the left or right also depends on the orientation of the main axis. If flex-direction is set to row, then you set justify-content, otherwise if it's set to column you set align-items.");

		auto container1 = example->add_child<FlexContainerView>(R"(
			flex-direction: row;
			justify-content: flex-end
			)");

		auto container2 = example->add_child<FlexContainerView>(R"(
			flex-direction: column;
			align-items: flex-end
			)");

		auto button = panel->add_child<FlexPanelButton>("Move flex items to the right");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example6()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Center everything");
		example->add_child<FlexParagraph>("Vertical and horizontal centering within a flexcontainer is trivial. Just set justify-content and/or align-items to center. Again, it depends on the orientation of the main axis, so whether flex-direction is set to row or column.");

		auto container1 = example->add_child<FlexContainerView>(R"(
			flex-direction: column;
			justify-content: center;
			align-items: center
			)");

		auto container2 = example->add_child<FlexContainerView>(R"(
			flex-direction: row;
			justify-content: center;
			align-items: center
			)");

		auto button = panel->add_child<FlexPanelButton>("Center everything");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example7()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Grow a flex item X times as big as other flex items");
		example->add_child<FlexParagraph>("You can define how an individual flex item grows and shrinks relative to other flex items in the container. To do this set the flex property on each flex item you want to grow or shrink.");

		auto container = example->add_child<FlexContainerView>("flex-direction: row", 2);
		container->boxes[0]->style()->set("flex: 2 0 0");
		container->boxes[1]->style()->set("flex: 1 0 0");

		auto button = panel->add_child<FlexPanelButton>("Grow a flex item X times as big as other flex items");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example8()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Wrap flex items into multiple rows");

		auto container = example->add_child<FlexContainerView>(R"(
			flex-flow: row wrap;
			align-items: center;
			justify-content: center;
			align-content: flex-end
			)", 12);

		auto button = panel->add_child<FlexPanelButton>("Wrap flex items into multiple rows");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example9()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Wrap flex items into multiple columns");

		auto container = example->add_child<FlexContainerView>(R"(
			flex-flow: column wrap;
			align-items: center;
			justify-content: center;
			align-content: stretch
			)", 8);

		auto button = panel->add_child<FlexPanelButton>("Wrap flex items into multiple columns");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example10()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Remove the space from wrapped rows or columns");
		example->add_child<FlexParagraph>("The align-content property gives you options for distributing the space around wrapped rows or columns. The options are flex-start, flex-end, space-between, space-around and stretch. To simply remove the space around wrapped columns, set align-content to center.");

		auto container = example->add_child<FlexContainerView>(R"(
			flex-direction: column;
			align-items: center;
			justify-content: center;
			flex-flow: column wrap;
			align-content: center
			)", 8);

		auto button = panel->add_child<FlexPanelButton>("Remove the space from wrapped rows or columns");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	void add_example11()
	{
		auto example = examples->add_child<FlexExampleView>();
		example->add_child<FlexHeadline>("Pin an element to one side of the flex container");
		example->add_child<FlexParagraph>("You can control the align-items value for individual elements with align-self. You can also use margins to move individual elements up or down or left of right. e.g. on a column layout you can move an individual flex item all the way to the left of the container by setting margin-right: auto.");

		auto container = example->add_child<FlexContainerView>(R"(
			flex-direction: column;
			align-items: center
			)");
		container->boxes[1]->style()->set("align-self: flex-start");
		container->boxes[2]->style()->set("margin-left: auto");

		auto button = panel->add_child<FlexPanelButton>("Pin an element to one side of the flex container");
		slots.connect(button->sig_clicked(), [=] { for (auto &v : examples->children()) v->set_hidden(v != example); });
	}

	std::shared_ptr<uicore::ColumnView> panel;
	std::shared_ptr<View> examples;
};
