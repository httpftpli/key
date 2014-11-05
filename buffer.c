/**
 * @brief ��ʼ�����ζ���
 * @param [in] queue  ����ָ��
 * @param [in] buf    ������ָ��
 * @param [in] sizeOfItem
 * @param [in] nItem  ������item����
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
 * @brief ���
 *
 * ͨ�����ݿ�����ӣ����ʱ�Ѳ��� \b item
 * ָ������ݿ������������
 * @param [in] ringBuf
 * @param [in] item
 * @return
 * - FALSE   ʧ�ܣ���Ϊ��ǰfifo��
 * - TRUE    �ɹ�
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
 * @brief ����
 *
 * ͨ�����ݿ�������
 * @param [in]  ringBuf
 * @param [out]  item  item������
 * @return
 * - FALSE   ʧ�ܣ���Ϊ���п�
 * - TRUE    �ɹ�
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
 * @brief ������ͷ����������
 * @param [in] ringBuf
 * @param [out] item ����item��ָ��
 * @return
 * - FALSE   ʧ�ܣ���Ϊ���п�
 * - TRUE    �ɹ�
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