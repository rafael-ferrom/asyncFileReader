#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

unsigned LoadData(FILE *file, float *array, unsigned sz);

typedef struct _asyncState_t 
{
    pthread_t thread; // referência para o objeto de thread da biblioteca
    FILE *file; // os argumentos originais da função
    float *data; // os argumentos originais da função
    unsigned read; // o retorno original da função
    int error; // uma variável para informar algum erro para thread main
    unsigned sz; // Adiciona o tamanho em bytes que estava faltando
} asyncState_t;

asyncState_t * BeginLoadData(FILE *file, float *array, unsigned sz);

unsigned EndLoadData(asyncState_t *state);
void * AsyncLoadData(void *ctx);

int main(int argc, char *argv[])
{
float array[5] = {};
FILE *datafile = fopen("datafile.txt", "r+");
asyncState_t *as = BeginLoadData(datafile, array, sizeof(array));

printf("Foram lidos %d bytes do arquivo.", EndLoadData(as));
int i ;
for (i = 0; i < sizeof(array)/sizeof(float); i++)
{
printf("%d: %f\n", i, array[i]);
}
system("pause");
return 0;
}

asyncState_t * BeginLoadData(FILE *file, float *array, unsigned sz)
{
asyncState_t *state = (asyncState_t *)calloc(1, sizeof(asyncState_t));
state->file = file;
state->data = array;
state->sz = sz;
pthread_create(&state->thread, NULL, AsyncLoadData, (void *)state);
return state;
}

unsigned EndLoadData(asyncState_t *state)
{
pthread_join(state->thread, NULL);
unsigned ret = state->read;
if (state->error) 
{
printf("Erro ao carregar dados!\n");
}
free(state);
return ret;
}

void * AsyncLoadData(void *ctx)
{
asyncState_t *state = (asyncState_t *)ctx;
state->read = LoadData(state->file, state->data, state->sz);
return NULL;
}

unsigned LoadData(FILE *file, float *data, unsigned sz)
{
char line[32]; // buffer para ler linhas do arquivo
int i =0;
unsigned read = 0;
while ((fgets(line, sizeof(line), file) != NULL) && (read < sz))
{
data[i] = strtof(line, NULL);
read += sizeof(float);
i++;
}
return read;
}
