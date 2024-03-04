#include <system.h>

#define MAX_FILE_NAME_LEN 256
#define MAX_FILE_TYPE_LEN 16

typedef unsigned char folder_type_t;

typedef struct file_t {
	char *data;
	size_t size;
	char name[MAX_FILE_NAME_LEN];
	char type[MAX_FILE_TYPE_LEN];
	struct file_t *next;
} file_t;

typedef struct folder_t {
	char name[MAX_FILE_NAME_LEN];
	struct folder_t *parent;
	struct folder_t *child;
	struct folder_t *next;
	file_t *files;
} folder_t;

folder_t *root_folder = NULL;
folder_t *cache_f = NULL;
folder_t *docs_f = NULL;
folder_t *media_f = NULL;
folder_t *mod_f = NULL;

folder_t *create_folder(char *name, folder_t *parent) {
	folder_t *folder = alloc(sizeof(folder_t));
	strcpy(folder->name, name);
	folder->parent = parent;
	folder->child = NULL;
	folder->next = NULL;
	folder->files = NULL;

	if (parent == NULL) {
		root_folder = folder;
	} else {
		if (parent->child == NULL) {
			parent->child = folder;
		} else {
			folder_t *current_folder = parent->child;
			while (current_folder->next != NULL) { current_folder = current_folder->next; }
			current_folder->next = folder;
		}
	}

	return folder;
}

file_t *create_file(char *name, char *type, folder_t *parent) {
	file_t *file = alloc(sizeof(file_t));

	file->data = NULL;
	file->size = 0;
	strcpy(file->name, name);
	strcpy(file->type, type);
	file->next = NULL;

	if (parent->files == NULL) {
		parent->files = file;
	} else {
		file_t *current_file = parent->files;
		while (current_file->next != NULL) { current_file = current_file->next; }
		current_file->next = file;
	}

	return file;
}

void write_file(file_t *file, void *data, size_t size) {
	file->data = realloc(file->data, file->size + size);
	memcpy(file->data + file->size, data, size);
	file->size += size;
}

file_t *add_file(char *name, char *type, folder_t *folder, void *data, size_t size) {
	file_t *file = create_file(name, type, folder);
	write_file(file, data, size);
	return file;
}

void read_file(file_t *file, char *buffer, size_t size) {
	memcpy(buffer, file->data, size);
}

void delete_file(file_t *file) {
	if (file->data != NULL) { free(file->data); }
	if (file != NULL) { free(file); }
}

void delete_folder(folder_t *folder) {
	if (folder == NULL) { return; }

	while (folder->files != NULL) {
		file_t *file = folder->files;
		folder->files = file->next;
		delete_file(file);
	}

	while (folder->child != NULL) {
		folder_t *child_folder = folder->child;
		folder->child = child_folder->next;
		delete_folder(child_folder);
	}

	if (folder->parent == NULL) {
		root_folder = NULL;
	} else {
		folder_t *parent_folder = folder->parent;
		if (parent_folder->child == folder) {
			parent_folder->child = folder->next;
		} else {
			folder_t *current_folder = parent_folder->child;
			while (current_folder->next != folder) { current_folder = current_folder->next; }
			current_folder->next = folder->next;
		}
	}

	free(folder);
}

void print_folder_contents(folder_t *folder, size_t depth) {
	for (size_t i = 0; i < depth; i++) { log_printf("\t"); }
	log_printf("- %s/\n", folder->name);

	file_t *file = folder->files;

	while (file != NULL) {
		for (size_t i = 0; i < depth + 1; i++) { log_printf("\t"); }
		log_printf("- %8s %4s | %8u килобайт\n", file->name, file->type, (file->size + 1024) / 1024);
		file = file->next;
	}

	folder_t *child_folder = folder->child;

	while (child_folder != NULL) {
		print_folder_contents(child_folder, depth + 1);
		child_folder = child_folder->next;
	}
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);
	create_folder("", NULL);

	cache_f = create_folder("cache", root_folder);
	docs_f = create_folder("docs", root_folder);
	media_f = create_folder("media", root_folder);
	mod_f = create_folder("mod", root_folder);

	file_t *readme = create_file("readme", "txt", root_folder);
	write_file(readme, "БМПОС 2023-2024", 21);

	return (module_info_t){
		.name = (char *)"[FS][IMFS]",
		.message = (char *)"IMFS (in memory filesystem) - файловая система работающая исключительно в ОЗУ.",
		.type = 0,
		.data_size = 0,
		.data = (void *)0,
		.err_code = 0,
		.module_id = 0,
		.irq = 0,
		.irq_handler = 0,
		.get_func = 0,
		.after_init = 0
	};
}
