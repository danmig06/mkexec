#include <argp.h>
#include <stdbool.h>
#include <stdio.h>
#include "util.h"

#define OPT_DESKTOP	 "create-desktop"
#define OPT_DESKTOP_ONLY "desktop-only"
#define OPT_COMMENT      "comment"
#define OPT_NAME 	 "name"
#define OPT_INPUT 	 "input"
#define OPT_RUN		 "run-command"
#define OPT_OUTPUT 	 "output"
#define OPT_VERBOSE 	 "verbose"
#define OPT_EXE 	 "winexe"

struct options {
	char *comment, *program_name, *run_command, *input_file_path, *output_file_path;
	bool verbose, create_desktop, no_script;
};

static struct argp_option argp_options[] = {
	{OPT_VERBOSE, 	     'v', 0, 0, "print the contents of each file"},
	{OPT_COMMENT, 	     'c', "DESCRIPTION", 0, "set the comment for the application (does nothing if desktop flags are not set)"},
	{OPT_DESKTOP_ONLY,   'd', 0, 0, "create a .desktop file, used by many application launchers"},
	{OPT_DESKTOP, 	     'D', 0, 0, "create a .desktop file only, setting a run command won't have any effect"},
	{OPT_NAME, 	     'n', "NAME", 0 , "specify the name for the program in the .desktop file"},
	{OPT_INPUT, 	     'i', "PROGNAME", 0, "specify the input filename"},
	{OPT_OUTPUT, 	     'o', "FILENAME", 0, "specify the output filename (will be the input without the extension if not specified, any paths will be ignored)"},
	{OPT_RUN, 	     'r', "COMMAND", 0, "specify the command used to run the application"},
	{OPT_EXE, 	     'e', 0, 0, "indicates the program PROGNAME to be a windows executable and will automatically use wine as a run-command"},
	{ 0 }
};

static error_t parse_opt(int key, char* arg, struct argp_state* state) {
	struct options* opts = state->input;
	bool is_exe = false;

	switch(key) {
	case 'c':
	        debug("parsed comment ('%s')", arg);
		opts->comment = arg;
		break;
	case 'D':
		opts->no_script = true;
	case 'd':
		debug("parsed desktop");
		opts->create_desktop = true;
		break;
	case 'n':
		debug("parsed name ('%s')", arg);
		opts->program_name = arg;
		break;
	case 'i':
		debug("parsed input ('%s')", arg);
		opts->input_file_path = arg;
		break;
	case 'v':
		debug("parsed verbose");
		opts->verbose = true;
		break;
	case 'o':
		debug("parsed output ('%s')", arg);
		opts->output_file_path = arg;
		break;
	case 'r':
		debug("parsed run command ('%s')", arg);
		opts->run_command = arg;
		break;
	case 'e':
		debug("parsed winexe");
		opts->run_command = "wine";
		is_exe = true;
		break;
	case ARGP_KEY_END:
		if(opts->input_file_path == NULL || (opts->run_command == NULL && !(is_exe || opts->no_script)))
			argp_usage(state);
		if(opts->no_script && opts->output_file_path == NULL)
			opts->output_file_path = opts->input_file_path;
		debug("all arguments parsed");
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static char args_doc[] = "-i PROGNAME [-r COMMAND | -e | -D]...";
static char doc[] = "this utility creates an executable setup for non-native applications";
struct argp argparser = {argp_options, parse_opt, args_doc, doc};
