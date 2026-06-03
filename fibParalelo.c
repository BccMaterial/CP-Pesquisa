#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define LIMITE_SEQUENCIAL 25

long long fibonacci_seq(int n) {
    if(n <= 1) {
        return n;
    }

    return fibonacci_seq(n - 1) + fibonacci_seq(n - 2);
}

long long fibonacci_omp(int n) {
    if(n <= 1) {
        return n;
    }

    //Usa soma sequencial para valores pequenos para evitar criar tarefas demais
    if(n <= LIMITE_SEQUENCIAL) {
        return fibonacci_seq(n);
    }

    long long x, y;

    #pragma omp task shared(x)
    {
        x = fibonacci_omp(n - 1);
    }

    #pragma omp task shared(y)
    {
        y = fibonacci_omp(n - 2);
    }

    #pragma omp taskwait

    return x + y;
}

int main(int argc, char *argv[]) {
    int n = 40;

    if(argc > 1) {
        n = atoi(argv[1]);
    }

    double inicio = omp_get_wtime();

    long long resultado;

    #pragma omp parallel
    {
        #pragma omp single
        {
            resultado = fibonacci_omp(n);
        }
    }

    double fim = omp_get_wtime();

    printf("Fibonacci paralelo de %d = %lld\n", n, resultado);
    printf("Tempo paralelo: %.6f segundos\n", fim - inicio);

    return 0;
}

//Ganhos ou perdas de desempenho:
//Para valores grandes de n, apresenta ganho de desempenho, devido a existência de diversas chamadas recursivas independentes qaue podem ser distribuídas entre threads
//Para valores pequenos de n, a versão paralela pode ser mais lenta do que a sequencial, devido ao custo atribuído a criação de tarefas. Considerando que cada tarefa faça um pouco de trabalho, o tempo gasto criando e gerenciando tarefas pode ser maior do que o ganho atribuído ao paralelismo.

//Overhead:
//No contexto de OpenMp, "pragma omp task" cria uma tarefa que será executada por uma thread que esteja disponível. Isso é o que torna o paralelismo possível, mas também pode causar overhead. Durante o tempo de execução, precisamos criar a tarefa, coloca-la em uma fola, distribui-la a uma thread e sincrozinar os resultados utilizando taskwait. No contexto de fibonacci recursivo, o overhead é bem evidente, já que o existe um crescimento muito rápido no número de chamadas. Como criar uma tarefa para toda chamada recursiva seria ineficiente, a versão paralela utiliza uma estrágeia onde apenas as maiores chamdas são paralelzadas e resolve as chamadas menores de maneira sequencial.

//Escalabilidade:
//A escalabilidade é dependente do número de threads e o tamanho do n. Para valores menores de n, o aumento de threads provavelmente não resultará em uma melhora no desempenho, pois não existe quantidades de trabalho o suficiente para compensar o overhead. Para valroes maiores de n, como n = 40, o uso de diversas threads pode reduzir o tempo total, já que existem mais tarefas disponíveis para serem distribuídas. Então, podemos concluir que depois de um certo ponto, a adição de threads pode não trazer nenhuma evolução significativa na performance, considerando que existem custos de sincronização, criação de tarefas e disputa entre threads.

//Work stealing no OpenMp:
//No OpenMp, é possível utilizar uma estratégia similar a work stealing. Nesse mecanismo, após uma thread se tornar ociosa quando concluir suas tarefas, a mesma pode "roubar" tarefas na fila de uma outra thread que ainda possue trabalhos pendentes. No contexto do Fibonacci, Work Stealing é útil já que a árvore de chamadas recursivas é desbalanceada. Uma chamada fib(n - 1) costuma gerar mais trabalho do que fib(n - 2). Utilizando Wokr Stealing, é possível uma melhor redistribuição de tarefas entre threads no OpenMp, eviatando a ociosidade de algumas enquanto outras trabalham.