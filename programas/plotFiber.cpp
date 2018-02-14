/* plotFiber.cpp
recive en nombre de una fibra colectora de luz y genera un grafico con cuatro puntos dando el valor amplitud media
*/

#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include "iostream"
using namespace std;

void plotFiber(string fibra)
{
	//mostrar los nombre de las 4 tuplas que leer buscando informacion
	const char *arr[] = {"-05.1mm.root","-33.9mm.root","-62.7mm.root","-91.5mm.root"};

	//string raiz = "/Users/pablinho/SILAB/PET/QsF/tuplas/";
	//supongamos que este programa se encuentra en $raiz/git/QsF/programas
	// y que las tuplas procesadas se encuentran en $raiz/tuplas
	string tuplasDir = "../../../tuplas/";

	string target;
	for (int n=0; n<4; ++n)
	{
		target= tuplasDir + fibra + arr[n];
		printf("%s\n",target.c_str());
	}


	//Abrir los archivos correspondientes
	target=  tuplasDir + fibra + arr[0];
	TFile* file0 = new TFile(target.c_str(),"read");
	TTree* tabla0 = (TTree*)file0->Get("tabla");

	Int_t ev;								tabla0->SetBranchAddress("event",&ev);
	Int_t d;								tabla0->SetBranchAddress("dim",&d);
	Float_t* tm = new Float_t[1002];		tabla0->SetBranchAddress("T",tm);
	Float_t* c1 = new Float_t[1002];		tabla0->SetBranchAddress("ch1",c1);
	Float_t* c2 = new Float_t[1002];		tabla0->SetBranchAddress("ch2",c2);
	Float_t* c3 = new Float_t[1002];		tabla0->SetBranchAddress("ch3",c3);

	target=  tuplasDir + fibra + arr[1];
	TFile* file1 = new TFile(target.c_str(),"read");
	TTree* tabla1 = (TTree*)file1->Get("tabla");

	target=  tuplasDir + fibra + arr[2];
	TFile* file2 = new TFile(target.c_str(),"read");
	TTree* tabla2 = (TTree*)file2->Get("tabla");

	target=  tuplasDir + fibra + arr[3];
	TFile* file3 = new TFile(target.c_str(),"read");
	TTree* tabla3 = (TTree*)file3->Get("tabla");


/*
	tabla0->Print();
	tabla0->Scan();
	tabla1->Print();
	tabla2->Print();
	tabla3->Print();
*/


	//Calculo de pedestal

	//Calculo de Amplitud Media por medicion

	cout << "Calculo de Amplitud Media, primer archivo" << endl;

	float max0 = 0;
	float tmax0 = 0;
	float sum0 = 0;
	float average0 = 0;
	cout << "Total de eventos: "<< tabla0->GetEntries() << endl;
	for (int k=0; k<tabla0->GetEntries(); k++)
	{
		tabla0->GetEntry(k);
		max0=0;

		//Calculo de amplitud maxima por evento
		for (int j=0; j<d; j++)
		{
			if (max0<c1[j])
				max0 = c1[j];
				tmax0 = tm[j];
		}
		cout << "Para el evento "<< k<< ", el maximo es "<< max0 << "en el tiempo "<< tmax0 << endl;
		sum0 += max0;
	}
	average0 = sum0/(tabla0->GetEntries());
	cout << "El promedio es: "<< average0 << endl;

	//float amplitud_media0 =

	//Calculo de amplitud media normalizada


}
