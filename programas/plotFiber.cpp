/* plotFiber.cpp
recive el nombre de una fibra colectora de luz y genera un grafico con cuatro puntos dando el valor amplitud media
*/

#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include <iostream>
#include "TMath.h"
#include "TGraphErrors.h"

using namespace std;

TGraphErrors *tge;
void AmpStat(string fibra, string pos, float &mean,float &desv);

//mostrar los nombre de las 4 tuplas que leer buscando informacion
TGraphErrors *plotFiber(string fibra)
{
	cout << "\ncalling to plotFiber\n" << endl;
	const char *arr[] = {"-05.1mm.root","-33.9mm.root","-62.7mm.root","-91.5mm.root"};
  float MEAN[4], DESV[4], POSX[4]={5.1,33.9,62.7,91.5}, ePOSX[4]={0,0,0,0};

	for (int i = 0; i<4 ; i++)
	{
		string pos   = arr[i];
		cout << "--------------------------------------------------------" << endl;
		printf("AmpStat(%s, %s, MEAN[%d], DESV[%d]);\n",fibra.c_str(),pos.c_str(),i,i);
		AmpStat( fibra,  pos, MEAN[i], DESV[i] );
	}
	cout << "--------------------------------------------------------\n" << endl;
	//salida de la funcion
	tge = new TGraphErrors(4, POSX, MEAN, ePOSX, DESV);
	tge->Draw();
	return tge;
}




//estadisticas simples de la amplitud
void AmpStat(string fibra, string pos, float &mean,float &desv)
{

	//string raiz = "/Users/pablinho/SILAB/PET/QsF/tuplas/";
	//supongamos que este programa se encuentra en $raiz/git/QsF/programas
	// y que las tuplas procesadas se encuentran en $raiz/tuplas
	string tuplasDir = "../../../tuplas/";

	string target;

	//Abrir los archivos correspondientes
	//target=  tuplasDir + fibra + arr[0];
	target=  tuplasDir + fibra + pos;

	TFile* file0 = new TFile(target.c_str(),"read");
	TTree* tabla0 = (TTree*)file0->Get("tabla");

	Int_t ev;								tabla0->SetBranchAddress("event",&ev);
	Int_t d;								tabla0->SetBranchAddress("dim",&d);
	Float_t* tm = new Float_t[1002];		tabla0->SetBranchAddress("T",tm);
	Float_t* c1 = new Float_t[1002];		tabla0->SetBranchAddress("ch1",c1);
	Float_t* c2 = new Float_t[1002];		tabla0->SetBranchAddress("ch2",c2);
	Float_t* c3 = new Float_t[1002];		tabla0->SetBranchAddress("ch3",c3);

	//revision de los datos evento por evento

	float tped1=-100, tped2=0;

	float max0 = 0;
	float tmax0 = 0;
	int jmax =0;
	float sum0 = 0, sum2 = 0;
	float average0 = 0, varianza = 0;
	cout << "Total de eventos: "<< tabla0->GetEntries() << endl;
	for (int k=0; k<tabla0->GetEntries(); k++)
	{
		tabla0->GetEntry(k);

		//Calculo del pedestal
		int nped=0;
		float ped=0;
		for (int j=0; j<d; j++)
		{
			if ( tped1<=tm[j] && tm[j]<=tped2)
			{
				nped++;
				ped+=c1[j];
			}
		}
		ped/=nped;

		//Calculo de amplitud maxima por evento
		max0=0;
		for (int j=0; j<d; j++)
		{
			if (max0<c1[j])
			{
				max0 = c1[j];
				tmax0 = tm[j];
				jmax = j;
			}
		}
		average0 += (max0-ped); // falta restar pedestal y dividir por numero de muestras
		varianza += (max0-ped)*(max0-ped);
		// estadisticas por evento
		//cout << "Para el evento "<< k<< ", el maximo es "<< max0 << "en el tiempo "<< tmax0 << endl;
	}
	average0 = average0/(tabla0->GetEntries());
	varianza = varianza/(tabla0->GetEntries());

	// efectos en las variables de entrada mean y desv
	mean = average0;
	desv = TMath::Sqrt(varianza-mean*mean);
	float v = (desv*desv)/mean;
	float n = (desv*desv)/(v*v);
	cout << "El promedio es: "<< mean << endl;
	cout << "La varianza es: "<< desv << endl;
	cout << "Pixeles activados: " << n <<endl;
	cout << "Amplitud por pixel: " << v <<endl;
	return;
}
