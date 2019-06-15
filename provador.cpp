#include "provador.h"

using namespace std;

Provador::Provador(list<FormulaMarcada*> ListaFormulas, list<FormulaAtomica> atomos)
        : listaFormulas(ListaFormulas), valoracaoContraExemplo(atomos), proxNumOrdem{listaFormulas.size() + 1} {}

list<Ramo*>::iterator Provador::EncontrarRamo(){

    if (Ramos.size() == 0) return Ramos.end();
    else if (Ramos.size() == 1) return Ramos.begin();

    list<Ramo*> candidatos = Ramos;

    

    //iterador para percorrer a lista - estratégia
    list<tiposEstrategia>::iterator estrategiaAtual = estrategia.begin();

    while (estrategiaAtual != estrategia.end()) {
        
        switch (*estrategiaAtual) {

        case est_esq : //se a estratégia for escolher o candidato mais a esquerda, elimina todos os outros candidatos
            candidatos.erase(++candidatos.begin(), candidatos.end());
            break;

        case est_dir : //idem
            candidatos.erase(candidatos.begin(), --candidatos.end());
            break;


        case est_lin :
        case est_bif :
           EncontrarLinBif(*estrategiaAtual, &candidatos);
           break;

        case est_men :
        case est_mai :
            EncontrarMenMai(*estrategiaAtual, &candidatos);
            break;

        case est_peq :
        case est_grd :
            EncontrarPeqGrd(*estrategiaAtual, &candidatos);
            break;

        case est_nao :
        case est_e   :
        case est_ou  :
        case est_imp :
            EncontrarOp(*estrategiaAtual, &candidatos);
            break;

        default:
            erro = true;
            break;
        }

        //se só há um candidato restante, é nele onde a regra será aplicada
        if (candidatos.size() == 1) {
            
            
            list<Ramo*>::iterator iterador = Ramos.begin();

            //encontra o iterador para o ramo na lista original. se não encontrar, houve erro
            while (!(*candidatos.begin())->comparar(*iterador) && iterador != Ramos.end()) {
                ++iterador;
            }

            if (iterador == Ramos.end()) {
                erro = true;
                cout << "Erro! Ramo aonde a regra seria aplicada não foi encontrado na lista original!\n";
            }
            return iterador;

        }

        ++estrategiaAtual;
    }
    return Ramos.end();
}


void Provador::EncontrarLinBif(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos) {

    //se a estratégia for escolher um ramo onde uma regra linear pode ser aplicada,
    //remove todos os candidatos onde não podem ser aplicadas regras lineares.
    //idem para regras que bifurcam

    //cria uma cópia para poder deletar as fórmulas na cópia sem alterar a lista original
    list<Ramo*> copia = *candidatos;

    list<Ramo*>::iterator iterador = copia.begin();

    while (iterador != copia.end()) {
        if (!(*iterador)->estLinBif(estrategiaAtual)) iterador = copia.erase(iterador);
        else iterador++;
    }

    //se a cópia terminar vazia, a estratégia não conseguiu limitar o número de escolhas, então a lista de candidatos não muda
    if (copia.size() == 0) return;
    //do contrário, atualiza a lista de candidatos
    else *candidatos = copia;

}

void Provador::EncontrarMenMai(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos) {

    //se a estratégia é encontrar o ramo que contenha a fórmula com menor/maior número de ordem
    //cria uma lista com esse número de ordem extremo de cada ramo e descobre o maior/menor número de ordem na lista
    //depois, remove da lista de candidatos todos os ramos que não tiverem o número de ordem extremo batendo com o extremo global


    list<unsigned int> numerosDeOrdemExtremos = list<unsigned int>();
    unsigned int numeroExtremo;

    list<Ramo*>::iterator iterador = candidatos->begin();

    numeroExtremo = (**(iterador++)).estMenMai(estrategiaAtual);
    numerosDeOrdemExtremos.emplace_back(numeroExtremo);

    while (iterador != candidatos->end()) {

        unsigned int extremo = (*(iterador++))->estMenMai(estrategiaAtual);

        //se o extremo do ramo atual for mais extremo do que o global atual, atualiza o global
        if ((extremo > numeroExtremo && estrategiaAtual == est_mai) || (extremo < numeroExtremo && estrategiaAtual == est_men)) numeroExtremo = extremo;

        //adiciona o extremo dessa ramo à lista de extremos
        numerosDeOrdemExtremos.emplace_back(extremo);


    }

    iterador = candidatos->begin();
    list<unsigned int>::iterator iteradorExtremos = numerosDeOrdemExtremos.begin();

    //com a lista de extremos e número extremo global encontrados, remove da lista de candidatos todos aqueles cujo extremo
    //não é igual ao extremo global

    while (iterador != candidatos->end()) {
        if (*(iteradorExtremos++) != numeroExtremo) iterador = candidatos->erase(iterador);
        else iterador++;
    }

}


