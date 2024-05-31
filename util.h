#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
#define debug(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#else
#define debug(msg, ...) 
#endif

char* strlchr(char* str, char c) {
	char* pos = NULL;
	while(*str++)
		if(*str == c)
			pos = str;
	return pos;
}

char* get_path_filename(char* path) {
	char* slash = strlchr(path, '/');
	return (slash) ? slash + 1 : path;
}

void handle_input_pathname(char* src, char** dst) {
	*dst = strdup(src);
	char* extension = strlchr(*dst, '.');
	if(extension != NULL)
		*extension = '\0';
	else {
		*dst = realloc(*dst, strlen(*dst) + sizeof(".sh"));
		strcat(*dst, ".sh");
	}
}

