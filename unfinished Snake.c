#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <sys/select.h>

#define WIDTH 20
#define HEIGHT 10
#define MAX_SNAKE 100

typedef struct {
    int x, y;
} Segment;

Segment snake[MAX_SNAKE];
int length, foodX, foodY;
char dir;
int gameOver = 0;
int paused = 0;
int highScore = 0;

void setup();
void draw();
void input();
void logic();
void generateFood();
void delay(int ms);
char getch();
int kbhit();
void loadHighScore();
void saveHighScore();

int main() {
    srand(time(0));
    loadHighScore();

    while (1) {
        setup();
        while (!gameOver) {
            draw();
            input();
            if (!paused) logic();
            delay(200);
        }

        if ((length - 1) > highScore) {
            highScore = length - 1;
            saveHighScore();
        }

        draw();
        printf("Game Over! Press 'r' to restart or any other key to quit.\n");
        char c = getch();
        if (c != 'r' && c != 'R') break;
    }

    return 0;
}

void setup() {
    length = 1;
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    dir = 'd';
    gameOver = 0;
    paused = 0;
    generateFood();
}

void draw() {
    system("clear");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int printed = 0;

            for (int i = 0; i < length; i++) {
                if (snake[i].x == x && snake[i].y == y) {
                    printf(i == 0 ? "O" : "o");
                    printed = 1;
                    break;
                }
            }

            if (!printed && x == foodX && y == foodY) {
                printf("F");
                printed = 1;
            }

            if (!printed) {
                if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1)
                    printf("#");
                else
                    printf(" ");
            }
        }
        printf("\n");
    }

    printf("Score: %d\tHigh Score: %d\n", length - 1, highScore);
    if (paused) printf("Game Paused. Press 'p' to resume.\n");
}

void input() {
    if (kbhit()) {
        char c = getch();
        if ((c == 'w' || c == 'W') && dir != 's') dir = 'w';
        else if ((c == 's' || c == 'S') && dir != 'w') dir = 's';
        else if ((c == 'a' || c == 'A') && dir != 'd') dir = 'a';
        else if ((c == 'd' || c == 'D') && dir != 'a') dir = 'd';
        else if (c == 'p' || c == 'P') paused = !paused;
    }
}

void logic() {
    for (int i = length - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    if (dir == 'w') snake[0].y--;
    else if (dir == 's') snake[0].y++;
    else if (dir == 'a') snake[0].x--;
    else if (dir == 'd') snake[0].x++;

    if (snake[0].x <= 0 || snake[0].x >= WIDTH - 1 ||
        snake[0].y <= 0 || snake[0].y >= HEIGHT - 1)
        gameOver = 1;

    for (int i = 1; i < length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameOver = 1;
            break;
        }
    }

    if (snake[0].x == foodX && snake[0].y == foodY) {
        if (length < MAX_SNAKE) {
            length++;
            generateFood();
        }
    }
}

void generateFood() {
    foodX = rand() % (WIDTH - 2) + 1;
    foodY = rand() % (HEIGHT - 2) + 1;
}

void delay(int ms) {
    usleep(ms * 1000);
}

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(0, TCSANOW, &oldt);
    return ch;
}

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

void loadHighScore() {
    FILE *file = fopen("highscore.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d", &highScore);
        fclose(file);
    } else {
        highScore = 0;
    }
}

void saveHighScore() {
    FILE *file = fopen("highscore.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d", highScore);
        fclose(file);
    }
}