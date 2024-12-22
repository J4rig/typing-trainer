#include <ncurses.h>
#include <string>
#include <time.h>
#include <cmath>
#include <ctype.h>

#define MODE 2
#define SIZE 3

enum class STATES
{
  MENU = 0,
  NEW,
  RESUME,
  TEST,
  RESULT,
  EXIT,
};

void drawMenu(WINDOW *menu_win, unsigned row_select, bool resume, unsigned mode, unsigned size)
{
  mvwaddstr(menu_win, 1, 0, "  START NEW         ");

  if (!resume)
  {
    wattron(menu_win, A_DIM);
  }
  mvwaddstr(menu_win, 2, 0, "  RESUME            ");
  if (!resume)
  {
    wattroff(menu_win, A_DIM);
  }

  mvwaddstr(menu_win, 3, 0, "  MODE");

  if (mode % 2 == 0)
  {
    mvwaddstr(menu_win, 3, 6, "  <FREE>      ");
  }
  else
  {
    mvwaddstr(menu_win, 3, 6, "  <FIXED>     ");
  }

  mvwaddstr(menu_win, 4, 0, "  SIZE");

  if (size % 3 == 0)
  {
    mvwaddstr(menu_win, 4, 6, "  <SMALL>     ");
  }
  else if (size % 3 == 1)
  {
    mvwaddstr(menu_win, 4, 6, "  <MEDIUM>    ");
  }
  else
  {
    mvwaddstr(menu_win, 4, 6, "  <LARGE>     ");
  }

  mvwaddstr(menu_win, 5, 0, "  EXIT              ");
  mvwchgat(menu_win, row_select + 1, 1, getmaxx(menu_win) - 2, A_REVERSE, 0, nullptr);
  box(menu_win, 0, 0);
  mvwaddstr(menu_win, 0, (getmaxx(menu_win) - 4) / 2, "MENU");
  wrefresh(menu_win);
}

void navigateMenu(int ch, unsigned *row_select, unsigned *mode, unsigned *size)
{
  if (ch == KEY_UP)
  {
    *row_select != 0 ? (*row_select)-- : *row_select = 4;
  }
  else if (ch == KEY_DOWN)
  {
    *row_select != 4 ? (*row_select)++ : *row_select = 0;
  }

  else if (ch == KEY_LEFT)
  {
    if (*row_select == MODE)
    {
      (*mode)--;
    }
    else if (*row_select == SIZE)
    {
      (*size)--;
    }
  }

  else if (ch == KEY_RIGHT)
  {
    if (*row_select == MODE)
    {
      (*mode)++;
    }
    else if (*row_select == SIZE)
    {
      (*size)++;
    }
  }
}

enum STATES transitFromMenu(unsigned row_select, bool resume)
{
  if (row_select == 0)
  {
    return STATES::NEW;
  }
  else if (row_select == 1 && resume)
  {
    return STATES::RESUME;
  }
  else if (row_select == 4)
  {
    return STATES::EXIT;
  }
  return STATES::MENU;
}

void drawResult(WINDOW *result_win, double time, unsigned mistakes)
{
  mvwaddstr(result_win, 2, 2, "TIME:             ");
  mvwaddstr(result_win, 2, 13, std::to_string((int)time).c_str());

  mvwaddstr(result_win, 3, 2, "MISTAKES:        ");
  mvwaddstr(result_win, 3, 13, std::to_string(mistakes).c_str());

  mvwaddstr(result_win, 5, 7, "[DONE]");
  mvwchgat(result_win, 5, 7, 6, A_REVERSE, 0, nullptr);
  box(result_win, 0, 0);
  mvwaddstr(result_win, 0, (getmaxx(result_win) - 6) / 2, "RESULT");
  wrefresh(result_win);
}

