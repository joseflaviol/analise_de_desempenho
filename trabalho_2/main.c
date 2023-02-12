#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

    double intervalo_medio_chegada;
    double tempo_medio_servico;
    double largura_link;

    double chegada;
    double servico;

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

    tempo_coleta = 100; // primeiro instante a coletar dados

    //gerando o tempo de chegada da primeira requisicao.
    chegada = (-1.0 / (1.0/intervalo_medio_chegada)) * log(aleatorio());

    while(tempo_decorrido <= tempo_simulacao){
        tempo_decorrido = !fila ? minimo(chegada, tempo_coleta) : minimo(tempo_coleta, minimo(chegada, servico));
        
        // coleta de dados
        if (tempo_decorrido == tempo_coleta) {

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

            tempo_coleta += 100;

        } else if (tempo_decorrido == chegada) { //chegada
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
                
                soma_tempo_servico += servico - tempo_decorrido;
            }
            fila++;
            max_fila = fila > max_fila? fila: max_fila;

            chegada = tempo_decorrido + (-1.0 / (1.0/intervalo_medio_chegada)) * log(aleatorio());

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