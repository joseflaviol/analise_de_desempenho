#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double aleatorio();
double minimo(double a, double b);
double maximo(double a, double b);

int main() {

    double tempo_simulacao;
    double tempo_decorrido = 0.0;

    double intervalo_medio_chegada;
    double tempo_medio_servico;

    double chegada;
    double servico;

    double soma_tempo_servico;

    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    srand(time(NULL));

    printf("tempo de simulacao (segundos): ");
    scanf("%lf", &tempo_simulacao);

    printf("intervalo medio entre chegadas (segundos): ");
    scanf("%lf", &intervalo_medio_chegada);

    printf("tempo medio de servico (segundos): ");
    scanf("%lf", &tempo_medio_servico);

    // gerando o tempo de chegada da primeira requisicao.
    chegada = (-1.0 / (1.0 / intervalo_medio_chegada) ) * log(aleatorio());

    while (tempo_decorrido <= tempo_simulacao) {
        tempo_decorrido = !fila ? chegada : minimo(chegada, servico);

        if (tempo_decorrido == chegada) {
            if (!fila) {
                servico = tempo_decorrido + (-1.0 / (1.0 / tempo_medio_servico) ) * log(aleatorio());
                soma_tempo_servico += servico - tempo_decorrido;
            }
            fila++;
            max_fila = fila > max_fila ? fila : max_fila;
            chegada = tempo_decorrido + (-1.0 / (1.0 / intervalo_medio_chegada) ) * log(aleatorio());
        } else {
            fila--;

            if (fila) {
                servico = tempo_decorrido + (-1.0 / (1.0 / tempo_medio_servico) ) * log(aleatorio());  
                soma_tempo_servico += servico - tempo_decorrido;  
            }
        }
    }

    printf("Ocupacao: %lf\n", soma_tempo_servico / maximo(tempo_decorrido, servico));
    printf("Max fila: %ld\n", max_fila);
 
    return 0;

}

double aleatorio() {
    double u = rand() / ((double) RAND_MAX + 1);

    u = 1.0 - u;

    return u;
}

double minimo(double a, double b) {
    if (a < b) {
        return a;
    }

    return b;
}

double maximo(double a, double b) {
    if (a > b) {
        return a;
    }

    return b;
}