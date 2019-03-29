
void             display_entries    (struct options*, struct file*);
struct metadata *generate_metadata  (struct options*, struct file*);
void             display_long_format(struct options*, struct file*,
                                     struct metadata*, struct max_metadata*);
void             display_singleline (struct options*, struct file*,
                                     struct metadata*, struct max_metadata*);
void             display_default    (struct options*, struct file*,
                                     struct metadata*, struct max_metadata*);

void display_entries(struct options *options, struct file *files) {
	int length;
	struct metadata *metadata;
	struct max_metadata *max_metadata;
	int is_terminal;
	
	length = files_length(files);
	
	metadata = generate_metadata(options, files);
	max_metadata = generate_max_metadata(metadata, length);
	
	if ((is_terminal = isatty(STDOUT_FILENO)) < 0) {
		fprintf(stderr, "%s: unable to determine if STDOUT is terminal: %s\n",
		        getprogname(), strerror(errno)
		);
		exit(EXIT_FAILURE);
	}
	
	if (options->DISPLAY_BLOCKS_USED && is_terminal) {
		printf("total %zu\n", total_blocksize(options, files));
	}
	
	if (options->DISPLAY_LONG_FORMAT) {
		display_long_format(options, files, metadata, max_metadata);
	} else if (options->SINGLE_ENTRY_PER_LINE) {
		display_singleline(options, files, metadata, max_metadata);
	} else {
		display_default(options, files, metadata, max_metadata);
	}
}



struct metadata *generate_metadata(struct options *options, struct file *files) {
	struct metadata *metadata;
	int n;
	int i;
	
	n = files_length(files);
	
	if ((metadata = calloc(n, sizeof(struct metadata))) == NULL) {
		fprintf(stderr, "%s: unable to allocate memory\n", getprogname());
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < n; i++) {
		metadata[i].blocksize     = blocksize_string(options, &files[i]);
		metadata[i].filemode      = filemode(options, &files[i]);
		metadata[i].links         = links_string(options, &files[i]);
		metadata[i].owner_id      = owner_id_string(options, &files[i]);
		metadata[i].owner_name    = owner_name(options, &files[i]);
		metadata[i].group_id      = group_id_string(options, &files[i]);
		metadata[i].group_name    = group_name(options, &files[i]);
		metadata[i].size          = size_string(options, &files[i]);
		metadata[i].time_string   = time_string(options, &files[i]);
		metadata[i].serial_number = serial_number(options, &files[i]);
		metadata[i].descriptor    = descriptor_string(options, &files[i]);
	}
	
	return metadata;
}


void display_long_format(struct options *options, struct file *files,
                         struct metadata *metadata, struct max_metadata *max)
{
	int length;
	int i;
	
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
	
	int max_blocksize;
	int max_filemode;
	int max_links;
	int max_owner_id;
	int max_owner_name;
	int max_group_id;
	int max_group_name;
	int max_size;
	int max_time_string;
	int max_serial_number;
	int max_descriptor;
	
	length = files_length(files);
	for (i = 0; i < length; i++) {
		blocksize     = metadata[i].blocksize;
		filemode      = metadata[i].filemode;
		links         = metadata[i].links;
		owner_id      = metadata[i].owner_id;
		owner_name    = metadata[i].owner_name;
		group_id      = metadata[i].group_id;
		group_name    = metadata[i].group_name;
		size          = metadata[i].size;
		time_string   = metadata[i].time_string;
		serial_number = metadata[i].serial_number;
		descriptor    = metadata[i].descriptor;
		
		max_blocksize     = max->blocksize;
		max_filemode      = max->filemode;
		max_links         = max->links;
		max_owner_id      = max->owner_id;
		max_owner_name    = max->owner_name;
		max_group_id      = max->group_id;
		max_group_name    = max->group_name;
		max_size          = max->size;
		max_time_string   = max->time_string;
		max_serial_number = max->serial_number;
		max_descriptor    = max->descriptor;
		if (!options->DISPLAY_BLOCKS_USED) {
			blocksize = "";
			max_blocksize = 0;
		}
		
		if (!options->APPEND_FILETYPE_DESCRIPTOR) {
			descriptor = "";
			max_descriptor = 0;
		}
		
		if (!options->DISPLAY_FILE_SERIAL_NUMBER) {
			serial_number = "";
			max_serial_number = 0;
		}
		
		if (options->NUMERIC_UID_GID) {
			owner_name = "";
			max_owner_name = 0;

			group_name = "";
			max_group_name = 0;
		} else {
			owner_id = "";
			max_owner_id = 0;

			group_id = "";
			max_group_id = 0;
		}
		
		printf("%s%s %s %s%s %s%s %s %s %s%s%s\n",
			padding_left(blocksize, max_blocksize),
			padding_left(filemode, max_filemode),
			padding_left(links, max_links),
			padding_left(owner_id, max_owner_id),
			padding_left(readable(options, owner_name), max_owner_name),
			padding_left(group_id, max_group_id),
			padding_left(readable(options, group_name), max_group_name),
			padding_left(size, max_size),
			padding_left(time_string, max_time_string),
			padding_left(serial_number, max_serial_number),
			readable(options, files[i].name),
			descriptor
		);
	}
}


void display_singleline(struct options *options, struct file *files,
                        struct metadata *metadata, struct max_metadata *max_metadata)
{
	int length;
	int i;
	
	char *blocksize;
	char *serial_number;
	char *descriptor;
	
	int max_blocksize;
	int max_serial_number;
	int max_descriptor;
	
	length = files_length(files);
	
	for (i = 0; i < length; i++) {
		blocksize     = metadata[i].blocksize;
		serial_number = metadata[i].serial_number;
		descriptor    = metadata[i].descriptor;
		
		max_blocksize = max_metadata->blocksize;
		max_serial_number = max_metadata->serial_number;
		max_descriptor = max_metadata->descriptor;
		
		if (!options->DISPLAY_BLOCKS_USED) {
			blocksize = "";
			max_blocksize = 0;
		}
		
		if (!options->APPEND_FILETYPE_DESCRIPTOR) {
			descriptor = "";
			max_descriptor = 0;
		}
		
		if (!options->DISPLAY_FILE_SERIAL_NUMBER) {
			serial_number = "";
			max_serial_number = 0;
		}
		printf("%s%s%s%s\n",
			padding_left(blocksize, max_blocksize),
			padding_left(serial_number, max_serial_number),
			readable(options, files[i].name),
			descriptor
		);
	}
}


void display_default(struct options *options, struct file *files,
                     struct metadata *metadata, struct max_metadata *max_metadata)
{
	int length;
	int i;
	
	char *blocksize;
	char *serial_number;
	char *descriptor;
	
	length = files_length(files);
	for (i = 0; i < length; i++) {
		blocksize     = metadata[i].blocksize;
		serial_number = metadata[i].serial_number;
		descriptor    = metadata[i].descriptor;
		if (!options->DISPLAY_BLOCKS_USED) {
			blocksize = "";
		}
		
		if (!options->APPEND_FILETYPE_DESCRIPTOR) {
			descriptor = "";
		}
		
		if (!options->DISPLAY_FILE_SERIAL_NUMBER) {
			serial_number = "";
		}
		printf("%s%s%s%s  ",
			blocksize,
			serial_number,
			readable(options, files[i].name),
			descriptor
		);
	}
	printf("\n");
}
