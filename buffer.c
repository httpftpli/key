/**
 * @brief 初始化环形队列
 * @param [in] queue  队列指针
 * @param [in] buf    缓冲区指针
 * @param [in] sizeOfItem
 * @param [in] nItem  缓冲区item个数
 * @return  NONE
 * @date    2013/5/10
 * @note
 */
void ringqueueInit(RINGQUEUE *ringBuf, void ** buf,unsigned int nItem)
{
	ringBuf->writeIndex = 0;
	ringBuf->readIndex  = 0;
	ringBuf->nItem  =  nItem+1;
	ringBuf->buf  =  buf;
}


BOOL ringqueueIsFull(RINGQUEUE *ringBuf){
   unsigned int i = ringBuf->writeIndex+1;
   if (i==ringBuf->nItem) {
      i=0;
   }
   if (i==ringBuf->readIndex) {
      return TRUE;
   }
   return FALSE;
}

BOOL ringqueueIsEmpty(RINGQUEUE *ringBuf){
   if (ringBuf->writeIndex == ringBuf->readIndex ) {
      return TRUE;
   }
   return FALSE;
}



/**
 * @brief 入队
 *
 * 通过数据拷贝入队，入队时把参数 \b item
 * 指向的内容拷贝到缓冲队列
 * @param [in] ringBuf
 * @param [in] item
 * @return
 * - FALSE   失败，因为当前fifo满
 * - TRUE    成功
 * @date    2013/5/10
 */
BOOL ringqueuePush(RINGQUEUE *ringBuf, void *item) {

	if(isRingBufFull(ringBuf))
       return FALSE;
    *(ringBuf->buf+ringBuf->writeIndex) = item;
	if(++ringBuf->writeIndex == ringBuf->nItem)
		ringBuf->writeIndex = 0;
	return TRUE;
}


/**
 * @brief 出队
 *
 * 通过数据拷贝出队
 * @param [in]  ringBuf
 * @param [out]  item  item缓冲区
 * @return
 * - FALSE   失败，因为队列空
 * - TRUE    成功
 * @date    2013/5/10
 */
void * ringqueuePop(RINGQUEUE * ringBuf)
{
   void *p = NULL;
   if (isRingBufEmpty(ringBuf)) {
      return NULL;
   }
   p = *(ringBuf->buf + ringBuf->readIndex);
   if(++ringBuf->readIndex == ringBuf->nItem)
		ringBuf->readIndex = 0;
   return p;
}


/**
 * @brief 读队列头，但不出队
 * @param [in] ringBuf
 * @param [out] item 返回item的指针
 * @return
 * - FALSE   失败，因为队列空
 * - TRUE    成功
 * @date    2013/5/10

 */
void * ringqueueRead(RINGQUEUE * ringBuf, void ** item){
   void * p = NULL;
   if (isRingBufEmpty(ringBuf)) {
      return NULL;
   }
   p = *(buf->ringBuf+ ringBuf->readIndex)
   return p;
}


void ringqueueClear(RINGQUEUE * ringBuf){
   ringBuf->readIndex = 0;
   ringBuf->writeIndex = 0;
}