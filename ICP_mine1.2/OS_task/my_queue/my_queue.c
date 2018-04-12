/**
  ******************************************************************************
  * @file   queue.c
  * @author  liucongjun
  * @version 0.2
  * @date    2017.4.13
  * @brief ���ڵ�Ƭ����malloc֧�ֲ��ã����þ�̬���齨��ѭ������
	         0.1 3.30 ����ʵ����ѭ������;
					 0.2 4.12 �Ľ���ReadQueue_n����;
					 1.0 4.13 �޸�ReadQueue_n�����У����n=max_num�����еĳ��Ȳ�����bug
******************************************************************************
*/
//#define USART_DEBUG

#include "my_queue.h"
#ifdef USART_DEBUG
#include "usart.h"
#endif

//  @ fuction:  
//  @ description:  ��ʼ������
//  @ input: 1.��Ҫ��ʼ���Ķ���ָ��q  2.��̬�����׵�ַ array
//  @ output:
//  @ note: 
bool Init_queue(QueueArray_type *q,Elemtype *array,int num_max){
   q->data = array;
	 q->front = 0;
	 q->rear = 0;
//	 q->length = 0;
	 q->queue_max = num_max;
	
	return true;
}

//  @ fuction:  
//  @ description:  ���
//  @ input:
//  @ output:
//  @ note: 
bool EnQueue(QueueArray_type *q,Elemtype e){
	//�ж϶����Ƿ�����
  if((q->rear+1)%q->queue_max == q->front) return false;
	
	q->data[q->rear] = e;
	q->rear = (q->rear + 1) % q->queue_max;//rearָ��β���ݵ���һ���ռ�
	
	return true;
}

//  @ fuction:  
//  @ description:  ����
//  @ input:
//  @ output:
//  @ note: 
bool DeQueue(QueueArray_type *q,Elemtype *e){
   //�ж϶����Ƿ�Ϊ��
  if(q->front == q->rear) return false;
	
	*e = q->data[q->front];
	q->front = (q->front + 1) % q->queue_max;
	
	return true;
}

//  @ fuction:  
//  @ description:  �ж϶����Ƿ�Ϊ��
//  @ input:
//  @ output:
//  @ note: 
bool IsQueue_empty(QueueArray_type q){
  if(q.front == q.rear) return true;
	
	return false;
}

//  @ fuction:  
//  @ description:  �ж϶����Ƿ�Ϊ��
//  @ input:
//  @ output:
//  @ note: ��������һ����Ԫ�����ֶ��пպ��������ʱ����һ�����е�Ԫ�����������Ĵ�СΪsize ʵ��ֻ�ܴ��(size-1)��Ԫ��
bool IsQueue_full(QueueArray_type q){
  if((q.rear+1)%q.queue_max == q.front) return true;
	
	return false;
}

//  @ fuction:  
//  @ description:  �ӻ������ж�ȡn��Ԫ�ص�array_print
//  @ input:
//  @ output:
//  @ note: num < 
bool ReadQueue_n(QueueArray_type *q,Elemtype *array_print,int num){
	int temp,temp_array;
  //�ж϶����Ƿ�Ϊ��
  if(q->front == q->rear){
		#ifdef USART_DEBUG
      printf("read empty queue!\r\n");
    #endif
	   return false;
	}
	if(num > q->queue_max){
		#ifdef USART_DEBUG
      printf("read full queue!\r\n");
    #endif
	  return false;
	}
	
	if(q->front < q->rear){ //�������
		for(temp = 0,temp_array = 0;temp<num;temp++){
		  array_print[temp_array++] = q->data[q->front+temp];
		} 
		q->front = (q->front+num)%q->queue_max;//����
		
		if(num == q->queue_max) q->rear = q->front;//���� ���num=max,��ʹ�����ˣ����Ȼ���û�б�
	}
	else{
	  for(temp = 0,temp_array = 0;temp<num;temp++){
		  array_print[temp_array++] = q->data[(temp+q->front)%q->queue_max];
		}
		q->front = (q->front+num)%q->queue_max;//����
		
		if(num == q->queue_max) q->rear = q->front;//���� ���num=max,��ʹ�����ˣ����Ȼ���û�б�
	}
	
	return true;
}

//  @ fuction:  
//  @ description:  �ӻ�������ȡ���Ե�Ԫ��
//  @ input:
//  @ output:
//  @ note: ͬʱ��Ҫ����
bool ReadQueue_all(QueueArray_type *q,Elemtype *array_print){
  return ReadQueue_n(q,array_print,GetQueue_length(*q));
}

//  @ fuction:  
//  @ description:  ��ȡ��ǰ�Ķ��еĳ���
//  @ input:
//  @ output:
//  @ note: 
int GetQueue_length(QueueArray_type q){
	//int temp;
	
  if(q.front == q.rear) return 0;//empty
	if((q.rear+1)%q.queue_max == q.front) return q.queue_max;//full
	
	if(q.front < q.rear){ //�������
		//temp = q.rear - q.front;
		return (q.rear - q.front);
	}
	else{
	  //temp = q.queue_max - (q.front - q.rear);
		return (q.queue_max + q.rear - q.front);
	}
	
	//return temp;
}

