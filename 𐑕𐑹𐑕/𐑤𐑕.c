#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include <time.h>

//all the information abt a file
typedef struct
{
	struct stat;
	char name[256];
	char permission[11];
	char color[16];
	char extension[256];
	char indicator;
	char owner_name[256];
	char group_name[256];
} file_info;

void get_username_from_uid(int uid, char *username)
{
	struct passwd *pws;
	pws = getpwuid(uid);

	strncpy(username, pws->pw_name, 256);
}

void get_groupname_from_gid(int gid, char *groupname)
{
	struct group *grp;
	grp = getgrgid(gid);

	strncpy(groupname, grp->gr_name, 256);
}

void construct_permission_str(mode_t mode, char *permission)
{
	// Get the first character from file type
	if      (S_ISREG(mode)) permission[0] = '-';
	else if (S_ISDIR(mode)) permission[0] = 'd';
	else if (S_ISLNK(mode)) permission[0] = 'l';
	else if (S_ISCHR(mode)) permission[0] = 'c';
	else if (S_ISBLK(mode)) permission[0] = 'b';
	else if (S_ISSOCK(mode)) permission[0] = 's';
	else if (S_ISFIFO(mode)) permission[0] = 'p';

	permission[1] = (mode & S_IRUSR) ? 'r' : '-';
	permission[2] = (mode & S_IWUSR) ? 'w' : '-';
	permission[3] = (mode & S_IXUSR) ? 'x' : '-';
    
	permission[4] = (mode & S_IRGRP) ? 'r' : '-';
	permission[5] = (mode & S_IWGRP) ? 'w' : '-';
	permission[6] = (mode & S_IXGRP) ? 'x' : '-';

	permission[7] = (mode & S_IROTH) ? 'r' : '-';
	permission[8] = (mode & S_IWOTH) ? 'w' : '-';
	permission[9] = (mode & S_IXOTH) ? 'x' : '-';
	permission[10] = 0;
}

void get_file_extension(char *file_name, char *extension)
{
	char *p = strrchr(file_name, '.');

	if (p) {
		strcpy(extension, strrchr(file_name, '.'));
	}
}

#define COLOR_RESET         "\x1b[0m" 
#define COLOR_REGULAR       COLOR_RESET
#define COLOR_EXECUTABLES   "\x1b[1;32m"
#define COLOR_DIRECTORIES   "\x1b[1;34m"
#define COLOR_LINK          "\x1b[1;36m"
#define COLOR_FIFO          "\x1b[40;33m"
#define COLOR_SOCKET        "\x1b[1;35m"
#define COLOR_BLK_CHR       "\x1b[40;33;1m"
#define COLOR_BROKEN_LINK   "\x1b[40;31;1m"
#define COLOR_ARCHIVE       "\x1b[1;31m"

void get_color(char* permission, char *extension, char *color)
{
    switch (permission[0])
    {
    case '-':
        if (permission[3] == 'x' || permission[6] == 'x' || permission[9] == 'x')
            strcpy(color, COLOR_EXECUTABLES);
        //else if (is_archive_file(extension))
        //    strcpy(color, COLOR_ARCHIVE);
        else
            strcpy(color, COLOR_REGULAR);
        break;
    case 'd':
        strcpy(color, COLOR_DIRECTORIES);
        break;
    case 'l':
        strcpy(color, COLOR_LINK);
        break;
    case 'c':
    case 'b':
        strcpy(color, COLOR_BLK_CHR);
        break;
    case 's':
        strcpy(color, COLOR_SOCKET);
        break;
    case 'p':
        strcpy(color, COLOR_FIFO);
        break;
    default:
        break;
    }
}

char get_indicator(char* permission)
{
    switch (permission[0])
    {
    case '-':
        if (permission[3] == 'x' || permission[6] == 'x' || permission[9] == 'x')
            return '*';
        
        return ' '; 
    case 'd':
        return '/';
    case 'l':
        return '@';
    case 's':
        return '=';
    case 'p':
        return '|';
    default:
        return ' ';
    }
}

int main(int argc, char *argv[])
{
	DIR *dir;
	struct dirent *file;
	file_info files[256];
	dir = opendir(".");
	int i = 0;

	while ((file = readdir(dir)) != NULL) 
	{
		lstat(file->d_name, (struct stat *)&files[i]);
		
		strncpy(files[i].name, file->d_name, 256);
		get_username_from_uid(files[i].st_uid, files[i].owner_name);
		get_groupname_from_gid(files[i].st_gid, files[i].group_name);
		construct_permission_str(files[i].st_mode, files[i].permission);
		get_file_extension(files[i].name, files[i].extension);
		get_color(files[i].permission, files[i].extension, files[i].color);

		files[i].indicator = get_indicator(files[i].permission);
		++i;
	}

	char filetime[20];
	struct tm *timeinfo;

	for(int j=0; j<i; ++j)
	{
		timeinfo = localtime(&files[j].st_mtime);
		strftime(filetime, 20, "%b %-2d %H:%M", timeinfo);

		printf("%llu %lu %s %lu %-8s %-7s %lu %s %s%s%s\n", 
				(unsigned long long) files[j].st_ino, 
				(unsigned long)files[j].st_blocks / 2, 
				files[j].permission, 
				(unsigned long) files[j].st_nlink, 
				files[j].owner_name, 
				files[j].group_name, 
				(unsigned long) files[j].st_size, 
				filetime, 
				files[j].color, 
				files[j].name, 
				COLOR_RESET);
	}
    closedir(dir);
	return EXIT_SUCCESS;
}
