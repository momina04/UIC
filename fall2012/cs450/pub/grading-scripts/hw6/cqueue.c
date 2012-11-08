
#include<stdio.h>

int front = -1,rear = -1,queue_size = 100;
char* arr[100];


int doQInsert(int element_to_insert){
    if(front == -1 && rear == -1){
        front = 0;
        rear = 0;
    }
    if( (rear == queue_size -1 && front == 0) || (rear + 1 == front)        ){
        printf("queue is full\n");
        return 0;
    }
    if(rear == queue_size - 1){
        rear = 0;
    }
    arr[rear] = element_to_insert;
    rear += 1;
    return 1;
}

void doQDelete(){
    if(front == -1 && rear == -1){
        printf("Queue is empty\n");
    }
    if(front == queue_size-1){
        front = 0;
    }
    front  = front + 1;
}

int getCurrentQSize(){
    int i = front,count=1;
    while(1){
        i++;
        i = i % queue_size;
        if(i == rear){
            printf("current queue size --> %d\n",count);
            break;
        }
        count++;
    }
    return count;
}

int getFirstQElement(){
    printf("First Q element --> %d\n\n",arr[front]);
    return arr[front];
}

void printQ(){
    int i=front;
    while(1){
        printf("front index --> %d...val --> %d\n",i,arr[i]);
        i++;
        i = i % queue_size;
        if(i == rear)
            break;
        //sleep(1);
    }
}

/*
int main(){

    doQInsert(12);
    doQInsert(13);
    doQInsert(15);
    doQInsert(18);
    doQInsert(25);
    printQ();
    getCurrentQSize();
    getFirstQElement();

    doQDelete();
    doQDelete();
    printQ();
    getCurrentQSize();
    getFirstQElement();

    doQInsert(27);
    doQInsert(28);
    printQ();
    getCurrentQSize();
    getFirstQElement();

    doQDelete();
    printQ();
    getCurrentQSize();
    getFirstQElement();
}
*/