// void Provador::EncontrarMenMai(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos) {

//     //se a estratégia é encontrar o ramo que contenha a fórmula com menor/maior número de ordem
//     //descobre o ramo com o maior/menor número de ordem na lista e
//     //remove da lista de candidatos todos os outros ramos


//     list<unsigned int> numerosDeOrdemExtremos = list<unsigned int>();
//     unsigned int numeroExtremo;

//     list<Ramo*>::iterator iterador = candidatos->begin();

//     numeroExtremo = (**(iterador++)).estMenMai(estrategiaAtual);
//     list<Ramo*>::iterator ramoExtremo = candidatos->begin();

//     while (iterador != candidatos->end()) {

//         unsigned int extremo = (*(iterador++))->estMenMai(estrategiaAtual);

//         //se o extremo do ramo atual for mais extremo do que o global atual, atualiza o global
//         if ((extremo > numeroExtremo && estrategiaAtual == est_mai) || (extremo < numeroExtremo && estrategiaAtual == est_men)) {
//             numeroExtremo = extremo;
//             ramoExtremo = iterador;
//         }
//     }

//     iterador = candidatos->begin();
//     list<unsigned int>::iterator iteradorExtremos = numerosDeOrdemExtremos.begin();

//     //Remove da lista todos os ramos antes do ramo com o extremo global 
//     ramoExtremo = candidatos->erase(candidatos->begin(), ramoExtremo);
    
//     //E remove todos depois
//     ++ramoExtremo;
//     candidatos->erase(ramoExtremo, candidatos->begin());

// }


void Provador::EncontrarPeqGrd(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos) {
    //essencialmente idêntica a EncontrarMenMai()

    list<int> TamanhoExtremos = list<int>();
    int tamanhoExtremo;

    list<Ramo*>::iterator iterador = candidatos->begin();

    tamanhoExtremo = (*(iterador++))->estPeqGrd(estrategiaAtual);
    TamanhoExtremos.emplace_back(tamanhoExtremo);

    while (iterador != candidatos->end()) {

        int extremo = (*(iterador++))->estPeqGrd(estrategiaAtual);


        if ((extremo > tamanhoExtremo && estrategiaAtual == est_grd) || (extremo < tamanhoExtremo && estrategiaAtual == est_peq)) tamanhoExtremo = extremo;


        TamanhoExtremos.emplace_back(extremo);


    }

    iterador = candidatos->begin();
    list<int>::iterator iteradorExtremos = TamanhoExtremos.begin();

    while (iterador != candidatos->end()) {
        if (*(iteradorExtremos++) != tamanhoExtremo) iterador = candidatos->erase(iterador);
        else iterador++;
    }

}

void Provador::EncontrarOp(tiposEstrategia estrategiaAtual, list<Ramo*>* candidatos) {

    //essencialmente idêntica a EncontrarLinBif()

    list<Ramo*> copia = *candidatos;

    list<Ramo*>::iterator iterador = copia.begin();


    while (iterador != copia.end()) {
        if (!(*iterador)->estOp(estrategiaAtual)) iterador = copia.erase(iterador);
        else iterador++;
    }


    if (copia.size() == 0) return;
    else *candidatos = copia;

}

