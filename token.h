#ifndef TOKEN
#define TOKEN
enum tiposToken{
    PARENTESES_ESQ, PARENTESES_DIR, E, OU, IMPLICA, NEGACAO,
    CONSEQUENCIA, VIRGULA,
    PADRAO,
    ATOMO
};



#include <string>
#include <list>
#include <iostream>
using namespace std;


//Classe que representa cada componente léxico lido da entrada do usuário. Contém o tipo do componente léxico lido e a string correspondente.
class Token {
    public:
        tiposToken tipo = PADRAO;
        std::string lexema = "";

        Token(tiposToken Tipo, std::string Lexema) : tipo(Tipo), lexema(Lexema){};
        Token(tiposToken Tipo, char Lexema) : tipo(Tipo), lexema(string(1, Lexema)){};
        Token(){};
};

#endif
