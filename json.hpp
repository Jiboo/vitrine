//          Copyright Lepesme "Jiboo" Jean-Baptiste 2012
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <istream>
#include <ostream>
#include <cassert>
#include <sstream>
#include <tuple>

#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/locale.hpp>

namespace json
{
	class value;
	
	typedef std::map<std::string, value> object;
	typedef std::vector<value> array;
	typedef boost::variant<
		long long,
		long double,
		std::string,
		boost::recursive_wrapper<array>,
		boost::recursive_wrapper<object>
		> __variant;

	class value
	{
	public:
		enum class types
		{
			NONE,
			STRING,
			REAL,
			INTEGER,
			OBJECT,
			ARRAY,
			BOOLEAN,
			NILL,
		} type;
		
		__variant variant;
	
	public:
		value()							: type(types::NILL),	variant(static_cast<long long>(0)) {}
		value(bool val)					: type(types::BOOLEAN),	variant(static_cast<long long>(val)) {}
		value(int val)					: type(types::INTEGER),	variant(static_cast<long long>(val)) {}
		value(long long val)			: type(types::INTEGER),	variant(static_cast<long long>(val)) {}
		value(double val)				: type(types::REAL),	variant(static_cast<long double>(val)) {}
		value(long double val)			: type(types::REAL),	variant(static_cast<long double>(val)) {}
		value(const char *val)			: type(types::STRING),	variant(std::string(val)) {}
		value(const std::string &val)	: type(types::STRING),	variant(val) {}
		value(const std::string &&val)	: type(types::STRING),	variant(val) {}
		value(const array &val)			: type(types::ARRAY),	variant(val) {}
		value(const array &&val)		: type(types::ARRAY),	variant(val) {}
		value(const object &val)		: type(types::OBJECT),	variant(val) {}
		value(const object &&val)		: type(types::OBJECT),	variant(val) {}

		bool is_null()		{ return this->type == types::NILL; }
		bool is_bool()		{ return this->type == types::BOOLEAN; }
		bool is_integer()	{ return this->type == types::INTEGER; }
		bool is_real()		{ return this->type == types::REAL; }
		bool is_number()	{ return this->is_integer() || this->is_real(); }
		bool is_string()	{ return this->type == types::STRING; }
		bool is_array()		{ return this->type == types::ARRAY; }
		bool is_object() 	{ return this->type == types::OBJECT; }

	private:
		template<typename tCastType>
		tCastType cast()
		{
			return boost::get<tCastType>(this->variant);
		}
		
		void array_add(const value &v)
		{
			assert(this->type == types::ARRAY);
			boost::get<array&>(this->variant).push_back(v);
		}
		
		void object_add(const std::string &k, const value &v)
		{
			assert(this->type == types::OBJECT);
			//boost::get<object&>(this->variant).insert({k, v});
			boost::get<object&>(this->variant).insert(std::make_pair(k, v));
		}
	
	public:
		bool& to_bool()
		{
			assert(this->type == types::BOOLEAN);
			return (bool&)this->cast<long long&>();
		}
		
		long long& to_integer()
		{
			assert(this->type == types::INTEGER);
			return this->cast<long long&>();
		}
		
		long double& to_real()
		{
			assert(this->type == types::REAL);
			return this->cast<long double&>();
		}
		
		std::string& to_string()
		{
			assert(this->type == types::STRING);
			return this->cast<std::string&>();
		}

		array& to_array()
		{
			assert(this->type == types::ARRAY);
			return this->cast<array&>();
		}
		
		object& to_object()
		{
			assert(this->type == types::OBJECT);
			return this->cast<object&>();
		}

		size_t size()
		{
			switch(this->type)
			{
			case types::ARRAY:
				return this->cast<array&>().size();
			case types::OBJECT:
				return this->cast<object&>().size();
			default:
				return 1;
			}
		}
		
		template<typename tVarType>
		void add(const tVarType &v)
		{
			this->array_add(v);
		}
		
		value& get(unsigned int i)
		{
			assert(this->type == types::ARRAY);
			return boost::get<array&>(this->variant).at(i);
		}
		
		template<typename tVarType>
		void add(const std::string &k, const tVarType &v)
		{
			this->object_add(k, v);
		}
		
		value& get(const std::string &k)
		{
			assert(this->type == types::OBJECT);
			return boost::get<object&>(this->variant).at(k);
		}
	};

	class parser
	{
	public:
		class parsing_error : public std::runtime_error
		{
		private:
			int row, col;

		public:
			parsing_error(int r, int c, const char *m) : std::runtime_error(m), row(r), col(c) {}
			const std::tuple<int, int> get_pos() { return std::tuple<int, int>(row, col); }
		};
		
	private:
		std::istream &stream;
		
		int col, row;
		
		void error(const char *mess)
		{
			throw parsing_error(this->row, this->col, mess);
		}
		
		void check_status()
		{
			if(!stream.good())
				this->error("Unexpected end of stream");
		}

		void trim()
		{
			std::string spaces(" \t\n\r"); //FIXME Faster with regexp?
			while(spaces.find(stream.peek()) != std::string::npos)
			{
				if(stream.peek() == '\n')
				{
					row++;
					col = 1;
				}
				else
					col++;
				stream.get();
				check_status();
			}
		}
		
		int next()
		{
			trim();
			return stream.peek();
		}
		
		int pop()
		{
			col++;
			return stream.get();
		}
		
		int check_pop(const std::string &chars)
		{
			this->trim();
			if(chars.find(this->next()) == std::string::npos)
				this->error("Unexpected character");

			return this->pop();
		}
		
