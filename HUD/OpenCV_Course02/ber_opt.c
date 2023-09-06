
#include<stdlib.h>
int main(){
    struct censo
    {
        float populacao;
        float idade;
    };

    struct censo *arrayCenso;
    arrayCenso = malloc(100*sizeof(struct censo));

   int i = 0;

    for(i = 0; i < 100; i++){
        arrayCenso[i].populacao = rand();
        arrayCenso[i].idade = rand();
    }

    for (i = 0; i < 100; i++)
    {
        printf("Pessoa %d idade %d \n", arrayCenso[i].populacao, arrayCenso[i].idade);
    }

   free(arrayCenso);
}
