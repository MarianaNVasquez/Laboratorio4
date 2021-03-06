#include "network.h"

void network::agregarRuter(char name) //Esta función permite agregar un enrutador a la red
{
    router Inter;
    if(Red.find(name)==Red.end()){
        Inter.agregarEnrutador(name,0);
        for(itN=Red.begin(); itN!=Red.end(); itN++){
            itN->second.agregarEnrutador(name,-1);
            Inter.agregarEnrutador(itN->first,-1);
        }
        Red.insert(pair <char, router>(name,Inter));
    }
}

void network::eliminarRuter(char name) //Esta funcion permite eliminar un enrutador de la red -> Mirar si en ves de eliminarlo del todo se puede poner en sus enlaces un costo de -1
{
    if(Red.find(name)!=Red.end()){
        Red.erase(name);
        for(itN=Red.begin(); itN!=Red.end(); itN++){
            itN->second.eliminarEnrutador(name);
        }
    }
    else cout << "No se pudo eliminar el enrutador. No existe" << endl;
}

void network::imprimirRuter(char name) // Esta funcion permite imprimir la tabla de enrutamiento de un nodo
{
    if(Red.find(name)!=Red.end()){
        cout << "\t------Tabla de enrutamiento------" << endl;
        cout << name;
        Red[name].imprimirTablaEnrutador();
    }
    else cout << "No existe el enrutador" << endl;
}

bool network::agregarEnlace(char name, char nom2, int cost) //Esta funcion permite agregra un enlace entre dos enrutadores de la red. También sirve para modificar en enlace entre dos nodos
{
    bool ban=false;

    if((Red.find(name) != Red.end()) && (Red.find(nom2) != Red.end())){
        if((Red[name].modificarEnrutador(nom2,cost)) && Red[nom2].modificarEnrutador(name,cost)){
            ban=true;
        }
    }
    else cout << "Uno de los enrutadores no existe" << endl;
    return ban;
}

bool network::eliminarEnlace(char name1, char name2) //Esta funcion permite eliminar un enlace de la red poniendo como costo -1
{
    bool band=false;
    if((Red.find(name1)!=Red.end()) && (Red.find(name2)!=Red.end())){
        if((Red[name1].modificarEnrutador(name2,-1)) && (Red[name2].modificarEnrutador(name1,-1))){
            cout << "Enlace eliminado" << endl;
            band=true;
        }
    }
    else cout << "Uno de los enrutadores no existe" << endl;

    return band;
}

void network::matrizadyacencia() //Esta matriz permite imprimir la matriz de adyacencia de la red -> Mejorarlo
{
    map <char,int>::iterator iE;
    bool ban=true;
    cout << "\t------Matriz de Adyacencia de la Red------" << endl;
    for(itN=Red.begin(); itN!=Red.end(); itN++){
        if(ban){
            cout << '\t';
            for(iE=itN->second.ruter.begin(); iE!=itN->second.ruter.end(); iE++){
                cout << iE->first << '\t';
            }
            cout << endl;
            ban=false;
        }
        cout << itN->first << '\t';
        for(iE=itN->second.ruter.begin(); iE!=itN->second.ruter.end(); iE++){
            cout << iE->second << '\t';
        }
        cout << endl;
    }

}

void network::leer() //Esta funcion permite leer un archivo que contiene todos los enlaces
{
    ifstream archivo;
    this->Red.clear();
    char router1, router2, *pnt;
    int peso;
    char linea[10];

    archivo.open("red_prueba.txt");
    if(archivo.is_open()){
        archivo.getline(linea, sizeof (linea));
        while(!archivo.eof()){
            pnt=strtok(linea," ");
            router1=*pnt;
            agregarRuter(router1);
            pnt=strtok(NULL," ");
            router2=*pnt;
            agregarRuter(router2);
            pnt=strtok(NULL,"\n");
            peso=atoi(pnt);
            agregarEnlace(router1,router2,peso);
            archivo.getline(linea, sizeof (linea));
        }
        archivo.close();
        cout << "Red leida exitosamente" << endl;
    }
    else{
        cout << "No se encontro un archivo con dicho nombre" << endl;
        exit(1);
    }
}

