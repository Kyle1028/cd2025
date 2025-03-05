#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char word;
    int num;
    struct node *next;
} Node;

void new_node(Node** head, char w){

    Node* point = *head;

    // add node at last
    if(*head == NULL){
        //new node
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode -> num = 1;
        newNode -> word = w;
        newNode -> next = NULL;
        *head = newNode;
    }else{
        while(point -> next!= NULL){
            //not the last
            if(point -> word != w){
                //not same word
                point = point -> next;
            }else{
                //the same word
                point -> num = point -> num +1;
                return;
            }
        }

        if(point -> next == NULL){
            //last
            if(point -> word == w){
                point -> num = point -> num + 1;
                return;
            }
        }

        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode -> num = 1;
        newNode -> word = w;
        newNode -> next = NULL;
        point -> next = newNode;
    }
}

void print(Node* head) {
    Node* point = head;
    while(point != NULL){
        //not the end
        if( point -> word == '\n') {

        }else if(point -> word == ' '){
            printf("%c : %d\n", point -> word, point -> num);
        }else if(point -> word == '\t'){

        }else{
            printf("%c : %d\n", point -> word, point -> num);
        }
        
        point = point -> next;
    }
}

void free_data(Node* head){
    Node* point = head;
    while(point != NULL){
        point = head;
        head = head -> next;
        free(point);
    }
}

int main() {
    Node* head = NULL;
    char get_word;

    //read itself
    FILE* file = fopen(__FILE__, "r");

    while((get_word = fgetc(file)) != EOF){
        //not the end
        new_node(&head, get_word);
    }

    print(head);

    free_data(head);

    fclose(file);

    return 0;
}
