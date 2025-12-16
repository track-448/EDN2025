#include <stdio.h>
#include <stdlib.h>

typedef struct pedido{
    char destino[30];
    int cantidad;
    struct pedido *sig;
}pedido;

typedef struct nodo{
    int fecha,stock,altura;
    char producto[30];
    pedido *cola;
    struct nodo *izq,*der;
}nodo;

void copiar(char *d,char *o){while((*d++=*o++));}
int igual(char *a,char *b){while(*a&&*b&&*a==*b){a++;b++;}return *a==*b;}

int h(nodo *n){return n?n->altura:0;}
int max(int a,int b){return a>b?a:b;}

nodo* nuevo(int f,int s,char *p){
    nodo *n=malloc(sizeof(nodo));
    n->fecha=f;n->stock=s;n->altura=1;
    copiar(n->producto,p);
    n->cola=NULL;n->izq=n->der=NULL;
    return n;
}

nodo* rot_der(nodo *y){
    nodo *x=y->izq,*t=x->der;
    x->der=y;y->izq=t;
    y->altura=max(h(y->izq),h(y->der))+1;
    x->altura=max(h(x->izq),h(x->der))+1;
    return x;
}

nodo* rot_izq(nodo *x){
    nodo *y=x->der,*t=y->izq;
    y->izq=x;x->der=t;
    x->altura=max(h(x->izq),h(x->der))+1;
    y->altura=max(h(y->izq),h(y->der))+1;
    return y;
}

int bal(nodo *n){return n?h(n->izq)-h(n->der):0;}

nodo* insertar(nodo *n,int f,int s,char *p,int *ok){
    if(!n)return nuevo(f,s,p);
    if(f<n->fecha)n->izq=insertar(n->izq,f,s,p,ok);
    else if(f>n->fecha)n->der=insertar(n->der,f,s,p,ok);
    else{*ok=0;return n;}
    n->altura=1+max(h(n->izq),h(n->der));
    int b=bal(n);
    if(b>1&&f<n->izq->fecha)return rot_der(n);
    if(b<-1&&f>n->der->fecha)return rot_izq(n);
    if(b>1&&f>n->izq->fecha){n->izq=rot_izq(n->izq);return rot_der(n);}
    if(b<-1&&f<n->der->fecha){n->der=rot_der(n->der);return rot_izq(n);}
    return n;
}

nodo* menor(nodo *n){while(n->izq)n=n->izq;return n;}

void liberar_cola(pedido *p){
    while(p){pedido *t=p;p=p->sig;free(t);}
}

nodo* eliminar(nodo *r,int f){
    if(!r)return r;
    if(f<r->fecha)r->izq=eliminar(r->izq,f);
    else if(f>r->fecha)r->der=eliminar(r->der,f);
    else{
        liberar_cola(r->cola);
        if(!r->izq||!r->der){
            nodo *t=r->izq?r->izq:r->der;
            free(r);
            return t;
        }
        nodo *t=menor(r->der);
        r->fecha=t->fecha;
        r->stock=t->stock;
        copiar(r->producto,t->producto);
        r->cola=t->cola;
        t->cola=NULL;
        r->der=eliminar(r->der,t->fecha);
    }
    r->altura=1+max(h(r->izq),h(r->der));
    int b=bal(r);
    if(b>1&&bal(r->izq)>=0)return rot_der(r);
    if(b>1&&bal(r->izq)<0){r->izq=rot_izq(r->izq);return rot_der(r);}
    if(b<-1&&bal(r->der)<=0)return rot_izq(r);
    if(b<-1&&bal(r->der)>0){r->der=rot_der(r->der);return rot_izq(r);}
    return r;
}

nodo* buscar_menor(nodo *r){
    if(!r)return NULL;
    while(r->izq)r=r->izq;
    return r;
}

void encolar(nodo *n,char *d,int c){
    pedido *p=malloc(sizeof(pedido));
    copiar(p->destino,d);p->cantidad=c;p->sig=NULL;
    if(!n->cola)n->cola=p;
    else{
        pedido *t=n->cola;
        while(t->sig)t=t->sig;
        t->sig=p;
    }
    n->stock-=c;
}

void cancelar_pedido(nodo *n,char *d){
    pedido *a=NULL,*p=n->cola;
    while(p){
        if(igual(p->destino,d)){
            if(a)a->sig=p->sig;
            else n->cola=p->sig;
            n->stock+=p->cantidad;
            free(p);return;
        }
        a=p;p=p->sig;
    }
}

int contar(pedido *p){
    int c=0;while(p){c++;p=p->sig;}return c;
}

void reporte(nodo *r){
    if(!r)return;
    reporte(r->izq);
    printf("Fecha:%d Producto:%s Stock:%d Pedidos:%d\n",
           r->fecha,r->producto,r->stock,contar(r->cola));
    reporte(r->der);
}

int main(){
    nodo *raiz=NULL;
    int op;
    do{
        printf("\nhola, bienvenido al sistema\n");
        printf("1. recepcion de mercancia\n");
        printf("2. registrar despacho\n");
        printf("3. cancelaciones\n");
        printf("4. reporte de estado\n");
        printf("0. salir\n");
        printf("eliga una opcion: ");
        scanf("%d",&op);

        if(op==1){
            int f,s,ok=1;char p[30];
            printf("fecha de vencimiento (AAAAMMDD): ");
            scanf("%d",&f);
            printf("cantidad: ");
            scanf("%d",&s);
            printf("producto: ");
            scanf("%s",p);
            raiz=insertar(raiz,f,s,p,&ok);
            if(!ok)printf("esa fecha ya existe\n");
        }

        if(op==2){
            nodo *n=buscar_menor(raiz);
            if(!n){printf("no hay productos\n");continue;}
            char d[30];int c;
            printf("destino: ");
            scanf("%s",d);
            printf("Cantidad para despachar: ");
            scanf("%d",&c);
            if(c<=n->stock)encolar(n,d,c);
            else printf("stock insuficiente\n");
        }

        if(op==3){
            int t;
            printf("1. eliminar lote\n2. cancelar pedido\nopcion: ");
            scanf("%d",&t);
            if(t==1){
                int f;
                printf("fecha a eliminar: ");
                scanf("%d",&f);
                raiz=eliminar(raiz,f);
            }else{
                int f;char d[30];
                printf("fecha del lote: ");
                scanf("%d",&f);
                printf("destino del pedido: ");
                scanf("%s",d);
                nodo *n=raiz;
                while(n&&n->fecha!=f)n=f<n->fecha?n->izq:n->der;
                if(n)cancelar_pedido(n,d);
            }
        }

        if(op==4)reporte(raiz);

    }while(op);
}
