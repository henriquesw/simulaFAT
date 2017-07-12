/*-------------------------------------------------
 * Simulador de FAT - File Allocation Table
 * Luiz Eduardo da Silva
 *-------------------------------------------------*/
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

void gravar(memoria Memo, ptnoSet *Area, ptnoArq *Arq, char nome[13], char texto[TAM_MEMORIA * TAM_GRANULO]) {
    
    ptnoArq auxArq = malloc(sizeof (noArq));
    ptnoSet auxSet = malloc(sizeof (noSet));
    ptnoSet percorre = auxSet;
    
    strcpy(auxArq->nome, nome);
    auxArq->caracteres = strlen(texto);
    auxArq->prox = NULL;
    auxArq->setores = percorre;
    
    int cont1 = 0;
    int i;
    
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
        
        } 
        /*O else percorre a lista de espaços vazios e adiciona
         * elemento na lista de setores*/
        else {
            
            percorre = percorre->prox;
            percorre = auxSet;
            
            auxSet = malloc(sizeof (noSet));
            
            ptnoSet auxArea = (*Area)->prox;
            free(Area);
            (*Area) = auxArea;
            
        }
        
    }
    
    /*Verifica memória cheia*/
    if (cont1 < auxArq->caracteres) {
        printf("A memória esta cheia!");
    }
    
    /*Adiciona final da seleção*/
    percorre = auxSet;
    percorre->prox = NULL;
    
    if (!(*Arq)) { /* ADICIONA SE A LISTA ESTIVER VAZIA */
        *Arq = auxArq;
    } else /*Adiciona se tiver outro elemento*/{
        ptnoArq percorreArquivo = *Arq;
        while (percorreArquivo->prox) {
            percorreArquivo = percorreArquivo->prox;
        }
        percorreArquivo->prox = auxArq;
    }
    
}

/*---------------------------------------------
 * Implementar as rotinas para simulacao da FAT
 *---------------------------------------------*/

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
                gravar(Memo, &Area, &Arq, nome, texto);
                /*
                 * Implementar as chamadas das funcoes pra GRAVAR arquivo
                 */
                break;
            case 'D':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                /*
                 * Implementar as chamadas das funcoes pra DELETAR arquivo
                 */
                break;
            case 'A':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                /*
                 * Implementar as chamadas das funcoes pra APRESENTAR arquivo
                 */
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
