#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <assert.h>
#include "args.h"
#define SCRIPT_FMT "#!/bin/sh\n"  \
		   "%s %s\n"

void handle_extensions(char* src, char** dst) {
	*dst = strdup(src);
	char* extension = strchr(*dst, '.');
	if(extension != NULL)
		*extension = '\0';
	else {
		*dst = realloc(*dst, strlen(*dst) + sizeof(".sh"));
		strcat(*dst, ".sh");
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
		printf("contents of '%s':\n" SCRIPT_FMT, opts->output_filename, opts->run_command, opts->input_filename);
	fclose(script);
}

void write_desktop_file(FILE* desktop_file, char* comment, char* exec_filename, bool verbose) {
	char* exec_realpath = realpath(exec_filename, NULL);
	if(exec_realpath == NULL) {
		printf("WARNING: could not determine the absolute path of '%s', desktop file creation was aborted", exec_filename);
		fclose(desktop_file);
		exit(EXIT_FAILURE);
	}
	fprintf(desktop_file, "[Desktop Entry]\nName=%s\n", exec_filename);
	if(comment)
		fprintf(desktop_file, "Comment=%s\n", comment);
	fprintf(desktop_file, "Exec=%s\n", exec_realpath);
	if(verbose) {
		printf("[Desktop Entry]\nName=%s\n", exec_filename);
		if(comment)
			printf("Comment=%s\n", comment);
		printf("Exec=%s\n", exec_realpath);
	}
	free(exec_realpath);
}

void create_desktop_file(struct options* opts) {
	assert(opts->output_filename != NULL && "This shouldn't happen");
	char* script_filename = opts->output_filename;
	opts->output_filename = strdup(opts->output_filename);
	opts->output_filename = realloc(opts->output_filename, strlen(opts->output_filename) + sizeof(".desktop"));
	strcat(opts->output_filename, ".desktop");
	int desktop_fd = open(opts->output_filename, O_WRONLY | O_CREAT, 0666);
	FILE* desktop_file = fdopen(desktop_fd, "w");
	if(desktop_file == NULL) {
		printf("FATAL: could not create file '%s'\n", opts->output_filename);
		exit(EXIT_FAILURE);
	}
	if(opts->verbose) printf("contents of '%s':\n", opts->output_filename);
	write_desktop_file(desktop_file, opts->description, script_filename, opts->verbose);
	free(script_filename);
	fclose(desktop_file);
}

int main(int argc, char** argv) {
	struct options opts = { 0 };
	if(argp_parse(&argparser, argc, argv, 0, 0, &opts) != 0)
		exit(EXIT_FAILURE);
	create_executable(&opts);
	if(opts.create_desktop)
		create_desktop_file(&opts);
	free(opts.output_filename);
	return 0;
}

