#include <SDL2/SDL.h>
#include <stdbool.h>

#define SNAKESIZE 10

#define WINDOW_WIDTH 1770
#define WINDOW_HEIGHT 1405

#define WINDOW_X 10
#define WINDOW_Y -20

#define GRIDSIZE 20
#define GRIDDIM 800


#define CELLSIZE (GRIDDIM / GRIDSIZE)

typedef enum {
    DOWN,
    LEFT,
    RIGHT,
    UP,
    NONE = -1,
} Direction;

typedef struct {
    int x, y;
} Apple;

typedef struct {
    int x, y;
    Direction dir;
    struct _snake* next;
} _snake;

typedef struct {
    _snake* head;
    _snake* tail;
    int length;
} Snake;

_snake* create_body( int x, int y, Direction d )
{
    _snake* sb = malloc( sizeof(_snake) );
    sb->x = x;
    sb->y = y;
    sb->dir = d;
    sb->next = NULL;
}

Snake* init_snake()
{
    _snake* sb = malloc( sizeof( sb ) );

    if ( sb == NULL ) {
        printf("Couldnt make snake\n");
        exit(1);
    }

    sb->x = rand() % (GRIDSIZE / 2) + (GRIDSIZE / 4);
    sb->y = rand() % (GRIDSIZE / 2) + (GRIDSIZE / 4);
    sb->next = NULL;
    sb->dir = UP;

    Snake* snake = malloc(sizeof(Snake));
    if ( snake == NULL ) {
        printf("Couldnt make snake :(\n");
        exit(1);
    }

    snake->head = sb;
    snake->tail = sb;
    snake->length = 1;

    return snake;
}

// TODO!! make growing smarter
void grow_snake( Snake* s )
{
    _snake* new = malloc(sizeof(s));

    new->dir = s->tail->dir;
    new->x = s->tail->x;
    new->y = s->tail->y - 1;

    new->next = NULL;
    s->tail->next = new;

    s->tail = new;

    s->length++;
}

void move_snake( Snake* s, Apple app, int x, int y )
{
    _snake* head = s->head;
    // Keep track of previous location
    int prevX = head->x;
    int prevY = head->y;
    
    // TODO: Figure out collisions with walls and apple

    switch ( head->dir )
    {
        case DOWN:
            {
                head->y++;
                break;
            }
        case UP:
            {
                head->y--;
                break;
            }
        case LEFT:
            {
                head->x--;
                break;
            }
        case RIGHT:
            {
                head->x++;
                break;
            }
        default:
            {
                // Debug; pause
                break;
            }
    }

    _snake* tmp = s->head;

    // Everythign after the head
    if ( tmp->next != NULL ) {
        tmp = tmp->next;
    }

    while ( tmp != NULL ) {

        int currX = tmp->x;
        int currY = tmp->y;

        tmp->x = prevX;
        tmp->y = prevY;


        prevX = currX;
        prevY = currY;

        tmp = tmp->next;
    }

}

void free_snake(Snake* s)
{
    struct _snake* tmp;

    while (s->head != NULL) {
        tmp = s->head;
        s->head = s->head->next;
        free( tmp );
    }

    free( s );
}

void render_snake( SDL_Renderer* rend, Snake* snake, int x, int y )
{
    SDL_SetRenderDrawColor( rend, 0x0, 0xFF, 0x0, 255 );

    int ss = GRIDDIM / GRIDSIZE;

    _snake* s = snake->head;

    SDL_Rect nr;
    nr.w = CELLSIZE;
    nr.h = CELLSIZE;

    while ( s != NULL ) {
        nr.x = x + s->x * ss;
        nr.y = y + s->y * ss;

        SDL_RenderFillRect( rend, &nr );

        s = s->next;
    }
}

void render_apple( SDL_Renderer* r, Apple app, int x, int y )
{
    // RED
    SDL_SetRenderDrawColor( r, 0xFF, 0x0, 0x0, 255 );

    // Figure out a random spot on the grid;
    SDL_Rect apple = {
        .w = CELLSIZE,
        .h = CELLSIZE,
        .x = app.x * CELLSIZE + x,
        .y = app.y * CELLSIZE + y,
    };

    SDL_RenderFillRect( r, &apple );
}

void draw_grid( SDL_Renderer* r, int x, int y )
{
    // Grey
    SDL_SetRenderDrawColor( r, 0x55, 0x55, 0x55, 255 );

    int c_size = CELLSIZE;

    SDL_Rect cell;
    cell.w = c_size;
    cell.h = c_size;

    for ( int i = 0; i < GRIDSIZE; ++i ) {
        for ( int j = 0; j < GRIDSIZE; ++j ) {
            cell.x = x + ( i * c_size );
            cell.y = y + ( j * c_size );

            SDL_RenderDrawRect( r, &cell );
        }
    }
    
}

void check_collisions( Snake* snake, Apple apple, int x, int y )
{

}

int main() {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
            "Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, 0 ); // IDK

    SDL_Event e;

    int GRIDX = (WINDOW_WIDTH / 2) - (GRIDDIM / 2);
    int GRIDY = (WINDOW_HEIGHT / 2) - (GRIDDIM / 2);

    int sz = GRIDSIZE;

    bool running = true;

    Apple app = {
        .x = rand() % GRIDSIZE,
        .y = rand() % GRIDSIZE,
    };

    Snake* snake = init_snake();
    grow_snake(snake);
    grow_snake(snake);
    grow_snake(snake);
    grow_snake(snake);

    int* dir = &snake->head->dir;

    while ( running )
    {
        // Check Input
        while ( SDL_PollEvent(&e) ) {

            switch ( e.type )
            {
                case SDL_QUIT:
                {
                    running = false;
                    break;
                };
                case SDL_KEYDOWN:
                {
                    if ( e.key.keysym.sym == SDLK_DOWN )  { *dir = DOWN; }
                    if ( e.key.keysym.sym == SDLK_UP )    { *dir = UP; }
                    if ( e.key.keysym.sym == SDLK_LEFT )  { *dir = LEFT; }
                    if ( e.key.keysym.sym == SDLK_RIGHT ) { *dir = RIGHT; }
                    if ( e.key.keysym.sym == SDLK_SPACE ) { *dir = NONE; }
                };
                default:
                    break;
            }
        }

        move_snake( snake, app, GRIDX, GRIDY );

        // check_collisions( snake, app, GRIDX, GRIDY );

        // Clear Window
        SDL_SetRenderDrawColor( renderer, 0,0,0,255 );
        SDL_RenderClear( renderer );

        render_apple( renderer, app, GRIDX, GRIDY );
        draw_grid( renderer, GRIDX, GRIDY );
        render_snake( renderer, snake, GRIDX, GRIDY );

        SDL_RenderPresent( renderer );
        SDL_Delay( 205 );

    }

    SDL_DestroyWindow( window );
    SDL_Quit();

    free_snake( snake );

    return 0;
}
