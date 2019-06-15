#ifndef FORMULA
#define FORMULA

#include "token.h"


enum Valor { // representa se a fórmula marcada é verdadeira, falsa, ou se ainda não foi avaliada (IND)
    V,F,IND
};

/*
    a estratégia que o usuário quer usar é representada por uma lista. o primeiro elemento da lista será a primeira estratégia utilizada
    para tentar determinar o próximo ramo onde uma regra deve ser aplicada. Se isso não for o suficiente para determinar apenas um ramo,
    utilizará a próxima estratégia da lista, até só um ramo sobrar, onde a estratégia será aplicada. Se o usuário não especificar nada,
    a estratégia utilizada será priorizar ramos a esquerda e então fórmulas com menor número de ordem (nessa ordem). Se algum desses 2
    aspectos da estratégia não forem especificados pelo usuário, serão adicionados por padrão (para que um ramo e fórmula sempre sejam encontrados).
*/

enum tiposEstrategia {
    est_esq, est_dir, //ramos priorizados
    est_lin, est_bif, //tipo de regra priorizada
    est_men, est_mai, //prioridade por número de ordem
    est_peq, est_grd, //prioridade por tamanho
    est_nao, est_e, est_ou, est_imp, //prioridade por operador
    est_inv //retornado quando se tenta perguntar para uma fórmula atômica que regra seria aplicada nela
};

//Classe abstrata da qual todas as outras classes de fórmulas herdam
class FormulaMarcada {

    public :
        Valor valor = IND;
    
        //função que retorna o tamanho da fórmula
        virtual int tamanho() = 0; 

        //função que retorna uma string representando a fórmula
        virtual string escreve() = 0; 

        //Escreve a fórmula junto com seu valor
        virtual string escreveValorada();

        //TiposFormula tipo; 

        //expande a fórmula e guarda. A primeira posição de resultado será a fórmula "à esquerda", e a segunda (se houver) a "à direita"
        virtual void expandir(FormulaMarcada** resultado) = 0;

        //retorna se uma regra linear ou que bifurca seria aplicada nessa fórmula, ou est_inv se for uma fórmula atômica
        virtual tiposEstrategia tipoRegra() = 0;

        //retorna se a fórmula contem esse operador, percorrendo toda a árvore recursivamente
        virtual bool contemOp(tiposToken op) = 0;

        //valora a fórmula e todas as suas expansões (subfórmulas) de acordo com o valor passado
        virtual void valorar(Valor V) = 0;

        //retorna se a fórmula é igual a outra (mesmo valor e escrita do mesmo jeito)
        bool eIgual(FormulaMarcada *outra);

        //Escreve o nome da regra que seria aplicada na fórmula
        virtual string nomeRegra() = 0;

        virtual ~FormulaMarcada();



};



class FormulaAtomica : public FormulaMarcada {
    private:
        string nome;
    
    public:

        FormulaAtomica(string Nome);
        int tamanho() override;
        string escreve() override;
        void expandir(FormulaMarcada** resultado) override;
        tiposEstrategia tipoRegra() override;
        bool contemOp(tiposToken op) override;
        void valorar(Valor V) override;
        string nomeRegra() override;


};

class FormulaBinaria : public FormulaMarcada {
    private: 
        FormulaMarcada *esquerda, *direita;
        Token operador;
        //Converte um tipoToken para um operador
        static Token operadorDeTipo(tiposToken op);
    public:

        FormulaBinaria(FormulaMarcada *Esquerda, tiposToken op,  FormulaMarcada *Direita);
        ~FormulaBinaria();
        int tamanho() override;
        string escreve() override;
        void expandir(FormulaMarcada** resultado) override;
        tiposEstrategia tipoRegra() override;
        bool contemOp(tiposToken op) override;
        void valorar(Valor V) override;
        string nomeRegra() override;
};

class FormulaUnaria : public FormulaMarcada {
    private:
        FormulaMarcada* formula;
        Token operador = {NEGACAO, '-'};

    public:

        FormulaUnaria(FormulaMarcada *Formula);
        ~FormulaUnaria();
        int tamanho() override;
        string escreve() override;
        void expandir(FormulaMarcada** resultado) override;
        tiposEstrategia tipoRegra() override;
        bool contemOp(tiposToken op) override;
        void valorar(Valor V) override;
        string nomeRegra() override;
};


//classe usada para representar um bloco com seu número de ordem
class FormulaOrdenada {
    public:
        FormulaMarcada* formula;
        unsigned int ordem;

        FormulaOrdenada(FormulaMarcada* Formula, unsigned int Ordem);
        FormulaOrdenada();
        //gera uma lista de fórmulas com número de ordem a partir da lista de fórmulas normal (usada para inicializar o primeiro ramo)
        static list<FormulaOrdenada> gerarLista(list<FormulaMarcada*> listaFormulas);
};
#endif