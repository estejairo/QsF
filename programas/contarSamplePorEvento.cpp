//contar samples por evento, reportar informacion por pantalla
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TROOT.h"
#include <iostream>

using namespace std;

int *contarSamplePorEvento(const char* nombre="tuplaOr.root")
{
	//abrir tupla, encontrar num eventos
	TFile* file = new TFile(nombre,"read");
	TNtuple* osc = (TNtuple*)file->Get("osc");
	osc->Print();
	float ev;
	osc->SetBranchAddress("event",&ev);
	osc->GetEntry(osc->GetEntries()-1);
	int Nev=int(ev);
	
	//crear arreglos para guardar la cuenta de samples
	int *ns = new int[Nev];
	for ( int k=0; k<Nev; k++) ns[k]=0;

	
	//recorrer tupla identificar eventos y sumar los samples encontrados
	
	for ( int row=0; row<=osc->GetEntries()-1; row++){
		osc->GetEntry(row);
		//float(ev) esta actualizado en este punto
		ns[int(ev)]=ns[int(ev)]+1;
	}
	
	return ns;
	//informar resultados
	//for (int k=0; k<Nev; k++)
	//	cout << "ns[" << k << "]=" << ns[k] << endl;
		
	
}
