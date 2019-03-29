void traverse_argv(struct options *options, int argc, char **argv) {
	struct file *files;
	int length;
	int i;
	int num_directories;
	
	if (argc == 0) {
		traverse_working_directory(options);
		return;
	}
	
	files = generate_files(options, argc, argv);
	
	sort_files(options, files);
	
	display_entries(options, files);
	
	length = files_length(files);
	if (length > 0) {
		printf("\n");
	}
	
	for (i = 0; i < length; i++) {
		if (S_ISDIR(files[i].stat.st_mode)) {
			traverse_directory(options, files[i].name);
			
			if (i != length - 1) {
				printf("\n");
			}
		}
	}
	
	free(files);
}


	void traverse_working_directory(struct options *options) {
		traverse_directory_helper(options, ".", 0);
	}



	void traverse_directory(struct options *options, char *path) {
		traverse_directory_helper(options, path, 1);
	}



	int count_directories(struct file *files) {
		int i;
		int n;
		int count;
		
		n = files_length(files);
		count = 0;
		
		for (i = 0; i < n; i++) {
			if (S_ISDIR(files[i].stat.st_mode) && !is_dotfile(files[i].name)) {
				count++;
			}
		}
		
		return count;
	}


	int count_directory_entries(char *path) {
		DIR *dp;
		struct dirent *dirp;
		int count;
		
		if ((dp = opendir(path)) == NULL) {
			fprintf(stderr, "%s: an error occurred while opening '%s': %s\n",
					getprogname(), path, strerror(errno)
			);
			exit(EXIT_FAILURE);
		}
		
		for (count = 0; (dirp = readdir(dp)) != NULL; count++);
		
		closedir(dp);
		
		return count;
}
