/*
 * tag_expression.cpp
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

#include <sstream>
#include <math.h>

#include <dcl/strutils.h>

#include "tag/tag_expression.h"
#include "tag/tag_factory.h"
#include <dbpager/consts.h>
#include <dbpager/tag_usr.h>

namespace dbpager {

using namespace std;
using namespace dbp;

void tag_expression::execute(context &ctx, std::ostream &out, const tag *caller) const {
	out << evaluator(this, get_text()).evaluate(ctx);
}

string tag_expression::evaluator::evaluate(context &ctx) const {
	return eval(ctx, _expr);
}

string tag_expression::evaluator::eval(context &ctx,
  const string &expression) const {
	istringstream in(expression, istringstream::in | istringstream::binary);
	ostringstream out(ostringstream::out | ostringstream::binary);
	char curr_ch, prev_ch = 0;
	int br_count = 0;
	string curr_expr("");
	while (in.get(curr_ch)) {
		// if a "\symbol" found, output as is
		if (prev_ch == '\\') {
			out << curr_ch;
			prev_ch = curr_ch;
			continue;
		}
		switch (curr_ch) {
			case '\\':
				break;
			case '{':
				br_count++;
				if (br_count > 1)
					curr_expr += curr_ch;
				break;
			case '}':
				br_count--;
				if (br_count == 0) {
					out << calc(ctx, eval(ctx, curr_expr));
					curr_expr.clear();
				} else
					curr_expr += curr_ch;
				break;
			default:
				if (br_count > 0)
					curr_expr += curr_ch;
				else
					out << curr_ch;

		}
		prev_ch = curr_ch;
	}
	if (br_count != 0)
		throw evaluator_exception(_("unbalanced expression braces found"));
	return out.str();
}

string tag_expression::evaluator::calc(context &ctx,
  const string &expression) const {
	istringstream in(expression, istringstream::binary);
	token curr_tok;
	while (1) {
		get_token(in, curr_tok);
		if (curr_tok.type == token::END) return "";
		return lexpr(ctx, in, curr_tok);
	}
}

void tag_expression::evaluator::get_token(istream &in,
  token &curr_tok) const {
	char ch = 0;
	// skip whitespace
	do {
		if (!in.get(ch)) { curr_tok.type = token::END; return; }
	} while (ch != '\n' && isspace(ch));
	// determine token type
	switch (ch) {
		case '*': curr_tok.type = token::MUL; return;
		case '/': curr_tok.type = token::DIV; return;
		case '%': curr_tok.type = token::MOD; return;
		case '^': curr_tok.type = token::PWR; return;
		case '+': curr_tok.type = token::PLUS; return;
		case '-': curr_tok.type = token::MINUS; return;
		case '(': curr_tok.type = token::LP; return;
		case ')': curr_tok.type = token::RP; return;
		case '<':
			if (in.get(ch)) {
				switch (ch) {
					case '=': curr_tok.type = token::LTEQ; return;
					case '>': curr_tok.type = token::NOTEQ; return;
					default:
						in.putback(ch);
						curr_tok.type = token::LT;
				}
			}
			return;
		case '>':
			if (in.get(ch) && (ch == '='))
				curr_tok.type = token::GTEQ;
			else {
				in.putback(ch);
				curr_tok.type = token::GT;
			}
			return;
		case '!':
			if (in.get(ch) && (ch == '='))
				curr_tok.type = token::NOTEQ;
			else {
				in.putback(ch);
				curr_tok.type = token::NOT;
			}
			return;
		case '=': curr_tok.type = token::EQ; return;
		case '&': curr_tok.type = token::AND; return;
		case '|': curr_tok.type = token::OR; return;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '.':
			in.putback(ch);
			while (in.get(ch) && (isdigit(ch) || (ch == '.')))
				curr_tok.value += ch;
			in.putback(ch);
			curr_tok.type = token::NUMBER;
			return;
		case '$': {
				curr_tok.name = "";
/* if quoted characters in variable name will required in the future
				char prev_ch = 0;
				while (in.get(ch) && (isalnum(ch) || (ch == '_') || (ch == '\\') || (prev_ch == '\\'))) {
					if (prev_ch == '\\') {
						curr_tok.name += ch;
						prev_ch = ch;
						continue;
					}
					prev_ch = ch;
					if (ch == '\\') continue;
					curr_tok.name += ch;
				}
*/
				while (in.get(ch) && (isalnum(ch) || (ch == '_') || (ch == '-')))
					curr_tok.name += ch;
				in.putback(ch);
				curr_tok.type = token::NAME;
				return;
		}
		case '\'': case '"': {
				char end = ch;
				do {
					if (!in.get(ch)) {
						throw evaluator_exception(
						  (format(_("invalid expression: {0} expected")) % ch).str());
					}
					if (ch != end)
						curr_tok.value += ch;
				} while (ch != end);
				curr_tok.type = token::NUMBER;
				return;
		}
		default:
			if (isalpha(ch) || (ch == '_') || (ch == '@')) {
				in.putback(ch);
				// read function name
				while (in.get(ch) &&
				  (isalnum(ch) || (ch == '_') || (ch == '@')))
					curr_tok.name += ch;
				if (ch != '(')
					throw evaluator_exception(
					  (format(_("parameter(s) are not defined for function"
					    " '{0}', expected '(', found '{1}'")) %
					    curr_tok.name % ch).str());
				// read function parameters
				int braces = 1;
				while (in.get(ch) && (braces > 0)) {
					switch (ch) {
						case '(':
							braces++;
						case ')':
							braces--;
							if (braces == 0)
								break;
						default:
							curr_tok.value += ch;
					}
				}
				if (braces != 0)
					throw evaluator_exception(
					  _("invalid expression: \")\" expected"));
				curr_tok.type = token::FUNC;
				return;
			}
			throw evaluator_exception(
			  (format(_("unknown expression token '{0}'")) % ch).str());
	}
}

