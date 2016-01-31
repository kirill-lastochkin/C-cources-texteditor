#include <stdio.h>
#include "editor.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

//�������� ������ �����
//���������� 1 ��� ����� � 0 ��� ������
int NewFile_f(char *fname)
{
    int fd,withpath=0;
    unsigned int i;
    //���� ������������ ������ ���� �� ����� (���� ���� /), �� �� ������� ���
    for(i=0;i<strlen(fname);i++)
    {
        if(fname[i]=='/')
        {
            withpath=1;
            break;
        }
    }
    //���� � ������� ��������, �� ��������� ������ �������
    char path[FILENAMELEN+3]="./";
    if(withpath)
    {
        fd=creat(fname,S_IRWXU|S_IRGRP|S_IROTH);
    }
    else
    {
        strcat(path,fname);
        fd=creat(path,S_IRWXU|S_IRGRP|S_IROTH);
    }
    if(fd==-1)
    {
        perror("error");
        ErrMsg("file creation error");
        return 0;
    }
    close(fd);
    return 1;
}

//�������� �����, ���������� �������� ������
//���������� 1 ��� ����� � 0 ��� ������
FILE* OpenFile_f(char *fname)
{
    FILE *fp;
    char path[FILENAMELEN+3]="./";
    int withpath=0;
    unsigned int i;
    //���� ������������ ������ ���� �� ����� (���� ���� /), �� �� ������� ���
    for(i=0;i<strlen(fname);i++)
    {
        if(fname[i]=='/')
        {
            withpath=1;
            break;
        }
    }
    if(withpath)
    {
        fp=fopen(fname,"r+");
    }
    else
    {
        strcat(path,fname);
        fp=fopen(path,"r+");
    }
    if(fp==NULL)
    {
        ErrMsg("file open error");
        return NULL;
    }
    return fp;
}
