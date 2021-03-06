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
#include "UICore/UI/Style/style_tokenizer.h"
#include "UICore/UI/Style/style_token.h"
#include "UICore/UI/ViewCompiler/view_compiler.h"
#include "view_compiler_impl.h"

namespace uicore
{
	std::string ViewCompiler::compile(const std::string &source)
	{
		ViewCompilerImpl impl;
		impl.parse(source);
		return impl.codegen();
	}

	/////////////////////////////////////////////////////////////////////////

	void ViewCompilerImpl::parse(const std::string &parse_source)
	{
		source = parse_source;
		StyleTokenizer tokenizer(source);
		
		StyleToken token;
		tokenizer.read(token, true);
		while (true)
		{
			if (token.type == StyleTokenType::null)
				break;
			
			parse_view_declaration(token, tokenizer);
		}
	}
	
	std::string ViewCompilerImpl::codegen()
	{
		code.clear();

		add_line("");
		add_line("// Note: this file was autogenerated by the uicore view compiler");
		add_line("");
		add_line("#pragma once");
		add_line("");
		add_line("#include <uicore.h>");
		add_line("");
		add_line("namespace uiview");
		add_line("{");
		add_line("\tusing namespace uicore;");
		add_line("");

		for (const auto &it : class_types)
		{
			const ViewClassType &type = it.second;
			add_line("\tclass %1;", type.name);
		}

		for (const auto &it : class_types)
		{
			codegen_class(it.first);
		}

		for (const auto &it : class_types)
		{
			codegen_constructor(it.first);
		}

		add_line("}");

		return code;
	}

	void ViewCompilerImpl::codegen_class(const std::string &name)
	{
		declared_types.insert(name);

		const ViewClassType &type = class_types[name];
		if (type.name.empty())
			return;

		if (!type.base.empty())
		{
			if (declared_types.find(type.base) == declared_types.end())
				codegen_class(type.base);
		}

		add_line("");
		if (!type.base.empty())
			add_line("\tclass %1 : public %2", type.name, type.base);
		else
			add_line("\tclass %1 : public View", type.name);
		add_line("\t{");
		add_line("\tpublic:");
		add_line("\t\t%1();", type.name);

		codegen_class_children(type.children);

		add_line("\t};");
	}

	void ViewCompilerImpl::codegen_class_children(const std::vector<ViewClassChild> &children)
	{
		for (const auto &child : children)
		{
			if (child.type.empty())
				continue;
			add_line("\t\tstd::shared_ptr<%1> %2;", child.type, child.id);
			codegen_class_children(child.children);
		}
	}

	void ViewCompilerImpl::codegen_constructor(const std::string &name)
	{
		const ViewClassType &type = class_types[name];
		if (type.name.empty())
			return;

		add_line("");
		add_line("\tinline %1::%2()", type.name, type.name);
		add_line("\t{");

		codegen_constructor_add_child(type.children, "");
		codegen_constructor_set_style("", type);
		codegen_constructor_set_value("", type);

		add_line("\t}");
	}

	void ViewCompilerImpl::codegen_constructor_add_child(const std::vector<ViewClassChild> &children, const std::string &parent_prefix)
	{
		for (const auto &child : children)
		{
			if (child.type.empty())
				continue;
			add_line("\t\t%1 = %2add_child<%3>();", child.id, parent_prefix, child.type);
			codegen_constructor_add_child(child.children, string_format("%1%2->", parent_prefix, child.id));
		}
	}

	void ViewCompilerImpl::codegen_constructor_set_style(const std::string &name, const ViewClassMembers &members)
	{
		if (!members.style.empty())
			add_line("\t\t%1style()->set(\"%2\");", name, string_escape(members.style));

		for (const auto &child : members.children)
		{
			codegen_constructor_set_style(child.id + "->", child);
		}
	}

	void ViewCompilerImpl::codegen_constructor_set_value(const std::string &name, const ViewClassMembers &members)
	{
		if (!members.values.empty())
		{
			add_line("\t\t%1set_properties(", name);
			add_line("\t\t{");
			bool first = true;
			for (const auto &it : members.values)
			{
				if (first)
					add_line("\t\t\t{ \"%1\", \"%2\" }", string_escape(it.first), string_escape(it.second));
				else
					add_line("\t\t\t,{ \"%1\", \"%2\" }", string_escape(it.first), string_escape(it.second));
				first = false;
			}
			add_line("\t\t});");
		}

		for (const auto &child : members.children)
		{
			codegen_constructor_set_value(child.id + "->", child);
		}
	}

	void ViewCompilerImpl::add_line(const std::string &text)
	{
		code += text;
#ifdef WIN32
		code += "\r\n";
#else
		code += "\n";
#endif
	}

	std::string ViewCompilerImpl::string_escape(const std::string &text)
	{
		return text; // To do: escape string
	}

