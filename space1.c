#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH 30
#define HEIGHT 15
#define MAX_ENEMIES 5
#define MAX_BULLETS 10

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

int ShipX, ShipY;
int enemyX[MAX_ENEMIES], enemyY[MAX_ENEMIES];
bool enemyActive[MAX_ENEMIES];
int bulletX[MAX_BULLETS], bulletY[MAX_BULLETS];
bool bulletActive[MAX_BULLETS];
char direction ='d';
int score=0;
bool game_over = false;


void MoveShip(int *x, int *y, char dir){
    switch (dir)
    {
    case 'a': case 'A': if(*x > 1) (*x)--; break;
    case 'd': case 'D': if(*x < WIDTH-2) (*x)++; break;
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

void spawnEnemy(){
    for(int i=0; i<MAX_ENEMIES; i++){
        if(!enemyActive[i]){
            enemyX[i] = rand() % (WIDTH-2) + 1;
            enemyY[i] = 1;
            enemyActive[i] = true;
            break;
        }
    }
}

void shootBullet(){
    for(int i=0; i<MAX_BULLETS; i++){
        if(!bulletActive[i]){
            bulletX[i] = ShipX;
            bulletY[i] = ShipY - 1;
            bulletActive[i] = true;
            break;
        }
    }
}

/**
 * Función que sirve para iniciar el juego
 */
void setup(){
    srand(time(NULL));
    ShipX = WIDTH/2;
    ShipY = HEIGHT - 2;
    for(int i=0; i<MAX_ENEMIES; i++) enemyActive[i] = false;
    for(int i=0; i<MAX_BULLETS; i++) bulletActive[i] = false;
}

void draw(){
    clearScreen();
    for(int i=0; i<HEIGHT;i++){
        for(int j=0; j< WIDTH; j++){
            // Bordes del mapa
            if(j==0 || i ==0 || i==HEIGHT-1 || j == WIDTH-1){
                 printf(YELLOW"▒");
                 continue;
            }
            
            // Dibujar la nave
            if(ShipY == i && ShipX == j){
                printf(GREEN"▲");
                continue;
            }
            
            // Dibujar enemigos
            bool drawn = false;
            for(int k=0; k<MAX_ENEMIES; k++){
                if(enemyActive[k] && enemyY[k] == i && enemyX[k] == j){
                    printf(RED"▼");
                    drawn = true;
                    break;
                }
            }
            if(drawn) continue;
            
            // Dibujar balas
            for(int k=0; k<MAX_BULLETS; k++){
                if(bulletActive[k] && bulletY[k] == i && bulletX[k] == j){
                    printf(BLUE"|");
                    drawn = true;
                    break;
                }
            }
            if(drawn) continue;
            
            printf(RESET" ");
        }
        printf(RESET"\n");
    }
    printf(RESET"Puntuación: %d\n", score);
}


void logic(){
    // Mover balas hacia arriba
    for(int i=0; i<MAX_BULLETS; i++){
        if(bulletActive[i]){
            bulletY[i]--;
            if(bulletY[i] < 1) bulletActive[i] = false;
        }
    }
    
    // Mover enemigos hacia abajo
    for(int i=0; i<MAX_ENEMIES; i++){
        if(enemyActive[i]){
            enemyY[i]++;
            if(enemyY[i] >= HEIGHT-1){
                enemyActive[i] = false;
                game_over = true; // Enemigo llegó abajo
            }
        }
    }
    
    // Colisiones bala-enemigo
    for(int i=0; i<MAX_BULLETS; i++){
        if(bulletActive[i]){
            for(int j=0; j<MAX_ENEMIES; j++){
                if(enemyActive[j] && bulletX[i] == enemyX[j] && bulletY[i] == enemyY[j]){
                    bulletActive[i] = false;
                    enemyActive[j] = false;
                    score += 10;
                }
            }
        }
    }
    
    // Colisión nave-enemigo
    for(int i=0; i<MAX_ENEMIES; i++){
        if(enemyActive[i] && ShipX == enemyX[i] && ShipY == enemyY[i]){
            game_over = true;
        }
    }
    
    // Spawnear nuevos enemigos aleatoriamente
    if(rand() % 20 == 0){ // Probabilidad baja
        spawnEnemy();
    }
}

int main(){
    char input;
    setup();
    #ifndef _WIN32
    enableRawMode();
    #endif
    while(!game_over){
        draw();
        input = readInput();
        
        if(input == 'a' || input == 'A' || input == 'd' || input == 'D'){
            direction = input;
            MoveShip(&ShipX, &ShipY, direction);
        }
        else if(input == ' '){ // Espacio para disparar
            shootBullet();
        }
        
        logic();
        
        #ifdef _WIN32
        Sleep(150);
        #else
        usleep(200000); // Un poco más rápido
        #endif
    }
    #ifndef _WIN32
    disableRawMode();
    #endif
    clearScreen();
    printf(BLUE"════════════════════════════════════════\n");
    printf(BLUE"      ¡GAME OVER!\n");
    printf(BLUE"════════════════════════════════════════\n");
    printf(GREEN"Puntuación final: %d\n" RESET, score);
    return 0;
}