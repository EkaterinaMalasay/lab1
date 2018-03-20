#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void arh(char *dir)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	char block[1024];
	char name_out[1024]={0};
	char end[6] = ".zippp";
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

	write(1, "Enter the name of the archive:", 30);
	read(0,name_out,1024);
	k=strcspn(name_out,"\n");
	name_out[k]=0;
	strcat(name_out,end);

////////////////////////////////////////////////////////////
	out = open(name_out, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	chdir(dir);
	k=0;
	while((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if (S_ISREG(statbuf.st_mode))
		{
			if(strcmp(name_out, entry->d_name) != 0)
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


void read_file(char *dir)
{
	DIR *dp;
	char block[1024] = {0};
	char name[1024] = {0};
	char name_in[1024]={0};
	char end[6] = ".zippp";
	int size;
	int in,out,i,j;
	int nread;
	int k = 0;

	chdir(dir);
	if ((dp = opendir(dir)) == NULL){
		fprintf(stderr, "cannot open directory: %s\n", dir);
		return;
	}

	write(1, "Enter the name of the archive:", 30);
	read(0,name_in,1024);
	k=strcspn(name_in,"\n");
	name_in[k]=0;
	strcat(name_in,end);
	k=0;

////////////////////////////////////////////////////////
	in = open(name_in, O_RDONLY);
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
		memset(name,0,1024);
		memset(block,0,1024);
		k--;
	}

	close(in);
	unlink(name_in);
}

void menu(void)
{
	int input = 1;
	char address[1024] = {0};
	int i;

	while(input!=0)
	{
		write(1, "1-create archive:\n", 19);
		write(1, "2-unpacking the archive:\n", 26);
		write(1, "0-exit:\n", 9);
		write(1, "Your choise:\n", 14);
		read(0,&input,2);
		input = input - 2608;
		switch ( input )
		{
			case 1:
			{
				write(1, "Enter the address of the directory:", 35);
				read(0,address,1024);
				i=strcspn(address,"\n");
				address[i]=0;
				arh(address);
				break;
			}
			case 2:
			{
				write(1, "Enter the address of the directory:", 35);
				read(0,address,1024);
				i=strcspn(address,"\n");
				address[i]=0;
				read_file(address);
				break;
			}
			case 0:
				return;
			default:
				write(1, "Input Error!!!!\n", 17);
		}
		memset(address,0,1024);
	}
}

void main(void)
{
	menu();
	exit(0);
}