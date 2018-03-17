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

	write(1, "Enter the name of the arh:", 26);
	read(0,name_out,1024);
	write(1, name_out, sizeof(name_out));

	k=strcspn(name_out,"\n");
	name_out[k]=0;

	strcat(name_out,end);
	printf("%s\n", name_out);

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
	printf("%d\n", k);
	closedir(dp);
	close(out);
}


void read_file(void)
{
	char block[1024] = {0};
	char name[1024] = {0};
	int size;
	int in,out,i,j;
	int nread;
	int k = 0;

	in = open("file_name.out", O_RDONLY);
	lseek(in,-4,SEEK_END);
	read(in, &k, 4);
	lseek(in,0,SEEK_SET);
	printf("k: %d\n", k);
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
	//unlink("/home/sleepovski/Документы/os/file_name.out");
}

/*void menu(void)
{
	char input[1]={0};
	write(1, "???:\n", 6);
	read(0,input,1);
	write(1, input, sizeof(input));
	printf("%d\n", input);
	switch ( input )
	{
		case 1://создание
			{create(); break;}
		case 2://вывод
		{disp(); break;}
		case 0:
			return;
		default:
			write(1, "Input Error!!!!\n", 17);
	}
}
*/


void main(void)
{
	char address[1024]="/home/sleepovski/test/";
	int i;
	//menu();
	write(1, "Enter the address of the directory:", 35);
	read(0,address,1024);
	write(1, address, sizeof(address));
	i=strcspn(address,"\n");
	address[i]=0;
	arh(address);
	//read_file();
	exit(0);
}