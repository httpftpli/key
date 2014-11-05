#ifndef __BUFFER__H__
#define __BUFFER__H__


#include <stdbool.h>

/**
 * @brief FIFI ���ζ��� 
 *  
 * һ��û�м����Ļ��ζ��У�һ��һд���ζ���
 * @date    2013/5/10 
 */
typedef struct tagfilo {
    unsigned long writeIndex;   /** < @brief д�±�  */
    unsigned long readIndex;    /** < @brief д�±�  */
    unsigned long nItem;        /** < @brief item�ܸ��� */  
	unsigned long nAvaItem;		/** < @brief ��¼��ǰfifi�е�item����*/
    void **        buf;        /** < @brief  ������ָ�� */
}RINGQUEUE;




extern void ringqueueInit(RINGQUEUE *ringBuf, void ** buf,unsigned int nItem)
extern BOOL ringqueueIsFull(RINGQUEUE *ringBuf){
extern BOOL ringqueueIsEmpty(RINGQUEUE *ringBuf){
bool ringqueuePush(RINGQUEUE *ringBuf, void *item) {
extern void * ringqueuePop(RINGQUEUE * ringBuf)
extern void * ringqueueRead(RINGQUEUE * ringBuf, void ** item){
extern void ringqueueClear(RINGQUEUE * ringBuf){


#endif
