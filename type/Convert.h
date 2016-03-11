/**
*   usage : 1.  convert between types with one function call.
*
*   note :  1.  require operator<< and operator>> interface.
*           2.  for a string, always extract the first token defined by operator>>,
*               if it is an illegal token, the behavior is undefined.
*/

#ifndef CONVERT_H
#define CONVERT_H


#include <string>
#include <sstream>


namespace szx
{

/// if there is `#define x  y`, VERBATIM_STRINGIFY(x) will get "x".
#define VERBATIM_STRINGIFY(x)  #x
/// if there is `#define x  y`, RESOLVED_STRINGIFY(x) will get "y".
#define RESOLVED_STRINGIFY(x)  VERBATIM_STRINGIFY(x)

template <typename T_INPUT>
std::string toString( const T_INPUT &in )
{
    std::ostringstream oss;
    oss << in;
    return oss.str();
}

template <typename T_OUTPUT>
T_OUTPUT toType( const std::string& s )
{
    std::istringstream iss( s );
    T_OUTPUT output;
    iss >> output;
    return output;
}

}


#endif