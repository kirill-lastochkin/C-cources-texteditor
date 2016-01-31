#include <stdio.h>
#include "editor.h"
#include <ncurses.h>
#include <curses.h>

extern WINDOW *stdscr;
WINDOW *menu,*editor;
extern struct File *fptr;

//������ ��������� ����, ������� ��� ������ �������� ��� ���������� ������
void InitScreen(void)
{
    initscr();
    cbreak();
    noecho();
    clear();
    curs_set(0);
    //�����
    start_color();
    init_pair(1,COLOR_YELLOW,COLOR_BLUE);
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
    //������� ���� ���� � ���������
    menu=newwin(3,getmaxx(stdscr)-1,getmaxy(stdscr)-3,1);
    editor=newwin(getmaxy(stdscr)-4,getmaxx(stdscr)-1,1,1);
    //������ �� ������ �����
    wbkgd(menu,COLOR_PAIR(1));
    wbkgd(editor,COLOR_PAIR(2));
    box(menu,'|','-');
    box(editor,'!','~');
    //�������� ����
    wattron(menu,COLOR_PAIR(1)| A_BOLD);
    wmove(menu,1,12);
    wprintw(menu,"F1 NEW");
    wmove(menu,1,getmaxx(menu)/5+10);
    wprintw(menu,"F2 OPEN");
    wmove(menu,1,getmaxx(menu)*2/5+10);
    wprintw(menu,"F3 SAVE AND CLOSE");
    wmove(menu,1,getmaxx(menu)*3/5+10);
    wprintw(menu,"F4 EDIT");
    wmove(menu,1,getmaxx(menu)*4/5+10);
    wprintw(menu,"F5 QUIT");
    //������� ����������� � ������
    wattron(editor,COLOR_PAIR(2)|A_BOLD);
    wmove(editor,getmaxy(editor)/2,getmaxx(editor)/2-20);
    wprintw(editor,"Start work with editor by pressing F1-F5!");
    //��������
    wrefresh(menu);
    wrefresh(editor);
    //��������� ��������� ����������� ������
    keypad(menu,TRUE);
    keypad(editor,TRUE);

}

//����������� ������� ����, ����������� ������
void DeleteScreen(void)
{
    delwin(menu);
    delwin(editor);
    endwin();
    clear();
}

//������ ����� ����� ��� ������/��������
//������ ���� � ���������� � ��������� ������ ���
//���������� ���� ������
char* FileNameRequest(char *fname)
{
    WINDOW *nf;
    int width,high;
    wclear(editor);
    wrefresh(editor);
    width=getmaxx(editor)/4;
    high=4;
    nf=derwin(editor,high,width,(getmaxy(editor)-high)/2,(getmaxx(editor)-width)/2);
    box(nf,'|','-');
    wbkgd(nf,COLOR_PAIR(1));
    wattron(nf,A_BOLD|COLOR_PAIR(1));
    wmove(nf,1,1);
    wprintw(nf,"type file name");
    wmove(nf,2,1);
    wattron(nf,COLOR_PAIR(2));
    curs_set(1);
    echo();
    wrefresh(nf);
    wscanw(nf,"%s",fname);
    noecho();
    curs_set(0);
    wclear(nf);
    wrefresh(nf);
    delwin(nf);
    wclear(editor);
    wrefresh(editor);
    return fname;
}

//���� ��� ��������� � �������
void ErrMsg(char* msg)
{
    WINDOW *nf;
    int width,high;
    wclear(editor);
    wrefresh(editor);
    width=getmaxx(editor)/4;
    high=4;
    nf=derwin(editor,high,width,(getmaxy(editor)-high)/2,(getmaxx(editor)-width)/2);
    box(nf,'|','-');
    wbkgd(nf,COLOR_PAIR(1));
    wattron(nf,A_BOLD|COLOR_PAIR(1));
    wmove(nf,1,1);
    wprintw(nf,msg);
    wmove(nf,2,1);
    wprintw(nf,"press any key to continue");
    wrefresh(nf);
    getchar();
    wclear(nf);
    wrefresh(nf);
    delwin(nf);
    wclear(editor);
    wrefresh(editor);
}


//��������� �������� ������� (������ ��� ��������� �����)
void ArrowHandler(int y0,int x0)
{
    int x,y;
    if(fptr!=NULL)
    {
        getyx(editor,y,x);
        //������� ����
        if(y==(fptr->high-1)&&y0>0&&fptr->curpage->number!=(fptr->NumOfPages-1))
        {
            wclear(editor);
            //������ �������
            fptr->curpage=&fptr->pages[fptr->curpage->number+1];
            PrintPage(fptr,fptr->curpage->number);
            wmove(editor,0,x);
        }
        else
        {
            //������� �����
            if(y==0&&y0<0&&fptr->curpage->number!=0)
            {
                wclear(editor);
                fptr->curpage=&fptr->pages[fptr->curpage->number-1];
                PrintPage(fptr,fptr->curpage->number);
                wmove(editor,fptr->high-1,x);
            }
            else
            {
                if(!((x==0&&x0<0)||(x==(fptr->width-1)&&x0>0)))
                wmove(editor,y+y0,x+x0);
            }
        }
        wrefresh(editor);
    }
}

//����������� �������� �� ������
void PrintPage(struct File *fp,int number)
{
    int i;
    for(i=0;i<fp->high;i++)
    {
        wprintw(editor,fp->pages[number].text[i]);
    }
    wrefresh(editor);
}

