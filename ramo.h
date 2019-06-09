#ifndef RAMO
#define RAMO

#include "formula.h"
#include <unordered_map>


enum estadoRamo {
    ramo_fechado,
    ramo_aberto,
    ramo_insaturado
};

class Ramo {
    
    private:
        
        //lista de fórmulas que estão no ramo
        list<FormulaMarcada*> listaFormulas;
        
        //fórmulas que já foram expandidas, mas que podem ser usadas para fechar o ramo
        list<FormulaMarcada*> expandidas;
        //número de ordem que será dado para a próxima fórmula adicionada no ramo
        unsigned int proxNumeroOrdem;

        //adiciona uma fórmula na lista de fórmulas expandidas, evitando duplicatas
        bool adicionaExpandidas(FormulaMarcada* formula); 

        
        //remove da lista de candidatos usada em aplicar() aqueles que são invalidados pelo argumento passado 
        void encontrarLinBif(tiposEstrategia estrategiaAtual, list<FormulaMarcada*>* candidatos);
        void encontrarPeqGrd(tiposEstrategia estrategiaAtual, list<FormulaMarcada*>* candidatos);
        void encontrarOp(tiposEstrategia estrategiaAtual, list<FormulaMarcada*>* candidatos);
        
    public:
        int posicao;

        //Imprime a posição do ramo e todas suas fórmulas com número de ordem, valoração e tamanho
        string imprime();
        
        //cria um ramo a partir de uma lista de fórmulas (o primeiro ramo)
        Ramo(list<FormulaMarcada*> ListaFormulas);
        
        //cria um ramo a partir de outro e de uma fórmula que deve ser adicionada ao final da lista (parte direita da expansão de alguma fórmula da lista original)
        Ramo(Ramo *ramo, FormulaMarcada* formula); 

        //retorna true se os ramos são o mesmo (mesma posição e mesmas fórmulas)
        bool comparar(Ramo *outro);

        //funções que retornam o valor usado pela classe provador para determinar em qual ramo a próxima regra deve ser aplicada
        bool estLinBif(tiposEstrategia tipo); //retorna se há uma fórmula onde possa ser aplicada uma regra linear ou que bifurca, dependendo do parâmetro passado
        unsigned int estMenMai(tiposEstrategia tipo); //retorna o maior (ou menor) número de ordem no ramo
        int estPeqGrd(tiposEstrategia tipo); //retorna o tamanho da maior (ou menor) fórmula do ramo
        bool estOp(tiposEstrategia tipo); //retorna se há alguma fórmula com o operador pedido no ramo

        // aplica a estratégia desejada no ramo. se a regra aplicada bifurcar, o ramo atual será o novo ramo à esquerda, e o ramo retornado é o à direita. Se a regra aplicada for linear, retorna NULL 
        Ramo* aplicar(list<tiposEstrategia> lista); 

        estadoRamo avaliar(); //retorna se o ramo está aberto (e saturado), fechado ou insaturado
        list<FormulaAtomica> gerarValoracao(list<FormulaAtomica> atomos); //retorna uma valoração contra-exemplo, se o ramo estiver aberto e saturado

     
};

#endif