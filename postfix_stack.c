#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef struct StackNode {
    int operand;
    struct StackNode* next;
} StackNode;

void push(StackNode** top, int operand){
    StackNode* n = malloc(sizeof(StackNode));
    n->operand = operand;
    n->next = *top;
    *top = n;
}

int pop(StackNode** top){
    StackNode* t = *top;
    int pop = t->operand;
    *top = t->next;
    free(t);
    return pop;
}

int isOperator(char c){
    return (c=='+' || c=='-' || c=='*' || c=='/' || c=='^');
}

int isDivisionable(int a,int b){
    if(b==0) return 0;
    if(a % b != 0) return 0;
    return 1;
}

int makeCalculation(int left, int right, char ch){
    switch(ch){
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/':
            if(!isDivisionable(left,right))
                return -100;  
            return left / right;
        case '^': return (int)pow((double)left,(double)right);
    }
    return -100;
}

int solutionPostfix(char** tokens, int size,int* caseArr,int numberOfUnknowns,int* operandList,char* operatorList
){
    StackNode* stack = NULL;

    int operand_index = 0;      
    int operator_index = 0;     
    int unknown_index   = 0;  
    
    int i;

    for(i=0;i<size;i++){
        char* t = tokens[i];
        char c = t[0];

        if(isdigit(c)){
            push(&stack, atoi(t));
        }
        else if(isOperator(c)){
            int right = pop(&stack);
            int left = pop(&stack);
            int new = makeCalculation(left, right, c);
            push(&stack, new);
        }
        else if(isalpha(c)){
            if(caseArr[unknown_index] == 0){
                push(&stack, operandList[operand_index++]);
            } else {
                int right = pop(&stack);
                int left = pop(&stack);
                char operator = operatorList[operator_index++];
                int new = makeCalculation(left, right, operator);
                push(&stack, new);
            }
            unknown_index++;
        }
    }

    return pop(&stack);
}

void recCases(int index,int numberOfUnknowns,int* cases,int unknownOperandNumber,int unknownOperatorNumber,int trueCases[][20],int* trueCount){
    if(index == numberOfUnknowns){
        int operandNumber=0, operatorNumber=0;
        int i;
        for(i=0;i<numberOfUnknowns;i++){
            if(cases[i]==0) operandNumber++;
            else operatorNumber++;
        }

        if(operandNumber == unknownOperandNumber && operatorNumber == unknownOperatorNumber){
            for(i=0;i<numberOfUnknowns;i++)
                trueCases[*trueCount][i] = cases[i];
            (*trueCount)++;
        }
        return;
    }

    cases[index] = 0;
    recCases(index+1, numberOfUnknowns, cases,unknownOperandNumber, unknownOperatorNumber,trueCases, trueCount);

    cases[index] = 1;
    recCases(index+1, numberOfUnknowns, cases,unknownOperandNumber, unknownOperatorNumber,trueCases, trueCount);
}


void cases(int numberOfUnknowns,int unknownOperandNumber,int unknownOperatorNumber,int trueCases[][20],int* trueCount){
    *trueCount = 0;
    int Case[20];

    recCases(0,numberOfUnknowns,Case,unknownOperandNumber,unknownOperatorNumber,trueCases,trueCount);
}

void try_unknowns(char** tokens, int size,int* caseArr,int numberOfUnknowns,int unknownOperandNumber,int unknownOperatorNumber,int result){
    int total =(int) pow(20, unknownOperandNumber) * pow(5, unknownOperatorNumber);
    char optList[5] = {'+','-','*','/','^'};

    int operandList[25];
    char operatorList[25];

    int m,i,j;
    int temp;
    int solution;

    for( m=0; m<total; m++){
        temp = m;

        for( i=0;i<unknownOperandNumber;i++){
            operandList[i] = (temp % 20) + 1;
            temp /= 20;
        }

        for(j=0;j<unknownOperatorNumber;j++){
            operatorList[j] = optList[temp % 5];
            temp /= 5;
        }

        solution = solutionPostfix(tokens, size,caseArr,numberOfUnknowns,operandList,operatorList);

        if(solution == result){

            for(i=0;i<unknownOperandNumber;i++)
                printf("%d ", operandList[i]);

            for(j=0;j<unknownOperatorNumber;j++)
                printf("%c ", operatorList[j]);

            printf("\n");
        }
    }
}

int main(int argc, char* arg_n[]){

    FILE* fptr = fopen(arg_n[1],"r");
    if(!fptr){
        printf("File Error");
        return 0;
    }

    char line[320];
    fgets(line,320,fptr);
    fclose(fptr);

    char tokens[60][20];
    char* ptrs[60];
    int size = 0;

    char* t = strtok(line," ");
    while(t && strcmp(t,"=") != 0){
        strcpy(tokens[size], t);
        ptrs[size] = tokens[size];
        size++;
        t = strtok(NULL," ");
    }

    fptr = fopen(arg_n[1], "r");

    int ch = fgetc(fptr);
    int result = 0;
    while (ch != EOF) {
        if (ch == '=') {

            ch = fgetc(fptr);
            while (ch == ' ') ch = fgetc(fptr);

            while (isdigit(ch)) {
                result = result * 10 + (ch - '0');
                ch = fgetc(fptr);
            }
            break;
        }
        ch = fgetc(fptr);
    }
    fclose(fptr);

    
    int numberOfUnknowns = 0;
    int numberOfOperands = 0;
    int numberOfOperators = 0;
    int i;
    for( i=0;i<size;i++){
        char c = tokens[i][0];

        if(isalpha(c))
            numberOfUnknowns++;
        else if(isdigit(c))
            numberOfOperands++;
        else if(isOperator(c))
            numberOfOperators++;
    }

    int unknownOperatorNumber =
        (numberOfOperands + numberOfUnknowns - 1 - numberOfOperators)/2;

    int unknownOperandNumber =
        numberOfUnknowns - unknownOperatorNumber;

    int trueCases[200][20];
    int trueCount = 0;

    cases(numberOfUnknowns,unknownOperandNumber,unknownOperatorNumber,trueCases,&trueCount);

    for( i=0;i<trueCount;i++){
        try_unknowns(ptrs,size,trueCases[i],numberOfUnknowns,unknownOperandNumber,unknownOperatorNumber,result);
    }

    return 0;
}
