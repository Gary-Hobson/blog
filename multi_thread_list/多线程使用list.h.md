# 安全函数不安全--多线程慎用list.h

絮絮叨叨： 一个 BUG 轻轻松松让我少了几个小时的睡眠时间！

## 前言
linux 开发应该多少都听过大名鼎鼎的 list.h ，其简洁优雅的设计，一个头文件完成了一个高可用的链表。

但是 list.h 并不是线程安全的，在多线程的情况下使用，必须考虑多线程数据同步的问题。

然而。。。。

我在使用互斥锁对链表的操作进行保护之后，还是被坑了！

下面是把我坑了的 list_for_each_entry 和 list_for_each_entry_safe 两个函数的详细分析。

## list.h 单线程使用

在 list.h 这个文件中有非常多值得学习的地方，比如其最经典的 container_of 的实现。

在这里只介绍几个常用的函数，然后重点分析在多线程使用时的碰到的坑。

### 链表初始化及添加节点
首先定义一个链表和链表节点，定义一个产品，其属性为产品重量(weight)。

```c
typedef struct product_s
{
    struct list_head product_node;
    uint32_t index;
    uint32_t weight;  
}product_t;

//初始化链表头
LIST_HEAD(product_list);

```

生产者在生产完产品后，将产品加入链表，等待消费者使用。
```c
void producer(void)
{
    product_t *product = malloc(sizeof(product_t));

    // 产品重量为 300 ± 10
    product->weight = 290 + rand() % 20;
    
    printf("product :%p, weight %d\n", product, product->weight);
    list_add_tail(&product->product_node, &product_list);
}
```

### 遍历链表


使用 ```list_for_each_entry``` 可以将链表进行遍历：

```c
// 遍历打印链表信息
void print_produce_list(void)
{
    product_t *product;
    list_for_each_entry(product, &product_list, product_node)
    {
        printf("manufacture product :%p, weight %d\n", product, product->weight);
    }
}
```

其具体实现是使用宏将 for 循环的初始条件和完成条件进行了替换：
```c
#define list_for_each_entry(pos, head, member)                \
    for (pos = list_first_entry(head, typeof(*pos), member);    \
         &pos->member != (head);                    \
         pos = list_next_entry(pos, member))
```

其中for循环的第一个参数将 ```pos = list_first_entry(head, typeof(*pos), member);``` 初始化为链表头指向的第一个实体链表成员。

第二个参数 ```&pos->member != (head)``` 为跳出条件，当pos->member再次指向链表头时跳出for循环。

for的第三个参数通过pos->member.next指针遍历整个实体链表，当pos->member.next再次指向我们的链表头的时候跳出for循环。

但是 <font color=red size=4 face="黑体"> list_for_each_entry 不能在遍历的循环体中删除节点</font>，因为在循环体中删除链表节点后，当前节点的前驱结点和后继结点指针会被置空。

在for循环的第三个参数中，获取下一个节点时，会发生非法指针访问

![](https://files.mdnice.com/user/11205/0b0e55fd-86b1-4cd9-89ac-c710678e9f90.png)

### “安全遍历链表”
为了解决在遍历链表过程中，无法删除结点的问题，在 list.h 中提供了一个安全删除节点的函数 

```c
// 删除重量小于300的节点
void remove_unqualified_produce(void)
{
    product_t *product, *temp;
    list_for_each_entry_safe(product, temp, &product_list, product_node)
    {
        // 移除重量小于300的产品
        if (product->weight < 300)
        {
            printf("remove product :%p, weight %d\n", product, product->weight);
            list_del(&product->product_node);
            free(product);
        }
    }
}
```

其实现是使用一个中间变量，在开始每次开始执行循环体前，将当前节点的下一个节点保存到中间变量，从而实现"安全"遍历

```c
#define list_for_each_entry_safe(pos, n, head, member)            \
    for (pos = list_first_entry(head, typeof(*pos), member),    \
        n = list_next_entry(pos, member);            \
         &pos->member != (head);                    \
         pos = n, n = list_next_entry(n, member))
 ```

## 多线程中使用list.h

上面我们在主线程里面创建了产品，并放入到链表中并，并过滤了重量小于300的产品。

后面我们在多线程中对产品进行消费（两个线程同时消费链表的数据，使用完成后删除并释放结点）。

这里的逻辑和单线程中的差不多，同样是遍历链表，然后从链表中删除节点。不同的是，由于list.h自身没有带锁，所以需要使用互斥锁将链表的操作进行保护。

于是很自然的有了下面的代码 
```c
void * consumer(void *arg)
{
    product_t *product, *temp;

    // 使用互斥锁对链表进行保护
    pthread_mutex_lock(&producer_mutex);
    list_for_each_entry_safe(product, temp, &product_list, product_node)
    {
        list_del(&product->product_node);
        printf("consume product :%p, weight %d, consumer :%p\n", product, product->weight, (void *)pthread_self());
        pthread_mutex_unlock(&producer_mutex);

        // 睡一会，防止太快了
        usleep(10*1000);
        free(product);
        pthread_mutex_lock(&producer_mutex);
    }
    pthread_mutex_unlock(&producer_mutex);
    
    return NULL;
}
```

在上面的这段代码中，在对链表操作时，使用互斥锁对链表进行了保护，使同时只能有一个线程访问链表。

不过你以为这样就好了嘛，如果时这样，这篇文章就没存在的必要了。。。

![](https://files.mdnice.com/user/11205/73c76b7c-7b78-4bd3-81ad-f6ebf9199fad.png)


在两个线程同时遍历时，即便是加了锁之后，数据访问也不安全。

在遍历使用的 list_for_each_entry_safe 宏中，使用了一个零时变量对保存着当前链表的下一个节点。

但是多线程访问链表时，有可能零时变量保存的节点，被另一个线程删除了，所以访问的时候又是  <font color=red size=4 face="黑体">Segmentation fault </font>

![](https://files.mdnice.com/user/11205/c1104785-c69b-4332-b670-770d97190523.png)

  
## 后记
  
原因找到了，也就好办了。以至于解决方法嘛，我是使用一个全局的零时变量，将需要删除节点的下一个节点保存起来，手动实现多线程的"安全删除"。
  
```c
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
```
  
  
一个晚上找到了这个bug，然后又花了一个晚上记录下来这个bug。
  
据说 klist.h 是 list.h 的线程安全版本，后面花时间在研究一下去，今天就先睡了。。。


