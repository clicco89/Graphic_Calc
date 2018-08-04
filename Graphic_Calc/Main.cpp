/*
* File:   main.cpp
* Author: Matteo Guglielmetti
*
* Created on 20 ottobre 2017, 17.06
*/

#include <iostream>
#include <limits.h>
#include <vector>
#include <stdexcept>
#include <functional>
#include "expr.h"
#include "olcConsoleGameEngine.h"

using namespace std;

enum DEPTH
{
	GRAPH = 0,
	MAIN_MENU = 1,
	ABOUT_WIN = 2,
	FUNCS_WIN = 3,
	FUNCEDITOR_WIN = 4
};

/* UI STRUCTS */
struct Menu
{
	bool visible = false,
		focus = false;

	vector<string> headers;
	vector<function<void(void)>> funcs;
	int sel = 0;

	int onClose_depth; //Depth set when window is closed
};
struct Button 
{
	bool focus = false;
	int x, y;
	
	string content;
	function<void(void)> func;
};
struct Label 
{
	int x, y;
	
	string content;
};
struct TextBox
{
	bool focus = false;
	int x, y, width;

	string content;
	int pos = -1; //Position in text
};
struct ListBox
{
	bool focus = false;

	int x, y, width, rows;

	vector<string> headers;
	vector<function<void(void)>> funcs;

	int sel = 0;
};
struct Window
{
	bool visible = false,
		focus = false;
	int x, y, width, height;

	string title;

	vector<Button> buttons;
	vector<Label> labels;
	vector<ListBox> listboxes;
	vector<TextBox> textboxes;

	int onClose_depth,
		child_focus = 0; //Element inside window focused
};

/* GRAPHIC CALC STRUCTS */
struct Function
{
	string function;
	vector<string> postfix_code;
	int color;
};

/* ENVIRONMENT CLASS */
class Environment : public olcConsoleGameEngine
{
public:
	Environment() 
	{
		m_sAppName = L"Graphics Calculator";
	}

private:
	//PROGRAM PARAMETERS
	string version = "0.1.0.1 (STABLE)";

	//UI OBJECTS
	vector<Function> graph_funcs;
	Menu main_menu;
	Window about_win, funcs_win, funceditor_win, error_win;

	bool error_win_drawn = false;

	//Function position to be changed
	int funceditor_funcpos;
	
	//Avaible colors
	vector<string> av_colors = {
		"BLUE", 
		"CYAN", 
		"GREEN", 
		"MAGENTA", 
		"RED", 
		"YELLOW"
	};

	//GRAPH VARS
	double zoom, min_zoom, max_zoom, zoom_k,
		calc_approx;
	int offset_x, offset_y;

	//DEPTH
	int depth; //Position in menus/windows
	bool changing_depth = false;
	void changeDepth(int new_depth)
	{
		depth = new_depth;
		changing_depth = true;

		//Set visibility
 		main_menu.visible = depth >= MAIN_MENU;
		about_win.visible = depth == ABOUT_WIN;
		funcs_win.visible = depth >= FUNCS_WIN;
		funceditor_win.visible = depth == FUNCEDITOR_WIN;
		if (error_win.visible)
		{
			if(error_win_drawn) error_win.visible = error_win_drawn = false;
			else error_win_drawn = true;
		}
		//Set focus
		if (error_win.visible)
		{
			error_win.focus = true;
			main_menu.focus = false;
			about_win.focus = false;
			funcs_win.focus = false;
			funceditor_win.focus = false;
		}
		else
		{
			main_menu.focus = depth == MAIN_MENU;
			about_win.focus = depth == ABOUT_WIN;
			funcs_win.focus = depth == FUNCS_WIN;
			funceditor_win.focus = depth == FUNCEDITOR_WIN;
		}

	}

