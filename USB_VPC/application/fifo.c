#include "fifo.h"
 
/**
* @brief 创建fifo
* @param ItemAmount：fifo中元素数.注意不是字节数
* @param ItemSize: 元素大小.单位: 字节
* @return fifo指针
*/
 
FIFO * FIFOCreate(int ItemAmount, int ItemSize)
{
    FIFO *fifo = (FIFO *)malloc(sizeof(FIFO));
    fifo->ItemAmount = ItemAmount;
    fifo->ItemSize = ItemSize;
    fifo->front = 0;
    fifo->rear = 0;
    fifo->IsFull = FALSE;
    fifo->base = (void *)malloc(ItemAmount * ItemSize);
    return fifo;
}
 
/**
* @brief 删除fifo
* @param FIFOIndex: fifo指针
*/
 
void FIFODelete(FIFO * FIFOIndex)
{
    FIFO *fifo = (FIFO *)FIFOIndex;
    free(fifo->base);
    fifo->base = NULL;
    free(fifo);
    fifo = NULL;
}
 
/**
* @brief fifo检查是否可以写入
* @param FIFOIndex: fifo指针
* @retval FALSE:不可以写入.TURE:可以写入
*/
 
int FIFOWriteAvailable(FIFO * FIFOIndex)
{	
    FIFO *fifo = (FIFO *)FIFOIndex;
    return !fifo->IsFull;
}
 
/**
* @brief fifo写入
* @param FIFOIndex: fifo指针
* @param frame:写入元素指针
* @return FALSE:失败.TURE:成功
*/
 
int FIFOWrite(FIFO * FIFOIndex, void *data)
{
    FIFO *fifo = (FIFO *)FIFOIndex;
    if (fifo->IsFull)
	{
		return FALSE;
	}

    memcpy((char *)(fifo->base) + fifo->front * fifo->ItemSize, data, fifo->ItemSize);
    fifo->front++;

    if (fifo->front >= fifo->ItemAmount)
	{
        fifo->front = 0;
	}
    if (fifo->front == fifo->rear)
	{
        fifo->IsFull = TURE;
	}
	
	return TURE;
}
 
/**
* @brief fifo批量写入
* @param FIFOIndex: fifo指针
* @param data: 写入元素指针
* @param num：写入的元素数目
* @return FALSE:失败.TURE:成功
*/
int FIFOBatchWrite(FIFO * FIFOIndex, void *data, int num)
{
    FIFO *fifo = (FIFO *)FIFOIndex;
    if (FIFOWritableAmount(fifo) < num)
    {
        return FALSE;
    }
 	
 	if (fifo->ItemAmount-fifo->front > num)
    {
        memcpy((char *)(fifo->base) + fifo->front * fifo->ItemSize, data, fifo->ItemSize * num);
    }
    else
    {
    	memcpy((char *)(fifo->base) + fifo->front * fifo->ItemSize, (char *)data, fifo->ItemSize * (fifo->ItemAmount-fifo->front));
    	memcpy((char *)(fifo->base) , (char *)data+ (fifo->ItemSize * (fifo->ItemAmount-fifo->front)),fifo->ItemSize * ( num+fifo->front-fifo->ItemAmount));
	}
    
    fifo->front = (fifo->front+num)%fifo->ItemAmount;

    if (fifo->front == fifo->rear)
    {
        fifo->IsFull = TURE;
    }
 
    return TURE;
}
 
/**
* @brief fifo检查是否可以读取
* @param FIFOIndex: fifo指针
* @return FALSE:不可以读取.TURE:可以读取
*/
int FIFOReadAvailable(FIFO * FIFOIndex)
{
    FIFO *fifo = (FIFO *)FIFOIndex;
    if (fifo->front == fifo->rear && !fifo->IsFull)
	{
		return FALSE;
	}
	return TURE;
}
 
/**
* @brief fifo只读取不删除数据 
* @param FIFOIndex: fifo指针
* @param data: 读取的数据
* @param depath: 读取数据深度 
* @return FALSE: 失败.TURE: 成功
*/
int FIFOPeek(FIFO * FIFOIndex, void *data,int depth)
{
    FIFO *fifo = (FIFO *)FIFOIndex;
    if (fifo->front == fifo->rear && !fifo->IsFull)
	{
		return FALSE;
	}
	
	if(depth>FIFOReadableAmount(fifo)-1)
	{
		return FALSE;
	}
    
    memcpy(data, (char *)(fifo->base) + ((fifo->rear+depth)%fifo->ItemAmount) * fifo->ItemSize, fifo->ItemSize);
	return TURE;
}

/**
* @brief fifo读取
* @param FIFOIndex: fifo指针
* @param data: 读取的数据
* @return FALSE: 失败.TURE: 成功
*/
int FIFORead(FIFO * FIFOIndex, void *data)
{
    FIFO *fifo = (FIFO *)FIFOIndex;
    if (fifo->front == fifo->rear && !fifo->IsFull)
	{
		return FALSE;
	}
    
    memcpy(data, (char *)(fifo->base) + fifo->rear * fifo->ItemSize, fifo->ItemSize);
    fifo->rear++;
    if (fifo->rear >= fifo->ItemAmount)
	{
        fifo->rear = 0;
	}
    fifo->IsFull = FALSE;
	return TURE;
}
 
/**
* @brief fifo批量读取
* @param FIFOIndex: fifo指针
* @param data: 读取的数据
* @param num：读取的元素数
* @return FALSE: 失败.TURE: 成功
*/
int FIFOBatchRead(FIFO * FIFOIndex, void *data, int num)
{
        FIFO *fifo = (FIFO *)FIFOIndex;
    if (FIFOReadableAmount(fifo) < num)
    {
        return FALSE;
    }
 
 	if (fifo->ItemAmount-fifo->rear > num)
    {
    	memcpy(data, (char *)(fifo->base) + fifo->rear * fifo->ItemSize, fifo->ItemSize * num);
    }
    else
    {
    	memcpy((char *)data, (char *)(fifo->base) + fifo->rear * fifo->ItemSize,  fifo->ItemSize * (fifo->ItemAmount-fifo->rear));
    	memcpy((char *)data+ (fifo->ItemSize * (fifo->ItemAmount-fifo->rear)),fifo->base,  fifo->ItemSize * ( num+fifo->rear-fifo->ItemAmount));
	}
    fifo->rear = (num+fifo->rear)%fifo->ItemAmount;

    fifo->IsFull = FALSE;
    return TURE;
}
 
/**
* @brief fifo可读的元素数,即已使用的数量 
* @param FIFOIndex: fifo指针
* @return 元素数
*/
int FIFOReadableAmount(FIFO * FIFOIndex)
{
    FIFO *fifo = (FIFO *)FIFOIndex;
    if (fifo->IsFull)
    {
        return fifo->ItemAmount;
    }
    else
    {
        return (fifo->ItemAmount + fifo->front - fifo->rear) % fifo->ItemAmount;
    }
}
 
/**
* @brief fifo可写的元素数,即未使用数量 
* @param FIFOIndex: fifo指针
* @return 元素数
*/
int FIFOWritableAmount(FIFO * FIFOIndex)
{
    FIFO *fifo = (FIFO *)FIFOIndex;
    if (fifo->IsFull)
    {
        return 0;
    }
    else
    {
        if (fifo->front == fifo->rear)
        {
            return fifo->ItemAmount;
        }
        else
        {
            return (fifo->ItemAmount + fifo->rear - fifo->front) % fifo->ItemAmount;
        }
    }
}

