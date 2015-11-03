#include <iostream>
#include <string>
#include <cassert>
#include <zmqpp/zmqpp.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <math.h>

using namespace std;
using namespace zmqpp;

class Kcenters{
  private:
    string name; 
    int x;
    int y;

  public:
    // CONSTRUCTOR
    Kcenters(){}
    Kcenters(const string name, const int &x, const int &y) {
      this->name = name;
      this->x = x;
      this->y = y;
    }
    // GETTERS
    const string &getname() const { return name; }
    const int &getx() const { return x; }
    const int &gety() const { return y; }

};

double get_distance(int x1, int x2, int y1, int y2){
  return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
}


string doubleToString(double dbl){
  ostringstream strs;
  strs << dbl;
  return strs.str();
}

vector<string> split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while (getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

//o<<cantK(int)<<"k1:2,3&k2:2,3"<<canRelaciones<<"1:2,3,5,7,10,2000&2:1,3,5,7,10,2000"

int main(){

  string coorK;
  string relaciones;

  vector<string> coordenadasK;
  vector<string> listAdya;



  context ctx;

  cout<<"SOY UN WORKER ASDF\n";

  //SOCKET RECIBO
  socket r(ctx,socket_type::pull);
  cout << "Escuchando socket tcp puerto 5510\n";
  r.bind("tcp://*:5510"); //digo direccion del socket del servidor
  

  poller p;
  p.add(r, poller::poll_in);

  // SOCKET ENVIO
  socket s(ctx,socket_type::push);
  cout << "Conectando al puerto tcp 5513\n";
  s.connect("tcp://localhost:5513"); //digo direccion del socket del servidor

  int init = true;


  while(true){

    if (p.poll(500)) {

      cout<< "\nLLEGO ALGO!!"<<endl;
      vector<Kcenters> Klist;


      if(init){
          string pr;
          message prueba;
          r.receive(prueba);
          prueba>>pr;
          cout<<pr;


          //--message m;
          //--s.receive(m);
          // Extraigo informacion de los K
          int cantK;

          //--m>>cantK;
          cantK=4;
          cout<<"Hay "<<cantK<<" Ks"<<endl;
          coorK="k1:3,5&k2:2,7&k3:1,0&k4:1,3";
          //coorK="k1:3,5&k2:2,7";                           //k1:2,3&k2:2,3       coordenadas de los k
          coordenadasK = split(coorK,'&');    

          for (const string &coordenada : coordenadasK) {
            vector<string> aux;
            vector<string> auxCoor;
            aux = split(coordenada,':');      //k1:2,3
            auxCoor = split(aux[1],',');
            Kcenters k(aux[0],atoi(auxCoor[0].c_str()),atoi(auxCoor[1].c_str()) );// creo un objeto k con (nombre,x,y);
            Klist.push_back(k);               // lo guardo en un vector de K
          }



          // Extraigo informacion de las relaciones
          int cantRel;

          //--m>>cantRel;
          cantRel=7;
          cout<<"Hay "<<cantRel<<" relaciones"<<endl;
          //--m>>relaciones;                      //1:2,3,5,7,10,2000&1:2,3,5,7,10,2000   Lista de adyasencia
          relaciones="0:1,3,7,9&1:0,7,8&2:3,6,9&3:0,2,7&4:5,6,7,8&5:4,6,8,9&6:2,4,5&7:0,1,3,4&8:1,4,5&9:0,2,5";
          listAdya = split(relaciones,'&');        //1:2,3,5,7,10,2000

          cout<<"llegue al hpt init"<<endl;
          init=false;


      }else{

          cout<<"LLEGO LA HIUEPUTAAAAAAAAA"<<endl;
          message t;
          r.receive(t);            //k1:6,3&k1:2,7&k1:2,0&k1:1,2&

          // Extraigo informacion de los K
          t>>coorK;
          cout<<"nuevas coordenadas-->"<<coorK;
          //--coorK="k1:3,5&k2:2,7&k3:1,0&k4:1,3";
          //coorK="k1:3,5&k2:2,7";                           //k1:2,3&k2:2,3       coordenadas de los k
          coordenadasK = split(coorK,'&');    

          for (const string &coordenada : coordenadasK) {
            vector<string> aux;
            vector<string> auxCoor;
            aux = split(coordenada,':');      //k1:2,3
            auxCoor = split(aux[1],',');
            Kcenters k(aux[0],atoi(auxCoor[0].c_str()),atoi(auxCoor[1].c_str()) );// creo un objeto k con (nombre,x,y);
            Klist.push_back(k);               // lo guardo en un vector de K
          }

      }



      //INFO DE LA MATRIZ

      vector<string> auxAdya;
      vector<string> friends;
      double distance;

      string results = "";

      //, separacion de coordenadas
      //: separacion de relacion
      //& separacion de kluster

      //kx,ky:px,py>distance
      //kx,ky:px,py>distance;px,py>distance;&kx,ky:px,py>distance;px,py>distance;&
      
      for (Kcenters &kluster: Klist) {     //cada kluster debe obtener su distancia con respecto a cada punto
        results += to_string( kluster.getx() )+","+to_string( kluster.gety() )+':';
        for (const string &relacion : listAdya) {
          auxAdya = split(relacion,':');    // 1 | 2,3,5,7,10,2000    auxAdya[0] --> X
          friends = split(auxAdya[1],',');  // 2 | 3 | 5 | 7 | 10 | 2000
          for(const string &fri : friends){
            distance = get_distance( kluster.getx(),atoi( auxAdya[0].c_str() ),kluster.gety(),atoi(fri.c_str()) );
            results += auxAdya[0] +',' + fri + '>' + doubleToString(distance) + ";";
          }
        }
        results +='&';
        //cout<<results<<endl;
        //cout<<"\n";
      }
      cout<<results<<endl;

      message m;
      m<<results;
      s.send(m);


    }
          
  }


  return 0;
}




