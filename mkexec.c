#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include "args.h"
#define SCRIPT_FMT "#!/bin/sh\n"  \
		   "%s %s\n"

void create_executable(struct options* opts) {
	assert(opts->output_file_path != NULL && "This shouldn't happen, malloc() may have failed?");
	int script_fd = open(get_path_filename(opts->output_file_path), O_WRONLY | O_CREAT, 0777);
	FILE* script = fdopen(script_fd, "w");
	if(script == NULL) {
		printf("FATAL: could not create file '%s'\n", opts->output_file_path);
		exit(EXIT_FAILURE);
	}
	fprintf(script, SCRIPT_FMT, opts->run_command, opts->input_file_path);
	if(opts->verbose)
		printf("contents of '%s':\n" SCRIPT_FMT, get_path_filename(opts->output_file_path), opts->run_command, opts->input_file_path);

	fclose(script);
}

void write_desktop_file(FILE* desktop_file, char* name, char* comment, char* exec_filename, bool verbose) {
	char* exec_realpath = realpath(exec_filename, NULL);
	if(exec_realpath == NULL) {
		if(errno != ENOENT)
			printf("WARNING: could not determine the location of '%s', desktop file creation was aborted", exec_filename);
		else
			printf("WARNING: '%s' does not exist", exec_filename);
		fclose(desktop_file);
		exit(EXIT_FAILURE);
	}
	if(name == NULL)
		name = exec_filename;
	fprintf(desktop_file, "[Desktop Entry]\nName=%s\n", name);
	if(comment)
		fprintf(desktop_file, "Comment=%s\n", comment);
	fprintf(desktop_file, "Exec=%s\n", exec_realpath);
	if(verbose) {
		printf("[Desktop Entry]\nName=%s\n", name);
		if(comment)
			printf("Comment=%s\n", comment);
		printf("Exec=%s\n", exec_realpath);
	}
	free(exec_realpath);
}

void create_desktop_file(struct options* opts) {
	assert(opts->output_file_path != NULL && "This shouldn't happen");
	char* script_file_path = opts->output_file_path;
	opts->output_file_path = strdup(opts->output_file_path);
	opts->output_file_path = realloc(opts->output_file_path, strlen(opts->output_file_path) + sizeof(".desktop"));
	strcat(opts->output_file_path, ".desktop");
	int desktop_fd = open(get_path_filename(opts->output_file_path), O_WRONLY | O_CREAT, 0666);
	FILE* desktop_file = fdopen(desktop_fd, "w");
	if(desktop_file == NULL) {
		printf("FATAL: could not create file '%s'\n", get_path_filename(opts->output_file_path));
		exit(EXIT_FAILURE);
	}
	if(opts->verbose) printf("contents of '%s':\n", get_path_filename(opts->output_file_path));
	write_desktop_file(desktop_file, opts->program_name, opts->comment, get_path_filename(script_file_path), opts->verbose);
	free(script_file_path);
	fclose(desktop_file);
}

int main(int argc, char** argv) {
	struct options opts = { 0 };
	if(argp_parse(&argparser, argc, argv, 0, 0, &opts) != 0)
		exit(EXIT_FAILURE);

	if(opts.output_file_path == NULL)
		handle_input_pathname(opts.input_file_path, &opts.output_file_path);
	else
		opts.output_file_path = strdup(opts.output_file_path);

	if(!(opts.no_script))
		create_executable(&opts);
	if(opts.create_desktop) 
		create_desktop_file(&opts);
	
	free(opts.output_file_path);
	return 0;
}

