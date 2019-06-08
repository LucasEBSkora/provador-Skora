//Lucas Eduardo Bonancio Skora - RA 1716883

#include "provador.h"
#include "Interpretador.h"

int main () {

    string fonte = "";

    cout << "Feito por Lucas Eduardo Bonancio Skora - github.com/LucasEBSkora\n"
    << "interpretador de formulas criado com base no livro \"Crafting Interpreters\" de Bob Nystrom (disponivel em http://www.craftinginterpreters.com/)\n\n"
    << "\tEscreva o sequente com as formulas da esquerda separadas por virgulas;\n"
    << "\tatomos validos comecam com letras e sao compostos por letras e numeros;\n"
    << "\toperadores:\n"
    << "\t\t& -> E\n"
    << "\t\t| -> OU\n"
    << "\t\t> -> IMPLICA\n"
    << "\t\t- -> NAO\n"
    << "\t\t= -> SEQUENTE\n";


    //lê a entrada
    do {
        string S;
        getline(cin, S);
        fonte += S;
    } while (fonte.find_first_of('=') == string::npos);

    cout << "Processando entrada...\n\n";

    //gera a lista de tokens
    Interpretador interpretador = Interpretador(fonte);
    list<Token> lista = interpretador.lerTokens();
    if (interpretador.erro) cout << "erro no processamento da entrada!\n";

    //imprime lista de tokens
    for(list<Token>::iterator i = lista.begin(); i != lista.end(); i++) {
        cout << "\"" << i->lexema << "\" ";
    }
    cout << "\n";

    if (interpretador.erro) return 0;

    cout << "Construindo formulas...\n\n";

    //gera fórmulas a partir da lista de tokens
    list<FormulaMarcada*> listaFormulas = interpretador.Analisar();

    if (interpretador.erro) cout << "erro na construção das formulas!\n";

    //imprime as fórmulas, seus tamanhos, valores e números de ordem
    for(list<FormulaMarcada*>::iterator i = listaFormulas.begin(); i != listaFormulas.end(); ++i) {
        cout << (**i).ordem << ": " << (**i).escreveValorada() << " Tamanho:" << (**i).tamanho() << "\n";
    }
    cout <<"\nAtomos : {" ;

    //imprime as fórmulas atômicas

    for(list<FormulaAtomica>::iterator atomo = interpretador.atomos.begin(); atomo != interpretador.atomos.end(); ++atomo) {
        cout << atomo->nome << ", ";
    }

    cout << "}\n\n";

    if (interpretador.erro) return 0;

    cout << "Descreva a estrategia desejada (sem espacos):\n"
     << "\te para comecar pelos ramos da esquerda, d pelos da direita\n"
     << "\tl para priorizar as regras lineares, b para as que bifurcam\n"
     << "\tm para formula com menor numero de ordem, M para maior\n"
     << "\tp para priorizar formulas com menor tamanho, g para formulas com maior\n"
     << "\t>, -, | ou & para priorizar formulas com esses operadores\n";

    string estrategia;
    getline(cin, estrategia);

    //Provador provador = Provador(listaFormulas, interpretador.atomos);
    Provador provador = {listaFormulas, interpretador.atomos};

    while (!provador.validarEstrategia(estrategia)) {
        cout << "estrategia invalida! Por favor, digite uma estrategia valida:\n";
        getline(cin, estrategia);
    }

    cout << "Estrategia valida!\nResolvendo tablo...\n";

    list<FormulaAtomica> valoracaoContraExemplo = provador.Resolver();


    if (valoracaoContraExemplo.size() == 0) {
        cout << "\nTodos os ramos fechados!\nEsta demonstrado: O sequente e valido!\n";
    } else {

        cout << "\nRamo aberto e saturado: o sequente e falso!\nValoracao contra exemplo:\n";

        for(list<FormulaAtomica>::iterator atomo = valoracaoContraExemplo.begin(); atomo != valoracaoContraExemplo.end(); ++atomo) {
            cout << "V(" << atomo->nome << ") = ";
            if (atomo->valor == IND) cout << "indiferente\n";
            else cout << (atomo->valor == V ? "V" : "F");
        }
    }

    return 0;
}
