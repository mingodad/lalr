#ifndef SWEET_PARSER_ERRORCODE_HPP_INCLUDED
#define SWEET_PARSER_ERRORCODE_HPP_INCLUDED

namespace sweet
{

namespace parser
{

/**
// Error codes reported by the %parser library.
*/
enum ErrorCode
{
    PARSER_ERROR_NONE, ///< No %error.
    PARSER_ERROR_OPENING_FILE_FAILED, ///< Opening a grammar file failed.
    PARSER_ERROR_PARSING_FAILED, ///< Parsing a grammar failed.
    PARSER_ERROR_UNEXPECTED, ///< An unexpected %error occured.
    PARSER_ERROR_SYNTAX, ///< Syntax %error occured while parsing some input.
    PARSER_ERROR_PARSE_TABLE_CONFLICT, ///< A shift-reduce or reduce-reduce conflict was found in the parse table.
    PARSER_ERROR_UNDEFINED_SYMBOL, ///< A grammar symbol is referenced but not defined.
    PARSER_ERROR_UNREFERENCED_SYMBOL, ///< A grammar symbol is defined but not referenced.
    PARSER_ERROR_ERROR_SYMBOL_ON_LEFT_HAND_SIDE ///< The 'error' symbol has been used on the left hand side of a production.
};

}

}

#endif
