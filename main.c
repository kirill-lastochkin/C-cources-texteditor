#include <stdio.h>
#include "editor.h"
#include <stdlib.h>
#include <ncurses.h>

extern WINDOW *menu,*editor;
extern int opened;
struct File *fptr;

int main(int argc, char* argv[])
{
    int ch;
    fptr=NULL;
    InitScreen();
    if(argc!=1)
    {
        fptr=OpenFile(argv[1]);
    }
    while((ch=wgetch(menu))!=KEY_F(5))
    {
        if(ch==KEY_F(1))
        {
            NewFile();
        }
        if(ch==KEY_F(2))
        {
            fptr=OpenFile(NULL);
        }
        if(ch==KEY_F(4))
        {
            EditFile(fptr);
        }

        if(ch==KEY_UP)
        {
            ArrowHandler(-1,0);
        }
        if(ch==KEY_DOWN)
        {
            ArrowHandler(1,0);
        }
        if(ch==KEY_LEFT)
        {
            ArrowHandler(0,-1);
        }
        if(ch==KEY_RIGHT)
        {
            ArrowHandler(0,1);
        }
        if(ch==KEY_F(6))
        {
            ErrMsg("test error message");
        }
    }
    //на случай если совершалось редактирование и файл уже закрыт
    if(opened)
    {
        CloseFile(fptr);
    }
    DeleteScreen();
    return 0;
}

