#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> 
 
static int pogrzch = 500;  //колличество товаров, которые грузит погрузчик (наверное, следовало сделать константой)
static int repository[5] = {1000, 1000, 1000, 1000, 1000}; //склад, и изначально колличество там товаров
static int buyer[3] = {10000, 10000, 10000}; //потребности покупателей
static int proverka_buyerov=0;  //мой костыль который работает)
pthread_mutex_t mutex;  //mutex-ы

void* buy(void* arg) {
    int* a = (int*)arg;   //определение переменной, переданной потоку. (Не уверен, но, возможно, после этого стоило освободить память)
    int local=0;          //Локальная переменная, которая играет роль номера магазина
    while(local<5){       //local<5 не имеет смысла, и, по сути, выполняет то же самое что и while(true)
        if(!pthread_mutex_trylock(&mutex) && repository[local]>0){  //если магазин не занят и в нем есть товар
            if(repository[local]>buyer[*a]){ //эти условия правильно отнимают от потребности покупателя колличество товара, которое он забрал
                repository[local]-=buyer[*a]; 
                buyer[*a]=0;
            }else {buyer[*a]-=repository[local];repository[local]=0;}
            pthread_mutex_unlock(&mutex); //освобождаю магазин
                      
            printf("pokupatel %d vzyal iz magazina %d maximum tovarov\n", *a, local);
            printf("pokupatel %d usnull\n", *a);
            sleep (1);
            printf("pokupatel %d prosnulsya\n", *a);
            pthread_exit (NULL);            
            return NULL;
        }else{
            if(repository[local]<=0)pthread_mutex_unlock(&mutex); //если условие не выполнено из-за того, что маган не имеет товара, то mutex все-равно заблокирует магазин, следовательно это освобождает магазин (mutex)
           if(local==4){
                local=0;
                pthread_exit (NULL);            
                return NULL;
            } else local++; //идем в следующий магазин закупаться
           
        }
    }
    pthread_mutex_unlock(&mutex);
    printf("nothing shop\n");  // по-сути такого не может быть...
    pthread_exit (NULL);
    return NULL;
}
 
void* add(void *args) { //эта функция добавляет к товару в каком-либо магазине 500 и отправляет погрузчик спать. В случае если магазин занят, погрузчик идет на следующий
    int local=0;     //Локальная переменная, которая играет роль номера магазина
    while(local<5){ //local<5 не имеет смысла, и, по сути, выполняет то же самое что и while(true)
        if(!pthread_mutex_trylock(&mutex)){ //проверка не занят ли магазин
            repository[local]+=pogrzch;     //добавляет товар на склад
             printf("pogruzchik zagruzil 500 tovarov v magazin %d\n", local);
            printf("v magazine %d seichas %d tovarov\n", local, repository[local]);
            pthread_mutex_unlock(&mutex);   //разблокирует магазин                
            printf("pogruzchik zasnull\n");
            sleep (2);
            printf("pogruzchik prosnulsya\n");
            pthread_exit (NULL);            
            return NULL;
        }else{ //если магазин занят, то погрузчик идет на другой
            if(local==4){
                local=0;
                pthread_exit (NULL);            
                return NULL;
            } else local++;
        }
    }
    pthread_mutex_unlock(&mutex);
    printf("nothing\n");           // по-сути такого не может быть...
    pthread_exit (NULL);
    return NULL;
}

 void* MAINPOGRZCH(void *args) {
    pthread_t thr_pogrzch; 
    while(!proverka_buyerov){
        pthread_create(&thr_pogrzch, NULL, add, NULL); //создаю поток погрузчика
        pthread_join(thr_pogrzch, NULL);               //жду завершения потока погрузчика
        if(buyer[0]==0 && buyer[1]==0 && buyer[2]==0) proverka_buyerov=1; //проверка опустошены ли потребности покупателей. Если ставить это напрямую в while, программа работает не корректно и путается в потоках 
    }
 }

 void* MAINBUYER(void *args) {
    pthread_t thr_buyer[3];
    int i,id[3]; //в массиве лучше передавать значения потоков
    while(!proverka_buyerov){
        for(i = 0; i < 3; i++) {      //через цикл создаю три потока покупателя, каждый передает потоку свой номер
            id[i] = i;
            pthread_create(&thr_buyer[i], NULL, buy, (void*)(id + i));
        }
        for(i = 0; i < 3; i++) {     //жду завершения потоков покупателя
            pthread_join(thr_buyer[i], NULL);
        }
        if(buyer[0]==0 && buyer[1]==0 && buyer[2]==0) proverka_buyerov=1; //поставил проверку в обоих потоках на всякий случай
    }
 }

 
int main() {
    pthread_t thr_MainPogrzch;  //    создаю
    pthread_t thr_MainBuyer;    //итендефикаторы потоков
    pthread_mutex_init(&mutex, NULL);//Инициализирую mutex-ы, чтобы занимать ячейки
    pthread_create(&thr_MainPogrzch, NULL, MAINPOGRZCH, NULL); //создаю поток погрузчика
    pthread_create(&thr_MainBuyer, NULL, MAINBUYER, NULL); //поток покупателей     (отдельные потоки создаются, чтобы покупатели небыли зависимы от погрузчика, и наоборот)
    pthread_join(thr_MainPogrzch, NULL);//жду завершения
    pthread_join(thr_MainBuyer, NULL); //потоков
    pthread_mutex_destroy(&mutex); // удаляю mutex
    printf("magazin 0: %d\nmagazin 1: %d\nmagazin 2: %d\nmagazin 3: %d\nmagazin 4: %d\n pokupatel 0: %d\n pokupatel 1: %d\n pokupatel 2: %d\n", repository[0], repository[1], repository[2], repository[3], repository[4], buyer[0], buyer[1], buyer[2]); //вывожу на экран результаты для проверки
    return 0;
    
}
