/*
 * Brian Chrzanowski's Terminal Pong
 * Fri Dec 02, 2016 17:00
 */

#include <string.h>
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/msp432p4111_classic.h"
#include <stdio.h>

extern Graphics_Context g_sContext;

#define DELAY 30000
#define ScreenX             32
#define ScreenY             25

typedef struct paddle {
    /* paddle variables */
    int x;
    int y;    /* y is the 'top' of the paddle */
    int len;
    int score;
} paddle_t;

typedef struct ball {
    /* ball variables */
    int x;
    int y;
    int next_x;
    int next_y;
    int x_vel;
    int y_vel;
} ball_t;

typedef struct dimensions {
    int x;
    int y;
} dimensions_t;

void draw_ball(ball_t *input);
void draw_paddle(paddle_t *paddle);
void draw_score(paddle_t *inpt_paddle, dimensions_t *wall);
void paddle_collisions(ball_t *inpt_ball, paddle_t *inpt_paddle);
void paddle_pos(paddle_t *pddl, dimensions_t *wall, char dir);

int wall_collisions(ball_t *usr_ball, dimensions_t *walls);
int kbdhit(int);
char cgetc();


int pong(void)
{

    char buf[128];
    char y, *src;
    /* initialize curses */

    int k = 1;
    int i=0;

    dimensions_t walls = { 0 };
    /* get dimensions */
    walls.y = ScreenY; //25
    walls.x = ScreenX; //32

    /* set the paddle variables */
    paddle_t usr_paddle = { 0 };

    usr_paddle.x = 5;
    usr_paddle.y = 11;
    usr_paddle.len = walls.y / 4; usr_paddle.score = 0;

    /* set the ball */
    ball_t usr_ball = { 0 };

    usr_ball.x = walls.x / 2;
    usr_ball.y = walls.y / 2;
    usr_ball.next_x = 0;
    usr_ball.next_y = 0;
    usr_ball.x_vel = 1;
    usr_ball.y_vel = 1;

    /* we actually have to store the user's keypress somewhere... */
    char keypress = 0;
    int run = 1;

    while (run) {
        while (kbdhit(k)) {


            walls.y = ScreenY;
            walls.x = ScreenX;

            //clear(); /* clear screen of all printed chars */
            Graphics_clearDisplay(&g_sContext);

            draw_ball(&usr_ball);
            draw_paddle(&usr_paddle);
            draw_score(&usr_paddle, &walls);


            /* set next positions */
            usr_ball.next_x = usr_ball.x + usr_ball.x_vel;
            usr_ball.next_y = usr_ball.y + usr_ball.y_vel;

            /* check for collisions */
            paddle_collisions(&usr_ball, &usr_paddle);
            if (wall_collisions(&usr_ball, &walls)) {
                run = 0;
                break;
            }


        /* we fell out, get the key press */
        keypress = cgetc();


        switch (keypress) {
        case 'j':
        case 'k':
            paddle_pos(&usr_paddle, &walls, keypress);
            break;

        case 'p': /* pause functionality, because why not */
            src  = "PAUSE - press any key to resume";
            sprintf(buf,"%.*s", walls.x, src);
            Graphics_drawString(&g_sContext, (int8_t *)buf, walls.x, 0, 1*5, OPAQUE_TEXT);
            while (!cgetc()) {
                //for(i=0;i<DELAY * 7;i++);
            }
            break;

        case 'q':
            run = 0;
            k=0;
            break;

        }

    }
    }


    Graphics_clearDisplay(&g_sContext);
    src = "GAME OVER\nFinal Score:";
    sprintf(buf,"%.*s%d", walls.x, src, usr_paddle.score);
    Graphics_drawString(&g_sContext, (int8_t*) buf, walls.x, 15, 26, OPAQUE_TEXT);

    return 0;
}

/*
 * function : paddle_pos
 * purpose  : have a function that will return a proper 'y' value for the paddle
 * input    : paddle_t *inpt_paddle, dimensions_t *wall, char dir
 * output   : void
 */

void paddle_pos(paddle_t *pddl, dimensions_t *wall, char dir)
{
    if (dir == 'k') { /* moving down */
        if (pddl->y + pddl->len + 1 <= wall->y)
            pddl->y+=2;
    } else {          /* moving up (must be 'k') */
        if (pddl->y - 1 >= 0)
            pddl->y-=2;

    }

    return;
}

