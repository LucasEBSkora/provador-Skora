#include "ramo.h"

using namespace std;

Ramo::Ramo(list<FormulaMarcada*> ListaFormulas) 
    : listaFormulas{ListaFormulas}, 
    expandidas{list<FormulaMarcada*>()},
    proxNumeroOrdem{ListaFormulas.size() + 1},
    posicao{1} {};
        
Ramo::Ramo(Ramo *ramo, FormulaMarcada* formula) 
    : listaFormulas{ramo->listaFormulas}, 
    expandidas{ramo->expandidas}, 
    proxNumeroOrdem{ramo->proxNumeroOrdem + 1}, 
    posicao{ramo->posicao + 1} {
    listaFormulas.push_back(formula); 
}

bool Ramo::adicionaExpandidas(FormulaMarcada* formula) {

    //itera a lista de fórmulas já expandidas procurando por alguma fórmula idêntica

    list<FormulaMarcada*>::iterator iterador = expandidas.begin();
    bool novo = true;

    while (iterador != expandidas.end()) {
        if (formula->eIgual(*iterador) ) {
            novo = false;
            break;
        }
        ++iterador;
    }

    //se não houver, adiciona o novo na lista
    if (novo) {
        expandidas.push_back(formula);
    }
    
    return novo;
}

string Ramo::imprime() {

    string RamoImpresso = " ";
    RamoImpresso = to_string(posicao) + ":\n"; 
    for (list<FormulaMarcada*>::iterator formula = listaFormulas.begin(); formula != listaFormulas.end(); ++formula) {
        RamoImpresso += "\t" + to_string((**formula).ordem) + ": " + (**formula).escreveValorada() + 
        " Tamanho: " + to_string((**formula).tamanho()) + "\n";
            
    }
    for (list<FormulaMarcada*>::iterator formula = expandidas.begin(); formula != expandidas.end(); ++formula) {
        RamoImpresso += "\t\t" + to_string((**formula).ordem) + ": " + (**formula).escreveValorada() + 
        " Tamanho: " + to_string((**formula).tamanho()) + "\n";
            
    }
    return RamoImpresso;
}


bool Ramo::comparar(Ramo *outro){
    return posicao == outro->posicao;
}


bool Ramo::estLinBif(tiposEstrategia tipo){

    list<FormulaMarcada*>::iterator formula = listaFormulas.begin();

    //itera pela lista de fórmulas procurando alguma onde a regra desejada possa ser aplicada
    while (formula != listaFormulas.end()) {


        if (tipo == (**formula).tipoRegra()) return true;

        ++formula;
    }

    return false;
}



unsigned int Ramo::estMenMai(tiposEstrategia tipo){

    //o menor número de ordem em um ramo sempre será o da primeira fórmula
    if (tipo == est_men) return (**listaFormulas.begin()).ordem;
    //e o maior será da última fórmula
    else return (**listaFormulas.rbegin()).ordem;
}

int Ramo::estPeqGrd(tiposEstrategia tipo){

    list<FormulaMarcada*>::iterator formula = listaFormulas.begin();

    //marca o tamanho da primeira fórmula da lista como tamanho extremo inicial
    int extremo = (**(formula++)).tamanho();

    //itera pela lista procurando um extremo "mais extremo" maior ou menor, dependendo da regra desejada
    while (formula != listaFormulas.end()) {

        if (tipo == est_peq && (**(formula++)).tamanho() < extremo) extremo = (**(formula++)).tamanho();
        else if (tipo == est_grd && (**(formula++)).tamanho() > extremo) extremo = (**(formula++)).tamanho();

        ++formula;
    }

    return extremo;
}

bool Ramo::estOp(tiposEstrategia tipo){

    list<FormulaMarcada*>::iterator formula = listaFormulas.begin();
    tiposToken operacao;

    //determina a operação que deve ser procurada
    switch (tipo) {
    case est_e:
        operacao = E;
        break;
    case est_ou:
        operacao = OU;
        break;
    case est_imp:
        operacao = IMPLICA;
        break;
    case est_nao:
        operacao = NEGACAO;
        break;

    default: 
        return false;

    }

    //procura nas fórmulas aquela operação
    while (formula != listaFormulas.end()) {

        if ((*formula)->contemOp(operacao)) return true;

        ++formula;
    }

    return false;
}

