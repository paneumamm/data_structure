#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct Polynomial {
   float coef;
   int expn;
   struct Polynomial* next; 
}Polyn;


Polyn* InitPolyn() {
    Polyn* p = (Polyn*)malloc(sizeof(Polyn));
    if(p == NULL) {
        fprintf(stderr, "内存分配失败");
        exit(1);
    }  
    
    p->coef = 0.0f;
    p->expn = -1;
    p->next = NULL;

    return p;
}

//根据指数查找该项是否存在
int LocatePolyn(Polyn* head, int expn) {
    Polyn* cur = head->next;
    while(cur != NULL) {
        //多项式默认为升序排列，若指数过大，直接返回0
        if(cur->expn > expn) {
            return 0;
        }

        if(cur->expn == expn) {
            return 1;
        }else{
            cur = cur->next;
        }
    }

    return 0;
}

void InsertAfterPolyn(Polyn* head, float coef, int expn) {
    if(head == NULL || coef < 1e-6) {
        return;
    }


    Polyn* prev = head;
    Polyn* cur = head->next;

    //找到与大于或等于插入项指数的节点
    while(cur != NULL && cur->expn < expn) {
        prev = cur;
        cur = cur->next;
    }
    //尾插及大指数项前插
    if(cur == NULL || cur->expn > expn) {
         Polyn* node = (Polyn*)malloc(sizeof(Polyn));
        if(node == NULL) {
            fprintf(stderr, "内存分配失败");
            exit(1);
        }
        node->coef = coef;
        node->expn = expn;

        node->next = cur;
        prev->next = node;
    
    }else {
        //同指数项合并
        if(fabs(cur->coef + coef) < 1e-6) {
            prev->next = cur->next;
            free(cur);
        }else {
            cur->coef = cur->coef + coef;
        }
    }
}

//回收指针
void Destroy(Polyn* head) {
    if(head == NULL) {
        return;
    }

    Polyn* p = head;
    Polyn* q;
    while(p != NULL) {
        q = p;
        p = p->next;
        free(q);
    }
}

Polyn* CreatePolyn(int m) {
    Polyn* head = InitPolyn();

    for(int i = 0; i < m; i++) {
        float coef;
        int expn;
        if(scanf("%f %d", &coef, &expn) != 2) {
            fprintf(stderr, "输入格式错误\n");
            Destroy(head);
            exit(EXIT_FAILURE);
        }
        InsertAfterPolyn(head, coef, expn);
    }

    return head;
}


//多项式加法实现
Polyn* AddPolyn(Polyn* add1, Polyn* add2) {
    Polyn* head = InitPolyn();

    Polyn* p1 = add1->next;
    Polyn* p2 = add2->next;

    //先处理完最短的
    while(p1 != NULL && p2 != NULL) {
        int e1 = p1->expn;
        int e2 = p2->expn;
        float c1 = p1->coef;
        float c2 = p2->coef; 

        if(e1 == e2) {
           if(fabs(c1 + c2) < 1e-6) {
                p1 = p1->next;
                p2 = p2->next;
                continue;
           }else{
                InsertAfterPolyn(head, c1 + c2, e1);
                p1 = p1->next;
                p2 = p2->next;
           }
        }else if(e1 < e2) {
            InsertAfterPolyn(head, c1, e1);
            p1 = p1->next;
        }else{
            InsertAfterPolyn(head, c2, e2);
            p2 = p2->next;
        }
    }


    //处理剩余的
    while(p1 != NULL) {
        InsertAfterPolyn(head, p1->coef, p1->expn);
        p1 = p1->next;
    }

    while(p2 != NULL) {
        InsertAfterPolyn(head, p2->coef, p2->expn);
        p2 = p2->next;
    }

    return head;
}

