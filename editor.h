#ifndef _EDITOR_H
#define _EDITOR_H

#define FILENAMELEN 50

void InitScreen(void);       //стартовый экран
void DeleteScreen(void);     //удаление экранных данных
void ArrowHandler(int,int);  //обработчик нажатий клавишей-стрелок


int NewFile(void);            //общая функция создания нового файла
struct File* OpenFile(char *argv);  //открытие файла
void SaveFile(struct File*);  //сохранение
void EditFile(struct File*);  //редактирование

char* FileNameRequest(char *fname);  //графическая функция для запроса имени файла

void CloseFile(struct File*);   //закрытие открытого файла

int NewFile_f(char*);           //новый файл, функции работы с файлами
FILE* OpenFile_f(char*);        //открытие файла, функции работы с файлами

void ErrMsg(char*);                            //выдача графического сообщения
void PrintPage(struct File *fp,int);           //печать страницы по номеру
int EOLcheck(char *str, int pos,int len);              //проверка на конец строки
int EOFcheck(struct File *fp, int strnum);     //проверка на конец файла
//страница с текстом и номерами
struct Page
{
    char **text;
    int number;
};
//расширенная структура для работы с файлами
struct File
{
    struct Page *pages,*curpage; //указатели на все страницы, на текущую страницу
    FILE *fp;                    //присоединенный файловый поток
    int width,high,NumOfPages;   //ширина и высота страницы, число страниц
    char *name;                  //имя присоединенного файла
};

#endif
