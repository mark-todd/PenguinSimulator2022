#include <ncurses.h>
#include <iostream>
#include <unistd.h>

std::string get_penguin_line(int l, int c) {
    if (l == 0) {
        return "   _   ";
    } else if (l == 1) {
        return " ('v') ";
    } else if (l == 2) {
        return "//-=-\\\\";
    } else if (l == 3) {
        return "(\\_=_/)";
    } else if (l == 4) {
        if (c == 0 || c == 2) {
            return " ^^ ^^ ";
        } else if (c == 1) {
            return "  ^^ ^^";
        } else {
            return "^^ ^^  ";
        }
        
    } else {
        return "       ";
    }
}

void generate_board(int penguin_x, int penguin_y) {
    const int width = 100;
    const int height = 100;
    const int offset_x = 50;
    const int offset_y = 20;
    for (int line_no=0; line_no < height; ++line_no) {
        const int graphic_height = line_no - offset_y + penguin_y;
        const std::string peng_line = get_penguin_line(graphic_height, (penguin_x+penguin_y+width+height)%4);
        const int x_dim = offset_x + penguin_x;
        const std::string new_str = std::string(x_dim, ' ');
        printw(new_str.c_str());
        printw(peng_line.c_str());
        printw("\n");
    }
}

int main()
{
    // Initialize ncurses
    initscr();
    cbreak();              // Line buffering disabled
    noecho();              // Don't display keypresses
    keypad(stdscr, TRUE);  // Enable function keys

    // Print instructions
    printw("Press any key to begin. Press 'q' to quit.\n");
    refresh();

    // Process keypresses
    int character;
    int penguin_x = 0;
    int penguin_y = 0;
    while ((character = getch()) != 'q')
    {
        clear();
        int asciiCode = static_cast<int>(character);
        if (asciiCode == 259) {
            // Up
            penguin_y = penguin_y + 1;
        } else if (asciiCode == 258) {
            // Down
            penguin_y = penguin_y - 1;
        } else if (asciiCode == 260) {
            // Left
            penguin_x = penguin_x - 1;
        } else if (asciiCode == 261) {
            // Right
            penguin_x = penguin_x + 1;
        }
        printw("penguin x '%d' penguin y '%d'\n", penguin_x, penguin_y);

        for (int i=0; i<6; ++i) {
            //printw(get_penguin_line(i).c_str());
            printw("\n");
        }
        generate_board(penguin_x, penguin_y);
        refresh();
    }

    // Clean up ncurses
    endwin();

    return 0;
}
