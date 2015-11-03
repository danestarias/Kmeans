#include <iostream>
#include <string>
#include <cassert>
#include <zmqpp/zmqpp.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <list>
#include <vector>
#include <math.h>


#include <unistd.h>



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

map<int, vector<int>> loadFile(string name) {
	int index,post;
	map<int, vector<int>> dic;
	ifstream fe(name);
	
	while(fe >>index >> post){
	  
	  dic[index].push_back(post);
	  dic[post].push_back(index);
	  //cout<<index<<":"<< post<<endl;
	  }
	fe.close();
	//imprimir el diccionario
	/*for (map<int,vector<int>>::iterator indx=dic.begin();indx!=dic.end();++indx ) {
		for (const int &l : indx->second)
			 cout<<indx->first <<":"<<l<<endl;
  }*/
  return dic;
}

//o<<1/4<<cantK(int)<<"k1:2,3&k2:2,3<<canRelaciones<<"1:2,3,5,7,10,2000&1:2,3,5,7,10,2000"/1
void InicialEnv(map<int,vector<int>>  dic,int Numk){
		int tamDic=0;
		int partes=1;
		int it=0;
		int part=1;
		bool ban;
		string aux;
		vector<string> vec; 
		for (map<int,vector<int>>::iterator indx=dic.begin();indx!=dic.end();++indx )
		tamDic++;
		partes =tamDic/1000;	
		
		for (map<int,vector<int>>::iterator indx=dic.begin();indx!=dic.end();++indx ) {
			ban=true;
			aux += to_string(indx->first )+':';
			for (const int &l : indx->second){
				if(ban){
					aux= aux+to_string(l);
					ban=false;
				}else{
					aux= aux +','+to_string(l);
				}
				//cout<<indx->first <<":"<<l<<endl;
			}
			it++;	
			if(it>=1000*part && part<partes){
				part++;
				vec.push_back(aux);
				aux="";
			}else{
				aux=aux+'&';
			}	
		}
		vec.push_back(aux);
		cout<<vec.size()<<endl;
		/*for (const string &f : vec) {
			cout<<f<<"\n"<<"\n"<<endl;
		}*/
			
	}




int main(int argc, char **argv) {
	int NumK=2;
	
	//lsita de adyacencia del grafo
	map<int,vector<int>>  dicc;
	
	//sockets	
	message m;
	context ctx;
	socket sPush(ctx,socket_type::push);
	cout << "Conectando al puerto tcp 5510\n";
	sPush.connect("tcp://localhost:5510");

	socket r(ctx,socket_type::pull);
	cout << "Escuchando socket tcp puerto 5443\n";
	r.bind("tcp://*:5443"); //digo direccion del socket del servidor
	poller p;
	p.add(r, poller::poll_in);


	cout<<"Algoritmo kmeans servidor de K \n";
	
	//operaciones qeu sehacen inicail mente 
	dicc= loadFile("facebook_combined.txt");
	InicialEnv(dicc,NumK);

	
	/*while(1){
		cin>>operacion;
		m<<operacion;
		//split(vertex,' ');
		sPush.send(m);		
	}*/



	//-----------MADE IN DANIEL--------
	cout<<endl;
	cout<<endl;
	cout<<endl;

	string fromN;
	vector<string> kcenters;
	vector<string> kinfo;
	vector<string> coorKinfo;
	vector<string> coorPinfo;
	vector<string> coorP;




			message q;
		    q<<"hola";
		    sPush.send(q);

	


	//3,5:4,5;4,6;5,4;5,6;6,2;6,4;6,5;7,1;7,3;7,4;8,1;8,4;8,5;9,0;9,2;9,5;&2,7:0,7;0,9;1,7;1,8;2,6;2,9;3,7;4,7;4,8;5,8;5,9;&1,0:0,1;1,0;3,0;7,0;&1,3:0,3;2,3;3,2;&
	while (true) {
		// cout<< "\nEsperando mensajes de los clientes****!\n";
	    if(p.poll(500)){
	    	cout<< "\nLLEGO ALGO!!"<<endl;

		    message n;
		    r.receive(n);
		    n>>fromN;

		    string result="";
		    string st="";

		    
		    int i=1;					//usado para numerar k1,k2,k3 etc

		    kcenters = split(fromN,'&');
		    cout<<"cantidad de centers --->"<<kcenters.size()<<endl;
		    for(const string &center : kcenters){
		    	int sumPx=0;
				int sumPy=0;
				int PromX=0;
				int PromY=0;
				double desplazamiento;

		    	kinfo = split(center,':');          // kx,ky | px,py;px,py;px,py
		        coorKinfo = split(kinfo[0],',');                // kx | ky
		        Kcenter k(atoi(coorKinfo[0].c_str()), atoi(coorKinfo[1].c_str()) );
		        cout<<"K-->"<<coorKinfo[0]<<","<<coorKinfo[1]<<endl;

		        coorPinfo = split(kinfo[1],';');	// px,py | px,py | px,py
		        for(const string &coor : coorPinfo){
		        	coorP = split(coor,',');		// px | py
		        	sumPx += atoi(coorP[0].c_str());
		        	sumPy += atoi(coorP[1].c_str());
		        }
		        cout<<"pinfo: "<<coorPinfo.size()<<endl;
		        PromX = sumPx/coorPinfo.size();
		        PromY = sumPy/coorPinfo.size();

		        desplazamiento = sqrt( (PromX-atoi(coorKinfo[0].c_str()))*(PromX-atoi(coorKinfo[0].c_str()))
		        					  +(PromY-atoi(coorKinfo[1].c_str()))*(PromY-atoi(coorKinfo[1].c_str())) );
		        cout<<"desplazamiento: "<<doubleToString(desplazamiento)<<endl;

		        //k1:3,5&k2:2,7&k3:1,0&k4:1,3
		       	result += "k"+to_string(i)+":"+doubleToString(PromX)+","+doubleToString(PromY)+"&";
		       	i++;
		    }

		    st = result.substr(0, result.size()-1);


		    cout<<st<<endl;
		    //return 0;
		    //envio nuevos K
		    message newsK;
		    newsK<<st;
		    sPush.send(newsK);

	   	}
	}




	return 0;
}






