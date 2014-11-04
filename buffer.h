#ifndef __BUFFER__H__
#define __BUFFER__H__


#include <stdbool.h>

/**
 * @brief FIFI 环形队列 
 *  
 * 一个没有加锁的环形队列，一读一写环形队列
 * @date    2013/5/10 
 */
typedef struct tagfilo {
    unsigned long writeIndex;   /** < @brief 写下标  */
    unsigned long readIndex;    /** < @brief 写下标  */
    unsigned long nItem;        /** < @brief item总个数 */  
	unsigned long nAvaItem;		/** < @brief 记录当前fifi中的item个数*/
    void **        buf;        /** < @brief  缓冲区指针 */
}RINGQUEUE;




extern void ringqueueInit(RINGQUEUE *ringBuf, void ** buf,unsigned int nItem)
extern BOOL ringqueueIsFull(RINGQUEUE *ringBuf){
extern BOOL ringqueueIsEmpty(RINGQUEUE *ringBuf){
bool ringqueuePush(RINGQUEUE *ringBuf, void *item) {
extern void * ringqueuePop(RINGQUEUE * ringBuf)
extern void * ringqueueRead(RINGQUEUE * ringBuf, void ** item){
extern void ringqueueClear(RINGQUEUE * ringBuf){


#endif
