#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int tipo;
    double tempo;
} Evento;

Evento *novo_evento(int tipo, double tempo) {
    Evento *e = (Evento *) malloc(sizeof(Evento));

    e->tipo = tipo;
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

int main() {

    MinHeap *h = inicia_min_heap(10);

    Evento *e = novo_evento(0, 1);
    insere(h, e);    
    Evento *e2 = novo_evento(0, 9); 
    insere(h, e2);  
    Evento *e3 = novo_evento(0, 4);
    insere(h, e3);   
    Evento *e4 = novo_evento(0, 0);
    insere(h, e4);   
    Evento *e5 = novo_evento(0, 7);
    insere(h, e5);   

    printf("%.2lf\n", extrai_minimo(h)->tempo);
    printf("%.2lf\n", extrai_minimo(h)->tempo);
    printf("%.2lf\n", extrai_minimo(h)->tempo);
    printf("%.2lf\n", extrai_minimo(h)->tempo);
    printf("%.2lf\n", extrai_minimo(h)->tempo);

    return 0;

}