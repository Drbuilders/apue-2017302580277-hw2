#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include <errno.h>
#include <libgen.h>
#include <bsd/stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


#include "options.h"
#include "display.h"
#include "traversal.h"

//先定义需要的数据结构
struct file {
	char *name;
	struct stat stat;
	struct stat lstat;
	off_t size;
	time_t time;
};



struct metadata {
	char *blocksize;
	char *filemode;
	char *links;
	char *owner_id;
	char *owner_name;
	char *group_id;
	char *group_name;
	char *size;
	char *time_string;
	char *serial_number;
	char *descriptor;
};


struct file *generate_files     (struct options*, int, char**);
int          files_length       (struct file*);
int          is_dotfile         (char*);
int          is_hidden_directory(char*);


struct file *generate_files(struct options *options, int argc, char **argv) {
	struct file *files;
	int i;
	int idx;
	size_t t;
	
	if ((files = calloc(argc + 1, sizeof(struct file))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory for files\n",
		        getprogname()
		);
		exit(EXIT_FAILURE);
	}
	
	for (i = 0, idx = 0; i < argc; i++) {
		if (options->IGNORE_DOTFILES && is_dotfile(argv[i])) {
			continue;
		}
		
		if (!options->INCLUDE_HIDDEN_DIRECTORIES &&
		    is_hidden_directory(argv[i])) {
			continue;
		}
		
		struct stat stat_buffer;
		struct stat lstat_buffer;
		
		if (stat(argv[i], &stat_buffer) < 0) {
			fprintf(stderr,
			        "%s: an error occurred while accessing '%s': %s\n",
			        getprogname(), argv[i], strerror(errno)
			);
			continue;
		}
		
		if (lstat(argv[i], &lstat_buffer) < 0) {
			fprintf(stderr,
			        "%s: an error occurred while accessing '%s': %s\n",
			        getprogname(), argv[i], strerror(errno)
			);
			continue;
		}
		
		if (options->USE_LAST_FILE_STATUS_TIME) {
			t = stat_buffer.st_ctime;
		} else if (options->USE_LAST_MODIFIED_TIME) {
			t = stat_buffer.st_mtime;
		} else if (options->USE_LAST_ACCESS_TIME) {
			t = stat_buffer.st_atime;
		} else {
			t = stat_buffer.st_mtime;
		}
		
		files[idx].name  = argv[i];
		files[idx].stat  = stat_buffer;
		files[idx].lstat = lstat_buffer;
		files[idx].size  = stat_buffer.st_size;
		files[idx].time  = t;
		
		idx++;
	}
	
	files[idx].name = NULL;
	
	return files;
}

struct max_metadata {
	int blocksize;
	int filemode;
	int links;
	int owner_id;
	int owner_name;
	int group_id;
	int group_name;
	int size;
	int time_string;
	int serial_number;
	int descriptor;
};


struct max_metadata *generate_max_metadata(struct metadata *metadata, int length) {
	struct max_metadata *max;
	int i;
	size_t tmp;
	if ((max = calloc(1, sizeof(struct max_metadata))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory\n",
		        getprogname()
		);
		exit(EXIT_FAILURE);
	}
	

	max->blocksize     = 0;
	max->filemode      = 0;
	max->links         = 0;
	max->owner_id      = 0;
	max->owner_name    = 0;
	max->group_id      = 0;
	max->group_name    = 0;
	max->size          = 0;
	max->time_string   = 0;
	max->serial_number = 0;
	max->descriptor    = 0;
	

	for (i = 0; i < length; i++) {
		tmp = strlen(metadata[i].blocksize);
		if (tmp > max->blocksize) {
			max->blocksize = tmp;
		}
		
		tmp = strlen(metadata[i].filemode);
		if (tmp > max->filemode) {
			max->filemode = tmp;
		}
		
		tmp = strlen(metadata[i].links);
		if (tmp > max->links) {
			max->links = tmp;
		}
		
		tmp = strlen(metadata[i].owner_id);
		if (tmp > max->owner_id) {
			max->owner_id = tmp;
		}
		
		tmp = strlen(metadata[i].owner_name);
		if (tmp > max->owner_name) {
			max->owner_name = tmp;
		}
		
		tmp = strlen(metadata[i].group_id);
		if (tmp > max->group_id) {
			max->group_id = tmp;
		}
		
		tmp = strlen(metadata[i].group_name);
		if (tmp > max->group_name) {
			max->group_name = tmp;
		}
		
		tmp = strlen(metadata[i].size);
		if (tmp > max->size) {
			max->size = tmp;
		}
		
		tmp = strlen(metadata[i].time_string);
		if (tmp > max->time_string) {
			max->time_string = tmp;
		}
		
		tmp = strlen(metadata[i].serial_number);
		if (tmp > max->serial_number) {
			max->serial_number = tmp;
		}
		
		tmp = strlen(metadata[i].descriptor);
		if (tmp > max->descriptor) {
			max->descriptor = tmp;
		}
	}
	
	return max;
}


int null_pointer_array_length(void **array) {
	int i;
	for (i = 0; array[i] != NULL; i++);
	return i;
}

