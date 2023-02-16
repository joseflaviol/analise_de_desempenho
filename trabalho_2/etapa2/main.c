#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

typedef struct {
    double tempo_final;
} Chamada;

typedef struct {
    int tipo; // 0 - chegada, 1 - saida, 2 - tempo coleta
    Chamada *chamada; // aponta para uma chamada, caso seja um evento relacionada a ela, aponta para NULL caso contrario
    bool eh_pacote;
    double tempo; 
} Evento;

Evento *novo_evento(int tipo, Chamada *chamada, double tempo) {
    Evento *e = (Evento *) malloc(sizeof(Evento));

    e->tipo = tipo;
    e->chamada = chamada;
    e->tempo = tempo;

    return e;
}

typedef struct _minHeap {
    int tamanho;
    int capacidade;
    Evento **heap;
} MinHeap;

MinHeap *inicia_min_heap(int capacidade) {
    MinHeap *minHeap = (MinHeap *) malloc(sizeof(MinHeap));

    minHeap->capacidade = capacidade;
    minHeap->heap = (Evento **) malloc(sizeof(Evento *) * capacidade);
    minHeap->tamanho = 0;

    return minHeap;
}

int pai(int i) {
    return (i - 1) / 2;
}

int esquerda(int i) {
    return (2 * i) + 1;
}

int direita(int i) {
    return (2 * i) + 2;
}

int insere(MinHeap *minHeap, Evento *e) {
    if (minHeap->tamanho == minHeap->capacidade) {
        return -1;
    }

    int i;
    Evento *aux;

    minHeap->tamanho += 1;
    i = minHeap->tamanho - 1;
    minHeap->heap[i] = e;

    while (i != 0 && minHeap->heap[pai(i)]->tempo > minHeap->heap[i]->tempo) {
        aux = minHeap->heap[pai(i)];
        minHeap->heap[pai(i)] = minHeap->heap[i];
        minHeap->heap[i] = aux;

        i = pai(i);    
    }

    return 1;
}

Evento *extrai_minimo(MinHeap *minHeap) {
    if (minHeap->tamanho == 0) {
        return NULL; // underflow
    }

    if (minHeap->tamanho == 1) {
        minHeap->tamanho = 0;
        return minHeap->heap[0];
    }

    Evento *aux;
    Evento *retorno = minHeap->heap[0];
    int i, menor;

    i = 0;

    minHeap->heap[i] = minHeap->heap[minHeap->tamanho - 1];
    minHeap->tamanho -= 1;

    while (1) {
        menor = i;

        if (esquerda(i) < minHeap->tamanho && minHeap->heap[esquerda(i)]->tempo < minHeap->heap[i]->tempo) {
            menor = esquerda(i);
        }

        if (direita(i) < minHeap->tamanho && minHeap->heap[direita(i)]->tempo < minHeap->heap[menor]->tempo) {
            menor = direita(i);
        }

        if (menor == i) {
            break;
        }

        aux = minHeap->heap[menor];
        minHeap->heap[menor] = minHeap->heap[i];
        minHeap->heap[i] = aux;

        i = menor;
    }

    return retorno;
}

typedef struct little_{
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
} little;

double aleatorio() {
	double u = rand() / ((double) RAND_MAX + 1);
	//limitando entre (0,1]
	u = 1.0 - u;

	return (u);
}

double minimo(double num1, double num2){
    if(num1 < num2){
        return num1;
    }
    return num2;
}

double maximo(double num1, double num2){
    if(num1 > num2){
        return num1;
    }
    return num2;
}

