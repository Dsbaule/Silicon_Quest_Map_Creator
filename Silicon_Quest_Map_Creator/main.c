/*
    Projeto: Silicon Quest Map Creator
    Alunos: Daniel de Souza Baulé
            Stefano Bergamini Poletto
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>

#define WIDTH       800
#define HEIGHT      600
#define FULLSCREEN  0
#define FPS         60
#define SHOWMOUSE   1
#define SHOW_BORDER 1
#define COR_BORDAS  242, 210, 99

#define MAX_COLUNAS 100
#define MAX_LINHAS  100

#define NUM_BLOCOS  6

#define COR_AR      0, 0, 0
#define COR_TERRA   94, 28, 13
#define COR_PEDRA   53, 53, 53
#define COR_SILICIO 249, 249, 249
#define COR_LAVA    255, 116, 21
#define COR_AGUA    0, 128, 255

//==============================================
//GLOBALS
//==============================================
ALLEGRO_EVENT ev;
bool keys[] = {false, false, false, false, false, false, false, false, false, false, false};
enum KEYS {UP, DOWN, LEFT, RIGHT, W, A, S, D, SPACE, ENTER, P};
bool done = false;
bool draw = true;
bool save = false;

int numColunas = 1;
int numLinhas = 1;

char blocos[MAX_LINHAS][MAX_COLUNAS] = {{0}};
const int blockHeight = 50;
const int blockWidth = 50;
int pos_blocos_x = 0;
int pos_blocos_y = 0;
char selectedBlock = 1;

int pos_mouse_x = WIDTH / 2;
int pos_mouse_y = HEIGHT / 2;
int coluna_Mouse = 0;
int linha_Mouse = 0;
int mouseWheelNow = 0;
int mouseWheelBefore = 0;

void readInputs()
{
        //==============================================
        //INPUT + OUTPUT
        //==============================================
        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }
        if(ev.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            pos_mouse_x = ev.mouse.x;
            pos_mouse_y = ev.mouse.y;
            mouseWheelNow = ev.mouse.z;
        }

        // READ MOUSE CLICKS (BUTTONS 1 AND 2)
        if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if(ev.mouse.button & 1)
            {
                blocos[linha_Mouse][coluna_Mouse] = selectedBlock;
            }

            if(ev.mouse.button & 2)
            {
                blocos[linha_Mouse][coluna_Mouse] = 0;
            }
        }
        // READ KEYBOARD INPUT
        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            case ALLEGRO_KEY_LEFT:
                keys[LEFT] = true;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT] = true;
                break;
            case ALLEGRO_KEY_UP:
                keys[UP] = true;
                break;
            case ALLEGRO_KEY_DOWN:
                keys[DOWN] = true;
                break;
            case ALLEGRO_KEY_A:
                keys[A] = true;
                break;
            case ALLEGRO_KEY_D:
                keys[D] = true;
                break;
            case ALLEGRO_KEY_W:
                keys[W] = true;
                break;
            case ALLEGRO_KEY_S:
                keys[S] = true;
                break;
            case ALLEGRO_KEY_P:
                keys[P] = true;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = true;
                break;
            case ALLEGRO_KEY_ENTER:
                keys[ENTER] = true;
                break;
            }
        }
        if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            case ALLEGRO_KEY_LEFT:
                keys[LEFT] = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT] = false;
                break;
            case ALLEGRO_KEY_UP:
                keys[UP] = false;
                break;
            case ALLEGRO_KEY_DOWN:
                keys[DOWN] = false;
                break;
            case ALLEGRO_KEY_A:
                keys[A] = false;
                break;
            case ALLEGRO_KEY_D:
                keys[D] = false;
                break;
            case ALLEGRO_KEY_W:
                keys[W] = false;
                break;
            case ALLEGRO_KEY_S:
                keys[S] = false;
                break;
            case ALLEGRO_KEY_P:
                keys[P] = false;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = false;
                break;
            case ALLEGRO_KEY_ENTER:
                keys[ENTER] = false;
                break;
            }

        }

        // READ MOUSE MOVEMENT (TO BLOCK LIMITS)
        if(((pos_mouse_x - pos_blocos_x) / blockWidth) < numColunas)
            coluna_Mouse = (pos_mouse_x - pos_blocos_x) / blockWidth;
        if(((pos_mouse_y - pos_blocos_y) / blockHeight)< numLinhas)
            linha_Mouse = (pos_mouse_y - pos_blocos_y) / blockHeight;

        // READ MOUSE WHEEL MOVEMENT
        if(mouseWheelNow > mouseWheelBefore){
            mouseWheelBefore = mouseWheelNow;
            selectedBlock++;
            if(selectedBlock >= NUM_BLOCOS)
                selectedBlock = 1;
        }else if(mouseWheelNow < mouseWheelBefore){
            mouseWheelBefore = mouseWheelNow;
            selectedBlock--;
            if(selectedBlock < 1)
                selectedBlock = NUM_BLOCOS - 1;
        }

        // READ MOVEMENT KEYS (WASD + ARROWS)
        if(!(pos_blocos_x >= 0))
            pos_blocos_x += (keys[LEFT] | keys[A]) * 10;
        if(!(pos_blocos_y >= 0))
            pos_blocos_y += (keys[UP] | keys[W]) * 10;
        if(!(pos_blocos_x <= ((-numColunas * blockWidth) + WIDTH)))
            pos_blocos_x -= (keys[RIGHT] | keys[D]) * 10;
        if(!(pos_blocos_y <= ((-numLinhas * blockHeight) + HEIGHT)))
            pos_blocos_y -= (keys[DOWN] | keys[S]) * 10;


}

void saveMap()
{
    //==============================================
    //PROJECT VARIABLES
    //==============================================
    FILE *fp;
    char mapName[100];

    //==============================================
    //AUXILIAR VARIABLES
    //==============================================
    int j, i;

    while((!keys[ENTER]) ^ done ){
        al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 100, 0, "NOME DO ARQUIVO: %s", mapName);
        getc()

        al_flip_display();
        al_clear_to_color(al_map_rgb(0,0,0));
    }

    // SAVE MAP TO FILE
    fp = fopen(mapName, "w");
    fprintf(fp, "%d %d\n", numLinhas, numColunas);
    for(i = 0; i < numLinhas; i++)
    {
        for(j = 0; j < numColunas; j++)
            fprintf(fp, "%d ", blocos[i][j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int main(void)
{
    //==============================================
    //SHELL VARIABLES
    //==============================================
    bool render = false;
    float gameTime = 0;
    int frames = 0;
    int gameFPS = 0;

    //==============================================
    //PROJECT VARIABLES
    //==============================================
    char selectedOption = 0;
    bool menu = true;
    bool pause = true;

    //==============================================
    //AUXILIAR VARIABLES
    //==============================================
    int j, i;

    //==============================================
    //ALLEGRO VARIABLES
    //==============================================
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONT *font18;

    //==============================================
    //ALLEGRO INIT FUNCTIONS
    //==============================================
    if(!al_init())										//initialize Allegro
        return -1;
    if(FULLSCREEN)
        al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    display = al_create_display(WIDTH, HEIGHT);			//create our display object
    if(!display)										//test display object
        return -1;

    //==============================================
    //ADDON INSTALL
    //==============================================
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    //==============================================
    //PROJECT INIT
    //==============================================
    font18 = al_load_font("arial.ttf", 18, 0);

    //==============================================
    //TIMER INIT AND STARTUP
    //==============================================
    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);

    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_start_timer(timer);
    gameTime = al_current_time();
    if(!SHOWMOUSE)
        al_hide_mouse_cursor(display);

    // MENU
    while(menu ^ done)
    {
        al_wait_for_event(event_queue, &ev);
        readInputs();

        al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 100, 0, "        NEW MAP");
        al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 125, 0, "        QUIT  ");

        if(selectedOption == 0){
            al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 100, 0, ">                           <");
        }else{
            al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 125, 0, ">                           <");
        }

        if(keys[UP] | keys[DOWN] | keys[W] | keys[S])
            selectedOption = !selectedOption;

        al_flip_display();
        al_clear_to_color(al_map_rgb(0,0,0));

        while(keys[UP] | keys[DOWN] | keys[W] | keys[S]){
            al_wait_for_event(event_queue, &ev);
            readInputs();
        }

        if(keys[ENTER]){
            done = selectedOption;
            selectedOption = 0;
            break;
        }
    }

    while(keys[ENTER]){
        al_wait_for_event(event_queue, &ev);
        readInputs();
    }

    // NUM COLUNAS E LINHAS
    while(pause ^ done){
        al_wait_for_event(event_queue, &ev);
        readInputs();

        al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 100, 0, "        LINHAS:     %d", numLinhas);
        al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 125, 0, "        COLUNAS:    %d", numColunas);

        if(selectedOption == 0){
            al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 100, 0, ">                                   <");
        }else if (selectedOption == 1){
            al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 125, 0, ">                                   <");
        }

        if(keys[UP] | keys[DOWN] | keys[W] | keys[S])
            selectedOption = !selectedOption;

        if((keys[A] || keys[LEFT]) && (selectedOption) && (numColunas>1))
            numColunas--;
        if((keys[D] | keys[RIGHT])&& (selectedOption) && (numColunas < MAX_COLUNAS))
            numColunas++;
        if((keys[A] | keys[LEFT])&&(!selectedOption)&&(numLinhas > 1))
            numLinhas--;
        if((keys[D] | keys[RIGHT])&&(!selectedOption)&&(numLinhas < MAX_LINHAS))
            numLinhas++;

        al_flip_display();
        al_clear_to_color(al_map_rgb(0,0,0));

        while(keys[UP] | keys[DOWN] | keys[W] | keys[S] | keys[A] | keys[D] | keys[LEFT] | keys[RIGHT]){
            al_wait_for_event(event_queue, &ev);
            readInputs();
        }

        if(keys[ENTER]){
            pause = false;
            break;
        }
    }

    // GAME
    while(!done)
    {
        al_wait_for_event(event_queue, &ev);

        //==============================================
        // INPUT + OUTPUT
        //==============================================
        readInputs();
        if(save)
            saveMap("MAP.txt");

        //==============================================
        //GAME UPDATE
        //==============================================
        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            render = true;

            //UPDATE FPS===========
            frames++;
            if(al_current_time() - gameTime >= 1)
            {
                gameTime = al_current_time();
                gameFPS = frames;
                frames = 0;
            }
            //=====================
        }

        if(keys[P])
            pause = true;

        while(pause ^ done){
            al_wait_for_event(event_queue, &ev);
            readInputs();

            al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 100, 0, "        CONTINUE");
            al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 125, 0, "        SAVE MAP");
            al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 150, 0, "        QUIT  ");

            if(selectedOption == 0){
                al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 100, 0, ">                           <");
            }else if(selectedOption == 1){
                al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 125, 0, ">                           <");
            }else{
                al_draw_textf(font18, al_map_rgb(COR_BORDAS), 100, 150, 0, ">                           <");
            }

            if(keys[UP] | keys[W])
                selectedOption--;
            if(keys[DOWN] | keys[S])
                selectedOption++;

            if(selectedOption > 2)
                selectedOption = 0;
            else if(selectedOption < 0)
                selectedOption = 2;

            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));

            while(keys[UP] | keys[DOWN] | keys[W] | keys[S]){
                al_wait_for_event(event_queue, &ev);
                readInputs();
            }

            if(keys[ENTER]){
                if(selectedOption == 0)
                    pause = false;
                else if(selectedOption == 1){
                    saveMap();
                    pause = false;
                }else
                    done = true;
                selectedOption = 0;
                break;
            }
        }

        //==============================================
        //RENDER
        //==============================================
        if(render && al_is_event_queue_empty(event_queue))
        {
            render = false;
            //BEGIN PROJECT RENDER================

            // DRAW BLOCKS:
            for(i = 0; i < numLinhas; i++){
                for(j = 0; j < numColunas; j++)
                    switch(blocos[i][j]){
                        case 0: // AR
                            al_draw_filled_rectangle(pos_blocos_x + j * blockWidth, pos_blocos_y + i * blockHeight, pos_blocos_x + (j * blockWidth) + blockWidth, pos_blocos_y + (i * blockHeight) + blockHeight, al_map_rgb(COR_AR));
                            break;
                        case 1: // TERRA
                            al_draw_filled_rectangle(pos_blocos_x + j * blockWidth, pos_blocos_y + i * blockHeight, pos_blocos_x + (j * blockWidth) + blockWidth, pos_blocos_y + (i * blockHeight) + blockHeight, al_map_rgb(COR_TERRA));
                            break;
                        case 2: // PEDRA
                            al_draw_filled_rectangle(pos_blocos_x + j * blockWidth, pos_blocos_y + i * blockHeight, pos_blocos_x + (j * blockWidth) + blockWidth, pos_blocos_y + (i * blockHeight) + blockHeight, al_map_rgb(COR_PEDRA));
                            break;
                        case 3: // SILICIO
                            al_draw_filled_rectangle(pos_blocos_x + j * blockWidth, pos_blocos_y + i * blockHeight, pos_blocos_x + (j * blockWidth) + blockWidth, pos_blocos_y + (i * blockHeight) + blockHeight, al_map_rgb(COR_SILICIO));
                            break;
                        case 4: // PEDRA INQUEBRAVEL
                            al_draw_filled_rectangle(pos_blocos_x + j * blockWidth, pos_blocos_y + i * blockHeight, pos_blocos_x + (j * blockWidth) + blockWidth, pos_blocos_y + (i * blockHeight) + blockHeight, al_map_rgb(COR_LAVA));
                            break;
                        case 5: // PEDRA INQUEBRAVEL
                            al_draw_filled_rectangle(pos_blocos_x + j * blockWidth, pos_blocos_y + i * blockHeight, pos_blocos_x + (j * blockWidth) + blockWidth, pos_blocos_y + (i * blockHeight) + blockHeight, al_map_rgb(COR_AGUA));
                            break;
                    }
            }

            // DRAW FPS
            al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 5, 0, "FPS: %i", gameFPS);	//display FPS on screen

            // DRAW BORDERS
            if(SHOW_BORDER){
                al_draw_rectangle(pos_blocos_x + coluna_Mouse * blockWidth, pos_blocos_y + linha_Mouse * blockHeight, pos_blocos_x + (coluna_Mouse * blockWidth) + blockWidth, pos_blocos_y + (linha_Mouse * blockHeight) + blockHeight, al_map_rgb(COR_BORDAS), 1);
                al_draw_rectangle(1, 1, WIDTH, HEIGHT, al_map_rgb(COR_BORDAS), 1);
            }

            // DRAW SELECTED BLOCK PREVIEW
            switch(selectedBlock){
                case 1: // TERRA
                    al_draw_filled_rectangle(WIDTH - (10 + blockWidth), 10, WIDTH - 10, 10 + blockHeight, al_map_rgb(COR_TERRA));
                    break;
                case 2: // PEDRA
                    al_draw_filled_rectangle(WIDTH - (10 + blockWidth), 10, WIDTH - 10, 10 + blockHeight, al_map_rgb(COR_PEDRA));
                    break;
                case 3: // SILICIO
                    al_draw_filled_rectangle(WIDTH - (10 + blockWidth), 10, WIDTH - 10, 10 + blockHeight, al_map_rgb(COR_SILICIO));
                    break;
                case 4: // LAVA
                    al_draw_filled_rectangle(WIDTH - (10 + blockWidth), 10, WIDTH - 10, 10 + blockHeight, al_map_rgb(COR_LAVA));
                    break;
                case 5: // AGUA
                    al_draw_filled_rectangle(WIDTH - (10 + blockWidth), 10, WIDTH - 10, 10 + blockHeight, al_map_rgb(COR_AGUA));
                    break;
            }
            if(SHOW_BORDER)
                al_draw_rectangle(WIDTH - (10 + blockWidth), 10, WIDTH - 10, 10 + blockHeight, al_map_rgb(COR_BORDAS), 1);

            //FLIP BUFFERS========================
            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        }
    }

    //==============================================
    //DESTROY PROJECT OBJECTS
    //==============================================

    //SHELL OBJECTS=================================
    al_destroy_font(font18);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}
