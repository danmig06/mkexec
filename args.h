#include <argp.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef DEBUG
#define debug(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#else
#define debug(msg, ...) 
#endif

#define OPT_DESKTOP	"create-desktop"
#define OPT_DESCRIPTION "description"
#define OPT_NAME 	"input"
#define OPT_RUN		"run-command"
#define OPT_OUTPUT 	"output"
#define OPT_VERBOSE 	"verbose"
#define OPT_EXE 	"winexe"

struct options {
	char *description, *override_name, *run_command, *input_filename, *output_filename;
	bool verbose, create_desktop;
};

static struct argp_option argp_options[] = {
	{OPT_VERBOSE, 'v', 0, 0, "print the contents of each file"},
	{OPT_DESCRIPTION, 'd', "DESCRIPTION", 0, "set the description for the application (does nothing if -D is not set)"},
	{OPT_DESKTOP, 'D', 0, 0, "create a .desktop file, used by application launchers"},
	{OPT_NAME, 'i', "PROGNAME", 0, "specify the input filename"},
	{OPT_OUTPUT, 'o', "FILENAME", 0, "specify the output filename (will be the input without the extension if not specified)"},
	{OPT_RUN, 'r', "COMMAND", 0, "specify the command used to run the application"},
	{OPT_EXE, 'e', 0, 0, "indicates the program PROGNAME to be a windows executable and will automatically use wine as a run-command"},
	{ 0 }
};

static error_t parse_opt(int key, char* arg, struct argp_state* state) {
	struct options* opts = state->input;
	bool is_exe = false;;

	switch(key) {
	case 'd':
	        debug("parsed description ('%s')", arg);
		opts->description = arg;
		break;
	case 'D':
		debug("parsed desktop");
		opts->create_desktop = true;
		break;
	case 'i':
		debug("parsed input ('%s')", arg);
		opts->input_filename = arg;
		break;
	case 'v':
		debug("parsed verbose");
		opts->verbose = true;
		break;
	case 'o':
		debug("parsed output ('%s')", arg);
		opts->output_filename = arg;
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
		if(opts->input_filename == NULL || (opts->run_command == NULL && !(is_exe)))
			argp_usage(state);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static char args_doc[] = "-i PROGNAME [-r COMMAND | -e]...";
static char doc[] = "this utility creates an executable setup for non-native applications";
struct argp argparser = {argp_options, parse_opt, args_doc, doc};
