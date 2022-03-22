#include "termlib.h"

#include <iostream>


#if defined(__unix__) //linux specific code

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
    if(ch == 3) {
        tty_set_normal();
        exit(127); //ctrl-c
    }
    if(ch == 27){ //For control sequences beginning with ESC
        getchar_unlocked(); //discard '['
        ch = getchar_unlocked(); //Get real result
    }
    tty_set_normal();
    return ch;
}

#elif defined(__WIN32) || defined(__WIN64) //windows specific code

auto termlib::set_echo(bool state) -> void {}

auto termlib::tty_set_raw() -> void {}

auto termlib::tty_set_normal() -> void {}

auto termlib::getch_raw() -> int {}

auto termlib::getch() -> int {}

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


auto termlib::printprogressbar(float progress, int max_size) -> int {
	std::cout << '[';
	auto amount = static_cast<int>(progress) / max_size;

    for(auto i = 0 ; i < amount ; ++i){
        std::cout << '#';
    }

    for(auto i = amount; i < max_size ; ++i){
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