	void ViewCompilerImpl::parse_view_declaration(StyleToken &token, StyleTokenizer &tokenizer)
	{
		ViewClassType type;
		
		if (token.type != StyleTokenType::ident || token.value != "view")
			throw_parse_error(token, "Expected 'view' keyword");
		tokenizer.read(token, true);

		if (token.type != StyleTokenType::ident)
			throw_parse_error(token, "Expected view identifier");
		type.name = token.value;

		if (class_types.find(type.name) != class_types.end())
		{
			throw_parse_error(token, string_format("View '%1' already declared", type.name).c_str());
		}

		tokenizer.read(token, true);
		
		if (token.type == StyleTokenType::colon)
		{
			tokenizer.read(token, true);
			if (token.type != StyleTokenType::ident)
				throw_parse_error(token, "Expected base view identifier");
			type.base = token.value;
			tokenizer.read(token, true);
		}
		else if (token.type == StyleTokenType::delim && token.value == "=")
		{
			tokenizer.read(token, true);
			if (token.type != StyleTokenType::ident)
				throw_parse_error(token, "Expected native class identifier");
			type.native_type = token.value;
			tokenizer.read(token, true);
		}
		
		if (token.type == StyleTokenType::curly_brace_begin)
		{
			tokenizer.read(token, true);
			while (true)
			{
				if (token.type == StyleTokenType::curly_brace_end)
					break;
				parse_view_member(type, token, tokenizer);
			}
			
			tokenizer.read(token, true);
			if (token.type == StyleTokenType::semi_colon)
				tokenizer.read(token, true);
		}
		else if (token.type != StyleTokenType::semi_colon)
		{
			throw_parse_error(token, "Expected ';' or '{'");
		}

		class_types[type.name] = type;
	}
	
	void ViewCompilerImpl::parse_view_member(ViewClassMembers &type, StyleToken &token, StyleTokenizer &tokenizer)
	{
		if (token.type != StyleTokenType::ident)
			throw_parse_error(token, "Expected identifier");
		
		std::string identifier1 = token.value;
		tokenizer.read(token, true);
		
		if (token.type == StyleTokenType::colon)
		{
			tokenizer.read(token, true);
			parse_style(type, identifier1, token, tokenizer);
		}
		else if (token.type == StyleTokenType::delim && token.value == "=")
		{
			tokenizer.read(token, true);
			parse_value(type, identifier1, token, tokenizer);
		}
		else if (token.type == StyleTokenType::ident)
		{
			parse_add_children(type, identifier1, token, tokenizer);
		}
		else
		{
			parse_update_child(type, identifier1, token, tokenizer);
		}
	}
	
	void ViewCompilerImpl::parse_style(ViewClassMembers &type, std::string property_name, StyleToken &token, StyleTokenizer &tokenizer)
	{
		size_t style_start = token.offset;

		bool important_flag = false;
		tokenizer.read_property_value(token, important_flag);

		size_t style_end = token.offset;

		if (token.type != StyleTokenType::semi_colon)
			throw_parse_error(token, "Expected ';'");
		tokenizer.read(token, true);

		std::string property_value = source.substr(style_start, style_end - style_start);

		if (!type.style.empty())
			type.style += "; ";
		type.style += property_name;
		type.style += ": ";
		type.style += property_value;
	}
	
	void ViewCompilerImpl::parse_value(ViewClassMembers &type, std::string value_name, StyleToken &token, StyleTokenizer &tokenizer)
	{
		// To do: parse full JSON value for more flexibility
		
		if (token.type != StyleTokenType::string)
			throw_parse_error(token, "Expected string");
		
		std::string value;
		while (token.type == StyleTokenType::string)
		{
			value += token.value;
			tokenizer.read(token, true);
		}

		type.values[value_name] = value;
		
		if (token.type != StyleTokenType::semi_colon)
			throw_parse_error(token, "Expected ';'");
		tokenizer.read(token, true);
	}
	
	void ViewCompilerImpl::parse_add_children(ViewClassMembers &type, std::string child_type, StyleToken &token, StyleTokenizer &tokenizer)
	{
		while (true)
		{
			if (token.type != StyleTokenType::ident)
				throw_parse_error(token, "Expected identifier");
			
			ViewClassChild child;
			child.type = child_type;
			child.id = token.value;
			
			tokenizer.read(token, true);
			
			if (token.type == StyleTokenType::curly_brace_begin)
			{
				tokenizer.read(token, true);
				while (true)
				{
					if (token.type == StyleTokenType::curly_brace_end)
						break;
					parse_view_member(child, token, tokenizer);
				}
				
				tokenizer.read(token, true);
			}
			
			type.children.push_back(child);
			
			if (token.type == StyleTokenType::semi_colon)
				break;
			
			if (token.type == StyleTokenType::delim && token.value == ",")
				tokenizer.read(token, true);
			else
				throw_parse_error(token, "Expected identifier");
		}
		tokenizer.read(token, true);
	}
	
	void ViewCompilerImpl::parse_update_child(ViewClassMembers &type, std::string child_id, StyleToken &token, StyleTokenizer &tokenizer)
	{
		if (token.type != StyleTokenType::curly_brace_begin)
			throw_parse_error(token, "Expected '{'");

		ViewClassChild child;
		child.id = child_id;
		
		tokenizer.read(token, true);
		while (true)
		{
			if (token.type == StyleTokenType::curly_brace_end)
				break;
			parse_view_member(child, token, tokenizer);
		}
		
		type.children.push_back(child);
		
		tokenizer.read(token, true);
		if (token.type == StyleTokenType::semi_colon)
			tokenizer.read(token, true);
	}
	
	void ViewCompilerImpl::throw_parse_error(const StyleToken &token, const char *message)
	{
		int line = 1;
		for (size_t i = 0; i < token.offset; i++)
		{
			if (source[i] == '\n')
				line++;
		}
		throw Exception(string_format("(%1): %2", line, message));
	}
	
}
