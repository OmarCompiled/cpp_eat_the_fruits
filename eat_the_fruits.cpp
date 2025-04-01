#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>

#define WHTB "\e[47m"
#define BLKB "\e[40m"
#define BLKT "\e[30m"
#define WHTT "\e[39m"

struct termios canonical;

void disable_raw_mode(void) {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &canonical);
}

void enable_raw_mode(void) {
	tcgetattr(STDIN_FILENO, &canonical);
	atexit(disable_raw_mode);

	struct termios raw = canonical;
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

class Mouse{
	public:
		int score;
		int x, y;
		char body = 'v';
};

class Game{
	public:
		int WIDTH;
		int HEIGHT;
		int fruit_x, fruit_y;
		bool gameover;
		enum direction {STOP = 0, UP, DOWN, LEFT, RIGHT};
		direction dir = STOP;
		Mouse mouse;

		Game(Mouse mouse) {
			this->mouse = mouse;
		}

		void setup() {
			gameover = false;
			mouse.x = WIDTH  / 2;
			mouse.y = HEIGHT / 2;
			fruit_x = rand() % WIDTH;
			fruit_y = rand() % HEIGHT;
			mouse.score = 0;
		}

		void draw() {
			std::cout << "\033[H\033[J";
			for(int cols = 0; cols < WIDTH; cols++) {
				printf("#");
			}
			std::cout << "\n";

			for(int cols = 0; cols < HEIGHT; cols++) {
				for(int rows = 0; rows < WIDTH; rows++) {
					if(rows == 0 || rows == WIDTH - 1) {
						printf("#");
					} else if(rows == fruit_x && cols == fruit_y) { 
						printf("O");
				  } else if(rows == mouse.x && cols == mouse.y) {
						printf("%c", mouse.body);
				  } else if(mouse.x == fruit_x &&
							mouse.y == fruit_y) {
							printf(" ");
							fruit_x = rand() % WIDTH;
							fruit_y = rand() % HEIGHT;
							mouse.score++;
					} else {
						printf(" ");
					}
				}
				printf("\n");
			}

			for(int cols = 0; cols < WIDTH; cols++) {
				printf("#");
			}
			printf("\n");
			printf("score: %i\n", mouse.score);
			if(mouse.score == 5) {printf("Good Job! Keep going.\n");}
			std::cout << WHTB << BLKT << "Press 'q' to exit"<< BLKB << WHTT << std::endl;
		}

		void key_handler() {
        char c;
				read(STDIN_FILENO, &c, 1);	
				switch(c) {
					case 'w':
						dir = UP;
						mouse.body = 'v';
						if(mouse.y > 0) {mouse.y--;}
						break;
					case 'a':
						dir = LEFT;
						mouse.body = '>';
						if(mouse.x > 1) {mouse.x--;}
						break;
					case 's':
						dir = DOWN;
						mouse.body = '^';
						if(mouse.y < HEIGHT - 1) {mouse.y++;}
						break;
					case 'd':
						dir = RIGHT;
						mouse.body = '<';
						if(mouse.x < WIDTH - 2){mouse.x++;}
						break;
					case 'q':
						printf("GoodBye!\n");
						sleep(1);
						printf("\033[H\033[J");
						exit(0);
				}
			}	
};

int main() {
	winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	
	enable_raw_mode();

	Mouse mouse;
	Game *game = new Game(mouse);
	game->WIDTH  = w.ws_col;
	game->HEIGHT = w.ws_row * 0.8;
	game->setup();
	while(!(game->gameover)) {
		game->draw();
		fflush(stdout);
		game->key_handler();
	}
	delete game;
}
