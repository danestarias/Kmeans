#include <iostream>
#include <string>
#include <cassert>
#include <zmqpp/zmqpp.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <math.h>
#include <vector>
#include <map>

using namespace std;
using namespace zmqpp;

class Kcenter{
  private:
    int x;
    int y;


  public:
    map<string, double> distancias; //distancia con respecto a cada punto  <"px,py",distancia>

    // CONSTRUCTOR
    Kcenter(){}
    Kcenter(const int &x, const int &y) {
      this->x = x;
      this->y = y;
    }
    // GETTERS
    const int &getx() const { return x; }
    const int &gety() const { return y; }

    map<string, double> getdistancias(){ 
      //cout<<"me llamaron ostia"<<endl;
      //cout<<"esta es la distancia__"<<distancias["0,1"]<<endl;
      return this->distancias; 
    }

    map<string, double> setdistancias(map<string, double> news){ 
      //cout<<"me llamaron ostia"<<endl;
      //cout<<"esta es la distancia__"<<distancias["0,1"]<<endl;
      return this->distancias = news; 
    }




};

void printMap(map<int, string> dic, bool index) {
  for (auto elem : dic) {
    if (index) {
      cout << to_string(elem.first) << ")- " << elem.second << "\n";
    } else {
      cout << "-) " << elem.second << "\n";
    }
  }
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


//o<<cantK(int)<<"k1:2,3&k2:2,3"<<canRelaciones<<"1:2,3,5,7,10,2000&1:2,3,5,7,10,2000"

int main(){

  cout<<"ESTOY ENCARGADO DE AGRUPAR LA INFO DE LOS WORKERS!!!\n";
  string fromWorker;

  context ctx;
  socket r(ctx,socket_type::pull);

  cout << "Escuchando socket tcp puerto 5513\n";
  r.bind("tcp://*:5513"); //digo direccion del socket del servidor
  

  // INICIO SOCKET PARA ENVIAR
  socket s(ctx,socket_type::push);

  cout << "Conectando al puerto tcp 5443\n";
  s.connect("tcp://localhost:5443"); //digo direccion del socket del servidor







  poller p;
  p.add(r, poller::poll_in);

  while (true) {
    // cout<< "\nEsperando mensajes de los clientes****!\n";
    if(p.poll(500)){
      cout<< "\nLLEGO ALGO!!"<<endl;

      message m;
      r.receive(m);
      m>>fromWorker;

      //int cantK;
      vector<string> kcenters;

      vector<string> kinfo;
      vector<string> coorKinfo;
      vector<string> pinfo;
      vector<string> patrib;

      vector<Kcenter> KLcenters;
      
      cout<<fromWorker<<endl;

      //kx,ky: px,py>distance; px,py>distance; px,py>distance;  &kx,ky:px,py>distance;px,py>distance;&

      kcenters = split(fromWorker,'&');
      //ASIGNO LOS PUNTOS A LOS K
      for(const string &center : kcenters){
        kinfo = split(center,':');          // kx,ky | px,py>distance;px,py>distance;
        coorKinfo = split(kinfo[0],',');                // kx | ky

        Kcenter k(atoi(coorKinfo[0].c_str()), atoi(coorKinfo[1].c_str()) );
        //cout<<"K->"<<coorKinfo[0]<<","<<coorKinfo[1]<<endl;

        pinfo = split(kinfo[1],';');    // px,py>distance | px,py>distance | px,py>distance |
        for(const string &info : pinfo){
          patrib = split(info,'>');                               // px,py | distance

          //cout<<"llave::: "<<patrib[0]<<endl;
          k.distancias[patrib[0]] = atof(patrib[1].c_str());      //el id son las cordenadas y el valor la distanci
          //cout<<patrib[0]<<"-->"<<k.distancias[patrib[0]]<<endl;

        }
        KLcenters.push_back(k);

      }


      //ASIGNO PUNTOS A Kcenters

      for (vector<Kcenter>::iterator it = KLcenters.begin() ; it != KLcenters.end(); ++it){
        //cout<<"entre"<<endl;
        //cout<<doubleToString(*it->getdistancias)<<endl;

        auto nx = next(it, 1);

        if(nx != KLcenters.end()){

          for (vector<Kcenter>::iterator it2 = nx ; it2 != KLcenters.end(); ++it2){

            for (auto elem : it->getdistancias()) {//para recorrer las llaves
              map<string,double> auxdist2= it2->getdistancias();
              map<string,double> auxdist= it->getdistancias();

              if(auxdist[elem.first] > auxdist2[elem.first]){
                auxdist.erase(elem.first);
                cout<<"K->"<<it->getx()<<","<<it->gety()<<endl;
                cout<<elem.first<<endl;
              }else{
                auxdist2.erase(elem.first);
                cout<<"Kelse->"<<it2->getx()<<","<<it2->gety()<<endl;
                cout<<elem.first<<endl;
              }

              it->setdistancias(auxdist);
              it2->setdistancias(auxdist2);

            }

          }



        }

      }


      // IMPRIMO LOS PUNTOS CORRESPONDIENTES A CADA Kcenter
      string results="";
      for (vector<Kcenter>::iterator it = KLcenters.begin() ; it != KLcenters.end(); ++it){
          cout<<"LOS ORIGINALESSSSSSSSS"<<endl;
          results += to_string(it->getx())+","+to_string(it->gety())+":";

          for (auto elem : it->getdistancias()) {
            cout<<"K->"<<it->getx()<<","<<it->gety()<<endl;
            cout<<elem.first<<endl;

            results += elem.first+";";
          }
          results +='&';
      }
      cout<<"RESULTADO---------------------------"<<endl;
      cout<<results<<endl;

      message r;

      r<<results;

      s.send(r);



    }
          
  }


  return 0;
}



