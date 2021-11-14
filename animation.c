#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include "audio.h"
#include "console_draw3.h"
#include "kb_input2.h"
#include "font.h"

#define REFRESH_DURATION 5

void putString(int x, int y, char *p, int fg_color, int bg_color);

typedef struct _image {
    int row;
    int col;
    char *pixel;
    int *color;
} Image;

Image * read_image(char *pixel_file, char *color_file);

typedef struct _robot {
    int blink_mode;
    int tick;
    int x;
    int y;
    Image *blink[2];
} Robot;

typedef struct _garbage {
    int blink_mode;
    int tick;
    int x;
    int y;
    Image *blink[5];
} Garbage;

void show_garbage(Garbage *ap,int mode);
void show_robot(Robot *rp);
void show_image(Image *ip, int offx, int offy);
void destroy_image(Image *ip);
char getpixel(Image *ip, int x, int y);
int getcolor(Image *ip, int x, int y);
void show_frog(Robot *fp);
void move_frog(Robot *fp);
void show_apple(Robot *ap, int mode);
int strong=0;

int main(void)
{
    int key_left=0,key_right=0,key_space=0;
    int game_tick = 0;
    int i,j,c;
    unsigned seed;
    int show[25]={0};
    char scr[10]={'/0'};
    int score = 0;
    int IsEnding=0;
    Font *str = read_font("font.txt");

    clock_t startc;
    Garbage garbage;
    Robot robot;
    robot.blink[0] = read_image("0115ST.pixel", "0115ST.color");
    robot.blink[1] = read_image("0115EX.pixel", "0115EX.color");
    robot.x=0;
    robot.y=0;
    robot.blink_mode = 0;

    initializeKeyInput();
    startc = clock();

    while (1) {
        if ((double)(clock()-startc) > 0.01*CLOCKS_PER_SEC) {
            game_tick++;
            startc = clock();
        }
        if (game_tick > REFRESH_DURATION) {
            game_tick = 0;
            clearScreen();
            show_robot(&robot);
            drawCmdWindow();
        }
        if (KEY_DOWN(VK_ESCAPE)) {
            return 0;
        }

        if (KEY_DOWN(VK_RETURN)) {
            if(robot.blink_mode == 0){
                break;
            }else{
                return 0;
            }
        }

        if (KEY_DOWN(VK_LEFT)) {
            key_left=1;
        }else if( KEY_DOWN(VK_RIGHT)){
            key_right=1;
        }
        if ( ( KEY_UP(VK_LEFT) && key_left)|| (KEY_UP(VK_RIGHT) && key_right) ){
            if(robot.blink_mode == 0) {
                    robot.blink_mode = 1;
            }
            else {robot.blink_mode = 0;}
            key_left=0;
            key_right=0;
        }
    }

    clearScreen();

    Robot frog;
    frog.blink[0]=read_image("girl04.pixel","girl04.color");
    frog.y=100;
    frog.x=100;

    Robot apple[20];//畫面上總共只會有20個蘋果和青蛙
    for(i=0;i<20;i++){
        apple[i].blink[0]=read_image("app02.pixel","app02.color");
        apple[i].blink[1]=read_image("frog04.pixel","frog04.color");
        apple[i].y=0;
    }

    seed = (unsigned)time(NULL);
    srand(seed);

    game_tick=0;
    i=0;
    c=0;
    while(!IsEnding){
        if ((double)(clock()-startc) > 0.01*CLOCKS_PER_SEC) {
            game_tick++;
            startc = clock();
        }
        if (game_tick > REFRESH_DURATION) {
            game_tick = 0;
            clearScreen();

            show_frog(&frog);
            if(c==2){
                apple[i].x = 50*(rand()%5 +1)+50;
                apple[i].blink_mode = rand()%2;
                show[i]=rand()%2 - 1;
                c=0;
                i++;
            }
            c++;

            for(j=0;j<20;j++){
                if(apple[j].y>120){
                    show[j]=0;
                    apple[j].y=0;
                }
                if(show[j]){
                    apple[j].y = apple[j].y + 5;
                    show_apple(&apple[j],apple[j].blink_mode);
                }
                if( (apple[j].y >=90) && (apple[j].x == frog.x) ){
                    if(apple[j].blink_mode==0){
                        show[j]=0;
                        apple[j].y=0;
                        score=score+10;
                    }else{
                        if(strong==0){
                            IsEnding=1;
                            break;
                        }
                    }
                }
            }
            sprintf(scr,"%d",score);
            putStringLarge(str, 50, 1, scr, 14);

            drawCmdWindow();

            move_frog(&frog);

            if(i==20){
                i=0;
            }
        }

        if (KEY_DOWN(VK_LEFT)) {
            key_left=1;
        }else if( KEY_DOWN(VK_RIGHT)){
            key_right=1;
        }else if( KEY_DOWN(VK_SPACE)){
            key_space=1;
        }
        if ( KEY_UP(VK_RIGHT)  && (key_right==1) ){
            frog.blink_mode=2;
            key_right=0;
        }
        if ( KEY_UP(VK_LEFT)  && (key_left==1) ){
            frog.blink_mode=1;
            key_left=0;
        }
        if ( KEY_UP(VK_SPACE)  && (key_space==1) ){
            frog.blink_mode=3;
            key_space=0;
        }
    }

    game_tick=0;
    garbage.blink[0]=read_image("you.pixel","you.color");
    garbage.blink[1]=read_image("are.pixel","are.color");
    garbage.blink[2]=read_image("such.pixel","such.color");
    garbage.blink[3]=read_image("garbage3.pixel","garbage3.color");
    garbage.blink[4]=read_image("final.pixel","final.color");
    garbage.x=10;
    garbage.y=10;
    i=0;
    while(1){
        if ((double)(clock()-startc) > 0.01*CLOCKS_PER_SEC) {
                game_tick++;
                startc = clock();
            }
        if (game_tick > 60) {
            if(i==5)
                break;
            garbage.blink_mode=i;
            game_tick = 0;
            clearScreen();
            putStringLarge(str, 20, 50, scr, 14);
            show_garbage(&garbage,garbage.blink_mode);
            drawCmdWindow();
            i++;
        }

    }

    return 0;
}

