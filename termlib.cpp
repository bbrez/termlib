#include "termlib.h"

#include <iostream>


#if defined(__unix__) //código específico para linux

auto setup_terminal() -> unsigned long {
    std::setlocale(LC_ALL, "pt_BR.UTF-8");
    return 0;
}

auto termlib::set_echo(bool state) -> void {
    if(state){
        system("stty echo");
    } else {
        system("stty -echo");
    }
}

auto termlib::tty_set_raw() -> void {
    system("stty raw");
    system("stty -echo");
}


auto termlib::tty_set_normal() -> void {
    system("stty cooked");
    system("stty echo");
}

auto termlib::getch_raw() -> int {
    tty_set_raw();
    int ch = getchar_unlocked();
    tty_set_normal();
    return ch;
}


auto termlib::getch() -> int {
    tty_set_raw();
    int ch = getchar_unlocked();
    if(ch == 3) { //ctrl-c
        tty_set_normal();
        exit(127);
    }
    if(ch == 27){ //Sequencias de controle começadas com ESC
        getchar_unlocked(); //descarta '['
        ch = getchar_unlocked(); //Le o valor real
    }
    tty_set_normal();
    return ch;
}

#elif defined(__WIN32) || defined(__WIN64) //código específico para windows
#include <windows.h>
#include <conio.h>

auto safe_SetConsoleMode(long handle, unsigned long mode) -> unsigned long {
	HANDLE conHandle = GetStdHandle(handle);
	if (conHandle == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(conHandle, &dwMode))
	{
		return GetLastError();
	}

	dwMode |= mode;
	if (!SetConsoleMode(conHandle, dwMode))
	{
		return GetLastError();
	}
	return 0;
}

auto termlib::setup_terminal() -> unsigned long {
	system("chcp 65001 > nul");
	safe_SetConsoleMode(STD_INPUT_HANDLE, ENABLE_VIRTUAL_TERMINAL_INPUT);
	return safe_SetConsoleMode(STD_OUTPUT_HANDLE, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

auto termlib::set_echo(bool state) -> void {
	safe_SetConsoleMode(STD_OUTPUT_HANDLE, state ? ENABLE_ECHO_INPUT : ~ENABLE_ECHO_INPUT);
}

auto termlib::tty_set_raw() -> void {
	unsigned long newmode = ~ENABLE_ECHO_INPUT | ~ENABLE_LINE_INPUT | ~ENABLE_PROCESSED_INPUT;
	safe_SetConsoleMode(STD_INPUT_HANDLE, newmode);
}

auto termlib::tty_set_normal() -> void {
	safe_SetConsoleMode(STD_INPUT_HANDLE, ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
}

auto termlib::getch_raw() -> int {
	termlib::tty_set_raw();
	int c = ::getch(); //getch() de conio.h
	termlib::tty_set_normal();
	return c;
}

auto termlib::getch() -> int {
	termlib::tty_set_raw();
	int ch = ::getch();
	if(ch == 3){ //ctrl-c
		termlib::tty_set_normal();
		exit(127);
	}
	if(ch == 224){ //setas
		ch = ::getch();
	}
	termlib::tty_set_normal();
	return ch;
}

#endif


auto termlib::setfg(termlib::color c) -> void {
	std::cout << CSI << 30+c << 'm';
}


auto termlib::setbg(termlib::color c) -> void {
	std::cout << CSI << 40+c << 'm';
}


auto termlib::reseteffect() -> void {
	std::cout << CSI << 0 << 'm';
}


auto termlib::seteffect(termlib::effect e) -> void {
	std::cout << CSI << e << 'm';
}


auto termlib::unseteffect(termlib::effect e) -> void {
	std::cout << CSI << e+20 << 'm';
}


auto termlib::movecur(direction d, int amount) -> void {
    std::cout << CSI << amount << d;
}


auto termlib::getcurxy(int *x, int *y) -> void {
    std::cout << CSI << "6n";
    tty_set_raw();
    fscanf(stdin, "\033[%d;%dR", x, y);
    tty_set_normal();
}


auto termlib::setcurxy(int x, int y) -> void {
    std::cout << CSI << x << ';' << y << 'H';
}


auto termlib::clrscr() -> void {
    std::cout << CSI << "2J";
}

auto termlib::savecurpos() -> void {
    std::cout << CSI << 's';
}


auto termlib::rescurpos() -> void {
    std::cout << CSI << 'u';
}


auto termlib::printprogressbar(int progress, int total, int bar_size) -> int {
	std::cout << '[';
	auto amount = progress * bar_size / total;

    for(auto i = 0 ; i < amount ; ++i){
        std::cout << '#';
    }

    for(auto i = amount; i < bar_size ; ++i){
        std::cout << '-';
    }

    std::cout << ']';

    return amount;
}

auto termlib::menu(std::vector<std::string> options) -> int {

    int selected = -1, current = 0, x, y;
    getcurxy(&x, &y);
    while(selected == -1){
        setcurxy(x, y);
        getcurxy(&x, &y);
        for(int i = 0; i < static_cast<int>(options.size()) ; ++i){
            if(i == current){
                setfg(color::Yellow);
                std::cout << '>' << options.at(i) << '\n';
                setfg(color::Default);
            } else {
                std::cout << '-' << options.at(i) << '\n';
            }
        }

        int lido = getch();
        switch (lido) {
            case 13: //Enter
                selected = current;
            case termlib::direction::Up:
                current = (current - 1 + options.size()) % options.size();
                break;
            case termlib::direction::Down:
                current = (current + 1) % options.size();
                break;
            default:
                std::cout << "Erro";
                break;
        }
    }

    return selected;
}