void Ramo::encontrarLinBif(tiposEstrategia estrategiaAtual, list<FormulaMarcada*>* candidatos) {

    //cria uma cópia da lista de fórmulas, para poder deletar as fórmulas na nova lista sem problemas
    list<FormulaMarcada*> copia = *candidatos;
    
    list<FormulaMarcada*>::iterator iterador = copia.begin();
    
    //remove da cópia todas as fórmulas onde o tipo de regra desejado não pode ser aplicado
    while (iterador != copia.end()) {
    
        if ((*iterador)->tipoRegra() != estrategiaAtual) iterador = copia.erase(iterador);
        else iterador++;

    }
    
    //se a cópia terminar vazia, nenhuma regra desse tipo pode ser aplicada, então a lista de candidatos não muda,
    //pois a estratégia não conseguiu limitar o número de escolhas
    if (copia.size() == 0) return;
    //se houver alguma fórmula onde o tipo de regra pode ser usado, então atualiza a lista de candidatos
    else *candidatos = copia;

}

void Ramo::encontrarPeqGrd(tiposEstrategia estrategiaAtual, list<FormulaMarcada*>* candidatos) {

    //se a estratégia é encontrar a fórmula com o menor/maior tamanho
    //cria uma lista com o tamanho extremo de cada fórmula e descobre o maior tamanho global
    //depois, remove da lista de candidatos todas as fórmulas que não tiverem o número de ordem extremo batendo com o extremo global


    list<int> numerosDeOrdemExtremos = list<int>();
    int numeroExtremo;

    list<FormulaMarcada*>::iterator iterador = candidatos->begin();

    numeroExtremo = (*(iterador++))->tamanho();
    numerosDeOrdemExtremos.emplace_back(numeroExtremo);

    while (iterador != candidatos->end()) {

        int extremo = (*(iterador++))->tamanho();

        //se o extremo do ramo atual for mais extremo do que o global atual, atualiza o global
        if ((extremo > numeroExtremo && estrategiaAtual == est_mai) || (extremo < numeroExtremo && estrategiaAtual == est_men)) numeroExtremo = extremo;

        //adiciona o extremo dessa ramo à lista de extremos
        numerosDeOrdemExtremos.emplace_back(extremo);


    }

    iterador = candidatos->begin();
    list<int>::iterator iteradorExtremos = numerosDeOrdemExtremos.begin();

    //com a lista de extremos e número extremo global encontrados, remove da lista de candidatos todos aqueles cujo extremo
    //não é igual ao extremo global

    while (iterador != candidatos->end()) {
        if (*(iteradorExtremos++) != numeroExtremo) iterador = candidatos->erase(iterador);
        else iterador++;
    }

}


void Ramo::encontrarOp(tiposEstrategia estrategiaAtual, list<FormulaMarcada*>* candidatos) {

    //cria uma cópia da lista de fórmulas, para poder deletar as fórmulas na nova lista sem problemas
    list<FormulaMarcada*> copia = *candidatos;
    
    list<FormulaMarcada*>::iterator iterador = copia.begin();
    
    tiposToken operacao;

    //determina a operação que deve ser procurada
    switch (estrategiaAtual) {
    case est_e:
        operacao = E;
        break;
    case est_ou:
        operacao = OU;
        break;
    case est_imp:
        operacao = IMPLICA;
        break;
    case est_nao:
        operacao = NEGACAO;
        break;
    default:
        return;
    }

    //remove todas as fórmulas que não possuam aquele operador da lista de candidatos
    
    while (iterador != copia.end()) {

        if (!(**iterador).contemOp(operacao)) iterador = copia.erase(iterador);
        else iterador++;
    }

    
    //se a cópia terminar vazia, nenhuma fórmula contém esse operador, então a lista de candidatos não muda,
    //pois a estratégia não conseguiu limitar o número de escolhas
    if (copia.size() == 0) return;
    //se houver alguma fórmula onde o tipo de regra pode ser usado, então atualiza a lista de candidatos
    else *candidatos = copia;


}


