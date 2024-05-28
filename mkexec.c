#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include "args.h"
#define SCRIPT_FMT "#!/bin/sh\n"  \
		    "%s %s\n"
#define DESKTOP_FMT "[Desktop Entry]\n"  \
			  "Name=%s\n"    \
			  "Comment=%s\n" \
			  "Exec=%s\n"    \
			  "TryExec=%s\n"

void handle_extensions(char* src, char** dst) {
	*dst = strdup(src);
	char* extension = strchr(*dst, '.');
	if(extension != NULL)
		*extension = '\0';
	else {
		*dst = realloc(*dst, strlen(*dst) + sizeof(".sh\0"));
		strcat(*dst, ".sh\0");
	}

}

void create_executable(struct options* opts) {
	if(opts->output_filename == NULL)
		handle_extensions(opts->input_filename, &opts->output_filename);
	else
		opts->output_filename = strdup(opts->output_filename);	
	
	int script_fd = open(opts->output_filename, O_WRONLY | O_CREAT, 0777);
	FILE* script = fdopen(script_fd, "w");
	if(script == NULL) {
		printf("FATAL: could not create file '%s'\n", opts->output_filename);
		exit(EXIT_FAILURE);
	}
	fprintf(script, SCRIPT_FMT, opts->run_command, opts->input_filename);
	if(opts->verbose)
		printf("contents of '%s':\n" SCRIPT_FMT, opts->input_filename, opts->run_command, opts->input_filename);
	fclose(script);
}

void create_desktop_file(struct options* opts) {
	assert(opts->output_filename != NULL || "This shouldn't happen");
	char* exec_filename = opts->output_filename;
	opts->output_filename = strdup(opts->output_filename);
	opts->output_filename = realloc(opts->output_filename, strlen(opts->output_filename) + sizeof(".desktop"));
	strcat(opts->output_filename, ".desktop");
	int desktop_fd = open(opts->output_filename, O_WRONLY | O_CREAT, 0777);
	FILE* desktop_file = fdopen(desktop_fd, "w");
	if(desktop_file == NULL) {
		printf("FATAL: could not create file '%s'\n", opts->output_filename);
		exit(EXIT_FAILURE);
	}

	fprintf(desktop_file, "[Desktop Entry]\nName=%s\n", exec_filename);
	if(opts->description)
		fprintf(desktop_file, "Comment=%s\n", opts->description);
	fprintf(desktop_file, "Exec=%s\n", exec_filename);
	if(opts->verbose) {
		printf("[Desktop Entry]\nName=%s\n", opts->output_filename);
		if(opts->description)
			printf("Comment=%s\n", opts->description);
		printf("Exec=%s\n", opts->output_filename);
	}
	fclose(desktop_file);
}

int main(int argc, char** argv) {
	struct options opts = { 0 };
	if(argp_parse(&argparser, argc, argv, 0, 0, &opts) != 0)
		exit(EXIT_FAILURE);
	bool no_override = opts.output_filename == NULL;
	create_executable(&opts);
	if(opts.create_desktop)
		create_desktop_file(&opts);
	free(opts.output_filename);
	return 0;
}

