#include "Interpretador.h"

bool Interpretador::adicionaAtomo(string lexema) {

    //itera a lista de átomos já encontrados procurando por algum átomo com o mesmo nome

    list<FormulaAtomica>::iterator iterador = atomos.begin();
    bool novo = true;
    while (iterador != atomos.end()) {
        if(iterador->nome.compare(lexema) == 0) novo = false;
        ++iterador;
    }

    //se não houver, adiciona o novo na lista

    if (novo) atomos.emplace_back(lexema);
    
    return novo ;
}

list<Token> Interpretador::lerTokens() {
    
    //lê tokens até chegar ao final da entrada do usuário
    while(atual < fonte.length()) {
    lerToken();

    }
    //se o programa chega ao final da entrada sem encontrar um "=" (teoricamente impossível), marca que houve um erro
    if (!encontrouConsequencia) {
        erro = true; 
        cout << "nenhum operador de consequencia logica encontrado!" << "\n";
    }
    return listaTokens;
}

void Interpretador::lerToken() {

    char c = fonte[atual];

    //se o digito atual é um caractere de formatação de texto, ignora-o e remove-o da string de entrada
    if (eFormatacao(c)) fonte.erase(atual, 1);
    else {
        
        //o switch abaixo procura tokens que são operadores.
        bool encontrado = false;
        switch (c) {

            case '(': 
                listaTokens.emplace_back(PARENTESES_ESQ, c);
                encontrado = true;
                break;

            case ')': 
                listaTokens.emplace_back(PARENTESES_DIR, c);
                encontrado = true;
                break;

            case '&': 
                listaTokens.emplace_back(E, c);
                encontrado = true;
                break;

            case '|': 
                listaTokens.emplace_back(OU, c);
                encontrado = true;
                break;

            case '>': 
                listaTokens.emplace_back(IMPLICA, c);
                encontrado = true;
                break;

            case '-': 
                listaTokens.emplace_back(NEGACAO, c);
                encontrado = true;
                break;

            case '=': 
                listaTokens.emplace_back(CONSEQUENCIA, c);
                encontrado = true;
                if (!encontrouConsequencia) encontrouConsequencia = true;
                else {
                    erro = true;
                    cout << "mais de um operador de consequencia logica!\n";
                }
                break;

            case ',': 
                listaTokens.emplace_back(VIRGULA, c);
                encontrado = true;
                break;
            }
            
            //se o token lido é um operador, a função já fez seu trabalho, e deve ser chamada novamente
            if (encontrado) atual++;
            else if(eLetra(c)) { //se o próximo caractere não era um operador, é necessário determinar se é um atomo ou se há algum caractere inválido

                unsigned int i = atual + 1; //iterador temporário que será usado para ler o átomo
                bool fimAtomo = false;
                
                while(!fimAtomo && i < fonte.length()) {
                    
                    if(eOperador(fonte[i])) fimAtomo = true; //se i chegou a um operador, o átomo já terminou
                    else {
                        
                        if (eFormatacao(fonte[i])) fonte.erase(i, 1); 
                        else {
                            if (!eLetraOuNumero(fonte[i])) { // se i está em algo que não é nem letra, nem número, nem operador, nem formatação, chegou a um caractere inválido
                                erro = true;
                                cout << "caractere inesperado : " << fonte[i] << "\n";
                            }
                            i++;
                        }
                
                    }  
                   
                }
                
                //adiciona a lista de tokens a parte da entrada que representa o nome do átomo
                listaTokens.emplace_back(ATOMO, fonte.substr(atual, i - atual));
                atual = i;
            } else {
                erro = true;
                cout << "caractere inesperado: " << c << "\n";
                atual++;
            }
    }
}

bool Interpretador::eOperador(char c) {
    return (c == '(') || (c == ')') || (c == '&') || (c == '|') || (c == '>') || (c == '-') || (c == '=') || (c == ',');
}

bool Interpretador::eLetra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Interpretador::eLetraOuNumero(char c) {
    return eLetra(c) || (c >= '0' && c <= '9');
}

bool Interpretador::eFormatacao(char c) {
    return (c == ' ') || (c == '\n') || (c == '\t');
}


