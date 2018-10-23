#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#define DELAY 30000

void * threadEntry (void * arg);

int x,y;
int max_y,max_x;

int main(int argc, char *argv[]) {
	int next_x = 0;
	int direction = 1;
	pthread_t input_tid;

	x = 0; y = 0;
	max_y = 0; max_x = 0;

	initscr();				// Global var `stdscr` is created by the call to `initscr()`
	cbreak();  
	keypad(stdscr, TRUE);   // Allow reading special keys from the keyboard (Fx, and arrows)
	noecho(); 				// Do not show key strokes
	curs_set(FALSE);
	
	int input_tresult = pthread_create(&input_tid, NULL, threadEntry, NULL);
	while(1){
		getmaxyx(stdscr, max_y, max_x);
		clear();
		mvprintw(y, x,"%c",'o');
		refresh();

		usleep(DELAY);

		next_x = x + direction;

		if (next_x >= max_x || next_x < 0)  direction*= -1;
		else      						    x        += direction;
	}

	endwin();
}

void * threadEntry (void * arg)
{
	int ch;
	while(1)
    {
    	ch = getch();
        switch(ch)
        {
            case KEY_UP:
                y = y-1>0?y-1:y;
                break;
            case KEY_DOWN:
                y = y+1<max_y?y+1:y;
                break;
            default:
                break;
        }
    }
    pthread_exit(NULL);
}
