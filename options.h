struct options {
	int IGNORE_DOTFILES;
	int INCLUDE_HIDDEN_DIRECTORIES;
	int FORCE_MULTICOLUMN_OUTPUT;
	int USE_LAST_FILE_STATUS_TIME;
	int PLAINFILE_DIRECTORIES;
	int APPEND_FILETYPE_DESCRIPTOR;
	int DISABLE_SORTING;
	int HUMAN_READABLE_SIZE;
	int DISPLAY_FILE_SERIAL_NUMBER;
	int DISPLAY_SIZE_KILOBYTES;
	int DISPLAY_LONG_FORMAT;
	int NUMERIC_UID_GID;
	int HUMAN_READABLE_NONPRINTABLE_CHARACTERS;
	int RECURSIVELY_LIST_SUBDIRECTORIES;
	int REVERSE_SORT_ORDER;
	int SORT_BY_SIZE_DESCENDING;
	int DISPLAY_BLOCKS_USED;
	int USE_LAST_MODIFIED_TIME;
	int USE_LAST_ACCESS_TIME;
	int FORCE_NONPRINTABLE_CHARACTERS;
	int SORT_MULTICOLUMN_OUTPUT_ACROSS;
	int SINGLE_ENTRY_PER_LINE;
};


struct options *parse_argv         (int*, char**);
struct options *get_default_options();
void            remove_options     (int*, char**);
int             count_options      (int*, char**);



struct options *parse_argv(int *argc, char **argv) {
	char *arguments;
	struct options *options;
	char character;
	
	arguments = "AaCcdFfhiklnqRrSstuwx1";
	
	options = get_default_options();
	
	while ((character = getopt(*argc, argv, arguments)) >= 0) {
		switch (character) {
			case 'A':
				options->IGNORE_DOTFILES = 1;
				break;
			case 'a':
				options->INCLUDE_HIDDEN_DIRECTORIES = 1;
				break;
			case 'c':
				options->USE_LAST_FILE_STATUS_TIME = 1;
				options->USE_LAST_ACCESS_TIME = 0;
				break;
			case 'd':
				options->PLAINFILE_DIRECTORIES = 1;
				options->RECURSIVELY_LIST_SUBDIRECTORIES = 0;
				break;
			case 'l':
				options->DISPLAY_LONG_FORMAT = 1;
				options->SINGLE_ENTRY_PER_LINE = 0;
				options->FORCE_MULTICOLUMN_OUTPUT = 0;
				options->SORT_MULTICOLUMN_OUTPUT_ACROSS = 0;
				break;
			case 's':
				options->DISPLAY_BLOCKS_USED = 1;
				break;
			case 'w':
				options->FORCE_NONPRINTABLE_CHARACTERS = 1;
				options->HUMAN_READABLE_NONPRINTABLE_CHARACTERS = 0;
				break;
			default:
			case '?':
				exit(EXIT_FAILURE);
		}
	}
	remove_options(argc, argv);
	return options;
}



struct options *get_default_options() {
	struct options *options;
	uid_t euid;
	int is_terminal;
	
	if ((options = calloc(1, sizeof(struct options))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory for options\n",
		        getprogname()
		);
		exit(EXIT_FAILURE);
	}
	
	euid = geteuid();
	
	if ((is_terminal = isatty(STDOUT_FILENO)) < 0) {
		fprintf(stderr,
		        "%s: unable to determine if STDOUT is terminal: %s\n",
		        getprogname(), strerror(errno)
		);
		exit(EXIT_FAILURE);
	}
	
	if (euid == 0) {
		options->IGNORE_DOTFILES = 1;
	} else {
		options->IGNORE_DOTFILES = 0;
	}
	
	options->INCLUDE_HIDDEN_DIRECTORIES = 0;
	
	if (is_terminal) {
		options->FORCE_MULTICOLUMN_OUTPUT = 1;
	} else {
		options->FORCE_MULTICOLUMN_OUTPUT = 0;
	}
	
	options->USE_LAST_FILE_STATUS_TIME  = 0;
	options->PLAINFILE_DIRECTORIES      = 0;
	options->APPEND_FILETYPE_DESCRIPTOR = 0;
	options->DISABLE_SORTING            = 0;
	options->HUMAN_READABLE_SIZE        = 0;
	options->DISPLAY_FILE_SERIAL_NUMBER = 0;
	options->DISPLAY_SIZE_KILOBYTES     = 0;
	options->DISPLAY_LONG_FORMAT        = 0;
	options->NUMERIC_UID_GID            = 0;
	
	if (is_terminal) {
		options->HUMAN_READABLE_NONPRINTABLE_CHARACTERS = 1;
	} else {
		options->HUMAN_READABLE_NONPRINTABLE_CHARACTERS = 0;
	}

	options->RECURSIVELY_LIST_SUBDIRECTORIES = 0;
	options->REVERSE_SORT_ORDER              = 0;
	options->SORT_BY_SIZE_DESCENDING         = 0;
	options->DISPLAY_BLOCKS_USED             = 0;
	options->USE_LAST_MODIFIED_TIME          = 0;
	options->USE_LAST_ACCESS_TIME            = 0;
	
	if (!is_terminal) {
		options->FORCE_NONPRINTABLE_CHARACTERS = 1;
	} else {
		options->FORCE_NONPRINTABLE_CHARACTERS = 0;
	}
	
	options->SORT_MULTICOLUMN_OUTPUT_ACROSS = 0;
	
	if (!is_terminal) {
		options->SINGLE_ENTRY_PER_LINE = 1;
	} else {
		options->SINGLE_ENTRY_PER_LINE = 0;
	}

	return options;
}


void remove_options(int *argc, char **argv) {
	int n;
	int count;
	int i;
	int j;
	
	n = *argc;
	count = count_options(argc, argv);
	
	for (i = 0; i < n; i++) {
		j = i + count + 1;
		/* Shift over indexes to remove ./ls and options */
		if (j < n) {
			argv[i] = argv[j];
		}
	}

	*argc -= count + 1;
}


int count_options(int *argc, char **argv) {
	int n;
	int count;
	int i;

	n = *argc;
	count = 0;

	for (i = 0; i < n; i++) {
		if (strncmp(argv[i], "-", 1) == 0) {
			count++;
		}
	}
	
	return count;
}
