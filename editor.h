#ifndef _EDITOR_H
#define _EDITOR_H

#define FILENAMELEN 50

void InitScreen(void);       //��������� �����
void DeleteScreen(void);     //�������� �������� ������
void ArrowHandler(int,int);  //���������� ������� ��������-�������


int NewFile(void);            //����� ������� �������� ������ �����
struct File* OpenFile(char *argv);  //�������� �����
void SaveFile(struct File*);  //����������
void EditFile(struct File*);  //��������������

char* FileNameRequest(char *fname);  //����������� ������� ��� ������� ����� �����

void CloseFile(struct File*);   //�������� ��������� �����

int NewFile_f(char*);           //����� ����, ������� ������ � �������
FILE* OpenFile_f(char*);        //�������� �����, ������� ������ � �������

void ErrMsg(char*);                            //������ ������������ ���������
void PrintPage(struct File *fp,int);           //������ �������� �� ������
int EOLcheck(char *str, int pos,int len);              //�������� �� ����� ������
int EOFcheck(struct File *fp, int strnum);     //�������� �� ����� �����
//�������� � ������� � ��������
struct Page
{
    char **text;
    int number;
};
//����������� ��������� ��� ������ � �������
struct File
{
    struct Page *pages,*curpage; //��������� �� ��� ��������, �� ������� ��������
    FILE *fp;                    //�������������� �������� �����
    int width,high,NumOfPages;   //������ � ������ ��������, ����� �������
    char *name;                  //��� ��������������� �����
};

#endif
