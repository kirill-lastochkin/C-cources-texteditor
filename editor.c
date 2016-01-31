#include <stdio.h>
#include "editor.h"
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

extern WINDOW *menu,*editor;
int opened=0;

//�������� ����� � �������� ����� � ����������� �����
//���������� 1 � ��������� � ���� ��� �����
//���������� 0 � ��������� ��� ������
int NewFile(void)
{
    char *ch;
    int chk;
    ch=(char*)malloc(sizeof(char)*FILENAMELEN);
    if(ch==NULL)
    {
        perror("memory error\n");
        return 0;
    }
    ch=FileNameRequest(ch);
    chk=NewFile_f(ch);
    free(ch);
    if(chk)
    {
        ErrMsg("File created, now you can open it");
        return 1;
    }
    else
        return 0;
}

//�������� �����
//���������� ��������� ����������� ��������
//��� NULL � ������ ������
//�� ���� �������� ���� ��� �����, ������� ����� ������
//���� NULL, ���� ���� ������� ������ �����
struct File* OpenFile(char *ch)
{
    char fs;
    int fsize,psize,i,j,nop,nop2,k=0;
    FILE *fp;
    struct File *file1;
    if(ch==NULL)
    {
        ch=(char*)malloc(sizeof(char)*FILENAMELEN);
        if(ch==NULL)
        {
            perror("some error\n");
            return NULL;
        }
        ch=FileNameRequest(ch);
        //��������� ����, ��� �������� ������
        k=1;
    }
    //����������� ��� � ��������� ���� � ������ ������

    fp=OpenFile_f(ch);
    if(fp==NULL)
    {
        if(k==1) free(ch);
        return NULL;
    }
    //�������� ����������� �������� �����
    file1=(struct File*)malloc(sizeof(struct File));
    if(file1==NULL)
    {
        perror("some error\n");
        if(k==1) free(ch);
        return NULL;
    }
    file1->fp=fp;
    file1->name=(char*)malloc(sizeof(char)*(strlen(ch)+1));
    strcpy(file1->name,ch);
    if(k==1) free(ch);
    //������� ������� ����� � �����
    for(fsize=1;(fs=fgetc(file1->fp))!=EOF;)
    {
        if(fs=='\n')fsize++;
    }
    //��������� ������� ����
    file1->width=getmaxx(editor);
    file1->high=getmaxy(editor);
    //���-�� �����
    psize=file1->high;
    //������� ����������� ����������� �������
    file1->NumOfPages=fsize/psize;
    if(fsize%psize!=0)
    {
        file1->NumOfPages++;
    }
    //�������� ������
    file1->pages=(struct Page*)calloc(file1->NumOfPages,sizeof(struct Page));
    if(file1->pages==NULL)
    {
        perror("some error\n");
        free(file1->name);
        free(file1);
        return NULL;
    }
    i=j=0;
    //�������� ����� ��� ������ ���������� �������� ���������
    for(i=0;i<file1->NumOfPages;i++)
    {
        file1->pages[i].text=(char**)calloc(file1->high,sizeof(char*));
        if(file1->pages[i].text==NULL)
        {
            perror("some error\n");
            nop=i;
            for(i=0;i<nop;i++)
            {
                free(file1->pages[i].text);
            }
            free(file1->pages);
            free(file1->name);
            free(file1);
            return NULL;
        }
        for(j=0;j<file1->high;j++)
        {
            file1->pages[i].text[j]=(char*)calloc(file1->width,sizeof(char));
            if(file1->pages[i].text[j]==NULL)
            {
                perror("some error\n");
                nop=i;
                nop2=j;
                for(i=0;i<nop;i++)
                {
                    for(j=0;j<nop2;j++)
                        free(file1->pages[i].text[j]);
                    free(file1->pages[i].text);
                }
                free(file1->pages);
                free(file1->name);
                free(file1);
                return NULL;
            }
        }
        file1->pages[i].number=i;
    }
    //���������� ������� �� ������
    fseek(file1->fp,0,SEEK_SET);
    i=j=k=0;
    //��������� ��� ���������� �� ����� � ���������� � ��������
    while(fgets(file1->pages[i].text[j],file1->width,file1->fp)!=NULL)
    {
        j++;
        if(j==file1->high)
        {
            j=0;
            i++;
            //wclear(editor);
        }
        if(i==file1->NumOfPages) break;
    }
    //������� �������� 1-�
    file1->curpage=&file1->pages[0];
    wclear(editor);
    //�������� ���������
    PrintPage(file1,0);
    wrefresh(editor);
    curs_set(1);
    //������ ���� � ���, ��� ���� �������� ����
    opened=1;
    return file1;
}

//�������� ����� �� ��������� �� ����������� ���������
//�� ���� - ������������� �������� � �������� ��������� ������
void CloseFile(struct File *file1)
{
    int i,j;
    if(file1!=NULL)
    {
        for(i=0;i<file1->NumOfPages;i++)
        {
            for(j=0;j<file1->high;j++)
            {
                free(file1->pages[i].text[j]);
            }
            free(file1->pages[i].text);
        }
        free(file1->pages);
        free(file1->name);
        fclose(file1->fp);
        free(file1);
        opened=0;
    }
}

