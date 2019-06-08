#include "formula.h"

bool FormulaMarcada::eIgual(FormulaMarcada *outra) {
    return (escreve().compare(outra->escreve()) == 0) && (valor == outra->valor);
}

string FormulaMarcada::escreveValorada() {
    return (valor == V ? "V: " : (valor == F ? "F: " : "IND: ")) + escreve();
}

FormulaMarcada::~FormulaMarcada() {
    if (esquerda) delete esquerda;
    if (direita) delete direita;
}

FormulaAtomica::FormulaAtomica(string Nome) {
            nome = Nome;
            tipo = atomica;
}

int FormulaAtomica::tamanho() {
    return 1;
}

string FormulaAtomica::escreve() {
    return nome;

}

void FormulaAtomica::valorar(Valor v){
    valor = v;
}

tiposEstrategia FormulaAtomica::tipoRegra() {
    //não dá para expandir uma fórmula atômica
    return est_inv;
}


void FormulaAtomica::expandir(FormulaMarcada** resultado) {
    //não dá para expandir uma fórmula atômica
    resultado[0] = this;
    resultado[1] = NULL;
}

bool FormulaAtomica::contemOp(tiposToken op){
    op = op;
    return false;
}

FormulaBinaria::FormulaBinaria(FormulaMarcada *Esquerda, tiposToken op, FormulaMarcada *Direita){
    tipo = binaria;
    esquerda = Esquerda;

    switch (op) {
        case E :
            operador = Token(op, '&');
            break;
        case OU :
            operador = Token(op, '|');
            break;
        case IMPLICA :
            operador = Token(op, '>');
            break;
        default :
            break;
    }

    direita = Direita;
}

int FormulaBinaria::tamanho() {
    return esquerda->tamanho() + 1 + direita->tamanho();
}

string FormulaBinaria::escreve() {
    return "(" + esquerda->escreve() + operador.lexema + direita->escreve() + ")";
}

void FormulaBinaria::valorar(Valor v){

    //essencialmente, a única coisa que muda nas novas fórmulas adicionadas ao ramo (no que diz respeito a regra aplicada)
    //é a relação entre a valoração da fórmula antiga com a fórmula da esquerda (pois a da direita sempre receberá o mesmo valor que a fórmula que a continha tinha)

    valor = v;

    if (operador.tipo == E || operador.tipo == OU) //T&, F&, T| e F|: esquerda e direita recebem o mesmo valor que o da fórmula atual
        esquerda->valor = valor;
    else //F> e T> : esquerda com valor oposto, direita com mesmo valor
        esquerda->valor = ((valor == V) ? F : V);

    direita->valor = valor;

}

void FormulaBinaria::expandir(FormulaMarcada** resultado) {
    resultado[0] = esquerda;
    resultado[1] = direita;
}

tiposEstrategia FormulaBinaria::tipoRegra() {


    if (operador.tipo == E) { //T& linear, F& bifurca
        if (valor == V) return est_lin;
        else return est_bif;
    }
    else if (operador.tipo == OU) { //T| bifurca, F| linear
        if (valor == V) return est_bif;
        else return est_lin;
    }
    else { //T> bifurca, F> linear
        if (valor == V) return est_bif;
        else return est_lin;
    }
}

bool FormulaBinaria::contemOp(tiposToken op){

    //verifica se o operador da fórmula é o desejado
    //se não for, verifica se os operadores dos operandos são os desejados.

    if (operador.tipo == op) return true;
    else if (esquerda->contemOp(op)) return true;
    else if (esquerda->contemOp(op)) return true;
    else return false;

}


FormulaUnaria::FormulaUnaria(FormulaMarcada *Esquerda) {
    tipo = unaria;
    operador = Token(NEGACAO, '-');
    esquerda = Esquerda;
}

int FormulaUnaria::tamanho() {
    return esquerda->tamanho() + 1;
}

string FormulaUnaria::escreve(){

    return operador.lexema + esquerda->escreve();

}

void FormulaUnaria::valorar(Valor v){

    //independente da regra ser T- ou F-, o que acontece é que o operando é expandido com o valor inverso do valor da atual
    valor = v;

    esquerda->valor = ((v == V) ? F : V);

}

tiposEstrategia FormulaUnaria::tipoRegra() {

    //fórmulas com o operador unário sempre são expandidas com regras lineares: T- e F-
    return est_lin;
}

void FormulaUnaria::expandir(FormulaMarcada** resultado) {
    resultado[0] = esquerda;
    resultado[1] = NULL;
}

bool FormulaUnaria::contemOp(tiposToken op){

    return operador.tipo == op;
}
