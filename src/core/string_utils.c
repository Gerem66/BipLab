/**
 * @file string_utils.c
 * @brief Implementation of string manipulation utilities
 */

#include "../../include/core/string_utils.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void String_SafeCopy(char* dst, size_t maxLen, const char* src)
{
    if (!dst || maxLen == 0) {
        return;
    }

    if (!src) {
        dst[0] = '\0';
        return;
    }

    // snprintf handles null termination automatically
    snprintf(dst, maxLen, "%s", src);
}

const char* String_CaseInsensitiveSearch(const char* haystack, const char* needle)
{
    if (!haystack || !needle) {
        return NULL;
    }

    size_t needleLen = strlen(needle);
    if (needleLen == 0) {
        return haystack;
    }

    for (const char* p = haystack; *p; ++p) {
        if (tolower((unsigned char)*p) == tolower((unsigned char)*needle)) {
            if (strncasecmp(p, needle, needleLen) == 0) {
                return p;
            }
        }
    }

    return NULL;
}

char* String_Trim(char* str)
{
    if (!str) {
        return NULL;
    }

    // Trim leading whitespace
    while (*str && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')) {
        str++;
    }

    if (*str == '\0') {
        return str;
    }

    // Trim trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }

    end[1] = '\0';
    return str;
}