//���������� �����
//�������� ����� ��������������
void SaveFile(struct File *fp)
{
    int i,j,k,eofj=0;
    i=j=k=0;
    //��������� ���� � ��������� ��� ������, ������ ��� ���� ������
    fclose(fp->fp);
    fp->fp=fopen(fp->name,"w+");
    if(fp->fp!=NULL)
    {
        //� �������� ���� ��������� ��� �������, ����� ���� � ���������
        for(j=0;j<fp->high;j++)
            for(k=0;k<fp->width;k++)
            {
                fp->pages[fp->curpage->number].text[j][k]=mvwinch(editor,j,k);
            }
        //�������� �� ��������� ��������
        //���� �� ����� ������ ������� - ������ ������ ����� ������
        //����� ��������� ��� ������ ���������� � ��������
        for(j=0;(j<fp->high);j++)
        {
            for(k=0;k<fp->width;k++)
            {
                if(EOLcheck(fp->pages[fp->curpage->number].text[j],k,fp->width))
                {
                    fp->pages[fp->curpage->number].text[j][k]='\n';
                    break;
                }
            }
        }
        //������ ������� ��������� ��������
        fp->curpage=&fp->pages[fp->NumOfPages-1];
        //���� ��������� ����� (����� ��� ������ �������� �� ����� ��������)
        for(j=0;j<fp->high;j++)
        {
            if(EOFcheck(fp,j))
            {
                eofj=j;
                break;
            }
        }
        //�������� ������ � ����
        fseek(fp->fp,0,SEEK_SET);
        for(i=0;i<fp->NumOfPages;i++)
        {
            for(j=0;j<fp->high;j++)
            {
                //�� ��������� �������� ���� ��������� ������� � ����������� ������
                if(j==eofj&&i==(fp->NumOfPages-1))
                {
                    break;
                }
                //����������� ����� � ����
                for(k=0;k<fp->width;k++)
                {
                        fputc(fp->pages[i].text[j][k],fp->fp);
                        //����� ����� ������ ��������� � ��������
                        if(fp->pages[i].text[j][k]=='\n')
                        {
                            break;
                        }
                }
            }
        }
    }
    else
    {
        ErrMsg("no such file");
    }
}

//�������������� �����
//������ �������� �������� ����������
//backspace �������� ��������� ������ �� ������
//����������� �� ����� � ������� �������
//�� ���������� ������ (F3) ��������� ��������� � �������
//��������� ���� ������ ��������� �������� ������
void EditFile(struct File *fp)
{
    int fs,x,y,i,j,k; //i -������, j-�������
    if(fp!=NULL&&opened)
    {
        //������� �� 1 ��������, 0 ������� �������
        i=j=k=0;
        fp->curpage=&fp->pages[0];
        wclear(editor);
        PrintPage(fp,0);
        curs_set(1);
        wmove(editor,0,0);
        wrefresh(editor);
        noecho();
        //���� �� ������ ���������, ���� ��������
        while((fs=wgetch(editor))!=KEY_F(3))
        {
            //��������� ����������� ������
            if(fs==KEY_UP||fs==KEY_DOWN||fs==KEY_LEFT||fs==KEY_RIGHT||fs==KEY_BACKSPACE)
            {
                if(fs==KEY_UP)
                {
                    //���������� ������ �� ������ � �� ������� ��������
                    ArrowHandler(-1,0);
                    getyx(editor,y,x);
                    i=fp->curpage->number;
                    j=y;
                    k=x;
                }
                if(fs==KEY_DOWN)
                {
                    ArrowHandler(1,0);
                    getyx(editor,y,x);
                    i=fp->curpage->number;
                    j=y;
                    k=x;
                }
                if(fs==KEY_LEFT)
                {
                    ArrowHandler(0,-1);
                    getyx(editor,y,x);
                    i=fp->curpage->number;
                    j=y;
                    k=x;
                }
                if(fs==KEY_RIGHT)
                {
                    ArrowHandler(0,1);
                    getyx(editor,y,x);
                    i=fp->curpage->number;
                    j=y;
                    k=x;
                }
                if(fs==KEY_BACKSPACE)
                {
                    ArrowHandler(0,-1);
                    wdelch(editor);
                    //ArrowHandler(0,-1);
                }
            }
            else
            {
                fp->pages[i].text[j][k]=fs;
                wprintw(editor,"%c",fs);
            }
        }
        //�� ������� F3 ������������ ������: ���������� � ��������
        curs_set(0);
        SaveFile(fp);
        wclear(editor);
        wrefresh(editor);
        CloseFile(fp);
    }
}

//�������� �� ����� ������
//mvwinch ������ �������, �� �� ����� ��������, ������ �����������
//���� �� ����� ������ ���� ������ ������� - ������ ���������
int EOLcheck(char *str, int pos, int len)
{
    int i,check=0;
    for(i=pos;i<len;i++)
    {
        if(str[i]!=' ')
        {
            check=1;
            break;
        }
    }
    if(check!=0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//����������� ������ �� ����������� ����� �����
//����������� � ��������� ��������, ���������� ����� ��������� ������ �� ���������� �� ��������
int EOFcheck(struct File *fp, int strnum)
{
    int i,check=0;
    for(i=strnum;i<fp->high;i++)
    {
        if(fp->pages[fp->curpage->number].text[i][0]!='\n')
        {
            check=1;
            break;
        }
    }
    if(check!=0)
        return 0;
    else
        return 1;
}