list<FormulaMarcada*> Interpretador::Analisar() {

    list<Token>::iterator iterador = listaTokens.begin();
    bool encontrouFinal = false;
    unsigned int numeroDeOrdem = 1;
    while (iterador != listaTokens.end()) {

        //limpa a lista de tokens para começar a fórmula seguinte
        listaAux.clear();

        //armazena os tokens na lista até encontrar ',', '=' ou o fim da entrada, que representam o final das fórmulas
        while (iterador->tipo != VIRGULA && iterador->tipo != CONSEQUENCIA && iterador != listaTokens.end()) {
            listaAux.push_back(*(iterador++));    
        }

        iteradorListaAux = listaAux.begin();

        //cria a fórmula, adiciona seu número de ordem e a valora, colocando-a na lista
        FormulaMarcada* form = formula();
        form->ordem = numeroDeOrdem++;
        form->valorar((encontrouFinal ? F : V)); // se a fórmula está a direita, marca-a como falsa. se é uma das fórmulas da esquerda, a marca como verdadeira.
        listaFormulas.push_back(form);

        //se há mais de uma fórmula após o '=', mostra que há um erro
        if (iterador->tipo == CONSEQUENCIA) {
            if (!encontrouFinal) encontrouFinal = true;
            else {
                erro = true;
                cout << "Erro! so pode haver uma formula apos o operador de sequente." << "\n";
            }
        } 
        if (iterador != listaTokens.end()) ++iterador; 

    }

    return listaFormulas; 
}

FormulaMarcada* Interpretador::formula() {

    //Toda regra "fórmula" leva a chamar a regra "implica"
    return implica();
}

FormulaMarcada* Interpretador::implica(){

    //cria uma lista de fórmulas para armazenar as fórmulas entre os operadores de implicação

    list<FormulaMarcada*> aux = list<FormulaMarcada*>();
    aux.push_front(ou());

    //enquanto houver operadores de implicação, continua armazenando o que há entre eles na lista
    while (iteradorListaAux->tipo == IMPLICA && iteradorListaAux != listaAux.end()) {

        iteradorListaAux++;
        aux.push_front(ou());
    }

    FormulaMarcada* f;

    //se não havia nenhuma implicação, a fórmula a retornar é a própria fórmula que foi gerada
    if (aux.size() == 1) {
        f = *aux.begin();
    } else { // do contrário, gera as implicação seguidas, respeitando o aninhamento à direita
        list<FormulaMarcada*>::iterator iAux = aux.begin();
        
        f = new FormulaBinaria(*(++iAux), IMPLICA, *aux.begin());
        if (aux.size() > 2) {
            iAux++;
            while (iAux != aux.end()) {
                f = new FormulaBinaria(*(iAux++), IMPLICA, f);
            }
        }
    }
    
    return f;
}


FormulaMarcada* Interpretador::ou(){

    //constrói o operando da esquerda
    FormulaMarcada *esq = e();

    //enquanto houver operadores OU, constrói a fórmula, respeitando o aninhamento a esquerda
    while (iteradorListaAux != listaAux.end() && iteradorListaAux->tipo == OU) {

        iteradorListaAux++;
        FormulaMarcada *dir = e();

        esq = new FormulaBinaria(esq, OU, dir);

    }

    return esq;
}

FormulaMarcada* Interpretador::e(){

    //ver explicação da função acima
    FormulaMarcada *esq = unario();

    while (iteradorListaAux != listaAux.end() && iteradorListaAux->tipo == E) {
        
        iteradorListaAux++;
        FormulaMarcada *dir = unario();

        esq = new FormulaBinaria(esq, E, dir);
      

    }

    return esq;
}

FormulaMarcada* Interpretador::unario(){
    //enquanto houverem operadores de negação, constrói a fórmula com as negações seguidas até encontrar uma fórmula primária
    if (iteradorListaAux != listaAux.end() && iteradorListaAux->tipo == NEGACAO) {
        iteradorListaAux++;
        FormulaMarcada *dir = unario();
        return new FormulaUnaria(dir);
    }
    else return primario();
}

FormulaMarcada* Interpretador::primario(){

    
    //se a fórmula primária for um átomo, cria a fórmula para o átomo e adiciona ela à lista de átomos
    if (iteradorListaAux != listaAux.end() && iteradorListaAux->tipo == ATOMO) {
        adicionaAtomo((iteradorListaAux)->lexema);
        return new FormulaAtomica((iteradorListaAux++)->lexema); 
    } 

    //se for outra fórmula com parenteses, constrói a fórmula, mostrando que houve erro se houver algum parenteses sem par
    if (iteradorListaAux != listaAux.end() && iteradorListaAux->tipo == PARENTESES_ESQ) {
        iteradorListaAux++;
        FormulaMarcada *f = formula();
        if ((iteradorListaAux++)->tipo != PARENTESES_DIR) erro = true;
        return f;
    }

    //se nem o parenteses nem o átomo for encontrado, existe algum problema na fórmula (um fecha parenteses antes de qualquer abre parenteses, por exemplo)

    erro = true;
    return NULL;
    
}

Interpretador::~Interpretador() { 

    //itera pela lista de fórmulas e destroi todas
    list<FormulaMarcada*>::iterator formula = listaFormulas.begin();
    
    while (formula != listaFormulas.end()) delete *(formula++);

    listaFormulas.clear();
}