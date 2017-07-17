/*+-------------------------------------------------------------+
 *|         UNIFAL – Universidade Federal de Alfenas.           |
 *| BACHARELADO EM CIENCIA DA COMPUTACAO.                       |
 *| Trabalho..: SIMULACAO DE SISTEMA DE ARQUIVOS FAT            |
 *| Disciplina: Estrutura de Dados I                            |
 *| Professor.: Luiz Eduardo da Silva                           |
 *| Aluno(s)..: Henrique dos Santos Wisniewski                  |
 *|             Vinicius Henrique dos Santos                    |
 *| Data......: 17/07/2017                                      |
 *+-------------------------------------------------------------+*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define TAM_GRANULO 3
#define TAM_MEMORIA 30
#define TRUE 1
#define FALSE 0

typedef struct noSet * ptnoSet;

typedef struct noSet {
    int inicio, fim;
    ptnoSet prox;
} noSet;

typedef struct noArq *ptnoArq;

typedef struct noArq {
    char nome[13];
    int caracteres;
    ptnoSet setores;
    ptnoArq prox;
} noArq;

typedef char memoria[TAM_MEMORIA][TAM_GRANULO];

void mostraSetores(ptnoSet S, char *n) {
    printf("%s = [", n);
    while (S) {
        printf("(%d,%d)", S->inicio, S->fim);
        S = S->prox;
        if (S) printf(",");
    }
    printf("]\n");
}

void mostraArquivos(ptnoArq A) {
    printf("Arquivos:\n");
    while (A) {
        printf("  %12s, %2d caracter(es).  ", A->nome, A->caracteres);
        mostraSetores(A->setores, "Setores");
        A = A->prox;
    }
    printf("\n");
}

void mostraMemoria(memoria Memo) {
    int i, j;
    for (i = 0; i < TAM_MEMORIA; i++) {
        printf("%3d:[", i);
        for (j = 0; j < TAM_GRANULO - 1; j++)
            printf("%c,", Memo[i][j]);
        printf("%c]", Memo[i][TAM_GRANULO - 1]);
        if ((i + 1) % 10 == 0)
            printf("\n");
    }
}

void inicia(ptnoSet *Area, ptnoArq *Arq, memoria Memo) {
    int i, j;
    *Area = (ptnoSet) malloc(sizeof (noSet));
    (*Area)->inicio = 0;
    (*Area)->fim = TAM_MEMORIA - 1;
    (*Area)->prox = NULL;
    *Arq = NULL;
    for (i = 0; i < TAM_MEMORIA; i++)
        for (j = 0; j < TAM_GRANULO; j++)
            Memo[i][j] = ' ';
}

/* Essa função é responsavel por gravar arquivos distribuindo 
 * entre os espaços vazios da memória */

int espacoLivre(ptnoSet Area) {
    int total = 0;
    while (Area) {
        total = total + (Area->fim - Area->inicio) + 1;
        Area = Area->prox;
    }
    return total;
}

void gravar(memoria Memo, ptnoSet *Area, ptnoArq *Arq, char nome[13], char texto[TAM_MEMORIA * TAM_GRANULO]) {
    
    ptnoArq auxArq = malloc(sizeof (noArq));
    ptnoArq percorreArquivo = *Arq;
    ptnoSet auxSet = malloc(sizeof (noSet));
    ptnoSet percorre = auxSet;
    
    strcpy(auxArq->nome, nome);
    auxArq->caracteres = strlen(texto);
    auxArq->prox = NULL;
    auxArq->setores = auxSet;
    
    int cont1 = 0;
    int i;
    int setoresNecessarios = (int) (auxArq->caracteres / TAM_GRANULO);
    /*Arredonda total de espacos*/
    if ((auxArq->caracteres % TAM_GRANULO) != 0)
    	setoresNecessarios++;
    
    /*procura arquivo na lista*/
    while (percorreArquivo && strcmp(percorreArquivo->nome, nome))
        percorreArquivo = percorreArquivo->prox;
    
    if (setoresNecessarios > espacoLivre(*Area)){
        printf("Memória insuficiente\n");
    } else if (percorreArquivo) {
        printf("Já possui arquivo com esse nome\n");
    } else {
        /*O while verifica se tem espaço disponível na memória ou se 
         * já escreveu todo o arquivo*/
        while(Area && cont1 < auxArq->caracteres) {

            /*O if adiciona as informações no granulos da memória*/
            if((*Area)->inicio <= (*Area)->fim) {

                auxSet->inicio = (*Area)->inicio;
                auxSet->fim = (*Area)->inicio - 1;

                while((*Area)->inicio <= (*Area)->fim && cont1 < auxArq->caracteres) {
                    for(i = 0; i < TAM_GRANULO && cont1 < auxArq->caracteres; i++){
                        Memo[(*Area)->inicio][i] = texto[cont1];
                        cont1++;
                    }

                    (*Area)->inicio++;
                    auxSet->fim++;
                }

            } else {
                /*O else percorre a lista de espaços vazios e adiciona elemento na lista de setores*/
                
                auxSet = malloc(sizeof (noSet));
                percorre->prox = auxSet;
                percorre = percorre->prox;

                ptnoSet auxArea = (*Area)->prox;
                free((*Area));
                (*Area) = auxArea;

            }
        }
        
        if ((*Area)->inicio > (*Area)->fim) {
            ptnoSet auxArea = (*Area)->prox;
            free((*Area));
            (*Area) = auxArea;
        }

        /*Adiciona final da seleção*/
        percorre->prox = auxSet;
        percorre = percorre->prox;
        percorre->prox = NULL;

        if (!(*Arq)) { /* ADICIONA SE A LISTA ESTIVER VAZIA */
            *Arq = auxArq;
        } else { /*Adiciona se tiver outro elemento*/
            percorreArquivo = *Arq;
            while (percorreArquivo->prox)
                percorreArquivo = percorreArquivo->prox;
            percorreArquivo->prox = auxArq;
        }
    }
    
}

