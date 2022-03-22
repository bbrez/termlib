#ifndef __TERMLIB_H_
#define __TERMLIB_H_

#include <vector>
#include <string>

namespace termlib{
	const auto CSI = "\x1B["; //Control Sequence Introducer

	typedef enum color {
		Black = 0,
		Red = 1,
		Green = 2,
		Yellow = 3,
		Blue = 4,
		Magenta = 5,
		Cyan = 6,
		White = 7,
        Default = 9
	} color;

	typedef enum effect {
		Reset,
		Bold = 1,
		Underline = 4,
		Inverse = 7,
	} effect;

    typedef enum direction {
        Up = 'A',
        Down = 'B',
        Right = 'C',
        Left = 'D',
        NextLine = 'E',
        PrevLine = 'F',
        LineNo = 'G'
    } direction;

    auto set_echo(bool state) -> void;

    auto tty_set_raw() -> void;
    auto tty_set_normal() -> void;

    auto setfg(termlib::color c) -> void;
	auto setbg(termlib::color c) -> void;

    auto reseteffect() -> void;
    auto seteffect(effect e) -> void;
	auto unseteffect(effect e) -> void;

    auto movecur(direction d, int amount) -> void;
    auto getcurxy(int *x, int *y) -> void;
    auto setcurxy(int x, int y) -> void;
    auto savecurpos() -> void;

    auto rescurpos() -> void;

    auto clrscr() -> void;

    auto getch_raw() -> int;
    auto getch() -> int;

	auto printprogressbar(float progress, int max_size=10) -> int;
    auto menu(std::vector<std::string> options) -> int;
}

#endif