		void check_pop_string(const std::string &str)
		{
			this->trim();
			
			for(const char c : str)
			{
				if(this->pop() != c)
					this->error("Unexpected character");
			}
		}
		
		std::string parse_unicode_sequence()
		{
			const char *test = "0123456789ABCDEFabcdef";
		
			std::stringstream buff;
			uint16_t uc;
			buff << (char)this->check_pop(test);
			buff << (char)this->check_pop(test);
			buff << (char)this->check_pop(test);
			buff << (char)this->check_pop(test); //FIXME ugly?
			buff >> std::hex >> uc;
			
			std::u16string us(1, uc);//FIXME Find a faster way than making a tmp string!
			return boost::locale::conv::utf_to_utf<char>(us);
		}
		
		std::string parse_string()
		{
			std::string ret;
			check_pop("\"");
			
			while(this->next() != '"')
			{
				if(this->next() == '\\')
				{
					this->pop();
					switch(this->pop())
					{
					case '"': ret += '"'; break;
					case '\\': ret += '\\'; break;
					case '/': ret += '/'; break;
					case 'b': ret += '\b'; break;
					case 'f': ret += '\f'; break;
					case 'n': ret += '\n'; break;
					case 'r': ret += '\r'; break;
					case 't': ret += '\t'; break;
					case 'u': ret += parse_unicode_sequence(); break;
					default:
						this->error("Unknow escape sequence");
					}
				}
				else
					ret += this->pop();
			}
			
			check_pop("\"");
			return ret;
		}
		
		value parse_number()
		{
			std::string chars("0123456789-+.eE");
			std::string buff;

			while(chars.find(this->next()) != std::string::npos)
				buff += this->pop();

			value ret = value();
			try
			{
				if(buff.find('.') != std::string::npos)
					ret = boost::lexical_cast<long double>(buff);
				else
					ret = boost::lexical_cast<long long>(buff);
			}
			catch(boost::bad_lexical_cast &e)
			{
				this->error("Can't decode number");
			}
			return ret;
		}
		
		value parse_value()
		{
			switch(this->next())
			{
			case '{': return parse_object();
			case '[': return parse_array();
			case '"': return parse_string();
			
			case 't': check_pop_string("true"); return true;
			case 'f': check_pop_string("false"); return false;
			case 'n': check_pop_string("null"); return value();
			
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
				return parse_number();
			
			default:
				this->error("Unexpected character");
			}
			return value();
		}

		object parse_object()
		{
			object obj;
			
			check_pop("{");
			while(this->next() != '}')
			{
				std::string k = parse_string();
				check_pop(":");
			
				//obj.insert({k, parse_value()});
				obj.insert(std::make_pair(k, parse_value()));
				if(this->next() == ',')
					this->pop();
				this->trim();
			}
			check_pop("}");
			
			return obj;
		}

		array parse_array()
		{
			array arr;
			
			check_pop("[");
			while(this->next() != ']')
			{
				arr.push_back(parse_value());
				if(this->next() == ',')
					this->pop();
				this->trim();
			}
			check_pop("]");
			
			return arr;
		}

	public:
		parser(std::istream &_stream) : stream(_stream), col(1), row(1) {}
		
		value parse()
		{
			switch(this->next())
			{
			case '{': return parse_object();
			case '[': return parse_array();
			default:
				this->error("JSON Root neither an object nor an array");
			}
			return value();
		}
	};

	inline std::ostream& operator<<(std::ostream &o, const json::array &arr);
	inline std::ostream& operator<<(std::ostream &o, const json::object &obj);
	
	inline std::ostream& print_escaped_string(std::ostream &o, const std::string &str)
	{
		o << "\"";
		for(char c : str)
		{
			switch(c)
			{
			case '"':	o << "\\\""; break;
			case '\\':	o << "\\\\"; break;
			case '/': 	o << "\\/"; break;
			case '\b':	o << "\\b"; break;
			case '\f':	o << "\\f"; break;
			case '\n':	o << "\\n"; break;
			case '\r':	o << "\\r"; break;
			case '\t':	o << "\\t"; break;
			
			default:
				//FIXME Handle unicode
				o << c;
			}
		}
		o << "\"";
		return o;
	}

	inline std::ostream& operator<<(std::ostream &o, const json::value &val)
	{
		switch(val.type)
		{
		case json::value::types::NONE:		return o;
		case json::value::types::NILL:		return o << "null";
		case json::value::types::BOOLEAN:	return o << ((boost::get<long long>(val.variant) != 0.) ? "true" : "false");
		case json::value::types::INTEGER:	return o << boost::get<long long>(val.variant);
		case json::value::types::REAL:		return o << boost::get<double>(val.variant);
		case json::value::types::STRING:	return print_escaped_string(o, boost::get<std::string>(val.variant));
		case json::value::types::ARRAY:		return o << boost::get<json::array>(val.variant);
		case json::value::types::OBJECT:	return o << boost::get<json::object>(val.variant);
		}
	
		// unreachable
		return o;
	}
	
	inline std::istream& operator>>(std::istream &i, json::value &val)
	{
		val = parser(i).parse();
		return i;
	}

	inline std::ostream& operator<<(std::ostream &o, const json::array &arr)
	{
		o << "[";
		for(auto it = arr.cbegin(); it != arr.cend(); it++)
		{
			if(it != arr.cbegin())
				o << ", ";
			o << *it;
		}
		return o << "]";
	}

	inline std::ostream& operator<<(std::ostream &o, const json::object &obj)
	{
		o << "{";
		for(auto it = obj.cbegin(); it != obj.cend(); it++)
		{
			if(it != obj.cbegin())
				o << ", ";
			o << '"' << (*it).first << "\" : " << (*it).second;
		}
		return o << "}";
	}

} //namespace json

