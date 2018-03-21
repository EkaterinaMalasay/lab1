#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N 1024
#define E 6

void arh(char *dir, char *name_arch)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	char block[N];
	int size;
	int out,in;
	int nread;
	int k=0;

	chdir(dir);
	if ((dp = opendir(dir)) == NULL){
		fprintf(stderr, "cannot open directory: %s\n", dir);
		return;
	}

	chdir("..");
	out = open(name_arch, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	chdir(dir);
	k=0;
	while((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if (S_ISREG(statbuf.st_mode))
		{
			if(strcmp(name_arch, entry->d_name) != 0)
			{
				size = statbuf.st_size;
				write(out, entry->d_name, strlen(entry->d_name));
				write(out, "/", 1);
				write(out, &(size), sizeof(size));
				in = open(entry->d_name, O_RDONLY);
				while((nread = read(in, block, sizeof(block))) > 0)
					write(out, block, nread);
				close(in);
				k++;
				//unlink(entry->d_name);
			}
		}
	}
	write(out, &(k), sizeof(k));
	closedir(dp);
	close(out);
}


void read_file(char *dir, char *name_arch)
{
	DIR *dp;
	char block[N] = {0};
	char name[N] = {0};
	int size;
	int in,out,i;
	int nread;
	int k = 0;

	chdir(dir);
	if ((dp = opendir(dir)) == NULL){
		fprintf(stderr, "cannot open directory: %s\n", dir);
		return;
	}

////////////////////////////////////////////////////////
	in = open(name_arch, O_RDONLY);
	lseek(in,-4,SEEK_END);
	read(in, &k, 4);
	lseek(in,0,SEEK_SET);

	while(k>0)
	{
		for(i = 0;;i++)
		{
			nread = read(in, block, 1);
			if (strcmp(block,"/")==0)
				break;
			name[i] = block[0];
		}
		out = open(name, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);

		read(in, &size, 4);
		if(size>sizeof(block))
		{
			while(size>sizeof(block))
			{
				nread = read(in, block, sizeof(block));
				write(out, block, nread);
				size = size - sizeof(block);
			}
		}
		nread = read(in, block, size);
		write(out, block, nread);
		close(out);
		memset(name,0,N);
		memset(block,0,N);
		k--;
	}

	close(in);
	unlink(name_arch);
}

char* n_arch(char *name_arch)
{
	char end[E] = ".zippp";
	int k = 0;

	write(1, "Enter the name of the archive: ", 31);
	read(0,name_arch,N);
	k=strcspn(name_arch,"\n");
	name_arch[k]=0;
	strcat(name_arch,end);
	k=0;

	return name_arch;
}

char* addr(char *address)
{
	int i=0;

	write(1, "Enter the address of the directory: ", 36);
	read(0,address,N);
	i=strcspn(address,"\n");
	address[i]=0;

	return address;
}

void menu(void)
{
	int input = 1;
	char address[N] = {0};
	char name_arch[N] = {0};
	int i;

	while(input!=0)
	{
		write(1, "1-create archive\n", 18);
		write(1, "2-unpacking the archive\n", 25);
		write(1, "0-exit\n", 8);
		write(1, "Your choise:\n", 14);
		read(0,&input,2);
		input = input - 2608;
		switch ( input )
		{
			case 1:
			{
				addr(address);
				n_arch(name_arch);
				arh(address,name_arch);
				break;
			}
			case 2:
			{
				addr(address);
				n_arch(name_arch);
				read_file(address,name_arch);
				break;
			}
			case 0:
				return;
			default:
				write(1, "Input Error!!!!\n", 17);
		}
		memset(address,0,N);
		memset(name_arch,0,N);
	}
}

void main(void)
{
	menu();
	exit(0);
}