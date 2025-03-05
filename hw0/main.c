#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node {
    char c;             // 儲存單一字元
    struct Node *next;  // 指向下一個節點
} Node;

//getFrequency
//O(n)的逐次走訪 linked list，計算字元在整個串列出現的次數
int getFrequency(Node *head, char target) {
    int count = 0;
    Node *p = head;
    while (p != NULL) {
        if (p->c == target) {
            count++;
        }
        p = p->next;
    }
    return count;
}

int main(void) {
    FILE *fp = fopen(__FILE__, "r");
    if (!fp) {
        printf("Failed to open file.\n");
        return 1;
    }

    Node *head = NULL, *tail = NULL;

    //讀檔同時用陣列freq[]紀錄每個字元出現次數 (O(1) 累加) 
    int freq[256];
    memset(freq, 0, sizeof(freq));

    //逐字讀取檔案，並把每個字元加到 linked list 
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        Node *newNode = (Node *)malloc(sizeof(Node));
        newNode->c = (char)ch;
        newNode->next = NULL;

        if (head == NULL) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }

        freq[ch]++;
    }
    fclose(fp);

    //以下「逐次找法 (O(n^2))」與「用 freq 陣列 (O(n))」：
    //1. 逐次找法：對每個字元都掃一次整個 linked list。
    //2. freq 陣列：直接查看 freq[ch]。

    printf("逐次找法計算字元出現次數\n");
    int printed[256];
    memset(printed, 0, sizeof(printed));

    Node *cur = head;
    while (cur != NULL) {
        unsigned char cval = (unsigned char)cur->c;
        if (!printed[cval]) {
            int count = getFrequency(head, cur->c);

            if (cval == '\n') {
                printf("\\n : %d\n", count);
            } else if (cval == '\r') {
                printf("\\r : %d\n", count);
            } else if (cval == '\t') {
                printf("\\t : %d\n", count);
            } else {
                printf("%c : %d\n", cval, count);
            }
            printed[cval] = 1;
        }
        cur = cur->next;
    }

    printf("利用 freq 陣列\n");
    memset(printed, 0, sizeof(printed));

    cur = head;
    while (cur != NULL) {
        unsigned char cval = (unsigned char)cur->c;
        if (!printed[cval]) {
            int count = freq[cval];
            if (cval == '\n') {
                printf("\\n : %d\n", count);
            } else if (cval == '\r') {
                printf("\\r : %d\n", count);
            } else if (cval == '\t') {
                printf("\\t : %d\n", count);
            } else {
                printf("%c : %d\n", cval, count);
            }
            printed[cval] = 1;
        }
        cur = cur->next;
    }

    //釋放 linked list
    Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
