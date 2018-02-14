/*
En un grafico, colocar nueve curvas mostrando la respuesta del MPPC/normalizada con el PIN
a lo largo de la fibra.
Puede ser en terminos de
* la amplitud media
* la carga media
* el numero medio de pixeles activados
Conviene que se lean todas las tuplas (dentro de una lista) almacenando los datos en un arreglo de C++;
* recordar que
* C1 -> MPPC positive pulses
* C2 -> PIN diode monitoring
* C3 -> synchronization signal
*/

/*
v1.0.: crea funcion que tome una tupla y crea un arbol de arreglos
	     TNtuple* data = new TNtuple("data","titulo","time:ch0:...:ch7;event")
       donde las muestras por evento son n, iguales para cada muestra y son
			 dependientes de la tupla a trabajar.
       El arbol resultante es agregado al archivo en el cual se encontraba la
			 tupla, de la forma:
	     TTree* tabla = new TTree("tabla");
	      // event : dim : float* time[dim] : float* ch0[dim] : ....
	       const int dim=100;
      La tupla que se utiliza es una resultante del script example.cxx que
			utiliza la libreria CaenDigitizerSiLab, creada por Orlando.
*/

#include "TTree.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TROOT.h"
#include "iostream"
using namespace std;

// lista de tuplas con datos crudos:
//const char* lista[] = {"/Users/pablinho/SILAB/PET/QsF/tuplas/E1up-05.1mm.root"};

// --------------------------------------------

void tuple2tree(const char* name, int muestras_por_evento=1002)
{
  //Acceso a la tupla
	//TFile* file = new TFile("data_from_digitizer.root","update");
	TFile* file = new TFile(name,"update");
	TNtuple* data = (TNtuple*)file->Get("osc");

///////////////////////////////////////////////////////
///////Inicializacion
//////////////////////////////////////////////////////

	// descripion de la tupla de entrada
	//Evento
	Float_t ev;		data->SetBranchAddress("event",&ev);
	//Tiempo
	Float_t t;		data->SetBranchAddress("time",&t);
	// C1,Ch1
	Float_t C1;		data->SetBranchAddress("C1",&C1);
	// C2, Ch2
	Float_t C2;		data->SetBranchAddress("C2",&C2);
	// C3, Ch3
	Float_t C3;		data->SetBranchAddress("C3",&C3);

	// definicion del arbol de salida
	TTree* tabla = new TTree("tabla","tabla");

  //Definicion de branches del arbol
	Int_t event;
	tabla->Branch("event",&event,"event/I");

	/*La dimension del arreglo depende de las muestras por evento*/
	Int_t dim = muestras_por_evento;
	tabla->Branch("dim", &dim, "dim/I");

	Float_t* T = new float[dim];		tabla->Branch("T",T,"T[dim]/F");

	Float_t* ch1 = new float[dim];		tabla->Branch("ch1",ch1,"ch1[dim]/F");
	Float_t* ch2 = new float[dim];		tabla->Branch("ch2",ch2,"ch2[dim]/F");
	Float_t* ch3 = new float[dim];		tabla->Branch("ch3",ch3,"ch3[dim]/F");


///////////////////////////////////////////////////////
///////Llenado del arbol
//////////////////////////////////////////////////////

  //El siguiente ciclo recorre arreglos y los inicializa en cero.
	for (int l=0;l<dim;l++)	{ T[l]=0; ch1[l]=0; ch2[l]=0; ch3[l]=0; }

	Int_t evAct=0; //Evento actual
	Int_t llArr=0; //Contador para indicar el indice del arreglo
	for (int k=0; k<data->GetEntries(); k++)
	{
		data->GetEntry(k);
		if( int(ev)!=evAct ) //Si pasamos al siguiente evento
		{
      //Entonces guardamos el evento anterior recien procesado
			event=int(ev)-1; //Evento anterior
			tabla->Fill(); //Guardado
			evAct=int(ev); //Pasamos al evento correspondiente
			llArr=0; //Reiniciamos el indice de los arreglos y los canales en cero
			for (int l=0;l<dim;l++) { T[l]=0; ch1[l]=0; ch2[l]=0; ch3[l]=0; }
		}
    //Copiamos el valor de cada canal de la tupla al canal correspidiente en el arbol
		T[llArr]=t*1e9;
		ch1[llArr]=C1*1e3;
		ch2[llArr]=C2*1e3;
		ch3[llArr]=C3*1e3;

		llArr++; //Pasamos a la siguiente muestra (siguiente indice del arreglo)

		if (k==data->GetEntries()-1)//si es la ultima muestra
		{
      //Entonces guardamos este ultimo evento
			event=(int)ev;//ultimo Evento
			tabla->Fill();//Guardamos
			break;
		}
	}
///////////////////////////////////////////////////////
///////Final
//////////////////////////////////////////////////////

	tabla->Write("",TObject::kOverwrite); //agregamos el arbol al archivo
	file->Close(); //Cerramos el archivo
	
	cout << name << " : convertido;" << endl;
}

// --------------------------------------------


