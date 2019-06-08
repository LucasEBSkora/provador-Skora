#ifndef INTERPRETADOR
#define INTERPRETADOR

#include "formula.h"

//classe que lê a entrada do usuário, a divide em tokens (componentes léxicos) e então a organiza em fórmulas (faz o papel do scanner e do parser)

class Interpretador
{
private:


    string fonte;
    unsigned int atual = 0; //posição da string de entrada que está sendo lida atualmente
    list<Token> listaTokens = list<Token>(); //lista onde os tokens lidos na entrada são armazenados
    list<Token> listaAux = list<Token>(); //lista que guarda os tokens que fazem parte da fórmula que está sendo construída
    list<Token>::iterator iteradorListaAux = listaAux.begin(); //iterador usado para percorrer a listaAux
    list<FormulaMarcada*> listaFormulas = list<FormulaMarcada*>(); //lista de fórmulas que é gerada pelo parser
    bool encontrouConsequencia = false; // variável que controla se algum operador "=" já foi encontrado

    bool eOperador(char c);
    bool eLetra(char c);
    bool eLetraOuNumero(char c);
    bool eFormatacao(char c);


    /*
            as fórmulas da lógica proposicional poderiam ser construídas a partir das seguintes regras:

            fórmula -> átomo OU binária OU unária
            binária -> fórmula (& OU | OU >) fórmula
            unária -> - fórmula
            átomo -> nome

            Mas isso não respeitaria as regras de precedência (NÃO, E, OU, IMPLICA) nem as de aninhamento (NÃO e E aninham à esquerda, IMPLICA alinha à direita)
            Para consertar isso, as fórmulas devem ser criadas a partir das seguintes regras:
            (onde o * significa que o que está dentro do parênteses podem acontecer nenhuma, uma ou várias vezes)
             

            fórmula -> implicação
            implicação -> ( ou >)* ou
            ou -> e ( | e)*
            e -> unária OU primária ( & unária OU primária)*
            unária -> - (unária OU primário)
            primário -> átomo OU (fórmula)

            Isso respeita as regras de precedência pois garante que as fórmulas que são operandas de algum operador tem como operação principal
            sempre algo com precedência maior.
            Isso também preserva as regras de aninhamento pois, para a implicação, as repetições opcionais estão antes da chamada obrigatória,
            e para o "ou" e o "e", estão depois.
            
            Cada uma dessas regras está implementada nas funções abaixo.

        */

    FormulaMarcada *formula();
    FormulaMarcada *implica();
    FormulaMarcada *ou();
    FormulaMarcada *e();
    FormulaMarcada *unario();
    FormulaMarcada *primario();

    bool adicionaAtomo(string lexema); //adiciona um átomo a lista de fórmulas atômicas, evidando duplicatas. se o átomo não estava na lista, retorna true.
    void lerToken(); //função que lê o próximo token


public:
    list<FormulaAtomica> atomos = list<FormulaAtomica>();
    Interpretador(string Fonte): fonte(Fonte) {};
    bool erro = false;                         //se há algum erro que impedirá o programa de prosseguir, essa variável é setada
    list<Token> lerTokens();           //Função chamada para dividir a entrada em tokens
    list<FormulaMarcada *> Analisar(); //função que organiza os tokens em fórmulas
    ~Interpretador(); //desaloca o espaço usado pela lista de fórmulas
};

#endif