std::string generateTest(unsigned size)
{
  switch (size % 3)
  {
  case 0:
    return "Hello world!";
  case 1:
    return "Nulla lacinia massa ut sapien ullamcorper, a posuere mauris rutrum. Nullam aliquam metus massa, ut suscipit diam volutpat et. Praesent tristique nunc a arcu tempor ultricies. Quisque vel bibendum ligula. Curabitur non nisi vel urna finibus efficitur at et ligula. In faucibus aliquam nibh ultricies dapibus. Donec accumsan viverra gravida. Nunc ultrices arcu ante, sit amet lobortis ante faucibus in. Nam nec faucibus risus. Vestibulum leo diam, mattis eget mi consequat, posuere pretium urna.";
  case 2:
    return "Mauris suscipit consectetur porta. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec ullamcorper purus at nisl elementum tincidunt. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Proin maximus, massa sed bibendum dignissim, lorem dolor ullamcorper diam, eget varius lorem sem sit amet nunc. Ut ac blandit est. Praesent ac tortor felis. Duis tempor massa in dui porttitor, a commodo metus ornare. Curabitur auctor arcu sit amet pellentesque tempus. Donec imperdiet erat a mi blandit, id iaculis dolor aliquam. Nullam nibh libero, rhoncus a mattis quis, tincidunt vitae tellus. Integer ultrices nisi massa, sit amet dignissim risus tristique porta. Sed eget venenatis justo. Curabitur imperdiet auctor tellus et facilisis. Proin cursus nisi eu dui tempor, a dapibus magna ornare. Etiam ut enim elit. Donec a est a ipsum congue fermentum. Etiam bibendum rhoncus nisi. Donec nec euismod diam, at volutpat erat. Sed nec nunc fringilla, venenatis nisi at, condimentum nisi. Mauris mollis faucibus metus a pharetra. Donec sit amet vulputate metus. Nam tempor turpis nisi, et tincidunt sem dapibus non. Etiam quis efficitur tortor, eu euismod leo. Vivamus libero neque, egestas eu justo vitae, pellentesque egestas neque. Proin varius tempus erat eget tempor. Nulla facilisi.";
  default:
    return "This should not be here";
  }
}

void drawTest(WINDOW *test_win, std::string test)
{
  box(test_win, 0, 0);
  mvwaddstr(test_win, 0, 5, "TIME:");
  mvwaddstr(test_win, 0, (getmaxx(test_win) - 14) / 2, "typing-trainer");
  // wrefresh(test_win);
  int curx = 1;
  int cury = 1;
  wattron(test_win, A_DIM);
  while (test != "")
  {
    mvwaddnstr(test_win, cury, curx, test.c_str(), getmaxx(test_win) - 2);
    test.erase(0, getmaxx(test_win) - 2);
    curx = 1;
    cury++;
  }
  wattroff(test_win, A_DIM);

  while (cury < getmaxy(test_win) - 2)
  {
    mvwaddch(test_win, cury, curx, ' ');
    curx++;
    if (curx > getmaxx(test_win) - 2)
    {
      curx = 1;
      cury++;
    }
  }
}

void freeMode(WINDOW *test_win, int ch, std::string test, std::string *input,
              unsigned *cury, unsigned *curx, enum STATES *state, unsigned *mistakes)
{
  if (ch == KEY_BACKSPACE && (*input).length() > 0)
  {
    (*input).pop_back();

    if (*curx == 1 && *cury > 1)
    {
      (*curx) = getmaxx(test_win) - 2;
      (*cury)--;
    }
    else
    {
      (*curx)--;
    }

    mvwaddch(test_win, *cury, *curx, test[(*curx - 1) + (*cury - 1) * (getmaxx(test_win) - 2)]);
    mvwchgat(test_win, *cury, *curx, 1, A_DIM, 0, nullptr);
  }
  else if (isprint(ch))
  {
    if (ch == test[(*curx - 1) + (*cury - 1) * (getmaxx(test_win) - 2)])
    {
      *input += ch;
      wattron(test_win, COLOR_PAIR(1));
      mvwaddch(test_win, *cury, *curx, ch);
      wattroff(test_win, COLOR_PAIR(1));
      wattroff(test_win, A_DIM);
      (*curx)++;
    }
    else
    {
      *input += ch;
      wattron(test_win, COLOR_PAIR(2));
      mvwaddch(test_win, *cury, *curx, test[(*curx - 1) + (*cury - 1) * (getmaxx(test_win) - 2)]);
      wattroff(test_win, COLOR_PAIR(2));
      wattroff(test_win, A_DIM);
      (*curx)++;
      (*mistakes)++;
    }

    if (*curx == getmaxx(test_win) - 1 && *cury < getmaxy(test_win) - 1)
    {
      (*curx) = 1;
      (*cury)++;
    }

    if ((*input).length() == test.length())
    {
      (*state) = STATES::RESULT;
      box(test_win, 0, 0);
      mvwaddstr(test_win, 0, (getmaxx(test_win) - 14) / 2, "typing-trainer");
      wrefresh(test_win);
    }
  }
}