string tag_expression::evaluator::lexpr(context &ctx, istream &in,
  tag_expression::evaluator::token &curr_tok) const {
	string left = expr(ctx, in, curr_tok);
	for (;;)
		switch (curr_tok.type) {
			case token::LT:
				get_token(in, curr_tok);
				try {
					if (from_string<double>(left) <
					  from_string<double>(expr(ctx, in, curr_tok)))
						return "1";
					else
						return "0";
				}
				catch (...) {
					if (left < expr(ctx, in, curr_tok))
						return "1";
					else
						return "0";
				}
				break;
			case token::GT:
				get_token(in, curr_tok);
				try {
					if (from_string<double>(left) >
					  from_string<double>(expr(ctx, in, curr_tok)))
						return "1";
					else
						return "0";
				}
				catch (...) {
					if (left > expr(ctx, in, curr_tok))
						return "1";
					else
						return "0";
				}
				break;
			case token::EQ:
				get_token(in, curr_tok);
				try {
					if (from_string<double>(left) ==
					  from_string<double>(expr(ctx, in, curr_tok)))
						return "1";
					else
						return "0";
				}
				catch (...) {
					if (left == expr(ctx, in, curr_tok))
						return "1";
					else
						return "0";
				}
				break;
			case token::LTEQ:
				get_token(in, curr_tok);
				try {
					if (from_string<double>(left) <=
					  from_string<double>(expr(ctx, in, curr_tok)))
						return "1";
					else
						return "0";
				}
				catch (...) {
					if (left <= expr(ctx, in, curr_tok))
						return "1";
					else
						return "0";
				}
				break;
			case token::GTEQ:
				get_token(in, curr_tok);
				try {
					if (from_string<double>(left) >=
					  from_string<double>(expr(ctx, in, curr_tok)))
						return "1";
					else
						return "0";
				}
				catch (...) {
					if (left >= expr(ctx, in, curr_tok))
						return "1";
					else
						return "0";
				}
				break;
			case token::NOTEQ:
				get_token(in, curr_tok);
				try {
					if (from_string<double>(left) !=
					  from_string<double>(expr(ctx, in, curr_tok)))
						return "1";
					else
						return "0";
				}
				catch (...) {
					if (left != expr(ctx, in, curr_tok))
						return "1";
					else
						return "0";
				}
				break;
			case token::AND:
				get_token(in, curr_tok);
				if ((left == "1" && (expr(ctx, in, curr_tok) == "1")))
					return "1";
				else
					return "0";
				break;
			case token::OR:
				get_token(in, curr_tok);
				if ((left == "1" || (expr(ctx, in, curr_tok) == "1")))
					return "1";
				else
					return "0";
				break;
			default:
				return left;
		}
}