	//STRINGS FUNCS
	vector<vector<bool>> str_getCharFillers(char ch)
	{
		switch (ch) {
		case 'A': case 'a':
			return
			{
				{ 0, 1, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
			};
			break;
		case 'B': case 'b':
			return
			{
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 0, 0 },
			};
			break;
		case 'C': case 'c':
			return
			{
				{ 0, 1, 1, 1, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 1, 1, 1, 0 },
			};
			break;
		case 'D': case 'd':
			return
			{
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 0, 0 },
			};
			break;
		case 'E': case 'e':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case 'F': case 'f':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		case 'G': case 'g':
			return
			{
				{ 0, 1, 1, 1, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 0, 1, 1, 1, 0 },
			};
			break;
		case 'H': case 'h':
			return
			{
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
			};
			break;
		case 'I': case 'i':
			return
			{
				{ 1, 1, 1, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 1, 1, 1, 0, 0 },
			};
			break;
		case 'J': case 'j':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 1, 0, 1, 0, 0 },
				{ 0, 1, 0, 0, 0 },
			};
			break;
		case 'K': case 'k':
			return
			{
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 1, 0, 0 },
				{ 1, 1, 0, 0, 0 },
				{ 1, 0, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
			};
			break;
		case 'L': case 'l':
			return
			{
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case 'M': case 'm':
			return
			{
				{ 1, 0, 0, 0, 1 },
				{ 1, 1, 0, 1, 1 },
				{ 1, 0, 1, 0, 1 },
				{ 1, 0, 0, 0, 1 },
				{ 1, 0, 0, 0, 1 },
			};
			break;
		case 'N': case 'n':
			return
			{
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 0, 1, 0 },
				{ 1, 0, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
			};
			break;
		case 'O': case 'o':
			return
			{
				{ 0, 1, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 0, 1, 1, 0, 0 },
			};
			break;
		case 'P': case 'p':
			return
			{
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		case 'Q': case 'q':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 1, 1, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case 'R': case 'r':
			return
			{
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
			};
			break;
		case 'S': case 's':
			return
			{
				{ 0, 1, 1, 1, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 1, 1, 0, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 1, 1, 1, 0, 0 },
			};
			break;
		case 'T': case 't':
			return
			{
				{ 1, 1, 1, 1, 1 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 0, 1, 0, 0 },
			};
			break;
		case 'U': case 'u':
			return
			{
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 0, 1, 1, 0, 0 },
			};
			break;
		case 'V': case 'v':
			return
			{
				{ 1, 0, 0, 0, 1 },
				{ 1, 0, 0, 0, 1 },
				{ 1, 0, 0, 0, 1 },
				{ 0, 1, 0, 1, 0 },
				{ 0, 0, 1, 0, 0 },
			};
			break;
		case 'W': case 'w':
			return
			{
				{ 1, 0, 0, 0, 1 },
				{ 1, 0, 0, 0, 1 },
				{ 1, 0, 0, 0, 1 },
				{ 1, 0, 1, 0, 1 },
				{ 0, 1, 0, 1, 0 },
			};
			break;
		case 'X': case 'x':
			return
			{
				{ 1, 0, 0, 0, 1 },
				{ 0, 1, 0, 1, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 1, 0, 1, 0 },
				{ 1, 0, 0, 0, 1 },
			};
			break;
		case 'Y': case 'y':
			return
			{
				{ 1, 0, 0, 0, 1 },
				{ 0, 1, 0, 1, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 0, 1, 0, 0 },
			};
			break;
		case 'Z': case 'z':
			return
			{
				{ 1, 1, 1, 1, 1 },
				{ 0, 0, 0, 1, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 1, 1, 1, 1, 1 },
			};
			break;
		case '0':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case '1':
			return
			{
				{ 1, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
			};
			break;
		case '2':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case '3':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 0, 1, 1, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case '4':
			return
			{
				{ 1, 0, 0, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 0, 0, 0, 1, 0 },
			};
			break;
		case '5':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 1, 1, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case '6':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case '7':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 0, 0, 0, 1, 0 },
			};
			break;
		case '8':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case '9':
			return
			{
				{ 1, 1, 1, 1, 0 },
				{ 1, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 1, 1, 1, 1, 0 },
			};
			break;
		case '!':
			return
			{
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		case '?':
			return
			{
				{ 1, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		case '.':
			return
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		case ':':
			return
			{
				{ 0, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			};
			break;
		case ';':
			return
			{
				{ 1, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		case '+':
			return
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			};
			break;
		case '-':
			return
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			};
			break;
		case '*':
			return
			{
				{ 1, 0, 1, 0 },
				{ 0, 1, 0, 0 },
				{ 1, 0, 1, 0 },
				{ 0, 0, 0, 0 },
				{ 0, 0, 0, 0 },
			};
			break;
		case '/':
			return
			{
				{ 0, 0, 1, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		case '^':
			return
			{
				{ 0, 1, 0, 0 },
				{ 1, 0, 1, 0 },
				{ 0, 0, 0, 0 },
				{ 0, 0, 0, 0 },
				{ 0, 0, 0, 0 },
			};
			break;
		case '(':
			return
			{
				{ 0, 1, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
			};
			break;
		case ')':
			return
			{
				{ 1, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		case '=':
			return
			{
				{ 0, 0, 0, 0, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 1, 1, 1, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			};
			break;
		case '\'':
			return
			{
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			};
			break;
		case '_':
			return
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 1, 1, 1, 1, 1 },
			};
			break;
		case ' ':
			return
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			};
			break;
		case '|':
			return
			{
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0 },
			};
			break;
		default:
			throw invalid_argument("Char unsupported!");
		}
	}
	int str_getCharWidth(vector<vector<bool>> fillers)
	{
		bool width_got = false; //Got the width
		int w;
		for (w = 0; w < 5 && !width_got; w++)
		{
			width_got = true;
			for (int h = 0; h < 5; h++) width_got = !fillers[h][w] && width_got;
			if (width_got) w--;
		}
		//Space control
		if (w == 0) w++;

		return w;
	}
	int str_length(string content)
	{
		int filled_px = 0;

		for (int i = 0; i < content.length(); i++)
		{
			//Get char
			vector<vector<bool>> fillers = str_getCharFillers(content[i]);

			//Get width
			int width = str_getCharWidth(fillers);

			filled_px += width + 1; //Add space
		}

		return filled_px;
	}
	void str_draw(int x, int y, string content, COLOUR color, int max_chars = 0)
	{
		int filled_px = 0;
		for (int i = 0; i < content.length() || ((max_chars > 0) ? max_chars : false); i++)
		{
			vector<vector<bool>> fillers = str_getCharFillers(content[i]);
			int width = str_getCharWidth(fillers);

			//Draw
			for (int _y = 0; _y < 5; _y++)
				for (int _x = 0; _x < width; _x++)
					if (fillers[_y][_x]) Draw(_x + x + filled_px, _y + y, L' ', color);

			filled_px += width + 1; //Add space
		}
	}

	//UI DRAWER
	void ui_drawDepth()
	{
		if (depth == MAIN_MENU) ui_drawHorMenu(main_menu, 0, 0, m_nScreenWidth);
		if (depth == ABOUT_WIN) ui_drawWindow(about_win);
		if (depth == FUNCS_WIN) ui_drawWindow(funcs_win);
		if (depth == FUNCEDITOR_WIN) ui_drawWindow(funceditor_win);
		ui_drawWindow(error_win);
	}
	//UI UPDATES
	void ui_updateMenu(Menu &menu)
	{
		if (menu.focus) 
		{
			if (m_keys[VK_RIGHT].bPressed && menu.sel < menu.headers.size() - 1) menu.sel++;
			if (m_keys[VK_LEFT].bPressed && menu.sel > 0) menu.sel--;
			if (m_keys[VK_SPACE].bPressed) menu.funcs[menu.sel]();
			if (m_keys[VK_ESCAPE].bPressed)
			{
				changeDepth(menu.onClose_depth);
				menu.sel = 0;
			}
		}
	}
	void ui_updateButton(Button &button)
	{
		if (m_keys[VK_SPACE].bPressed && button.focus) button.func();
	}
	void ui_addTextBoxChar(TextBox &textbox, char c)
	{
		if (textbox.content.length() < 256)
		{
			textbox.pos++;
			textbox.content.insert(textbox.pos, string(1, c));
		}
	}
	void ui_updateTextBox(TextBox &textbox)
	{
		if (textbox.focus)
		{
#pragma region keys
			if (m_keys[VK_SHIFT].bHeld)
			{
				//Divide
				if (m_keys['7'].bPressed)
					ui_addTextBoxChar(textbox, '/');
				//Parenthesis
				if (m_keys['8'].bPressed)
					ui_addTextBoxChar(textbox, '(');
				if (m_keys['9'].bPressed)
					ui_addTextBoxChar(textbox, ')');
				//Multiply
				if (m_keys[VK_OEM_PLUS].bPressed)
					ui_addTextBoxChar(textbox, '*');
				//Equal
				if (m_keys['0'].bPressed)
					ui_addTextBoxChar(textbox, '=');
				//Pow
				if (m_keys[0xDD].bPressed)
					ui_addTextBoxChar(textbox, '^');
				
			}
			else
			{ 
				//Numbers
				for (int i = '0'; i <= '9'; i++)
				{
					if (m_keys[i].bPressed || m_keys[i + 0x30].bPressed)
						ui_addTextBoxChar(textbox, i - 0x30 + '0');
				}
				//Plus
				if (m_keys[VK_OEM_PLUS].bPressed) 
					ui_addTextBoxChar(textbox, '+');
				//Comma
				if (m_keys[VK_OEM_COMMA].bPressed || m_keys[VK_OEM_PERIOD].bPressed) 
					ui_addTextBoxChar(textbox, '.');
				//Minus
				if (m_keys[VK_OEM_MINUS].bPressed) 
					ui_addTextBoxChar(textbox, '-');
			}

			//Alpha
			for (int i = 'A'; i <= 'Z'; i++) 
				if (m_keys[i].bPressed)
					ui_addTextBoxChar(textbox, i - 0x40 + 'a' - 1);
#pragma endregion
#pragma region move
			if (m_keys[VK_LEFT].bPressed && textbox.pos < textbox.content.length()) textbox.pos--;
			if (m_keys[VK_RIGHT].bPressed && (textbox.pos + 1) < textbox.content.length()) textbox.pos++;
#pragma endregion
#pragma region remove
			if ((m_keys[VK_BACK].bPressed && textbox.content.length() > 0 && textbox.pos > -1) || m_keys[VK_DELETE].bPressed)
			{
				if (m_keys[VK_BACK].bPressed) textbox.pos--;
				textbox.content.erase(textbox.pos + 1, 1);
			}
#pragma endregion
		}
	}
	void ui_updateListBox(ListBox &listbox)
	{
		if (listbox.focus && listbox.headers.size() > 0)
		{
			if (m_keys[VK_UP].bPressed && listbox.sel > 0) listbox.sel--;
			if (m_keys[VK_DOWN].bPressed && listbox.sel < listbox.headers.size() - 1) listbox.sel++;
			if (m_keys[VK_SPACE].bPressed) listbox.funcs[listbox.sel]();
		}
	}
	void ui_changeWindowChild(Window &win, int target)
	{
		//Update child_focus state
		win.child_focus = target;

		//Update focus states
		for (int i = 0; i < (win.buttons.size() + win.listboxes.size() + win.textboxes.size()); i++)
		{
			bool focus_state = i == win.child_focus;
			if (i < win.buttons.size()) win.buttons[i].focus = focus_state;
			else if ((i - win.buttons.size()) < win.textboxes.size()) win.textboxes[i - win.buttons.size()].focus = focus_state;
			else win.listboxes[i - win.buttons.size() - win.textboxes.size()].focus = focus_state;
		}
	}
	void ui_updateWin(Window &win)
	{
		if (win.focus)
		{
			if (m_keys[VK_ESCAPE].bPressed) changeDepth(win.onClose_depth);
			if (m_keys[VK_TAB].bPressed || win.child_focus == -1)
			{
				if (win.child_focus == (win.buttons.size() + win.listboxes.size() + win.textboxes.size() - 1))
					ui_changeWindowChild(win, 0);
				else
					ui_changeWindowChild(win, win.child_focus + 1);
			}
		}
		else ui_changeWindowChild(win, -1);
	}
	//UI DRAWS
	void ui_drawHorMenu(Menu &menu, int cont_x, int cont_y, int cont_width)
	{
		if (menu.visible)
		{
			//Update
			if (!changing_depth) ui_updateMenu(menu);

			//Draw
			Fill(cont_x, cont_y, cont_x + cont_width, cont_y + 7, L' ', BG_DARK_GREY);

			int total_length = 1;
			for (int i = 0; i < menu.headers.size(); i++)
			{
				int length = str_length(menu.headers[i]);

				if (menu.sel == i && menu.focus) Fill(cont_x + total_length - 1, cont_y, cont_x + total_length + length, cont_y + 7, L' ', BG_BLACK);
				str_draw(cont_x + total_length, cont_y + 1, menu.headers[i], BG_WHITE);

				total_length += length + 2;
			}
		}
	}
	void ui_drawButton(Button &button, int cont_x, int cont_y)
	{
		//Update
		if (!changing_depth) ui_updateButton(button);

		int x = cont_x + button.x,
			y = cont_y + button.y;

		Fill(x, y, x + str_length(button.content) + 3, y + 7, L' ', button.focus ? BG_BLACK : BG_GREY);
		str_draw(x + 2, y + 1, button.content, BG_WHITE);
	}
	void ui_drawLabel(Label &label, int cont_x, int cont_y, COLOUR color)
	{
		str_draw(cont_x + label.x, cont_y + label.y, label.content, color);
	}
	void ui_drawTextBox(TextBox &textbox, int cont_x, int cont_y)
	{
		//Update
		if (!changing_depth) ui_updateTextBox(textbox);

		const int min_distance_txt = 7; //Minimum distance in pixel from right before shifting the text

		int x = textbox.x + cont_x,
			y = textbox.y + cont_y;

		Fill(x, y, x + textbox.width, y + 9, L' ', textbox.focus ? BG_DARK_GREY : BG_GREY);

		string content = textbox.content.substr(0, textbox.pos);
		int distance;
		if (str_length(textbox.content.substr(0, textbox.pos + 1)) >= (textbox.width - min_distance_txt))
		{
			for (distance = textbox.pos; str_length(textbox.content.substr(distance, textbox.pos - distance + 1)) < (textbox.width - min_distance_txt) && distance > 0; distance--);
			content = textbox.content.substr(distance, textbox.pos - distance + 1);
		}
		else
		{
			for (distance = 0; distance < textbox.content.length() && str_length(textbox.content.substr(0, distance)) < (textbox.width - min_distance_txt); distance++);
			content = textbox.content.substr(0, distance);
		}
		
		str_draw(x + 1, y + 2, content, textbox.focus ? BG_BLACK : BG_DARK_GREY);
		if(textbox.focus) 
			str_draw(x + 1 + str_length(content.substr(0, textbox.pos + 1)), y + 2, "|", BG_WHITE);
	}
	void ui_drawListBox(ListBox &listbox, int cont_x, int cont_y)
	{
		//Update
		if (!changing_depth) ui_updateListBox(listbox);

		int x = cont_x + listbox.x,
			y = cont_y + listbox.y;

		//Draw border
		Fill(x - 1, y - 1, x + listbox.width + 1, y + listbox.rows * 10, L' ', (listbox.focus) ? BG_DARK_GREY : BG_GREY);
		Fill(x, y, x + listbox.width, y + listbox.rows * 10 - 1, L' ', BG_WHITE);

		//Draw content
		if (listbox.headers.size() == 0) 
			str_draw(x + (listbox.width - str_length("EMPTY")) / 2, y + 2, "EMPTY", BG_GREY);
		else
		{
			for (int i = 0; i < listbox.rows && i < listbox.headers.size(); i++)
			{
				if (listbox.sel == i)
					Fill(x, y + i * 10, x + listbox.width, y + i * 10 + 9, L' ', listbox.focus ? BG_BLACK : BG_GREY);
				DrawLine(x, y + i * 10 + 9, x + listbox.width - 1, y + i * 10 + 9, L' ', listbox.focus ? BG_DARK_GREY : BG_GREY);
				str_draw(x + (listbox.width - str_length(listbox.headers[i])) / 2, y + i * 10 + 2, listbox.headers[i], (listbox.sel == i) ? BG_WHITE : listbox.focus ? BG_DARK_GREY : BG_GREY);
			}
		}
	}
	void ui_drawWindow(Window &win)
	{
		if (win.visible)
		{
			//Update
			if(!changing_depth) ui_updateWin(win);

			//Draw border
			Fill(win.x - 1, win.y - 1, win.x + win.width + 1, win.y + win.height + 1, L' ', (win.focus) ? BG_BLACK : BG_DARK_GREY);

			//Draw title
			Fill(win.x, win.y, win.x + win.width, win.y + 8, L' ', (win.focus) ? BG_BLACK : BG_DARK_GREY);
			str_draw(win.x + (win.width - str_length(win.title)) / 2, win.y + 1, win.title, BG_WHITE);
			str_draw(win.x + win.width - str_length("ESC") - 1, win.y + 1, "ESC", BG_WHITE);

			//Draw content
			Fill(win.x, win.y + 8, win.x + win.width, win.y + win.height, L' ', BG_WHITE);
			int x = win.x,
				y = win.y + 8,
				width = win.width,
				height = win.height - 8;
			//Draw labels
			for (int i = 0; i < win.labels.size(); i++)
				ui_drawLabel(win.labels[i], x, y, (win.focus) ? BG_DARK_GREY : BG_GREY);
			//Draw buttons
			for (int i = 0; i < win.buttons.size(); i++)
				ui_drawButton(win.buttons[i], x, y);
			//Draw listboxes
			for (int i = 0; i < win.listboxes.size(); i++)
				ui_drawListBox(win.listboxes[i], x, y);
			//Draw textboxes
			for (int i = 0; i < win.textboxes.size(); i++)
				ui_drawTextBox(win.textboxes[i], x, y);
		}
	}

	//CALC GRAPHICS FUNCS
	void drawPlan()
	{
		//Draw background and axys
		Fill(0, 0, m_nScreenWidth, m_nScreenHeight, L' ', BG_WHITE);
		DrawLine(offset_x, 0, offset_x, m_nScreenHeight, L' ', BG_DARK_GREY);
		DrawLine(0, offset_y, m_nScreenWidth, offset_y, L' ', BG_DARK_GREY);

		for (int i = 0; i < graph_funcs.size(); i++)
		{
			//Set color
			int color;
			switch (graph_funcs[i].color)
			{
			case 'B':
				color = BG_BLUE;
				break;
			case 'C':
				color = BG_CYAN;
				break;
			case 'G':
				color = BG_GREEN;
				break;
			case 'M':
				color = BG_MAGENTA;
				break;
			case 'R':
				color = BG_RED;
				break;
			case 'Y':
				color = BG_YELLOW;
				break;
			default:
				show_error("UNKNOWN COLOR!");
				return;
			}

			//Draw function
			double last_y = 0;
			bool last_impossible = true;

			for (int x = 0; x <= m_nScreenWidth; x++)
			{
				bool impossible = false;
				double y;

				try
				{
					y = -parsePostfix(graph_funcs[i].postfix_code, (x - offset_x) * zoom) / zoom + offset_y;
				}
				catch (exception ex)
				{
					impossible = true;
				}

				//Draw line if not impossible or out of screen
				if (!last_impossible && !impossible && ((last_y > 0 && last_y < m_nScreenHeight) || (y > 0 && y < m_nScreenHeight) || (y <= 0 && last_y >= m_nScreenHeight) || (last_y <= 0 && y >= m_nScreenHeight)))
					DrawLine(x - 1, last_y, x, y, L' ', color);
				
				last_impossible = impossible;
				last_y = y;
			}
		}

		//Draw cross
		DrawLine(m_nScreenWidth / 2 - 2, m_nScreenHeight / 2, m_nScreenWidth / 2 + 2, m_nScreenHeight / 2, L' ', BG_BLACK);
		DrawLine(m_nScreenWidth / 2, m_nScreenHeight / 2 - 2, m_nScreenWidth / 2, m_nScreenHeight / 2 + 2, L' ', BG_BLACK);

		//Draw position and zoom
		str_draw(m_nScreenWidth - str_length("ZOOM: " + to_string(zoom)) - 1, 1, "ZOOM: " + to_string(zoom), BG_GREY);
		str_draw(m_nScreenWidth - str_length("X: " + to_string((-offset_x + m_nScreenWidth / 2) * zoom)) - 3, 7, "X : " + to_string((-offset_x + m_nScreenWidth / 2) * zoom), BG_GREY);
		str_draw(m_nScreenWidth - str_length("Y: " + to_string((offset_y - m_nScreenHeight / 2) * zoom)) - 3, 13, "Y : " + to_string((offset_y - m_nScreenHeight / 2) * zoom), BG_GREY);
	}
		
	void updateFuncsListbox()
	{
		funcs_win.listboxes[0].sel = 0;
		funcs_win.listboxes[0].headers = { };
		funcs_win.listboxes[0].funcs = { };

		for (int i = 0; i < graph_funcs.size(); i++)
		{
			funcs_win.listboxes[0].headers.push_back("Y=" + graph_funcs[i].function);
			funcs_win.listboxes[0].funcs.push_back([=] { 
				string color;
				switch (graph_funcs[funcs_win.listboxes[0].sel].color)
				{
				case 'B':
					color = "BLUE";
					break;
				case 'C':
					color = "CYAN";
					break;
				case 'G':
					color = "GREEN";
					break;
				case 'M':
					color = "MAGENTA";
					break;
				case 'R':
					color = "RED";
					break;
				case 'Y':
					color = "YELLOW";
					break;
				}
				av_colors.push_back(color);

				openFuncEditor(i, av_colors.size() - 1);
			});
		}
	}
	void openFuncEditor(int func_pos, int color_pos)
	{
		//func_pos == -1 -> new function

		funceditor_win.textboxes[0].content = (func_pos > -1) ? graph_funcs[func_pos].function : "";
		funceditor_win.textboxes[0].pos = -1;
		funceditor_win.listboxes[0].headers = av_colors;
		funceditor_win.listboxes[0].sel = color_pos;
		ui_changeWindowChild(funceditor_win, 1);
		funceditor_funcpos = func_pos;

		changeDepth(FUNCEDITOR_WIN);
	}
	void show_error(string text)
	{
		error_win.labels[0].content = text;
		error_win.labels[0].x = (error_win.width - str_length(text)) / 2;
		error_win.onClose_depth = depth;
		error_win.visible = true;
		changeDepth(depth);
	}

protected:
	virtual bool OnUserCreate() 
	{
		//Init functions array
		graph_funcs = { };

		//Init graph parameters
		zoom = 0.01;
		max_zoom = 0.00001;
		min_zoom = 10;
		zoom_k = 2;
		calc_approx = 0.001;
		offset_x = m_nScreenWidth / 2;
		offset_y = m_nScreenHeight / 2;

		//Init depth
		changeDepth(GRAPH);

		//Init ui components
#pragma region  main_menu
		main_menu.headers = { "FUNCTIONS", "ABOUT", "EXIT" };
		main_menu.funcs =
		{
			[&]()
			{
				updateFuncsListbox();
				ui_changeWindowChild(funcs_win, 0);
				changeDepth(FUNCS_WIN);
			},
			[&]() { changeDepth(ABOUT_WIN); },
			[]() { exit(0); },
		};
		main_menu.onClose_depth = GRAPH;
#pragma endregion
#pragma region about_win
		about_win.width = 200;
		about_win.height = 80;
		about_win.x = (m_nScreenWidth - about_win.width) / 2;
		about_win.y = (m_nScreenHeight - about_win.height) / 2;
		about_win.title = "ABOUT";
		about_win.onClose_depth = MAIN_MENU;

		Label about_text_1;
		about_text_1.content = "GRAPHICS CALC - VERSION: " + version;
		about_text_1.x = (about_win.width - str_length(about_text_1.content)) / 2;
		about_text_1.y = about_win.height / 2 - 18;

		Label about_text_2;
		about_text_2.content = "DEVELOPER:";
		about_text_2.x = (about_win.width - str_length(about_text_2.content)) / 2;
		about_text_2.y = about_win.height / 2 - 5;

		Label about_text_3;
		about_text_3.content = "MATTEO GUGLIELMETTI.";
		about_text_3.x = (about_win.width - str_length(about_text_3.content)) / 2;
		about_text_3.y = about_win.height / 2 + 3;

		Label about_text_4;
		about_text_4.content = "BASED ON OLC'S GAME LIBRARY.";
		about_text_4.x = (about_win.width - str_length(about_text_4.content)) / 2;
		about_text_4.y = about_win.height / 2 + 24;

		about_win.labels = { about_text_1, about_text_2, about_text_3, about_text_4 };
#pragma endregion
#pragma region funcs_win
		funcs_win.width = 120;
		funcs_win.height = 95;
		funcs_win.x = (m_nScreenWidth - funcs_win.width) / 2;
		funcs_win.y = (m_nScreenHeight - funcs_win.height) / 2;
		funcs_win.title = "FUNCTIONS";
		funcs_win.onClose_depth = MAIN_MENU;

		ListBox funcs_list;
		funcs_list.width = funcs_win.width - 20;
		funcs_list.rows = 6;
		funcs_list.x = 10;
		funcs_list.y = 10;

		Button new_btn;
		new_btn.x = 9;
		new_btn.y = funcs_win.height - 19;
		new_btn.func = [&] { 
			if (av_colors.size() > 0)
				openFuncEditor(-1, 0);
			else
				show_error("FUNCS LIMIT REACHED!");
		};
		new_btn.content = "NEW";

		Button remove_btn;
		remove_btn.x = funcs_win.width - 44;
		remove_btn.y = new_btn.y;
		remove_btn.func = [&] { 
			if (graph_funcs.size() > 0)
			{
				string color;
				switch (graph_funcs[funcs_win.listboxes[0].sel].color)
				{
				case 'B':
					color = "BLUE";
					break;
				case 'C':
					color = "CYAN";
					break;
				case 'G':
					color = "GREEN";
					break;
				case 'M':
					color = "MAGENTA";
					break;
				case 'R':
					color = "RED";
					break;
				case 'Y':
					color = "YELLOW";
					break;
				default:
					show_error("UNKNOWN COLOR!");
					return;
				}
				av_colors.push_back(color);

				graph_funcs.erase(graph_funcs.begin() + funcs_win.listboxes[0].sel);
				updateFuncsListbox();
				changeDepth(FUNCS_WIN);
			}
			else
				show_error("FUNCS LIST EMPTY!");
		};
		remove_btn.content = "REMOVE";

		funcs_win.listboxes = { funcs_list };
		funcs_win.buttons = { new_btn, remove_btn };

		updateFuncsListbox();
#pragma endregion
#pragma region funceditor_win
		funceditor_win.width = 100;
		funceditor_win.height = 121;
		funceditor_win.x = (m_nScreenWidth - funceditor_win.width) / 2;
		funceditor_win.y = (m_nScreenHeight - funceditor_win.height) / 2;
		funceditor_win.title = "NEW FUNCTION";
		funceditor_win.onClose_depth = FUNCS_WIN;

		Label y_lbl;
		y_lbl.x = 10;
		y_lbl.y = 12;
		y_lbl.content = "Y=";

		Label colors_lbl;
		colors_lbl.x = 10;
		colors_lbl.y = 27;
		colors_lbl.content = "COLORS:";

		TextBox function_txt;
		function_txt.width = 68;
		function_txt.x = 22;
		function_txt.y = 10;

		ListBox colors_list;
		colors_list.width = funceditor_win.width - 20;
		colors_list.rows = 6;
		colors_list.headers = av_colors;
		colors_list.funcs = { []() {}, []() {}, []() {}, []() {}, []() {}, []() {} };
		colors_list.x = 10;
		colors_list.y = 36;

		Button ok_btn;
		ok_btn.x = 9;
		ok_btn.y = funceditor_win.height - 19;
		ok_btn.func = [&] {
			if (funceditor_win.textboxes[0].content == "")
				show_error("FUNCTION EMPTY!");
			else
			{
				Function func;
				func.function = funceditor_win.textboxes[0].content;

				try
				{
					func.postfix_code = parseInfix(func.function);
				}
				catch (exception ex)
				{
					show_error(ex.what());
					return;
				}

				func.color = funceditor_win.listboxes[0].headers[funceditor_win.listboxes[0].sel].at(0);
				av_colors.erase(av_colors.begin() + funceditor_win.listboxes[0].sel); //Remove color from list

				if (funceditor_funcpos > -1) graph_funcs[funceditor_funcpos] = func;
				else graph_funcs.push_back(func);

				updateFuncsListbox();

				changeDepth(FUNCS_WIN);
			}
		};
		ok_btn.content = "OK";

		funceditor_win.labels = { y_lbl, colors_lbl };
		funceditor_win.textboxes = { function_txt };
		funceditor_win.listboxes = { colors_list };
		funceditor_win.buttons = { ok_btn };
#pragma endregion
#pragma region error_win
		error_win.width = 110;
		error_win.height = 50;
		error_win.x = (m_nScreenWidth - error_win.width) / 2;
		error_win.y = (m_nScreenHeight - error_win.height) / 2;
		error_win.title = "ABOUT";

		Label error_text;
		error_text.content = "";
		error_text.x = (error_win.width - str_length(error_text.content)) / 2;
		error_text.y = error_win.height / 2 - 10;

		error_win.labels = { error_text };
#pragma endregion

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime) 
	{
		//Draw & update
 		ui_drawDepth();

		//Graph updater
		if (depth == GRAPH && !error_win.visible)
		{
			if ((m_keys[VK_TAB].bPressed || m_keys[VK_SPACE].bPressed)) changeDepth(MAIN_MENU);
			else
			{
				if ((m_keys[VK_UP].bPressed ^ m_keys[VK_DOWN].bPressed) || (m_keys[VK_RIGHT].bPressed ^ m_keys[VK_LEFT].bPressed))
				{
					int move_offset;
					if (m_keys[VK_SHIFT].bHeld) move_offset = 100;
					else if (m_keys[VK_CONTROL].bHeld) move_offset = 1;
					else move_offset = 10;

					if (m_keys[VK_UP].bPressed) offset_y += move_offset;
					if (m_keys[VK_DOWN].bPressed) offset_y -= move_offset;

					if (m_keys[VK_RIGHT].bPressed) offset_x -= move_offset;
					if (m_keys[VK_LEFT].bPressed) offset_x += move_offset;

					drawPlan();
				}
				else if (m_keys[VK_OEM_PLUS].bPressed ^ m_keys[VK_OEM_MINUS].bPressed)
				{
					double zoom_before = zoom;
					if (m_keys[VK_OEM_PLUS].bPressed) zoom /= zoom_k;
					if (m_keys[VK_OEM_MINUS].bPressed) zoom *= zoom_k;
					if (zoom < max_zoom) zoom = max_zoom;
					if (zoom > min_zoom) zoom = min_zoom;
					double zoom_after = zoom;

					//offset correction
					offset_x = (offset_x - m_nScreenWidth / 2) * zoom_before / zoom_after + m_nScreenWidth / 2;
					offset_y = (offset_y - m_nScreenHeight / 2) * zoom_before / zoom_after + m_nScreenHeight / 2;
					
					drawPlan();
				}
				else if (m_keys['C'].bPressed)
				{
					offset_x = m_nScreenWidth / 2;
					offset_y = m_nScreenHeight / 2;

					drawPlan();
				}

			}
		}
		
		//If depth has changed...
		if (changing_depth)
		{
			//...redraw everything
			drawPlan();
			ui_drawHorMenu(main_menu, 0, 0, m_nScreenWidth);
			ui_drawWindow(about_win);
			ui_drawWindow(funcs_win);
			ui_drawWindow(funceditor_win);
			ui_drawWindow(error_win);

			changing_depth = false;
		}

		return true;
	}
};

int main() {
	Environment env;
	env.ConstructConsole(300, 300, 2, 2);
	env.Start();

	return 0;
}