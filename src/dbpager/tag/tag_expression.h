/*
 * tag_expression.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko
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

#ifndef _TAG_EXPRESSION_H_
#define _TAG_EXPRESSION_H_

#include <string>
#include <istream>
#include <ostream>

#include <dcl/exception.h>

#include <dbpager/tag_impl.h>

namespace dbpager {

class evaluator_exception: public dbp::exception {
public:
	evaluator_exception(const std::string &msg): dbp::exception(msg) { }
};

class tag_expression: public tag_impl {
public:
	tag_expression(const std::string &tag_name = ""): tag_impl(tag_name) { };
	virtual void execute(context &ctx, std::ostream &out, const tag *caller) const;
private:
	class evaluator {
	public:
		//! Initializes the evaluator with expression
		evaluator(const tag *parent, const std::string &expression = ""):
		  _expr(expression), _parent(parent) { };
		//! Evaluates the given expression
		std::string evaluate(context &ctx) const;
	private:
		std::string _expr;
		const tag *_parent;
		class token {
		public:
			enum token_type {
				NAME, FUNC, NUMBER, END,
				PLUS, MINUS, MUL, DIV, MOD, PWR, LP, RP,
				LT, GT, EQ, LTEQ, GTEQ, NOTEQ, AND, OR, NOT
			};
			token_type type;
			std::string name, value;
		};
		std::string eval(context &ctx, const std::string &expression) const;
		std::string calc(context &ctx, const std::string &expression) const;
		void get_token(std::istream &in, token &curr_tok) const;
		std::string lexpr(context &ctx, std::istream &in, token &curr_tok) const;
		std::string expr(context &ctx, std::istream &in, token &curr_tok) const;
		std::string term(context &ctx, std::istream &in, token &curr_tok) const;
		std::string prim(context &ctx, std::istream &in, token &curr_tok) const;
		std::string execute_function(context &ctx, std::string &name,
		  std::string &params) const;
	};
};

}

#endif /*_TAG_EXPRESSION_H_*/

