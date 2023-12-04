#include <fstream>
#include <ncurses.h>
#include <math.h>
#include <cmath>
#include <unistd.h>

using namespace std;

int main()
{
    // initialize ncurses related stuff
    initscr();
    cbreak();
    curs_set(0);
    int height, width, map_height, map_width, x, y;
    float rotation, theta, r, posx, posy, max_distance;
    height = 40;
    width = 80;
    map_width = 20;
    map_height = 20;
    max_distance = sqrt(map_width * map_width + map_height * map_height);
    posx = 1;
    posy = 1;
    WINDOW *win = newwin(map_height, map_width, 0, width + 1);
    WINDOW *viewport = newwin(height, width, 0, 0);
    keypad(win, true);
    refresh();

    // loadmap
    bool map[map_height][map_width];
    ifstream mapfile("map.txt");
    string line;
    for (int i = 0; i < map_height; i++)
    {
        getline(mapfile, line);
        for (int j = 0; j < map_width; j++)
        {
            if (line[j] == '#')
            {
                map[i][j] = true;
            }
            else
            {
                map[i][j] = false;
            }
        }
    }
    mapfile.close();

    // gameloop
    while (1)
    {
        int c = wgetch(win);
        // rotate
        if (c == KEY_RIGHT)
        {
            rotation += M_PI / 45;
        }
        else if (c == KEY_LEFT)
        {
            rotation -= M_PI / 45;
        }

        // lock rotation to between 0 and 2pi
        if (rotation < 0)
        {
            rotation += M_PI * 2;
        }
        else if (rotation > M_PI * 2)
        {
            rotation -= M_PI * 2;
        }

        // movement
        if (c == KEY_UP)
        {
            posx += 0.5 * cos(rotation);
            posy += 0.5 * sin(rotation);
            if (map[(int)floor(posy)][(int)floor(posx)] == true)
            {
                posx -= 0.5 * cos(rotation);
                posy -= 0.5 * sin(rotation);
            }
        }
        else if (c == KEY_DOWN)
        {
            posx -= 0.5 * cos(rotation);
            posy -= 0.5 * sin(rotation);
            if (map[(int)floor(posy)][(int)floor(posx)] == true)
            {
                posx += 0.5 * cos(rotation);
                posy += 0.5 * sin(rotation);
            }
        }

        // top-down map
        for (int i = 0; i < map_height; i++)
        {
            wmove(win, i, 0);
            for (int j = 0; j < map_width; j++)
            {
                if (map[i][j] == true)
                {
                    wprintw(win, "#");
                }
                else
                {
                    wprintw(win, " ");
                }
            }
        }

        // raycasting
        for (int i = 0; i < width; i++)
        {
            r = 0.1;
            x = posx;
            y = posy;
            theta = (rotation - M_PI / 8) + (((M_PI * i) / (4 * width)));
            while (map[y][x] == false)
            {
                mvwprintw(win, y, x, ".");
                r += 0.1;
                x = round(posx + r * cos(theta));
                y = round(posy + r * sin(theta));
            }
            r = round(((max_distance - r) / (max_distance - 0.1)) * (height / 2));

            for (int j = (height / 2) - r; j < ((height / 2) - r) + (r * 2); j++)
            {
                // shade y-facing sides brighter for lighting effect
                if ((map[y][x - 1] == false) && (((3 * M_PI / 2) < theta) || (theta < M_PI / 2)))
                {
                    mvwprintw(viewport, j, i, "@");
                }
                else if ((map[y][x + 1] == false) && (((3 * M_PI / 2) >= theta) && (theta >= M_PI / 2)))
                {
                    mvwprintw(viewport, j, i, "@");
                }
                else
                {
                    mvwprintw(viewport, j, i, "#");
                }
            }
        }

        wrefresh(win);
        wrefresh(viewport);
        wclear(viewport);
    }

    // close ncurses window
    endwin();
    return 0;
}