string tag_expression::evaluator::expr(context &ctx, istream &in,
  tag_expression::evaluator::token &curr_tok) const {
	string left = term(ctx, in, curr_tok);
	try {
		for (;;)
			switch (curr_tok.type) {
				case token::PLUS:
					get_token(in, curr_tok);
					left = to_string<double>(from_string<double>(left) +
					  from_string<double>(term(ctx, in, curr_tok)));
					break;
				case token::MINUS:
					get_token(in, curr_tok);
					left = to_string<double>(from_string<double>(left) -
					  from_string<double>(term(ctx, in, curr_tok)));
					break;
				default:
					return left;
			}
	}
	catch (type_conversion_exception &e) {
		throw evaluator_exception(
		  _("can't convert the string to a number"));
	}
}

string tag_expression::evaluator::term(context &ctx, istream &in,
  tag_expression::evaluator::token &curr_tok) const {
	string left = prim(ctx, in, curr_tok);
	try {
		for (;;)
			switch (curr_tok.type) {
				case token::MUL:
					get_token(in, curr_tok);
					left = to_string<double>(from_string<double>(left) *
					  from_string<double>(prim(ctx, in, curr_tok)));
					break;
				case token::DIV:
					get_token(in, curr_tok);
					left = to_string<double>(from_string<double>(left) /
					  from_string<double>(prim(ctx, in, curr_tok)));
					break;
				case token::MOD:
					get_token(in, curr_tok);
					left = to_string<double>(fmod(from_string<double>(left),
					  from_string<double>(prim(ctx, in, curr_tok))));
					break;
				case token::PWR:
					get_token(in, curr_tok);
					left = to_string<double>(pow(from_string<double>(left),
					  from_string<double>(prim(ctx, in, curr_tok))));
					break;
				default:
					return left;
			}
	}
	catch (type_conversion_exception &e) {
		throw evaluator_exception(
		  _("can't convert the string to a number"));
	}
}

string tag_expression::evaluator::prim(context &ctx, istream &in,
  tag_expression::evaluator::token &curr_tok) const {
	switch (curr_tok.type) {
		case token::FUNC:
			get_token(in, curr_tok);
			{
				string rslt = execute_function(ctx, curr_tok.name, curr_tok.value);
				curr_tok.value = "";
				return rslt;
			}
		case token::NUMBER:
			get_token(in, curr_tok);
			{
				string rslt = curr_tok.value;
				curr_tok.value = "";
				return rslt;
			}
		case token::NAME:
			get_token(in, curr_tok);
			{
				string rslt = curr_tok.name;
				curr_tok.name = "";
				return ctx.get_value(rslt);
			}
		case token::MINUS:
			get_token(in, curr_tok);
			try {
				return to_string<double>(-1.0 * from_string<double>(prim(ctx, in,
				  curr_tok)));
			}
			catch (type_conversion_exception &e) {
				throw evaluator_exception(
				  _("can't convert the string to a number"));
			}
		case token::NOT:
			get_token(in, curr_tok);
			if (prim(ctx, in, curr_tok) == "1")
				return "0";
			else
				return "1";
		case token::LP:
			get_token(in, curr_tok);
			{
				string e = lexpr(ctx, in, curr_tok);
				if (curr_tok.type != token::RP)
					throw evaluator_exception(_("invalid expression: \")\" expected"));
				get_token(in, curr_tok);
				return e;
			}
		case token::END:
			return "1";
		default:
			throw evaluator_exception(_("invalid expression: primary expected"));
	}
}

string tag_expression::evaluator::execute_function(context &ctx, string &name,
  string &params) const {
	// try to create a function with a factory
	tag_factory &_factory = tag_factory::instance();
	tag *rslt = NULL;
	if (!name.empty() && name[0] != '@') {
		// if the function isn't starts with @ symbol - its internal
		rslt = _factory.create(dbpager_uri, "", "@" + name);
	} else {
		// if it starts with @ - its declared in the script as dbp:tag
		rslt = new tag_usr(name.substr(1));
	}
	try {
		rslt->set_parent(_parent);
		// add all parameters as arguments
		vector<string> p = tokenize()(params);
		int cnt = 1;
		for (vector<string>::const_iterator i = p.begin();
		  i != p.end(); ++i, ++cnt) {
		  	tag *param = new tag_expression();
		  	param->set_parent(rslt);
		  	param->set_text("{" + *i + "}");
			rslt->add_parameter("arg" + to_string<int>(cnt), param);
		}
		// execute custom tag
		ostringstream s(ostringstream::out | ostringstream::binary);
		rslt->execute(ctx, s, NULL);
		// clean up
		delete rslt;
		// return tag output
		return s.str();
	}
	catch (...) {
		delete rslt;
		throw;
	}
}

} // namespace