Ramo* Ramo::aplicar(list<tiposEstrategia> lista){
    //assim como é necessário descobrir em qual ramo se deve aplicar a próxima regra, é preciso descobrir em qual fórmula daquele ramo
    //a regra será aplicada. Isso pode ser feito do mesmo jeito: iterando pela lista de estratégias até só haver um candidato.
    //Porém, nesse caso, todas as fórmulas atômicas são removidos da lista de candidatos, já que nenhuma regra pode ser aplicada nelas

    list<FormulaMarcada*> candidatos = listaFormulas;

    list<FormulaMarcada*>::iterator iterador = candidatos.begin();

    while (iterador != candidatos.end()) {
        if ((**iterador).tipo == atomica) {
            iterador = candidatos.erase(iterador);
        } else ++iterador;
    }

    //iterador para percorrer a lista - estratégia
    list<tiposEstrategia>::iterator estrategiaAtual = lista.begin();

    FormulaMarcada* formulaEscolhida;

    while (estrategiaAtual != lista.end()) {

        iterador = candidatos.begin();
        switch (*estrategiaAtual) {

        //estratégias que envolvam usar o ramo de alguma extremidade não são relevantes
        //em relação a escolher uma fórmula dentro de um ramo
        case est_esq :
        case est_dir :
            break;


        case est_lin :
        case est_bif :
            encontrarLinBif(*estrategiaAtual, &candidatos);
            break;

        case est_men : //se a estratégia for escolher o candidato com o maior número de ordem, elimina todos os outros candidatos
            candidatos.erase(++candidatos.begin(), candidatos.end());
            break;

        case est_mai : //idem
            candidatos.erase(candidatos.begin(), --candidatos.end());
            break;

        case est_peq :
        case est_grd :
            encontrarPeqGrd(*estrategiaAtual, &candidatos);
            break;

        case est_nao :
        case est_e   :
        case est_ou  :
        case est_imp :
            encontrarOp(*estrategiaAtual, &candidatos);
            break;

        default:
            break;
        }

        //se não há candidatos, nenhuma regra é aplicada
        if (candidatos.size() == 0) return NULL;
        //se só há um candidato restante, é nele onde a regra será aplicada
        if (candidatos.size() == 1) {

            //com a fórmula onde a regra será aplicada encontrada (*candidatos.begin()),
            //só é preciso realmente aplicar a regra.
    
            formulaEscolhida = *candidatos.begin();

            //Remove todas as aparições daquela fórmula naquele ramo
            list<FormulaMarcada*>::iterator iterador = listaFormulas.begin();

            //encontra o iterador para a fórmula na lista original.
            while (iterador != listaFormulas.end()) {
                
                //se a fórmula é escrita do mesmo jeito e tem o mesmo valor (é igual),
                //remove-a da lista, para que uma regra não seja aplicada na mesma fórmula várias vezes no mesmo ramo
                
                if (formulaEscolhida->eIgual(*iterador)) {
                        iterador = listaFormulas.erase(iterador);
                }
                ++iterador;
            }
            break;
        }
        ++estrategiaAtual;
    }

    if (formulaEscolhida) {

        adicionaExpandidas(formulaEscolhida);

        cout << "\tRegra " <<  formulaEscolhida->nomeRegra()
        << " aplicada na formula \"" << formulaEscolhida->escreve() << "\" no ramo " << posicao << ": \n";


        //armazena as novas fórmulas que serão geradas com a aplicação da regra
        FormulaMarcada* novasFormulas[2];

        formulaEscolhida->expandir(novasFormulas);

        //se a regra for linear, guarda as novas fórmulas no mesmo ramo:

        if (formulaEscolhida->tipoRegra() == est_lin) {

            cout << "\t\tNova(s) formula(s) adicionada(s) ao mesmo ramo:\n";
            if (novasFormulas[0]) {
                novasFormulas[0]->ordem = proxNumeroOrdem++; //adiciona o número de ordem da fórmula
                cout << "\t\t\t\"" << novasFormulas[0]->escreveValorada() << "\"\n";
                listaFormulas.push_back(novasFormulas[0]);
            }

            if (novasFormulas[1]) {
                novasFormulas[1]->ordem = proxNumeroOrdem++; //adiciona o número de ordem da fórmula
                cout << "\t\t\t\"" << novasFormulas[1]->escreveValorada() << "\"\n";
                listaFormulas.push_back(novasFormulas[1]);
            }
            return NULL;

        } else if (formulaEscolhida->tipoRegra() == est_bif) { //se a regra bifurcar, cria os novos ramos

            cout << "\t\tNovas formulas adicionadas aos novos ramos: \n";
            if (novasFormulas[0]) cout << "\t\t\t\"" << novasFormulas[0]->escreveValorada() << "\"\n";
            if (novasFormulas[1]) cout << "\t\t\t\"" << novasFormulas[1]->escreveValorada() << "\"\n";

            Ramo* novoRamo = NULL;

            //cria o novo ramo adicionando a fórmula da direita, e adiciona a fórmula da esquerda ao ramo antigo

            if (novasFormulas[1]) {
                novasFormulas[1]->ordem = proxNumeroOrdem; //adiciona o número de ordem da fórmula  (sem incrementar pois essa fórmula irá para outro ramo)

                novoRamo = new Ramo(this, novasFormulas[1]);
            }
            if (novasFormulas[0]) {
                novasFormulas[0]->ordem = proxNumeroOrdem++; //adiciona o número de ordem da fórmula
                listaFormulas.push_back(novasFormulas[0]);
            }

            return novoRamo;

        }
    
    }

    return NULL;

}

