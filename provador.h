#ifndef PROVADOR
#define PROVADOR

#include "ramo.h"

class Provador {
    private:
    
        list<tiposEstrategia> estrategia = list<tiposEstrategia>(); //lista que representa a estratégia usada 
        list<FormulaMarcada*> listaFormulas; //lista de entrada
        list<FormulaAtomica> valoracaoContraExemplo; 
        list<Ramo*> Ramos = list<Ramo*>();
        list<Ramo*>::iterator EncontrarRamo(); //procura o ramo onde a regra deve ser aplicada. Se nenhum for encontrado, retorna o fim da lista de ramos
        
        //remove da lista de candidatos usada em EncontrarRamo() aqueles que são invalidados pelo argumento passado 
        void EncontrarLinBif(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos);
        void EncontrarMenMai(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos);
        void EncontrarPeqGrd(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos);
        void EncontrarOp(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos);
        
        bool erro = false;

    public:

        Provador(list<FormulaMarcada*> ListaFormulas, list<FormulaAtomica> atomos)
        : listaFormulas(ListaFormulas), valoracaoContraExemplo(atomos) {}
        //processa a string de entrada da estratégia e retorna se ela é valida
        bool validarEstrategia(string Estrategia); 
        
        //resolve o tablô, retornando uma valoração contra-exemplo se o sequente não for válido, e uma lista vazia se for
        list<FormulaAtomica> Resolver(); 
        

};

#endif