char *strlowercase(char *string) {
	int i;
	int n;
	char *modified;
	
	n = strlen(string);
	
	if ((modified = calloc(1, sizeof(string)+1)) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory\b", getprogname());
		exit(EXIT_FAILURE);
	}
	
	for (i = 0; i < n; i++) {
		modified[i] = tolower(string[i]);
	}
	
	return modified;
}


char *padding_left(char *string, size_t length) {
	char *modified;
	int n;
	int i;
	int j;
	
	n = strlen(string);
	
	if ((modified = calloc(length+1, sizeof(char))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory\n",
		        getprogname()
		);
		exit(EXIT_FAILURE);
	}
	
	for (i = 0, j = 0; i < length; i++) {
		if (i < length - n) {
			modified[i] = ' ';
		} else {
			modified[i] = string[j];
			j++;
		}
	}
	
	return modified;
}



char *padding_right(char *string, size_t length) {
	char *modified;
	int n;
	int i;
	
	n = strlen(string);
	
	if ((modified = calloc(length+1, sizeof(char))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory\n",
		        getprogname()
		);
		exit(EXIT_FAILURE);
	}
	
	strncpy(modified, string, length);

	for (i = n; i < length; i++) {
		modified[i] = ' ';
	}
	
	return modified;
}


char *readable(struct options *options, char *string) {
	char *modified;
	int length;
	int i;
	
	if (options->FORCE_NONPRINTABLE_CHARACTERS) {
		return string;
	}
	
	length = strlen(string);

	if ((modified = calloc(length + 1, sizeof(char))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory\n",
		        getprogname()
		);
		exit(EXIT_FAILURE);
	}
	
	strcpy(modified, string);
	
	for (i = 0; i < length; i++) {
		if (!isprint(string[i])) {
			modified[i] = '?';
		}
	}
	
	return modified;
}


void sort_files(struct options *options, struct file *files) {
	int length;
	
	if (options->DISABLE_SORTING) {
		return;
	}
	
	length = files_length(files);

	if (options->SORT_BY_SIZE_DESCENDING) {
		qsort(files, length, sizeof(struct file), compare_size);
		reverse_sort(files);
	} else if (options->USE_LAST_FILE_STATUS_TIME ||
	           options->USE_LAST_MODIFIED_TIME    ||
	           options->USE_LAST_ACCESS_TIME) {
		qsort(files, length, sizeof(struct file), compare_time);
		reverse_sort(files);
	} else {
		qsort(files, length, sizeof(struct file), compare_name);
	}

	if (options->REVERSE_SORT_ORDER) {
		reverse_sort(files);
	}
}


int compare_size(const void *a, const void *b) {
	off_t x;
	off_t y;
	
	x = ((struct file *) a)->size;
	y = ((struct file *) b)->size;
	
	return (int) (x - y);
}



int compare_time(const void *a, const void *b) {
	time_t x;
	time_t y;
	
	x = ((struct file *) a)->time;
	y = ((struct file *) b)->time;
	
	return (int) (x - y);
}


int compare_name(const void *a, const void *b) {
	char *x;
	char *y;
	int ret;
	x = strlowercase(((struct file *) a)->name);
	y = strlowercase(((struct file *) b)->name);
	
	ret = strcmp(x, y);
	
	return ret;
}

void reverse_sort(struct file *files) {
	int i;
	int n;
	struct file temp;
	
	n = files_length(files);
	
	for (i = 0; i < n / 2; i++) {
		temp = files[i];
		files[i] = files[n-i-1];
		files[n-i-1] = temp;
	}
}



int files_length(struct file *files) {
	int i;
	for (i = 0; files[i].name != NULL; i++);
	return i;
}

int is_dotfile(char *filename) {
	char *base;
	base = basename(filename);
	return (strcmp(base, ".") == 0 || strcmp(base, "..") == 0);
}


int is_hidden_directory(char *filename) {
	int n;
	char *parent;
	char *base;
	if (is_dotfile(filename)) {
		return 0;
	}
	
	n = strlen(filename);
	
	if ((base = calloc(n+1, sizeof(char))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory\n",
		        getprogname()
		);
		exit(EXIT_FAILURE);
	}

	if ((parent = calloc(n+1, sizeof(char))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory\n",
		        getprogname()
		);
		exit(EXIT_FAILURE);
	}

	strcpy(base, filename);
	strcpy(parent, filename);

	strcpy(base, basename(base));
	strcpy(parent, dirname(parent));
	
	while (strcmp(base, "/") != 0 && strlen(base) > 1) {
		if (strlen(base) > 1 && strncmp(base, ".", 1) == 0) {
			return 1;
		}

		strcpy(parent, dirname(parent));
		strcpy(base, basename(parent));
	}
	
	return 0;
}


int main(int argc, char **argv) {
	struct options *options;
	
	setprogname(argv[0]);
	options = parse_argv(&argc, argv);
	traverse_argv(options, argc, argv);
	
	free(options);

	return EXIT_SUCCESS;
}
