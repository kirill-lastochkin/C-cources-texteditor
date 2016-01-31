#include <stdio.h>
#include "editor.h"
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

extern WINDOW *menu,*editor;
int opened=0;

//открытие файла с запросом имени в графической форме
//возвращает 1 и сообщение в окне при удаче
//возвращает 0 и сообщение при ошибке
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

//открытие файла
//возвращает указатель расширенную стрктуру
//или NULL в случае ошибки
//на вход подается либо имя файла, который нужно открть
//либо NULL, если нжно сделать запрос имени
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
        //временный флаг, что выделяли память
        k=1;
    }
    //вытаскиваем имя и открываем файл в нужном режиме

    fp=OpenFile_f(ch);
    if(fp==NULL)
    {
        if(k==1) free(ch);
        return NULL;
    }
    //собираем расширенное описание файла
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
    //считаем сколько строк в файле
    for(fsize=1;(fs=fgetc(file1->fp))!=EOF;)
    {
        if(fs=='\n')fsize++;
    }
    //вычисляем границы окна
    file1->width=getmaxx(editor);
    file1->high=getmaxy(editor);
    //кол-во строк
    psize=file1->high;
    //считаем колчичество необходимых страниц
    file1->NumOfPages=fsize/psize;
    if(fsize%psize!=0)
    {
        file1->NumOfPages++;
    }
    //выделяем память
    file1->pages=(struct Page*)calloc(file1->NumOfPages,sizeof(struct Page));
    if(file1->pages==NULL)
    {
        perror("some error\n");
        free(file1->name);
        free(file1);
        return NULL;
    }
    i=j=0;
    //выделяем место под полное заполнение страницы символами
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
    //возвращаем каретку на начало
    fseek(file1->fp,0,SEEK_SET);
    i=j=k=0;
    //считываем всю информацию из файла и записываем в страницы
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
    //текущая страница 1-я
    file1->curpage=&file1->pages[0];
    wclear(editor);
    //печатаем страничку
    PrintPage(file1,0);
    wrefresh(editor);
    curs_set(1);
    //ставим флаг о том, что есть открытый файл
    opened=1;
    return file1;
}

//закрытие файла по указателю на расширенную структуру
//по сути - высвобождение ресурсов и закрытие файлового потока
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

//сохранение файла
//работает после редактирования
void SaveFile(struct File *fp)
{
    int i,j,k,eofj=0;
    i=j=k=0;
    //закрываем файл и открываем для записи, стирая что было раньше
    fclose(fp->fp);
    fp->fp=fopen(fp->name,"w+");
    if(fp->fp!=NULL)
    {
        //с текущего окна считываем все символы, какие есть и сохраняем
        for(j=0;j<fp->high;j++)
            for(k=0;k<fp->width;k++)
            {
                fp->pages[fp->curpage->number].text[j][k]=mvwinch(editor,j,k);
            }
        //проходим по считанным символам
        //если до конца строки пробелы - ставим символ конца строки
        //после последней все строки начинаются с переноса
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
        //делаем текущей последнюю страницу
        fp->curpage=&fp->pages[fp->NumOfPages-1];
        //ищем последнюю линию (после нее только переносы до конца страницы)
        for(j=0;j<fp->high;j++)
        {
            if(EOFcheck(fp,j))
            {
                eofj=j;
                break;
            }
        }
        //начинаем писать в файл
        fseek(fp->fp,0,SEEK_SET);
        for(i=0;i<fp->NumOfPages;i++)
        {
            for(j=0;j<fp->high;j++)
            {
                //на последней странице ищем последнюю строчку и заканчиваем писать
                if(j==eofj&&i==(fp->NumOfPages-1))
                {
                    break;
                }
                //посимвольно пишем в файл
                for(k=0;k<fp->width;k++)
                {
                        fputc(fp->pages[i].text[j][k],fp->fp);
                        //найдя конец строки переходим к следющей
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

//редактирование файла
//печать символов затирает предыдущие
//backspace заменяет предыдщий символ на пробел
//перемещение по файлу с помощью стрелок
//по завершению работы (F3) сохраняет изменения и выходит
//расширить файл дальше последней страницы нельзя
void EditFile(struct File *fp)
{
    int fs,x,y,i,j,k; //i -строка, j-столбец
    if(fp!=NULL&&opened)
    {
        //переход на 1 страницу, 0 позицию курсора
        i=j=k=0;
        fp->curpage=&fp->pages[0];
        wclear(editor);
        PrintPage(fp,0);
        curs_set(1);
        wmove(editor,0,0);
        wrefresh(editor);
        noecho();
        //пока не нажмем сохранить, бдет работать
        while((fs=wgetch(editor))!=KEY_F(3))
        {
            //обработка управляющих клавиш
            if(fs==KEY_UP||fs==KEY_DOWN||fs==KEY_LEFT||fs==KEY_RIGHT||fs==KEY_BACKSPACE)
            {
                if(fs==KEY_UP)
                {
                    //перемещаем курсор на экране и по ячейкам страницы
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
                    wprintw(editor," ");
                    ArrowHandler(0,-1);
                }
            }
            else
            {
                fp->pages[i].text[j][k]=fs;
                wprintw(editor,"%c",fs);
            }
        }
        //по нажатию F3 прекращается работа: сохранение и закрытие
        curs_set(0);
        SaveFile(fp);
        wclear(editor);
        wrefresh(editor);
        CloseFile(fp);
    }
}

//проверка на конец строки
//mvwinch читает пробелы, но не знаки переноса, поэтом анализируем
//если до конца экрана были только проблеы - строка кончилась
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

//аналогичная фнкция на определение конца файла
//применяется к последней странице, определяет номер последней строки со значениями на странице
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