bool Provador::validarEstrategia(string estrategiaFonte) {

    //essas variáveis controlam se as estratégias de cada tipo já foram encontradas (pois só pode haver uma de cada)
    bool linOuBif = false, esqOuDir = false, menOuMai = false, peqOuGrd = false, Op = false;

    estrategia.clear(); //limpa a lista, para garantir que não haverá problemas caso a função tenha que ser chamada várias vezes

    //itera pela string, construindo a lista de estratégias e retornando um erro caso existam estratégias duplicadas
    for (unsigned int i = 0;  i < estrategiaFonte.length(); ++i) {
        switch (estrategiaFonte[i]) {
        case 'e' :
        case 'd' :
            if (!esqOuDir) {
                estrategia.push_back((estrategiaFonte[i] == 'e') ?  est_esq : est_dir);
                esqOuDir = true;
            } else {
                return false;
            }
            break;

        case 'l' :
        case 'b' :
            if (!linOuBif) {
                estrategia.push_back((estrategiaFonte[i] == 'l') ?  est_lin : est_bif);
                linOuBif = true;
            } else {
                return false;
            }
            break;

        case 'm' :
        case 'M' :
            if (!menOuMai) {
                estrategia.push_back((estrategiaFonte[i] == 'm') ?  est_men : est_mai);
                menOuMai = true;
            } else {
                return false;
            }
            break;

        case 'p' :
        case 'g' :
            if (!peqOuGrd) {
                estrategia.push_back((estrategiaFonte[i] == 'p') ?  est_peq : est_grd);
                peqOuGrd = true;
            } else {
                return false;
            }
            break;

        case '&' :
        case '-' :
        case '|' :
        case '>' :
            if (!Op) {
                tiposEstrategia t;
                switch (estrategiaFonte[i]) {
                    case '&' :
                        t = est_e;
                        break;
                    case '-' :
                        t = est_nao;
                        break;
                    case '|' :
                        t = est_ou;
                        break;
                    case '>' :
                        t = est_imp;
                        break;
                }
                estrategia.push_back(t);
                Op = true;
            } else {
                return false;
            }
            break;
        case ' ' :
            break;
        default:
            return false;

        }

    }
    //adiciona a estratégia padrão
    if (!esqOuDir) estrategia.emplace_back(est_esq);
    if (!menOuMai) estrategia.emplace_back(est_men);
    return true;
}

list<FormulaAtomica> Provador::Resolver(){

    //inicializa a lista adicionando a lista de fórmulas da entrada como primeiro ramo
    Ramos.push_back(new Ramo(listaFormulas));
    bool tabloTerminado = false;

    while (!tabloTerminado) {
        
        for (list<Ramo*>::iterator ramo = Ramos.begin(); ramo != Ramos.end(); ++ramo) cout <<  (**ramo).imprime() << "\n";
        
        list<Ramo*>::iterator ramoEscolhido = EncontrarRamo();
        
        if (erro) break;

        //se não há mais ramos onde alguma regra pode ser aplicada, o tablô está fechado
        if (ramoEscolhido == Ramos.end()) {
            tabloTerminado = true;
            break;
        }


        Ramo* novoRamo = (*ramoEscolhido)->aplicar(estrategia, &proxNumOrdem);
        
        if (erro) break;

        //se o ramo onde a estratégia foi aplicada está aberto e saturado, o sequente é inválido, então uma valoração contra exemplo é criada
        //se o ramo está fechado, o tira da lista
        estadoRamo estadoRamoEscolhido = (*ramoEscolhido)->avaliar();

        if (estadoRamoEscolhido == ramo_aberto) return (*ramoEscolhido)->gerarValoracao(valoracaoContraExemplo);
        if (estadoRamoEscolhido == ramo_fechado) {
            delete *ramoEscolhido;
            ramoEscolhido = Ramos.erase(ramoEscolhido); //guarda o novo iterador caso seja necessário no tratamento do novo ramo
        } else ++ramoEscolhido; //se o ramo for deletado, ramoEscolhido já será o iterador para o próximo ramo, o que é importante
        //para a parte de inserir o novo ramo dar certo. Se não for, é necessário incrementá-lo.
        
        //se um novo ramo foi gerado e ele está aberto e saturado, o sequente é inválido
        //além disso, se ele não estiver fechado, adiciona-o na lista de ramos
        //se estiver fechado, deleta-o
        if (novoRamo != NULL) {

            //incrementa a posição de todos os ramos depois do novo ramo(mesmo se o ramo já começar fechado, para manter a numeração consistente)
            
            list<Ramo*>::iterator i = ramoEscolhido;
            while (i != Ramos.end()) {
                ++(**(i++)).posicao;
            }

            estadoRamo estadoNovoRamo = novoRamo->avaliar();
            if (estadoNovoRamo == ramo_aberto) return novoRamo->gerarValoracao(valoracaoContraExemplo);
            else if (estadoNovoRamo == ramo_insaturado) Ramos.insert(ramoEscolhido, novoRamo);
            else delete novoRamo;
        }

        if (erro) break;

    }

    return list<FormulaAtomica>();
}