void apresentar (memoria Memo, ptnoArq Arq, char nome[13]) {
    int i, j, posicao = 0;
    /*Procura o arquivo na lista*/
    while(Arq && strcmp(Arq->nome, nome))
        Arq = Arq->prox;
    
    if (!Arq) {
        printf("Não existe esse arquivo\n");
    } else {
    	/*Percorre toda a lista de setores*/
        ptnoSet auxSet = Arq->setores;
        while (auxSet) {
        	/*Le a memoria e imprime na tela*/
            for (i = auxSet->inicio; i <= auxSet->fim; i++) {
                for (j = 0; j < TAM_GRANULO && posicao < Arq->caracteres; j++) {
                    printf("%c", Memo[i][j]);
                    posicao++;
                }
            }
            auxSet = auxSet->prox;
        }
        printf("\n");
    }
    
}

void organizaArea (ptnoSet Area) {
	/*Responsavel por unir setores na area livre, exempro inicio 4 e fim 5 proxima area inicio 6 fim 7, ira ficar um setor de inicio 4 e fim 7*/
    ptnoSet aux;
    while (Area) {
        aux = Area->prox;
        if (aux) {
            if ((Area->fim + 1) == aux->inicio) {
                Area->fim = aux->fim;
                Area->prox = aux->prox;
                free(aux);
            }
        }
        Area = Area->prox;
    }
    
}

void deletar (memoria Memo, ptnoArq *Arq, ptnoSet *Area, char nome[13]) {
	
    int i, j, posicao = 0;
    ptnoArq anteriorArq = NULL, auxArq = (*Arq);
    ptnoSet anteriorArea = NULL, auxArea = (*Area);
    /*procura o arquivo na lista*/
    while(auxArq && strcmp(auxArq->nome, nome)){
        anteriorArq = auxArq;
        auxArq = auxArq->prox;
    }
    
    if (!auxArq) {
        printf("Não existe esse arquivo\n");
    } else {
        /*Percorre os setores do arquivo*/
        ptnoSet auxSet = auxArq->setores;
        while (auxSet) {
        	/*Limpa a memoria para melhor visualizacao*/
            for (i = auxSet->inicio; i <= auxSet->fim; i++) {
                for (j = 0; j < TAM_GRANULO && posicao <= auxArq->caracteres; j++) {
                    Memo[i][j] = ' ';
                    posicao++;
                }
	    }
	    /* Passa a area que era do arquivo para a memoria, assim n precisa desalocar*/
            auxArq->setores = auxSet->prox;
            while (auxArea && auxSet->fim > auxArea->inicio) {
                anteriorArea = auxArea;
                auxArea = auxArea->prox;
            }
            
            if (!anteriorArea)
                (*Area) = auxSet;
            else
                anteriorArea->prox = auxSet;
            
            if (!auxArea)
                auxSet->prox = NULL;
            else
                auxSet->prox = auxArea;

            auxArea = (*Area);
            auxSet = auxArq->setores;
        }
        /*desaloca o arquivo da memoria*/
        if (!anteriorArq)
            (*Arq) = (*Arq)->prox;
        else if(auxArq->prox)
            anteriorArq->prox = auxArq->prox;
        else
            anteriorArq->prox = NULL;

        auxArq = NULL;
        free(auxArq);
    }
}

void ajuda() {
    printf("\nCOMANDOS\n");
    printf("--------\n");
    printf("G <arquivo.txt> <texto><ENTER>\n");
    printf(" -Grava o <arquivo.txt> e conteúdo <texto> no disco\n");
    printf("D <arquivo.txt>\n");
    printf(" -Deleta o <arquivo.txt> do disco\n");
    printf("A <arquivo.txt>\n");
    printf(" -Apresenta o conteudo do <arquivo.txt>\n");
    printf("M\n");
    printf(" -Mostra as estruturas utilizadas\n");
    printf("H\n");
    printf(" -Apresenta essa lista de comandos\n");
    printf("F\n");
    printf(" -Fim da simulacao\n");
}

/*------------------------------------------
 * CORPO PRINCIPAL DO PROGRAMA
 *------------------------------------------*/
int main(void) {
    ptnoSet Area, set;
    ptnoArq Arq, ant;
    memoria Memo;
    char com[3];
    char nome[13];
    char texto[TAM_MEMORIA * TAM_GRANULO];

    inicia(&Area, &Arq, Memo);

    do {
        printf("\n=> ");
        scanf("%3s", com);
        com[0] = toupper(com[0]);
        switch (com[0]) {
            case 'G':
                scanf("%s %s", nome, texto);
                printf("nome = %s\n", nome);
                printf("texto = %s\n", texto);
                //funçao gravar
                gravar(Memo, &Area, &Arq, nome, texto);
                break;
            case 'D':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                //função deletar e para organizar memoria
                deletar(Memo, &Arq, &Area, nome);
                organizaArea(Area);
                break;
            case 'A':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                //função apresentar
                apresentar(Memo, Arq, nome);
                break;
            case 'M':
                mostraSetores(Area, "Area");
                mostraArquivos(Arq);
                printf("Memoria:\n");
                mostraMemoria(Memo);
                break;
            case 'H':
                ajuda();
                break;
        }
    } while (com[0] != 'F');
    printf("\nFim da Execucao\n\n");
    return (EXIT_SUCCESS);
}
