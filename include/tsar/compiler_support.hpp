
#pragma once

#ifdef __clang__
// Clang 14.1 doesn't seem to support TSAR_CONSTEVAL completely, it errors out. 
// GCC is fine
// Probably clang issue, as website says TSAR_CONSTEVAL support is incomplete
#define TSAR_CONSTEVAL constexpr
#else
#define TSAR_CONSTEVAL consteval
#endif


