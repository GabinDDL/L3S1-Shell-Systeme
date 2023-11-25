#ifndef STRING_UTILS_H
#define STRING_UTILS_H

bool start_with(const char *, const char *);
/* Returns true if the first string begins with
 * the same characters as the second */

char *concat_with_delimiter(const char *, const char *, char);
/* Returns the string of the two concatenated
 * strings separated by the character */

#endif
