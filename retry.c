/*
retry -- retry a command until is succeeds
Copyright (C) 2019 Johan Gunnarsson <johan.gunnarsson@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Fra
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#define VERSION "1.0"

#define HELP_TEXT \
"Usage: retry [OPTIONS] RETRIES COMMAND [ARG]...\n"                            \
"\n"                                                                           \
"Start COMMAND (with optional ARG arguments). If command exits with non-zero\n"\
"exit code, restart it up to RETRIES times.\n"                                 \
"\n"                                                                           \
"Optional arguments:\n"                                                        \
"  -s --sleep=N\n"                                                             \
"  -h --help\n"

#define VERSION_TEXT \
"retry " VERSION "\n"                                                          \
"Written by Johan Gunnarsson <johan.gunnarsson@gmail.com>\n"

/* Number of seconds to sleep before each retry */
static long opt_sleep = 0;

/* Number of retries */
static long opt_retries = 0;

/* Command */
static char opt_cmd[4096];

static void
parse_opts(int argc, char **argv)
{
	while (1) {
		static struct option options[] = {
			{ "sleep",   required_argument, NULL, 's' },
			{ "help",    no_argument,       NULL, 'h' },
			{ "version", no_argument,       NULL, 'v' },
			{ NULL,      0,                 NULL, 0   },
		};

		int c = getopt_long(argc, argv, "+s:hv", options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 's':
			opt_sleep = strtol(optarg, NULL, 10);
			break;
		case 'h':
			printf(HELP_TEXT);
			exit(EXIT_SUCCESS);
			break;
		case 'v':
			printf(VERSION_TEXT);
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
		}
	}

	if (optind + 2 > argc) {
		printf(HELP_TEXT);
		exit(EXIT_FAILURE);
	}

	/* Number of retries */
	opt_retries = strtol(argv[optind++], NULL, 10);

	/* Build a string of all commands and command arguments */
	int off = 0;
	for (; optind < argc; optind++) {
		strncat(&opt_cmd[off], argv[optind], sizeof (opt_cmd) - off - 1);
		off += strlen(argv[optind]);
		strncat(&opt_cmd[off], " ", sizeof (opt_cmd) - off - 1);
		off += 1;
	}

	if (off >= sizeof (opt_cmd)) {
		printf("retry: command too long (max %zd bytes)\n", sizeof (opt_cmd));
		exit(EXIT_FAILURE);
	}
}

static int
run_cmd() {
	int ret = system(opt_cmd);
	if (ret == -1) {
		exit(EXIT_FAILURE);
	} else if (WIFSIGNALED(ret)) {
		exit(128 + WTERMSIG(ret));
	} else if (WIFEXITED(ret)) {
		return WEXITSTATUS(ret);
	}

	return -1;
}

int
main(int argc, char **argv)
{
	parse_opts(argc, argv);

	int ret = run_cmd();
	for (; ret && opt_retries > 0; opt_retries--) {
		if (opt_sleep > 0)
			sleep(opt_sleep);
		ret = run_cmd();
	}

	return ret;
}
