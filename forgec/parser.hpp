//
//  parser.hpp
//  forgec
//
//  Created by Uli Kusterer on 12.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp

#include <vector>
#include "tokenizer.hpp"
#include "forgelib.hpp"


namespace forge {
	
	enum skip_type {
		skip,
		peek
	};
	
	class parameter_declaration {
	public:
		std::string	mName;
		bool		mIsReference = false;
		void	print( std::ostream &dest ) {
			if (mIsReference) {
				dest << "@";
			}
			dest << mName;
		}
	};
	
	class handler_call : public stack_suitable_value {
	public:
		std::string							mName;
		std::vector<stack_suitable_value *>	mParameters;

		virtual void		set( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual void		set( double inNum );
		virtual double		get_double() const;
		
		virtual void		set( std::string inString );
		virtual std::string	get_string() const;
		
		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

		void	print( std::ostream &dest ) {
			dest << get_string() << std::endl;
		}
	};
	
	class loop_call: public handler_call {
	public:
		std::vector<handler_call *>	mCommands;

		virtual std::string	get_string() const;
	};
	
	class handler_definition {
	public:
		std::string							mName;
		std::vector<parameter_declaration>	mParameters;
		std::vector<handler_call *>			mCommands;
		
		void	print( std::ostream &dest ) {
			dest << "on " << mName;
			for (auto p : mParameters) {
				dest << " ";
				p.print(dest);
			}
			dest << std::endl;
			
			for (auto c : mCommands) {
				dest << "\t";
				c->print(dest);
				dest << std::endl;
			}
			dest << "end " << mName << std::endl;
		}
	};
	
	class script {
	public:
		std::vector<handler_definition>		mHandlers;
		
		template<class T>
		T *	take_ownership_of(T *inValue) { mValuePool.push_back(std::unique_ptr<stack_suitable_value>(inValue)); return inValue; }

		void	print( std::ostream &dest ) { for (auto h : mHandlers) { h.print(dest); } }
		
	protected:
		std::vector<std::unique_ptr<stack_suitable_value>> mValuePool;
	};
	
	class parser {
	public:
		void	parse( std::vector<token>& inTokens, script &outScript );
		
	protected:
		void	skip_empty_lines();
		void	skip_rest_of_line();
		
		void	parse_handler( identifier_type inType, handler_definition &outHandler );
		void	parse_parameter_declaration( std::vector<parameter_declaration> &outParameters );
		stack_suitable_value	*parse_expression();
		stack_suitable_value	*parse_one_value();
		void					parse_one_line(std::vector<handler_call *> &outCommands);

		void	throw_parse_error( const char *msg ) __attribute__((noreturn));
		
		const token			*expect_token_type( token_type inType, skip_type inSkip = skip_type::skip );
		bool				expect_identifier( identifier_type inType, skip_type inSkip = skip_type::skip );
		const std::string	*expect_unquoted_string( const std::string inStr = std::string() );
		const std::string	*expect_string();

		std::vector<token> 					*mTokens = nullptr;
		std::vector<token>::const_iterator	mCurrToken;
		script								*mScript = nullptr;
	};
	
}

#endif /* parser_hpp */
