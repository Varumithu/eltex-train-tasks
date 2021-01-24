#include "add.h"
#include "subtract.h"
#include "multiply.h"
#include "divide.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Calculator!\n1 - add, 2 - subtract, 3 -");
    printf("multiply, 4 - divide\n");
    
    int choice = 0;
    scanf("%d", &choice);
    printf("enter operands\n");
    int a = 0, b = 0;
    scanf("%d%d", &a, &b);

    switch(choice) {
    case 1:
        printf("%d + %d = %d\n", a, b, add(a, b));
        break;
    case 2:
        printf("%d - %d = %d\n", a, b, subtract(a, b));
        break;
    case 3:
        printf("%d * %d = %d\n", a, b, multiply(a, b));
        break;
    case 4:
        printf("%d / %d = %d\n", a, b, divide(a, b));
        break;
    default:
        printf("NO\n");
        exit(1);
    }
}
