/*
 * optimizer.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2015 - Dennis Prochko
 *
 * dbPager Server is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * dbPager Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dbPager Server; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <typeinfo>

#include "dbpager/context.h"
#include "optimizer/optimizer.h"

//#include <dbpager/tag_usr.h>

//#include <iostream>
namespace dbpager {

tag* optimizer::optimize(tag *tree) {
/*	local_context ctx;
	tree->apply(ctx, create_delegate(this, &optimizer::optimize_orphans));
	tree->apply(ctx, create_delegate(this, &optimizer::optimize_constants));*/
	return tree;
}

void optimizer::optimize_orphans(tag *tag) {
/*	if (typeid(*tag) == typeid(tag_tag))
		funcs.push_back(tag->get_name());*/
}

void optimizer::optimize_constants(tag *tag) {
/*
если тэг это tag_expression, но не содержит выражения, то заменить текст его parent на себя а себя удалить
в тех тегах, которым нужна оптимизация, в перекрытом методе set_text оптимизировать данные (компиляция и т.п.): это tag_regexp, tag_usr

	if (typeid(*tag) == typeid(tag_usr))
		std::cerr << "USR> " << tag->get_name() << " (" << tag->get_text() << ")" << std::endl;
	else
		std::cerr << typeid(*tag).name() << "> " << tag->get_name() << " (" << tag->get_text() << ")" << std::endl;
*/
}

} // namespace

