#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WIDTH 30
#define HEIGHT 15

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

#define MAX_TAIL 300

int snakeX, snakeY; // posición de la primera parte de la vibora
int tailX[MAX_TAIL]; // Almacena las posciones en X de cada parte del cuerpo
int tailY[MAX_TAIL]; // Almacena las posciones en Y de cada parte del cuerpo
// int tail [MAX_TAIL][2];
int tailLen = 0; // Longitud del cuerpo del jugador
int fruitX, fruitY;
char direction ='d';
int score=0;
bool game_over = false;

int increaseScore(){
    static int score=0;
    score++;
    return score;
}


void moveSnake(int *x, int *y, char dir){
    switch (dir)
    {
    case 'w': case 'W': (*y)--; break;
    case 's': case 'S': (*y)++; break;
    case 'a': case 'A': (*x)--; break;
    case 'd': case 'D': (*x)++; break;
    }
}

void clearScreen(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

#ifdef _WIN32
char readInput(){
    if(_kbhit()) return _getch();
    return direction;
}
#else

struct termios oldt;
void enableRawMode(){
    struct termios  newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON| ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);    
    int flags = fcntl(STDIN_FILENO, F_GETFL, O_NONBLOCK); 
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK); 

}
void disableRawMode(){
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
char readInput(){    
    int ch;    
    ch = getchar();    
    if(ch!= EOF) return ch;
    return 0;
}
#endif

void generateFruit(){
    bool validPosition;

    do{
        validPosition = true;

        fruitX = (rand()%(WIDTH-2))+1;
        fruitY = (rand()%(HEIGHT-2))+1;

        if(snakeX == fruitX && snakeY == fruitY){
            validPosition= false;
            continue;
        }

        for(int i =0; i < tailLen; i++){
            if(tailX[i] == fruitX && tailY[i]== fruitY){
                validPosition = false;
                break;
            }
        }
    }while(!validPosition);
}

/**
 * Función que sirve  para iniciar el juego
 */
void setup(){
    snakeX=WIDTH/2;
    snakeY=HEIGHT/2;  
    generateFruit();  
}

void draw(){
    bool printed;
    clearScreen();
    for(int i=0; i<HEIGHT;i++){
        for(int j=0; j< WIDTH; j++){
            printed= false;
            if(j==0 || i ==0 || i==HEIGHT-1 || j == WIDTH-1){
                 printf(YELLOW"▒");
                 continue;
            }
            if(snakeX==j && snakeY==i){
                printf(GREEN"O");
                continue;
            } 

            for(int k =0; k < tailLen; k++){
                
                if(tailX[k]== j && tailY[k]==i){
                    printf(GREEN"o");
                    printed = true;
                    break;
                }
            }
            if(printed) continue;

            if(fruitX==j && fruitY==i){
                printf(RED"Ѽ");
                        continue;
            } 
            printf(RESET" ");
        }
        printf(RESET"\n");
    }
    printf(RESET"Score %d\n", score);
    //printf("Presiona W o S o D o A\n");
}


void logic(){
    // Colisiones con el muro
    if(snakeX <=0 || snakeX>= WIDTH -1 || snakeY <= 0 || snakeY >= HEIGHT-1){
        printf("Game over\n");
        game_over = true;
    }
    // Colisiones de la cabeza con el cuerpo 
    for(int i =0; i < tailLen; i++){
        if(tailX[i]== snakeX && tailY[i]==snakeY) game_over= true;

    }

    // Colisiones de la cabeza con la fruta
    if(snakeX == fruitX && snakeY == fruitY){        
        generateFruit();
        score= increaseScore();
        tailLen++;
    }
}

void updateTail(){
    int prevX = tailX[0];
    int prevY = tailY[0];
    int tmpX, tmpY;

    tailX[0] = snakeX;
    tailY[0] = snakeY;

    for(int i =1; i <  tailLen; i++){
        // Recupero la posición la parte actual
        tmpX = tailX[i];
        tmpY = tailY[i];

        // Actualizo la posición de la parte actual a la de la pieza anterior
        tailX[i] = prevX;
        tailY[i] = prevY;

        // Actualizo la posición previa para que sea la anterior de la pieza actual
        prevX = tmpX;
        prevY = tmpY;
    }
}

int main(){
    char newDirection;
    setup();
    #ifndef _WIN32
    enableRawMode();
    #endif
    while(!game_over){
        draw();
        newDirection = readInput();
        if((newDirection=='W' || newDirection=='w') && direction != 's' && direction != 'S')        
            direction = newDirection;
        else if((newDirection=='S' || newDirection=='s') && direction != 'w' && direction != 'W')
            direction = newDirection;
        else if((newDirection=='A' || newDirection=='a') && direction != 'd' && direction != 'D')
            direction = newDirection;
        else if((newDirection=='D' || newDirection=='d') && direction != 'a' && direction != 'A')
            direction = newDirection;
        
        updateTail();
        moveSnake(&snakeX, &snakeY, direction);
        logic();
        #ifdef _WIN32
        Sleep(150);
        #else
        usleep(300000);
        #endif
    }
    #ifndef _WIN32
    disableRawMode();
    #endif
    return 0;
}