void inicia_little(little * l){
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

int main(){

    FILE *arq_ocupacao;
    FILE *arq_ew;
    FILE *arq_en;
    FILE *arq_erro_little;

    double e_n_final;
    double e_w_final;
    double lambda;

    int tempo_coleta;
    double tempo_simulacao;
    double tempo_decorrido = 0.0;

    double intervalo_medio_chamada;
    double tempo_medio_duracao_chamada;
    double intervalo_medio_chegada;
    double tempo_medio_servico;
    double largura_link;

    double chegada;
    double servico;
    double chamada;

    double soma_tempo_servico = 0.0;

    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    /**
    Little
    */
    little e_n;
    little e_w_chegada;
    little e_w_saida;

    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);
    /**
    Little -- fim
    */

    //srand(time(NULL));
    srand(10000);

    printf("Informe o tempo de simulacao (segundos): ");
    scanf("%lF",&tempo_simulacao);

    printf("Informe o intervalo medio entre chegadas (segundos): ");
    scanf("%lF",&intervalo_medio_chegada);

    printf("Informe o intervalo medio entre inicio de chamadas (segundos): ");
    scanf("%lF",&intervalo_medio_chamada);

    printf("Informe o tempo medio de duracao da chamada (segundos): ");
    scanf("%lF",&tempo_medio_duracao_chamada);    

    /*printf("Informe o tempo medio de servico (segundos): ");
    scanf("%lF",&tempo_medio_servico);
    */

    printf("Informe a largura do link: ");
    scanf("%lf", &largura_link);

    // inicia arquivos
    arq_ocupacao = fopen("ocupacao.txt", "w");
    arq_en = fopen("en.txt", "w");
    arq_ew = fopen("ew.txt", "w");
    arq_erro_little = fopen("erro_little.txt", "w");
    int id = 1;
    MinHeap *h = inicia_min_heap(1000);

    insere(h, novo_evento(2, NULL, 100));

    //gerando o tempo de chegada da primeira requisicao.
    chegada = (-1.0 / (1.0/intervalo_medio_chegada)) * log(aleatorio());
    insere(h, novo_evento(0, NULL, chegada));
    
    // gerando o tempo de inicio da primeira chamada
    chamada = (-1.0 / (1.0/intervalo_medio_chamada)) * log(aleatorio());
    insere(h, novo_evento(3, NULL, chamada));

    while(tempo_decorrido <= tempo_simulacao){
        Evento *e = extrai_minimo(h);
        tempo_decorrido = e->tempo;
        //tempo_decorrido = !fila ? minimo(chegada, tempo_coleta) : minimo(tempo_coleta, minimo(chegada, servico));
        
        // inicio chamada
        if (e->tipo == 3) {
            Chamada *c = (Chamada *) malloc(sizeof(Chamada));

            c->tempo_final = tempo_decorrido + (-1.0 / (1.0 / tempo_medio_duracao_chamada)) * log(aleatorio());
            // fim da chamada
            insere(h, novo_evento( 6, c,  c->tempo_final));

            // gera primeiro pacote
            insere(h, novo_evento( 4, c, tempo_decorrido + 0.02 ));
        } else if (e->tipo == 4) {

            // envio do proximo pacote
            if (e->chamada->tempo_final <= tempo_decorrido + 0.02) {
                insere(h, novo_evento(4, e->chamada, tempo_decorrido + 0.02));    
            }

            // termino do envio do pacote
            insere(h, novo_evento( 5, e->chamada, tempo_decorrido + 1280 / largura_link ));

        } else if (e->tipo == 5) {



        } else if (e->tipo == 2) { // coleta de dados
            
            // ocupacao
            fprintf(arq_ocupacao, "%d %lf\n", tempo_coleta, soma_tempo_servico / maximo(tempo_coleta, servico));

            // en
            e_n_final = (e_n.soma_areas + (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos) / tempo_decorrido;
            fprintf(arq_en, "%d %lf\n", tempo_coleta, e_n_final);

            // ew
            e_w_final = ( (e_w_chegada.soma_areas + (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos) - 
                          (e_w_saida.soma_areas + (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos) ) /
                           e_w_chegada.no_eventos;
            fprintf(arq_ew, "%d %lf\n", tempo_coleta, e_w_final);

            // erro de little
            lambda = e_w_chegada.no_eventos / tempo_decorrido;
            fprintf(arq_erro_little, "%d %.20lf\n", tempo_coleta, fabs(e_n_final - lambda * e_w_final));

            insere(h, novo_evento(2, NULL, e->tempo + 100));

        } else if (e->tipo == 0) { //chegada
            //printf("Chegada em %lF.\n", tempo_decorrido);
            if (!fila) {
                int t = rand() % 100;
                servico = tempo_decorrido;
                
                if (t < 10) {
                    servico += (1500.0 / largura_link);
                } else if (t < 50) {
                    servico += (40.0 / largura_link);
                } else {
                    servico += (550.0 / largura_link);
                }
                
                insere(h, novo_evento(1, NULL, servico));

                soma_tempo_servico += servico - tempo_decorrido;
            }
            fila++;
            max_fila = fila > max_fila? fila: max_fila;

            chegada = tempo_decorrido + (-1.0 / (1.0/intervalo_medio_chegada)) * log(aleatorio());
            insere(h, novo_evento(0, NULL, chegada));

            //little
            e_n.soma_areas += 
            (tempo_decorrido - e_n.tempo_anterior)*e_n.no_eventos;
            e_n.tempo_anterior = tempo_decorrido;
            e_n.no_eventos++;

            e_w_chegada.soma_areas +=
            (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_chegada.tempo_anterior = tempo_decorrido;
            e_w_chegada.no_eventos++;

        } else {//saida
            //printf("Saida em %lF.\n", tempo_decorrido);
            fila--;

            if(fila){
                int t = rand() % 100;
                servico = tempo_decorrido;
                
                if (t < 10) {
                    servico += (1500.0 / largura_link);
                } else if (t < 50) {
                    servico += (40.0 / largura_link);
                } else {
                    servico += (550.0 / largura_link);
                }

                insere(h, novo_evento(1, NULL, servico));
                
                soma_tempo_servico += servico - tempo_decorrido;
            }

            //little
            e_n.soma_areas += 
            (tempo_decorrido - e_n.tempo_anterior)*e_n.no_eventos;
            e_n.tempo_anterior = tempo_decorrido;
            e_n.no_eventos--;

            e_w_saida.soma_areas +=
            (tempo_decorrido - e_w_saida.tempo_anterior)*e_w_saida.no_eventos;
            e_w_saida.tempo_anterior = tempo_decorrido;
            e_w_saida.no_eventos++;
        }
    }

    e_w_chegada.soma_areas +=
    (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
    
    e_w_saida.soma_areas +=
    (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

    e_n_final = e_n.soma_areas / tempo_decorrido;
    e_w_final =
    (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
    lambda = e_w_chegada.no_eventos / tempo_decorrido;

    printf("E[N]: %lF\n", e_n_final);
    printf("E[W]: %lF\n", e_w_final);
    printf("lambda: %lF\n\n", lambda);

    printf("Erro de Little: %.20lF\n\n", e_n_final - lambda * e_w_final);

    printf("Ocupacao: %lF.\n", soma_tempo_servico/maximo(tempo_decorrido, servico));
    printf("Max fila: %ld.\n", max_fila);

    return 0;
}