Image * read_image(char *pixel_file, char *color_file)
{
    FILE *fid1, *fid2;
    int row, col, i, j;
    Image *p;

    fid1 = fopen(pixel_file, "r");
    fid2 = fopen(color_file, "r");
    if (fid1==NULL || fid2==NULL) {
        return NULL;
    } else {
        fscanf(fid1, "%d%d", &row, &col);
        fscanf(fid2, "%d%d", &row, &col); // supposedly, size should be the same
        p = (Image*) malloc(sizeof(Image));
        p->row = row;
        p->col = col;
        p->pixel = (char*) malloc(sizeof(char)*row*col);
        p->color = (int*) malloc(sizeof(int)*row*col);
        fscanf(fid1, "%*c");
        for (i=0; i<row; i++) {
            for (j=0; j<col; j++) {
                fscanf(fid1, "%c", & (p->pixel[i*col+j]));
                fscanf(fid2, "%1x", & (p->color[i*col+j]));
            }
            fscanf(fid1, "%*c");
            fscanf(fid2, "%*c");
        }
        fclose(fid1);
        fclose(fid2);
        return p;
    }
}

char getpixel(Image *ip, int x, int y)
{
    return ip->pixel[y*ip->col + x];
}

int getcolor(Image *ip, int x, int y)
{
    return ip->color[y*ip->col + x];
}


void show_image(Image *ip, int offx, int offy)
{
    int x, y;
    char c;
    for (y=0; y<ip->row; y++) {
            for (x=0; x<ip->col; x++) {
            c = getpixel(ip, x, y);
            if (c!=' ')
            putASCII2( x + offx,
                y + offy, c, getcolor(ip, x, y), 0);
            }
    }
}

void destroy_image(Image *ip)
{
    free(ip->pixel);
    free(ip->color);
    free(ip);
}

void show_robot(Robot *rp)
{
    show_image(rp->blink[rp->blink_mode],rp->x,rp->y);
    return;
}

void show_frog(Robot *fp)
{
    show_image(fp->blink[0],fp->x,fp->y);
    return;
}

void move_frog(Robot *fp)
{

    if (fp->blink_mode==1) {
        if(fp->x>=150){
            fp->x=fp->x-50;
        }
        fp->blink_mode=0;
    }

    if (fp->blink_mode==2) {
        if(fp->x<300){
            fp->x=fp->x+50;
        }
        fp->blink_mode=0;
    }
    if (fp->blink_mode==3) {
        if(fp->x>=250){
            strong++;
        }
        fp->blink_mode=0;
    }

}

void show_apple(Robot *ap,int mode)
{
    show_image(ap->blink[mode],ap->x,ap->y);

    return;
}
void show_garbage(Garbage *ap,int mode)
{
    show_image(ap->blink[mode],ap->x,ap->y);

    return;
}
