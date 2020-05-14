#ifndef _FIFO_H_
#define _FIFO_H_
 
#include "stdlib.h"
#include "stdint.h"
#include "string.h" 

 
#define FALSE 0
#define TURE 1 
 
 /**
* @brief fifo结构
*/
 
typedef struct
{
	//FIFO头节点位置 
    int front;
	//FIFO尾结点位置     
    int rear;
    //FIFO状态 
    int IsFull;
    //FIFO缓存地址 
  	void *base;
    // fifo中存储的元素数,不是字节大小
    int ItemAmount;
    // 元素大小.单位: 字节
    int ItemSize;
} FIFO;
 
 
/**
* @brief 创建fifo
* @param ItemAmount：fifo中元素数.注意不是字节数
* @param ItemSize: 元素大小.单位: 字节
* @return fifo指针
*/
FIFO * FIFOCreate(int ItemAmount, int ItemSize);
 
/**
* @brief 删除fifo
* @param FIFOIndex: fifo指针
*/
void FIFODelete(FIFO * FIFOIndex);
 
/**
* @brief fifo检查是否可以写入
* @param FIFOIndex: fifo指针
* @retval FALSE:不可以写入.TURE:可以写入
*/
int FIFOWriteAvailable(FIFO * FIFOIndex);
 
/**
* @brief fifo写入
* @param FIFOIndex: fifo指针
* @param frame:写入元素指针
* @return FALSE:失败.TURE:成功
*/
int FIFOWrite(FIFO * FIFOIndex, void *data);
 
/**
* @brief fifo批量写入
* @param FIFOIndex: fifo指针
* @param data: 写入元素指针
* @param @param num：写入的元素数目
* @return FALSE:失败.TURE:成功
*/
int FIFOBatchWrite(FIFO * FIFOIndex, void *data, int num);
 
/**
* @brief fifo检查是否可以读取
* @param FIFOIndex: fifo指针
* @return FALSE:不可以读取.TURE:可以读取
*/
int FIFOReadAvailable(FIFO * FIFOIndex);
 
/**
* @brief fifo读取
* @param FIFOIndex: fifo指针
* @param data: 读取的数据
* @return FALSE: 失败.TURE: 成功
*/
int FIFORead(FIFO * FIFOIndex, void *data);
 
/**
* @brief fifo只读取不删除数据 
* @param FIFOIndex: fifo指针
* @param data: 读取的数据
* @param depath: 读取数据深度 
* @return FALSE: 失败.TURE: 成功
*/
int FIFOPeek(FIFO * FIFOIndex, void *data,int depth); 
 
/**
* @brief fifo批量读取
* @param FIFOIndex: fifo指针
* @param data: 读取的数据
* @param num：读取的元素数
* @return FALSE: 失败.TURE: 成功
*/
int FIFOBatchRead(FIFO * FIFOIndex, void *data, int num);
 
/**
* @brief fifo可读的元素数
* @param FIFOIndex: fifo指针
* @return 元素数
*/
int FIFOReadableAmount(FIFO * FIFOIndex);
 
/**
* @brief fifo可写的元素数
* @param FIFOIndex: fifo指针
* @return 元素数
*/
int FIFOWritableAmount(FIFO * FIFOIndex);
 
#endif