estadoRamo Ramo::avaliar(){

    //se existe no ramo alguma fórmula marcada simultaneamente como V e como F, o ramo está fechado:
    //como duas fórmulas iguais podem surgir de fórmulas diferentes, é preciso checar se todos os operadores e átomos são idênticos
    //para garantir que as fórmulas são iguais. Entretanto, isso pode ser feito comparando a maneira com a qual as duas fórmulas são escritas.
    
    //mapa de cada fórmula escrita e de seu valor
    unordered_map<string, Valor> paresFormulaValor = unordered_map<string, Valor>();

    //adiciona as fórmulas já expandidas ao mapa 

    for (list<FormulaMarcada*>::iterator formula = expandidas.begin(); formula != expandidas.end(); ++formula) {

        paresFormulaValor.insert({(**formula).escreve(), (**formula).valor});

    }

    for (list<FormulaMarcada*>::iterator formula = listaFormulas.begin(); formula != listaFormulas.end(); ++formula) {

        //recupera a fórmula escrita
        string formulaEscrita = (**formula).escreve();

        //procura ela no mapa
        unordered_map<string, Valor>::iterator par = paresFormulaValor.find(formulaEscrita);

        //se não encontrar, adiciona o par no mapa
        if (par == paresFormulaValor.end()) paresFormulaValor.insert({formulaEscrita, (**formula).valor});

        //se encontrar com um valor diferente, o ramo está fechado (contradição encontrada)
        else if (par->second != (**formula).valor) {
            cout << "\tRamo " << posicao << " fechado: Formula \"" << formulaEscrita << "\" com valoracao contraditoria!\n";;
            return ramo_fechado;
        }

    }

    
    //se nenhuma regra pode ser aplicada, o ramo está aberto
    bool algumaRegra = false;

    for (list<FormulaMarcada*>::iterator formula = listaFormulas.begin(); formula != listaFormulas.end(); ++formula) {
        //descobre se uma regra linear ou que bifurca (ou seja, qualquer regra) pode ser aplicada na fórmula
        if ( (**formula).tipoRegra() != est_inv) {
            algumaRegra = true;
            break;
        }
    }

    if (!algumaRegra) {
        cout << "\tRamo " << posicao << " aberto: nenhuma regra pode ser aplicada!\n";
        return ramo_aberto;
    }

    //se não estiver aberto e saturado nem fechado, o ramo está aberto e insaturado
    return ramo_insaturado;
}

list<FormulaAtomica> Ramo::gerarValoracao(list<FormulaAtomica> atomos){

    

    //encontra na lista de todos os atomos do sequente aqueles que foram valorados nesse ramo, e passam a valoração
    //do ramo para eles. assim, os atomos que não foram valorados ficarão com valor indefinido.

    for (list<FormulaMarcada*>::iterator formula = listaFormulas.begin(); formula != listaFormulas.end(); ++formula) {

        if ((**formula).tipo == atomica) {

            for (list<FormulaAtomica>::iterator atomo = atomos.begin(); atomo != atomos.end(); ++atomo) {

                if ((**formula).escreve().compare(atomo->escreve()) == 0) {
                    atomo->valor = (**formula).valor;
                }
            }
        }
        
    }
    return atomos;
}
