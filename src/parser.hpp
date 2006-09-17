/*
 * This file part of makedict - convertor from any dictionary format to any
 * http://sdcv.sourceforge.net
 * Copyright (C) 2005 Evgeniy <dushistov@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <glib.h>

#include "file.hpp"
#include "repository.hpp"

typedef std::map<std::string, std::string> StringMap;
typedef std::vector<std::string> StringList;

class IParserDictOps {
public:
	virtual ~IParserDictOps() {}
	virtual void set_dict_info(const std::string&, const std::string&) = 0;
	virtual void send_meta_info() = 0;
	virtual void send_info() = 0;

	virtual void abbrs_begin() = 0;
	virtual void abbrs_end() = 0;
	virtual void abbr(const StringList&, const std::string&) = 0;
	virtual void article(const StringList&, const std::string&, bool) = 0;
	virtual void end() = 0;
};

class PipeParserDictOps : public IParserDictOps {
public:
	PipeParserDictOps(File& out): out_(out) {}
	void send_meta_info();
	void send_info();
	void set_dict_info(const std::string& name, const std::string& val) {
		dict_info_[name] = val;
	}
	void abbrs_begin();
	void abbrs_end();
	void abbr(const StringList&, const std::string&);
	void article(const StringList&, const std::string&, bool);
	void end();
private:
	StringMap dict_info_;
	File &out_;
};

class ParserBase {
public:
	ParserBase(bool generate_xdxf = true);
	virtual ~ParserBase() {}
	int run(int argc, char *argv[]);
	int run(const StringList& options, const std::string& url);
	const std::string& format() const;
	virtual bool is_my_format(const std::string& url) { return false; }
	void reset_ops(IParserDictOps *dict_ops) { 
		if (dict_ops) 
			dict_ops_ = dict_ops; 
		else
			dict_ops_ = std_dict_ops_.get();
	}
protected:
	std::auto_ptr<IParserDictOps> std_dict_ops_;
	IParserDictOps *dict_ops_;
	StringMap parser_options_;

	virtual int parse(const std::string& url);

	void set_parser_info(const std::string& key, const std::string& val);
	void set_dict_info(const std::string& key, const std::string& val);

	void meta_info();
	void begin();
	void abbrs_begin();
	void abbrs_end();
	void abbr(const StringList& keys, const std::string& val);
	void article(const StringList& keys, const std::string& val,
		     bool keys_in_article);

	//usefull routine for some parsers
	std::set<gunichar> not_valid_chars;
	void remove_not_valid(std::string &str);
	virtual void basename(const std::string& url);
private:
	StringMap parser_info_;
	bool generate_xdxf_;

	bool parse_option(const std::string& optarg);
	int do_run(const std::string& url);
};

class ParsersRepo : public CodecsRepo<ParserBase, ParsersRepo> {
public:
	ParserBase *create_suitable_parser(const std::string& url);
};

#define REGISTER_PARSER(Class,name) \
	class Class##name##Registrator : public ICreator<ParserBase> { \
	public: \
		Class##name##Registrator() { \
			ParsersRepo::get_instance().register_codec(#name, this); \
		} \
		ParserBase *create() const { \
			return new Class; \
		} \
	} Class##name##_registrator

#endif//!PARSER_HPP
