#include <stdio.h>
#include <stdlib.h>

typedef struct pasajero {
    char genero[20];
    char apellido[20];
    struct pasajero *sig;
} Pasajero;

Pasajero *inicio = NULL, *fin = NULL, *abordados = NULL, *no_abordados = NULL;
int capacidad = 0, total_vendidos = 0, abordaje_iniciado = 0;

void agregar_pasajero() {
    if (abordaje_iniciado) {
        printf("Ya no se pueden vender tiquetes.\n");
        return;
    }
    if (total_vendidos >= capacidad * 1.1) {
        printf("Limite de sobreventa alcanzado.\n");
        return;
    }
    Pasajero *nuevo = (Pasajero*) malloc(sizeof(Pasajero));
    printf("Genero (Femenino, Masculino, No_Binario): ");
    scanf("%s", nuevo->genero);
    printf("Primer apellido: ");
    scanf("%s", nuevo->apellido);
    nuevo->sig = NULL;
    if (inicio == NULL) inicio = fin = nuevo;
    else { fin->sig = nuevo; fin = nuevo; }
    total_vendidos++;
    printf("Tiquete vendido.\n");
}

void iniciar_abordaje() {
    if (abordaje_iniciado || inicio == NULL) {
        printf("No se puede iniciar abordaje.\n");
        return;
    }
    abordaje_iniciado = 1;
    int cont = 0;
    Pasajero *act = inicio;
    while (act != NULL) {
        Pasajero *sig = act->sig;
        if (cont < capacidad) {
            act->sig = abordados;
            abordados = act;
        } else {
            act->sig = no_abordados;
            no_abordados = act;
        }
        cont++;
        act = sig;
    }
    inicio = fin = NULL;
    printf("Abordaje completado.\n");
}

void mostrar_lista(Pasajero *lista) {
    if (!lista) {
        printf("Lista vacia.\n");
        return;
    }
    Pasajero *act = lista;
    while (act) {
        printf("%s - %s\n", act->apellido, act->genero);
        act = act->sig;
    }
}

int main() {
    int op;
    do {
        printf("\n1. Establecer capacidad\n2. Vender tiquete\n3. Iniciar abordaje\n4. Ver abordados\n5. Ver no abordados\n6. Salir\nOpcion: ");
        scanf("%d", &op);
        switch (op) {
            case 1:
                if (capacidad > 0) printf("Ya establecida.\n");
                else {
                    printf("Capacidad maxima: ");
                    scanf("%d", &capacidad);
                }
                break;
            case 2: if (capacidad > 0) agregar_pasajero(); else printf("Primero establezca capacidad.\n"); break;
            case 3: iniciar_abordaje(); break;
            case 4: mostrar_lista(abordados); break;
            case 5: mostrar_lista(no_abordados); break;
        }
    } while (op != 6);
    return 0;
}