void fixedMode(WINDOW *test_win, int ch, std::string test, std::string *input,
               unsigned *cury, unsigned *curx, enum STATES *state, unsigned *mistakes)
{
  if (ch == test[(*curx - 1) + (*cury - 1) * (getmaxx(test_win) - 2)])
  {
    (*input) += ch;
    wattron(test_win, COLOR_PAIR(1));
    mvwaddch(test_win, *cury, *curx, ch);
    wattroff(test_win, COLOR_PAIR(1));
    wattroff(test_win, A_DIM);
    (*curx)++;

    if (*curx == getmaxx(test_win) - 1 && *cury < getmaxy(test_win) - 1)
    {
      (*curx) = 1;
      (*cury)++;
    }
    if ((*input).length() == test.length())
    {
      (*state) = STATES::RESULT;
      box(test_win, 0, 0);
      mvwaddstr(test_win, 0, (getmaxx(test_win) - 14) / 2, "typing-trainer");
      wrefresh(test_win);
    }
  }
}

int main()
{
  initscr();
  noecho();
  cbreak();

  start_color();

  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);

  curs_set(0);

  // mvaddstr(0, 0, "PRESS F1 TO EXIT");
  mvaddstr(0, 0, "PRESS F2 TO PAUSE");
  mvaddstr(1, 0, "USE ARROW KEYS TO NAVIGATE");
  refresh();

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  WINDOW *test_win = newwin(32, 52, (max_y - 32) / 2, (max_x - 52) / 2);

  WINDOW *menu_win = newwin(7, 20, (max_y - 6) / 2, (max_x - 20) / 2);

  WINDOW *result_win = newwin(7, 20, (max_y - 6) / 2, (max_x - 20) / 2);

  int ch;
  enum STATES state = STATES::MENU;

  unsigned mode = 0;
  unsigned selectedMode = 0;
  unsigned size = 0;
  unsigned row_select = 0;
  bool resume = false;

  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, COLOR_RED);

  unsigned curx = 1;
  unsigned cury = 1;

  std::string test;
  std::string input;

  double time_passed = 0;
  time_t frame_time = 0;

  unsigned mistakes = 0;

  box(test_win, 0, 0);
  // mvwaddstr(test_win, 0, 5, "TIME:");
  mvwaddstr(test_win, 0, (getmaxx(test_win) - 14) / 2, "typing-trainer");
  wrefresh(test_win);
  while (state != STATES::EXIT)
  {

    ch = getch();

    // if (ch == KEY_F(1))
    // {
    //   state = STATES::EXIT;
    // }

    if (state == STATES::MENU)
    {
      navigateMenu(ch, &row_select, &mode, &size);
      drawMenu(menu_win, row_select, resume, mode, size);

      if (resume)
      {
        mvwchgat(test_win, 0, 10, std::log10((int)time_passed) + 1, A_BLINK, 0, nullptr);
        wrefresh(test_win);
      }

      if (ch == '\n')
      {
        state = transitFromMenu(row_select, resume);
      }
    }
    else if (state == STATES::NEW)
    {
      wclear(test_win);
      test = generateTest(size);
      drawTest(test_win, test);
      state = STATES::TEST;
      resume = true;
      curx = 1;
      cury = 1;
      input = "";

      selectedMode = mode;

      time_passed = 0;
      frame_time = 0;

      mistakes = 0;
    }
    else if (state == STATES::RESUME)
    {
      wclear(test_win);
      drawTest(test_win, test);
      wattron(test_win, COLOR_PAIR(1));
      mvwaddstr(test_win, 1, 1, input.c_str());
      wattroff(test_win, COLOR_PAIR(1));
      state = STATES::TEST;

      frame_time = 0;
    }
    else if (state == STATES::TEST)
    {

      frame_time == 0 ? time_passed += 0 : time_passed += difftime(time(0), frame_time);
      frame_time = time(0);
      mvwaddstr(test_win, 0, 10, std::to_string((int)time_passed).c_str());

      if (ch == KEY_F(2))
      {
        state = STATES::MENU;
        row_select = 0;
      }
      /*else if (ch == KEY_F(3))
      {
        state = STATES::RESULT;
      }*/

      if (selectedMode == 0)
      {
        freeMode(test_win, ch, test, &input, &cury, &curx, &state, &mistakes);
      }
      else if (selectedMode == 1)
      {
        fixedMode(test_win, ch, test, &input, &cury, &curx, &state, &mistakes);
      }
      wrefresh(test_win);
    }
    else if (state == STATES::RESULT)
    {
      drawResult(result_win, time_passed, mistakes);
      if (ch == '\n')
      {
        state = STATES::MENU;
        row_select = 0;
        resume = false;
      }
    }
  }

  wclear(result_win);
  delwin(result_win);

  wclear(menu_win);
  delwin(menu_win);

  wclear(test_win);
  delwin(test_win);

  clear();
  endwin();
}
