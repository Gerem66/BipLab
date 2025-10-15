/**
 * @file string_utils.h
 * @brief String manipulation utilities
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>

/**
 * Safe string copy with null termination guarantee
 * Always null-terminates the destination string, unlike strncpy
 *
 * @param dst Destination buffer
 * @param maxLen Maximum size of destination buffer (including null terminator)
 * @param src Source string (can be NULL)
 */
void String_SafeCopy(char* dst, size_t maxLen, const char* src);

/**
 * Case-insensitive substring search
 * Similar to strcasestr but with simpler implementation
 *
 * @param haystack String to search in
 * @param needle String to search for
 * @return Pointer to first occurrence or NULL if not found
 */
const char* String_CaseInsensitiveSearch(const char* haystack, const char* needle);

/**
 * Trim whitespace from both ends of a string (in-place)
 * Modifies the original string
 *
 * @param str String to trim
 * @return Pointer to the trimmed string (same as input)
 */
char* String_Trim(char* str);

#endif // STRING_UTILS_H
