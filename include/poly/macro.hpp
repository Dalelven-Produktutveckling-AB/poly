#pragma once

#define POLY_DETAIL_STRINGIZE(x) #x
#define POLY_STRINGIZE(x) POLY_DETAIL_STRINGIZE(x)

#define POLY_LINE_STR POLY_STRINGIZE(__LINE__)

#ifndef NDEBUG
#define POLY_ERROR_STR(X) __FILE__ " (" POLY_LINE_STR "): " X ""_str
#else
#ifdef POLY_EMPTY_ERRORS
#define POLY_ERROR_STR(X) ""_str
#else
#define POLY_ERROR_STR(X) X ""_str
#endif
#endif