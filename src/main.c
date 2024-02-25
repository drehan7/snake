#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

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

bool checkCollision( SDL_Rect a, SDL_Rect b ) {

    // Sides
    int topA = a.y;
    int bottomA = a.y + a.h;
    int leftA = a.x;
    int rightA = a.x + a.w;

    int topB = b.y;
    int bottomB = b.y + b.h;

    int leftB = b.x;
    int rightB = b.x + b.w;

    bool isColliding = true;

    if (
            rightA <= leftB ||
            topA <= bottomB ||
            topB <= bottomA ||
            rightB <= leftA
        )
    {
        isColliding = false;
    }

    return isColliding;
}

// TODO!! make growing smarter
void grow_snake( Snake* s )
{
    _snake* new = malloc( sizeof( *s ) );

    new->dir = s->tail->dir;

    switch ( new->dir )
    {
        case DOWN: {
            new->x = s->tail->x;
            new->y = s->tail->y - 1;
            break;
        }
        case UP: {
            new->x = s->tail->x;
            new->y = s->tail->y + 1;
            break;
        }
        case LEFT: {
            new->x = s->tail->x + 1;
            new->y = s->tail->y;
            break;
        }
        case RIGHT: {
            new->x = s->tail->x - 1;
            new->y = s->tail->y;
            break;
        }
        default: break;
    }

    new->next = NULL;
    s->tail->next = (struct _snake*) new;

    s->tail = new;

    s->length++;
}

Snake* init_snake()
{
    _snake* sb = malloc( sizeof( *sb ) );

    if ( sb == NULL ) {
        exit(1);
    }

    sb->x = rand() % (GRIDSIZE / 2) + (GRIDSIZE / 4);
    sb->y = rand() % (GRIDSIZE / 2) + (GRIDSIZE / 4);
    sb->next = NULL;
    sb->dir = UP;

    Snake* snake = malloc(sizeof(Snake));
    if ( snake == NULL ) {
        exit(1);
    }

    snake->head = sb;
    snake->tail = sb;
    snake->length = 1;

    grow_snake( snake );
    return snake;
}

void move_snake( Snake* s)
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

    // Check Snake hit walls
    // Appear on opposite side
    int gridRight = CELLSIZE / 2;
    int gridLeft = 0;
    int gridTop = 0;
    int gridBottom = CELLSIZE / 2;

    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);

    if ( head->y == gridBottom ) {
        head->y = gridTop;
    }
    else if ( head->y == gridTop - 1 ) {
        head->y = gridBottom - 1;
    }
    else if ( head->x == gridRight ) {
        head->x = gridLeft;
    }
    else if ( head->x == gridLeft - 1 ) {
        head->x = gridRight - 1;
    }

    _snake* tmp = s->head;

    // Everything after the head
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

        s = (_snake*) s->next;
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

// Check snake collision with apple
// Generate new apple
void detect_apple( SDL_Renderer* r, Snake* s, Apple* app, int x, int y )
{
    if ( s == NULL ) return;

    SDL_Rect tmpApple = {
        .x = app->x,
        .y = app->y,
        .w = CELLSIZE / 2,
        .h = CELLSIZE / 2,
    };

    _snake* head = s->head;
    SDL_Rect tmpSnake = {
        .x = head->x,
        .y = head->y,
        .w = CELLSIZE / 2,
        .h = CELLSIZE / 2 ,
    };

    if ( SDL_RectEquals(&tmpSnake, &tmpApple) == SDL_TRUE )
    {
         app->x = (int) rand() % GRIDSIZE;
         app->y = (int) rand() % GRIDSIZE;
         grow_snake( s );
    }
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

int main() {

    srand(time(NULL));

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
        .x = (int)rand() % GRIDSIZE,
        .y = (int)rand() % GRIDSIZE,
    };

    Snake* snake = init_snake();

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

        move_snake( snake );
        detect_apple( renderer, snake, &app, GRIDX, GRIDY );

        // Clear Window
        SDL_SetRenderDrawColor( renderer, 0,0,0,255 );
        SDL_RenderClear( renderer );

        render_apple( renderer, app, GRIDX, GRIDY );
        draw_grid( renderer, GRIDX, GRIDY );
        render_snake( renderer, snake, GRIDX, GRIDY );

        SDL_RenderPresent( renderer );
        SDL_Delay( 100 );

    }

    SDL_DestroyWindow( window );
    SDL_Quit();

    free_snake( snake );

    return 0;
}