void network::red_aleatoria()
{
    this->Red.clear();
    int vertices=0, peso=0, contador=1, contador2=0;
    float Prob=0;
    char nombre='A';
    map <char,int>::iterator iE;

    do{
        cout << "Digite el numero de enrutadores (2-54) y la probabilidad: ";
        cin >> vertices >> Prob;
    } while((vertices < 2 || vertices > 54) && (Prob < 0 || Prob > 1)); //Ver si las condiciones si cumplen

    if((vertices >=2) && (vertices<=54)){
        while(nombre!='A'+vertices){ //Se agregar todos los enrutadores
            agregarRuter(nombre);
            nombre++;
        }
        nombre='A';
        srand(time(NULL));
        for(itN=Red.begin(); itN!=Red.end(); itN++){
            if(itN!=Red.begin() && contador2==0){
                contador--;
                itN--;
            }
            contador2=0;
            for(iE=itN->second.ruter.find(nombre+contador); iE!=itN->second.ruter.end(); iE++){
                if(booleanoAleatorio(Prob)){
                    peso=rand()%100+1;
                    agregarEnlace(itN->first, iE->first, peso);
                    contador2++;
                }
            }
            contador++;
        }
        cout << "Red creada exitosamente" << endl;
    }
    else cout << "La cantidad de enrutadores es invalida" << endl;
}

void network::mostrarEnrutadores() //Esta funcion permite mostrar los enrutadores que hay en la red
{
    cout << "Enrutadores: ";
    for(itN=Red.begin(); itN!=Red.end(); itN++) cout << itN->first << " ";
    cout << endl;
}

bool network::booleanoAleatorio(float p)
{
    unsigned long long num, x;
    x=RAND_MAX*p-1;
    num=rand();
    return num<=x;
}

void network::buscar_camino(char origin, char dest) //Algoritmo de dijkstra
{
    if((Red.find(origin)!=Red.end()) && (Red.find(dest)!=Red.end())){
        struct enlaces{
            struct prueba{
                char previous= ' ';
                int acumulado;
            }temp, perm;
        }link;

        map<char, enlaces> camino;
        map<char, enlaces>::iterator iC;

        link.temp.acumulado=2147483647;
        link.perm.acumulado=-1; //Quiere decir que no hay como un costo acumulado aún

        for(itN=Red.begin(); itN!=Red.end(); itN++) camino.insert(pair<char, enlaces>(itN->first,link));

        char enrutador=origin;
        camino[enrutador].perm.acumulado=0;

        list<int> acumulados;
        list<char> ruters;
        list<char>::iterator iR;

        int minA=0;

        while(true){
            for(itN=Red.begin(); itN!=Red.end(); itN++){
                if((Red[enrutador].costo(itN->first)!=-1) && (Red[enrutador].costo(itN->first)!=0)){
                    bool ban=true;
                    if(!ruters.empty())for(iR=ruters.begin(); iR!=ruters.end(); iR++)if(itN->first==*iR) ban=false;
                    if((camino[enrutador].temp.previous!=itN->first) && (ban)){
                        if(camino[enrutador].perm.acumulado+Red[enrutador].costo(itN->first) < camino[itN->first].temp.acumulado){
                            camino[itN->first].temp.acumulado=camino[enrutador].perm.acumulado+Red[enrutador].costo(itN->first);
                            camino[itN->first].temp.previous=enrutador;
                        }
                    }
                }
            }
            ruters.push_back(enrutador);
            acumulados.clear();
            for(iC=camino.begin(); iC!=camino.end(); iC++){
                bool ban=true;
                for(iR=ruters.begin(); iR!=ruters.end(); iR++) if(iC->first==*iR) ban=false;
                if(ban) acumulados.push_back(iC->second.temp.acumulado);
                ban=true;
            }

            acumulados.sort(); //menor - mayor
            minA=*acumulados.begin();
            if(!acumulados.empty()){
                for(iC=camino.begin(); iC!=camino.end(); iC++){
                    if(iC->second.temp.acumulado==minA){
                        iC->second.perm.acumulado=minA;
                        iC->second.perm.previous=iC->second.temp.previous;
                        iC->second.temp.acumulado=2147483647;
                        enrutador=iC->first;
                        break;
                    }
                }
            }
            else break;
        }

        string ruta, rutaF;
        ruta.push_back(dest);
        char enrutador2 = dest;
        iC=camino.find(dest);
        while(enrutador2!=origin){
            enrutador2=iC->second.perm.previous;
            ruta.push_back('-');
            ruta.push_back(enrutador2); //Organizar esta parte que lo vaya poniendo adelante
            iC=camino.find(enrutador2);
        }
        for(int m=ruta.length()-1; m >= 0; m--) rutaF.push_back(ruta[m]);
        cout << "El camino menos costoso  para ir de "<< origin << " a " << dest << " es:\n " << rutaF << " -> Costo: " << camino[dest].perm.acumulado << endl;
    }
    else cout << "Uno de los enrutadores ingresados no existe" << endl;
}




















