#include<stdio.h>
#include<math.h>

int main() {
    
    double media = 0.0;
    int n;
    scanf("%d", &n);
    double tangente[n];
    double raiz;
    double soma = 0.0;

    for(int i = 0; i < n; i++) {
        raiz = sqrt(i);
        tangente[i] = tan(raiz);
        soma = soma + tangente[i];
    }

    media = soma / n;
}