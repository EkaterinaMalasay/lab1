#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define N 1024
#define E 6

int arh(char *dir, char *name_arch)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	char block[N];
	int size;
	int out, in;
	int nread;
	int k = 0;
	int del = 0;

	chdir(dir);
	dp = opendir(dir);
	if (dp == NULL) {
		fprintf(stderr, "cannot open directory: %s\n", dir);
		return -1;
	}
	closedir(dp);
	chdir("..");
	out = open(name_arch, O_WRONLY|O_CREAT, 0600);
	if (out == -1) {
		fprintf(stderr, "cannot open file: %s\n", name_arch);
		return -1;
	}
	chdir(dir);
	k = 0;

	write(1, "Delete files after archiving? [1/0]:\n", 37);
	read(0, &del, 2);
	del = del - 2608;

	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISREG(statbuf.st_mode)) {
			if (strcmp(name_arch, entry->d_name) != 0) {
				size = statbuf.st_size;
				write(out, entry->d_name,
						strlen(entry->d_name));
				write(out, "/", 1);
				write(out, &(size), sizeof(size));
				in = open(entry->d_name, O_RDONLY);
					if (in == -1) {
						fprintf(stderr,
							"cannot open file: %s\n",
							entry->d_name);
						return -1;
					}
				while ((nread = read(in, block, sizeof(block)))
									 > 0)
					write(out, block, nread);
				close(in);
				k++;
				if (del == 1)
					unlink(entry->d_name);
			}
		}
	}
	write(out, &(k), sizeof(k));
	close(out);
	return 0;
}

int read_file(char *dir, char *name_arch)
{
	DIR *dp;
	char block[N] = {0};
	char name[N] = {0};
	char name_f[N] = {0};
	char dir_copy[N] = {0};
	int size;
	int in, out, i;
	int nread;
	int k = 0;
	int del = 0;

	chdir(dir);
	dp = opendir(dir);
	if (dp == NULL) {
		fprintf(stderr, "cannot open directory: %s\n", dir);
		return;
	}
	closedir(dp);

	in = open(name_arch, O_RDONLY);
	if (in == -1) {
		fprintf(stderr, "cannot open file: %s\n", name_arch);
		return -1;
	}

	write(1, "Enter the name of the folder: ", 30);
	read(0, name_f, N);
	k = strcspn(name_f, "\n");
	name_f[k] = 0;
	strcat(dir_copy, dir);
	strcat(dir_copy, name_f);
	k = 0;
	mkdir(dir_copy, O_CREAT|0600);
	chdir(dir_copy);

	lseek(in, -4, SEEK_END);
	read(in, &k, 4);
	lseek(in, 0, SEEK_SET);
	while (k > 0) {
		for (i = 0; ; i++) {
			nread = read(in, block, 1);
			if (strcmp(block, "/") == 0)
				break;
			name[i] = block[0];
		}
		out = open(name, O_WRONLY|O_CREAT|O_APPEND, 0600);
		if (in == -1) {
			fprintf(stderr, "cannot open file: %s\n", name);
			return -1;
		}

		read(in, &size, 4);
		if (size > sizeof(block)) {
			while (size > sizeof(block)) {
				nread = read(in, block, sizeof(block));
				write(out, block, nread);
				size = size - sizeof(block);
			}
		}
		nread = read(in, block, size);
		write(out, block, nread);
		close(out);
		memset(name, 0, N);
		memset(block, 0, N);
		k--;
	}
	chdir("..");

	write(1, "Delete archive? [1/0]:\n", 23);
	read(0, &del, 2);
	del = del - 2608;
	if (del == 1)
		unlink(name_arch);
	close(in);
	return 0;
}

char *n_arch(char *name_arch)
{
	char end[E] = ".zippp";
	int k = 0;

	write(1, "Enter the name of the archive: ", 31);
	read(0, name_arch, N);
	k = strcspn(name_arch, "\n");
	name_arch[k] = 0;
	strcat(name_arch, end);
	return name_arch;
}

char *addr(char *address)
{
	int i = 0;

	write(1, "Enter the address of the directory: ", 36);
	read(0, address, N);
	i = strcspn(address, "\n");
	address[i] = 0;
	return address;
}

void menu(void)
{
	int input = 1;
	char address[N] = {0};
	char name_arch[N] = {0};

	while (input != 0) {
		write(1, "1-create archive\n", 17);
		write(1, "2-unpacking the archive\n", 24);
		write(1, "0-exit\n", 7);
		write(1, "Your choise:\n", 13);
		read(0, &input, 2);
		input = input - 2608;
		switch (input) {
		case 1:
		{
			addr(address);
			n_arch(name_arch);
			arh(address, name_arch);
			break;
		}
		case 2:
		{
			addr(address);
			n_arch(name_arch);
			read_file(address, name_arch);
			break;
		}
		case 0:
			return;
		default:
			write(1, "Input Error!!!!\n", 16);
		}
		memset(address, 0, N);
		memset(name_arch, 0, N);
		input = 1;
	}
}

void main(void)
{
	menu();
	exit(0);
}