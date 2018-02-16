#include "TTree.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TROOT.h"
#include "iostream"
using namespace std;

// prototipos de fuciones
void tupleData2treeData(const char* rawTupleFile);
void treeData2treeInfo( const char* treeDirName );

void analysis(void)
{
	//recorre la carpeta tuplasDIR/"tuplas-crudas" y crea arbolesDIR/"arbole-informacion"
	const char *FIB[9] = {"S1up","S2up","S3up","M1up","M2up","F1up","F2up","E1up","E2up",};
  const char *POS[4] = {"-05.1mm","-33.9mm","-62.7mm","-91.5mm"};
	for (int i = 0; i<9 ; i++)
  {
    string fib = FIB[i];
    for (int j = 0; j<4 ; j++)
    {
      string pos 		= POS[j];
			string tuplasFile =  "../../../tuplas/" + fib + pos + ".root";
			string arbolesFile =  "../../../arboles/" + fib + pos + ".root";
			// TTree* treeData = tupleData2treeData(tuplasFile.c_str());
			tupleData2treeData( tuplasFile.c_str() );
			treeData2treeInfo( arbolesFile.c_str() );
    }
  }
}


// --------------------------------------------
//estadisticas simples de la amplitud
void treeData2treeInfo( const char* treeDirName )
{
	TFile* auxFile = new TFile("./auxFile.root","read");
	TTree* treeData = (TTree*)auxFile->Get("tabla");
	TFile* file0 = new TFile(treeDirName,"recreate");

	// arbol con datos crudos de entrada (treeData)
	Int_t ev;													treeData->SetBranchAddress("event",	&ev);
	Int_t dim=1002;										treeData->SetBranchAddress("dim",		&dim);
	Float_t* tm = new Float_t[dim];		treeData->SetBranchAddress("T",			tm);
	Float_t* c1 = new Float_t[dim];		treeData->SetBranchAddress("ch1",		c1);
	Float_t* c2 = new Float_t[dim];		treeData->SetBranchAddress("ch2",		c2);
	Float_t* c3 = new Float_t[dim];		treeData->SetBranchAddress("ch3",		c3);

	//agregar arbol con informacion extraida para construir histogramas de amplitud normalizada
	TTree* info = new TTree("info","info");
	info->Branch("event",	&ev,	"event/I");
	info->Branch("dim",		&dim, "dim/I");
	info->Branch("T",			tm,		"T[dim]/F");
	info->Branch("ch1",		c1,		"c1[dim]/F");
	info->Branch("ch2",		c2,		"c2[dim]/F");
	info->Branch("ch3",		c3,		"c3[dim]/F");
	Float_t aMPPC=0;
	info->Branch("aMPPC",	&aMPPC,"aMPPC/F");
	Float_t vPIN=0;
	info->Branch("vPIN",	&vPIN,	"vPIN/F");

	//revision de los datos evento por evento

	float tped1=-100, tped2=0;
	float max0 = 0, tmax0 = 0;
	int jmax =0;
	float sum0 = 0, sum2 = 0;
	float average0 = 0, varianza = 0;

	cout << "Total de eventos: "<< treeData->GetEntries() << endl;
	for (int k=0; k<treeData->GetEntries(); k++)
	{
		// direccionar variables conteniendo datos del evento: event,tm,c1,c2,c3
		treeData->GetEntry(k);

		//Calculo del pedestal -> ped
		int nped=0;
		float ped=0;
		for (int j=0; j<dim; j++)
		{
			if ( tped1<=tm[j] && tm[j]<=tped2)
			{
				nped++;
				ped+=c1[j];
			}
		}
		ped/=nped;

		//Calculo de amplitud maxima por evento -> aMPPC = max -ped
		max0 = -100;
		for (int j=0; j<dim; j++)
		{
			if (max0<c1[j])
			{
				max0 = c1[j];
				tmax0 = tm[j];
				jmax = j;
			}
		}
		aMPPC = (max0-ped);

		// calculo de valor medio para el voltage de salida del diodo PIN -> vPIN
		vPIN = 0;
		int nPIN = 0;
		for (int j=0; j<dim; j++)
		{
			nPIN++;
			vPIN+=c2[j];
		}
		vPIN/=nPIN;

		//agregar fila k-esima del arbol info
		info->Fill();
	}

	//guardar y cerrar cosas
	info->Write("",TObject::kOverwrite);
	auxFile->Close();
	file0->Close();
	return;
}

// --------------------------

// change.0: void tupleData2treeData(const char* name, int muestras_por_evento=1002)
void tupleData2treeData(const char* rawTupleFile)
{
  // --- Acceso a la tupla ---
	// change.1 TFile* file = new TFile(name,"update");

	TFile* file = new TFile(rawTupleFile,"read");
	TNtuple* data = (TNtuple*)file->Get("osc");
	// descripion de la tupla de entrada -> arbol de salida
	// event -> event
	Float_t ev;		data->SetBranchAddress("event",&ev);
	// time -> T
	Float_t t;		data->SetBranchAddress("time",&t);
	// C1		-> ch1
	Float_t C1;		data->SetBranchAddress("C1",&C1);
	// C2		-> ch2
	Float_t C2;		data->SetBranchAddress("C2",&C2);
	// C3		-> ch3
	Float_t C3;		data->SetBranchAddress("C3",&C3);


	// --- definicion del arbol de salida ---
	// change.2 TTree* tabla = new TTree("tabla","tabla");
	TFile* auxFile = new TFile("./auxFile.root","recreate");
	TTree* tabla = new TTree("tabla","tabla");

	//Definicion de branches del arbol
	Int_t event;
	tabla->Branch("event",&event,"event/I");

	/*La dimension del arreglo depende de las muestras por evento*/
	Int_t dim = 1002;
	tabla->Branch("dim", &dim, "dim/I");

	Float_t* T   = new float[dim];		tabla->Branch("T",		T,"T[dim]/F");
	Float_t* ch1 = new float[dim];		tabla->Branch("ch1",ch1,"ch1[dim]/F");
	Float_t* ch2 = new float[dim];		tabla->Branch("ch2",ch2,"ch2[dim]/F");
	Float_t* ch3 = new float[dim];		tabla->Branch("ch3",ch3,"ch3[dim]/F");

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
		T[llArr]  = t*1e9;
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

	// change.3 tabla->Write("",TObject::kOverwrite); //agregamos el arbol al archivo
	tabla->Write("",TObject::kOverwrite);
	file->Close(); //Cerramos el archivo
	auxFile->Close();
	//cout << rawTupleFile << " : convertido;" << endl;
	//return tabla;


}

// --------------------------------------------