//打印整个多项式
void PrintPolyn(Polyn* head) {
    //判断头指针是否为空
    if(head == NULL) { 
        return;
    }

    Polyn* p = head->next;
    //空多项式默认为0
    if(p == NULL) {
        printf("0\n");
        return;
    }
    //输出美观化，将符号与系数分开输出
    int first = 1;
    while(p != NULL) {
        float c = p->coef;
        int e = p->expn;
        //首项无需符号分离
        if(first) {
            if(c < 0) {
                printf("-");
            }
            first = 0;
        }else {
            if(c < 0) {
                printf(" - ");
            }if(c > 0){
                printf(" + ");
            }
        }

        c = fabs(c);
        //将指数为0和1的情况单独讨论
        if(e != 0 && e != 1) {
            printf("%gx^%d", c, e);
        }else if(e == 1) {
            printf("%gx",c);
        }else{
            printf("%g",c);
        }
           
        p = p->next;
    }
    printf("\n");
}

// ANSI 颜色定义
#define COLOR_RESET  "\033[0m"
#define COLOR_RED    "\033[31m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_CYAN   "\033[36m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BOLD   "\033[1m"

// 比较两个多项式是否相同
int EqualPolyn(Polyn* A, Polyn* B) {
    Polyn* p = A->next;
    Polyn* q = B->next;

    while (p != NULL && q != NULL) {
        if (p->expn != q->expn ||
            fabs(p->coef - q->coef) > 1e-6) {
            return 0;
        }

        p = p->next;
        q = q->next;
    }

    return p == NULL && q == NULL;
}

// 打印多项式表达式，不修改链表
void PrintTestPolyn(Polyn* P) {
    PrintPolyn(P);
}

// 执行一组测试
void RunTest(const char* name,
             Polyn* A, Polyn* B, Polyn* expected,
             int* pass, int* total) {

    Polyn* actual = AddPolyn(A, B);
    int success = EqualPolyn(actual, expected);

    (*total)++;

    printf("\n%s+--------------------------------------------------+\n",
           COLOR_CYAN);
    printf("| %-48s |\n", name);
    printf("+--------------------------------------------------+\n");
    printf("| 实际结果：");
    PrintTestPolyn(actual);

    printf("| 预期结果：");
    PrintTestPolyn(expected);

    if (success) {
        printf("| 测试状态：%sPASS%s\n",
               COLOR_GREEN, COLOR_RESET);
        (*pass)++;
    } else {
        printf("| 测试状态：%sERROR%s\n",
               COLOR_RED, COLOR_RESET);
    }

    printf("+--------------------------------------------------+%s\n",
           COLOR_RESET);

    Destroy(actual);
}