/*
 * function : wall_collisions
 * purpose  : to check for collisions on the terminal walls
 * input    : ball_t *, dimensions_t *
 * output   : nothing (stored within the structs)
 */
int wall_collisions(ball_t *usr_ball, dimensions_t *walls)
{
    /* check if we're supposed to leave quick */
    if (usr_ball->next_x < 0) {
        return 1;
    }

    /* check for X */
    if (usr_ball->next_x >= walls->x) {
        usr_ball->x_vel *= -1;
    } else {
        usr_ball->x += usr_ball->x_vel;
    }

    /* check for Y */
    if (usr_ball->next_y >= walls->y || usr_ball->next_y < 0) {
        usr_ball->y_vel *= -1;
    } else {
        usr_ball->y += usr_ball->y_vel;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

void paddle_collisions(ball_t *inpt_ball, paddle_t *inpt_paddle)
{
    /*
    * simply check if next_% (because we set the next_x && next_y first)
    * is within the bounds of the paddle's CURRENT position
    */

    if (inpt_ball->next_x == inpt_paddle->x) {
        if (inpt_paddle->y <= inpt_ball->y &&
            inpt_ball->y <=
            inpt_paddle->y + inpt_paddle->len) {

            inpt_paddle->score++;
            inpt_ball->x_vel *= -1;
        }
    }

    return;
}

/* -------------------------------------------------------------------------- */

/*
 * functions : draw_ball && draw_paddle
 * purpose   : condense the drawing functions to functions
 * input     : ball_t * && paddle_t *
 * output    : void
 */
void draw_ball(ball_t *input)
{
    Graphics_drawString(&g_sContext, (int8_t*) "O", 1, (input->x)*4,(input->y)*5, OPAQUE_TEXT);
    return;
}

void draw_paddle(paddle_t *paddle)
{
    int i;

    for (i = 0; i < paddle->len; i++)
        Graphics_drawString(&g_sContext,
                                          (int8_t *)"|",
                                          1,
                                          (paddle->x)*4,
                                          (paddle->y + i)*5,
                                          OPAQUE_TEXT);

    return;
}

void draw_score(paddle_t *inpt_paddle, dimensions_t *wall)
{
    char y, *src;
    char buf[128];
    int score;
    src = "Score: ";

    y =0;

    score = (inpt_paddle->score);


    sprintf(buf,"%.*s %d", wall->x, src,score); //0열 y행에 출력

    Graphics_drawString(&g_sContext,
                                      (int8_t *)buf,
                                      wall->x,
                                      wall->x / 2 - 7,
                                      0*5,
                                      OPAQUE_TEXT);
    return;
}

/* -------------------------------------------------------------------------- */

/*
 * function : kbdhit
 * purpose  : find out if we've got something in the input buffer
 * input    : void
 * output   : 0 on none, 1 on we have a key
 */

//int kbdhit()
//{
//  int key = cgetc();
//
//  if (!key) {
//      return 0;
//  } else {
//      return 1;
//  }
//}

int kbdhit(int k)
{
    if(k==1)
        return 1;
    else if(k==0)
        return 0;
}


char cgetc(void)
{
    static int s1state=0,s2state=0,bs1state=0,bs2state=0; // not pressed

    if (s1state==0 && !(P1->IN & BIT1)) {
        s1state=1;     // S1 in mainboard pressed
    } else if (s1state==1 && (P1->IN & BIT1)) {
        s1state=0;     // S1 in mainboard not pressed
        return 'p';     //start game in Demo screen
    }
    if (s2state==0 && !(P1->IN & BIT4)) {
        s2state=1;     // S2 in mainboard pressed
    } else if (s2state==1 && (P1->IN & BIT4)) {
        s2state=0;     // S2 in mainboard not pressed
        return 'q';    // restart
    }
    if (bs1state==0 && !(P3->IN & BIT5)) {
        bs1state=1;     // S1 in boostxl-edumkii pressed
    } else if (bs1state==1 && (P3->IN & BIT5)) {
        bs1state=0;     // S1 in boostxl-edumkii not pressed
        return 'k';
    }
    if (bs2state==0 && !(P5->IN & BIT1)) {
        bs2state=1;     // S2 in boostxl-edumkii pressed
    } else if (bs2state==1 && (P5->IN & BIT1)) {
        bs2state=0;     // S2 boostxl-edumkii not pressed
        return 'j';
    }

    return 0;
}
