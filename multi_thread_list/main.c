#include "list.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include <pthread.h>
#include <unistd.h>

typedef struct product_s
{
    struct list_head product_node;
    uint32_t weight;
} product_t;

//初始化链表头
LIST_HEAD(product_list);

pthread_mutex_t producer_mutex = PTHREAD_MUTEX_INITIALIZER;
product_t *temp = NULL;

void producer(void)
{
    product_t *product = malloc(sizeof(product_t));

    // 产品重量为 300 ± 10
    product->weight = 290 + rand() % 20;

    printf("manufacture product :%p, weight %d\n", product, product->weight);
    list_add_tail(&product->product_node, &product_list);
}

// 遍历打印链表信息
void print_produce_list(void)
{
    product_t *product;
    list_for_each_entry(product, &product_list, product_node)
    {
        printf("manufacture product :%p, weight %d\n", product, product->weight);
    }
}

// 删除重量小于300的节点
void remove_unqualified_produce(void)
{
    product_t *product, *temp;

    list_for_each_entry_safe(product, temp, &product_list, product_node)
    {
        if(product->weight < 300)
        {
            printf("remove product :%p, weight %d\n", product, product->weight);
            list_del(&product->product_node);
            free(product);
        }
    }
}

// 消费者
void * consumer(void *arg)
{
    product_t *product;

    // 使用互斥锁对链表进行保护
    pthread_mutex_lock(&producer_mutex);
    list_for_each_entry(product, &product_list, product_node)
    {
        temp = list_next_entry(product, product_node);
        list_del(&product->product_node);
        printf("consume product :%p, weight %d, consumer :%p\n", product, product->weight, (void *)pthread_self());
        pthread_mutex_unlock(&producer_mutex);

        // 睡一会，防止太快
        usleep(10*1000);
        free(product);

        pthread_mutex_lock(&producer_mutex);
        if(temp != NULL){
            product = list_prev_entry(temp, product_node);
        }
    }
    pthread_mutex_unlock(&producer_mutex);

    return NULL;
}


int main(void)
{
    pthread_t t, t1;
    product_t *product, *temp;

    printf("working...\n");
    for (size_t i = 0; i < 6; i++)
    {
        producer();
    }

    printf("\ncheck...\n");
    remove_unqualified_produce();

    printf("\nconsume...\n");
    pthread_create(&t, NULL, consumer, NULL);
    pthread_create(&t1, NULL, consumer, NULL);

    pthread_join(t, NULL);
    pthread_join(t1, NULL);
}