int main(void) {
    int pass = 0;
    int total = 0;

    Polyn *A, *B, *E;

    printf("%s%s\n", COLOR_BOLD, COLOR_CYAN);
    printf("====================================================\n");
    printf("           一元多项式加法自动化测试系统             \n");
    printf("====================================================\n");
    printf("%s", COLOR_RESET);


    // 测试1：普通相加
    A = InitPolyn();
    B = InitPolyn();
    E = InitPolyn();

    InsertAfterPolyn(A, 1, 0);
    InsertAfterPolyn(A, 3, 2);
    InsertAfterPolyn(A, 5, 4);

    InsertAfterPolyn(B, 2, 0);
    InsertAfterPolyn(B, 2, 4);
    InsertAfterPolyn(B, -1, 6);

    InsertAfterPolyn(E, 3, 0);
    InsertAfterPolyn(E, 3, 2);
    InsertAfterPolyn(E, 7, 4);
    InsertAfterPolyn(E, -1, 6);

    RunTest("TEST 1: 普通多项式相加",
            A, B, E, &pass, &total);

    Destroy(A);
    Destroy(B);
    Destroy(E);


    // 测试2：没有公共指数
    A = InitPolyn();
    B = InitPolyn();
    E = InitPolyn();

    InsertAfterPolyn(A, 1, 1);
    InsertAfterPolyn(A, 2, 3);
    InsertAfterPolyn(A, 3, 5);

    InsertAfterPolyn(B, 4, 0);
    InsertAfterPolyn(B, 5, 2);
    InsertAfterPolyn(B, 6, 4);

    InsertAfterPolyn(E, 4, 0);
    InsertAfterPolyn(E, 1, 1);
    InsertAfterPolyn(E, 5, 2);
    InsertAfterPolyn(E, 2, 3);
    InsertAfterPolyn(E, 6, 4);
    InsertAfterPolyn(E, 3, 5);

    RunTest("TEST 2: 没有公共指数",
            A, B, E, &pass, &total);

    Destroy(A);
    Destroy(B);
    Destroy(E);


    // 测试3：所有指数相同
    A = InitPolyn();
    B = InitPolyn();
    E = InitPolyn();

    InsertAfterPolyn(A, 1, 0);
    InsertAfterPolyn(A, 2, 2);
    InsertAfterPolyn(A, 3, 4);

    InsertAfterPolyn(B, 10, 0);
    InsertAfterPolyn(B, 20, 2);
    InsertAfterPolyn(B, 30, 4);

    InsertAfterPolyn(E, 11, 0);
    InsertAfterPolyn(E, 22, 2);
    InsertAfterPolyn(E, 33, 4);

    RunTest("TEST 3: 所有指数相同",
            A, B, E, &pass, &total);

    Destroy(A);
    Destroy(B);
    Destroy(E);


    // 测试4：正负系数抵消
    A = InitPolyn();
    B = InitPolyn();
    E = InitPolyn();

    InsertAfterPolyn(A, 5, 0);
    InsertAfterPolyn(A, 3, 2);
    InsertAfterPolyn(A, 7, 4);

    InsertAfterPolyn(B, -5, 0);
    InsertAfterPolyn(B, -3, 2);
    InsertAfterPolyn(B, 2, 6);

    InsertAfterPolyn(E, 7, 4);
    InsertAfterPolyn(E, 2, 6);

    RunTest("TEST 4: 正负系数抵消",
            A, B, E, &pass, &total);

    Destroy(A);
    Destroy(B);
    Destroy(E);


    // 测试5：A为空多项式
    A = InitPolyn();
    B = InitPolyn();
    E = InitPolyn();

    InsertAfterPolyn(B, 3, 1);
    InsertAfterPolyn(B, 2, 3);

    InsertAfterPolyn(E, 3, 1);
    InsertAfterPolyn(E, 2, 3);

    RunTest("TEST 5: A为空多项式",
            A, B, E, &pass, &total);

    Destroy(A);
    Destroy(B);
    Destroy(E);


    // 测试6：A存在剩余项
    A = InitPolyn();
    B = InitPolyn();
    E = InitPolyn();

    InsertAfterPolyn(A, 1, 0);
    InsertAfterPolyn(A, 2, 2);
    InsertAfterPolyn(A, 3, 4);
    InsertAfterPolyn(A, 4, 6);

    InsertAfterPolyn(B, 5, 0);
    InsertAfterPolyn(B, 6, 2);

    InsertAfterPolyn(E, 6, 0);
    InsertAfterPolyn(E, 8, 2);
    InsertAfterPolyn(E, 3, 4);
    InsertAfterPolyn(E, 4, 6);

    RunTest("TEST 6: A存在剩余项",
            A, B, E, &pass, &total);

    Destroy(A);
    Destroy(B);
    Destroy(E);


    // 汇总测试结果
    printf("\n%s%s", COLOR_BOLD, COLOR_CYAN);
    printf("====================================================\n");
    printf("                     测试报告                       \n");
    printf("====================================================\n");
    printf("%s", COLOR_RESET);

    printf("  测试总数：%d\n", total);
    printf("  %s通过数量：%d%s\n",
           COLOR_GREEN, pass, COLOR_RESET);
    printf("  %s失败数量：%d%s\n",
           COLOR_RED, total - pass, COLOR_RESET);

    if (pass == total) {
        printf("\n  %s%s[ ALL PASS ] 所有测试通过！%s\n",
               COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
    } else {
        printf("\n  %s%s[ FAILED ] 存在测试错误！%s\n",
               COLOR_BOLD, COLOR_RED, COLOR_RESET);
    }

    printf("%s====================================================%s\n",
           COLOR_CYAN, COLOR_RESET);

    return 0;
}    
