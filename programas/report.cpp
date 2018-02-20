#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include <iostream>
#include "TMath.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TF1.h"
#include "TSpectrum.h"

using namespace std;

//TCanvas *canvas = new TCanvas();
void rutina0(string fibra);
void rutina(string fibra);
void rutina2();

void report(void)
{
  const char *FIB[] = {"S1up","S2up","S3up","M1up","M2up","F1up","F2up","E1up","E2up"};
  //const char *POS[] = {"-05.1mm","-33.9mm","-62.7mm","-91.5mm"};

  for (int i = 0; i<9 ; i++)
  {
    string fib = FIB[i];
    rutina0(fib); // se procesan y juntan en un arbol los datos de cada posicion de una fibra
    rutina(fib); // se generan un histograma para cada fibra.
  }

  rutina2(); // grafico de valor medio versus posicion de cada fibra
}

void rutina0(string fibra)
{
  TFile* auxFile = new TFile("auxFile.root","recreate");
  TTree* auxTree = new TTree("auxTree","auxTree");
  float aMPPC;
  auxTree->Branch("aMPPC",&aMPPC,"aMPPC/F");
  //junta los 4 info-arboles por fibra y crea un arbol con los maximos por eventos
  //const char *FIB[] = {"S1up","S2up","S3up","M1up","M2up","F1up","F2up","E1up","E2up"};
  const char *POS[] = {"-05.1mm","-33.9mm","-62.7mm","-91.5mm"};
  TFile* infoFile[4];
  for (int j=0; j<4; j++)
  {
    string pos = POS[j];
    string arbolNesimosDir = "../../../arboles/" + fibra + pos + ".root";
    cout << "arbol " << j << " : " << arbolNesimosDir.c_str() << endl;
    infoFile[j] = new TFile(arbolNesimosDir.c_str(),"read");
    TTree* info = (TTree*)infoFile[j]->Get("info");
    info->SetBranchAddress("aMPPC",&aMPPC);
    for (int k=0; k<info->GetEntries();k++)
    {
      info->GetEntry(k);
      cout << aMPPC << endl;
      auxTree->Fill();
    }
    //infoFile[j]->Close();
  }
  auxFile->cd();
  auxTree->Write("",TObject::kOverwrite);
  auxFile->Close();
  for (int j=0; j<4; j++)
  {
    infoFile[j]->cd();
    infoFile[j]->Close();
  }
}

void rutina(string fibra)
{
  string target = "./auxFile.root";
  TFile* file = new TFile(target.c_str(),"read");
	TTree* info = (TTree*)file->Get("auxTree");
  TCanvas *canvas = new TCanvas();
  TH1F *H = new TH1F("haMPPC","haMPPC",100,0,12);
  info->Draw("aMPPC>>haMPPC");
  //encontrar peaks con TSpectrum
  int np = 4;
  TSpectrum *s = new TSpectrum(np);
  s->Search(H,1);
  double *xpeaks = s->GetPositionX();
  //float gpp = TMath::Abs(xpeaks[3]-xpeaks[0])/3;
  for (int n=0; n<np; n++)
    cout << "Peak " << n << " : " << xpeaks[n] << endl;

  // encontrar(approximadamente) amplitud media por pixel, y peaks lejanos
  float minS = *(min_element(xpeaks,xpeaks+np)) ;
  float maxS = *(max_element(xpeaks,xpeaks+np)) ;
  float gpp = ( maxS - minS )/(np-1);

  //fittear gaussianas a los maximos relativos
  //TF1 *fmin = new TF1("fmin","gaus",minP-0.3,minP+0.3);
  //H->Fit("fmin","R");
  //TF1 *fmax = new TF1("fmax","[0]*TMath::Gaus(x,[1],[2])",maxP-0.3,maxP+0.3);
  //H->Fit("fmax","R+");
  H->Fit("gaus","0","",minS-0.3,minS+0.3);
  TF1 *fmin = H->GetFunction("gaus");
  float minG0 = fmin->GetParameter(0);
  float minG1 = fmin->GetParameter(1);
  float minG2 = fmin->GetParameter(2);

  cout << "fmin stats." << endl;
  cout << fmin->GetParameter(0) << endl;
  cout << fmin->GetParameter(1) << endl;
  cout << fmin->GetParameter(2) << endl;

  fmin = new TF1("fmin","[0]*exp( -0.5*((x-[1])/[2])^2 )",minS-0.3,minS+0.3);
  fmin->SetParameters(minG0,minG1,minG2);
  fmin->Draw("SAME");

  H->Fit("gaus","0","",maxS-0.3,maxS+0.3);
  TF1 *fmax = H->GetFunction("gaus");
  float maxG0 = fmax->GetParameter(0);
  float maxG1 = fmax->GetParameter(1);
  float maxG2 = fmax->GetParameter(2);

  cout << "fmax stats." << endl;
  cout << fmax->GetParameter(0) << endl;
  cout << fmax->GetParameter(1) << endl;
  cout << fmax->GetParameter(2) << endl;

  fmax = new TF1("fmax","[0]*exp( -0.5*((x-[1])/[2])^2 )",maxS-0.3,maxS+0.3);
  fmax->SetParameters(maxG0,maxG1,maxG2);
  fmax->Draw("SAME");

  cout << "Amplitud por pixel[TSpectrum method]: " << gpp << endl;
  cout << "Amplitud por pixel[TFit gaus method]: " << (maxG1-minG1)/(np-1) << endl;
  //cout << "fmin stats:" << fmin->GetParameter(1) << ", " << fmin->GetParameter(2) << endl;
  //cout << "fmax stats:" << fmax->GetParameter(1) << ", " << fmax->GetParameter(2) << endl;
  //fmax->SetParameters(maxG0,maxG1,maxG2);

  //exportar figura y guardarla
  string figuresDir = "../../../figures/";
  string figureName = figuresDir + fibra + ".pdf";
  cout << "saving figure as :" << figureName.c_str() << endl;
  canvas->SaveAs(figureName.c_str());
}

void rutina2()
{
  // Configuracion de directorio y nombre de archivos
  const char *FIB[] = {"S1up","S2up","S3up","M1up","M2up","F1up","F2up","E1up","E2up"};
  const char *POS[] = {"-05.1mm","-33.9mm","-62.7mm","-91.5mm"};
  float MEAN[4], DESV[4], POSX[4]={5.1,33.9,62.7,91.5}, ePOSX[4]={0,0,0,0};

  // Creacion de canvas y objetos relativos al grafico final
  TCanvas *grafico = new TCanvas();
  TGraph *tge[9];
  auto legend = new TLegend(0.9,0.7,0.52,0.9);
  legend->SetNColumns(2);

  // Lectura y procesamiento de los datos
  string arbolesDir = "../../../arboles/";
  TFile* file[4];
  Float_t aMPPC;
  Float_t vPIN;

  //Variables para el procesamiento de datos
  float sum = 0;
  float sum2 = 0;
  float sumvPIN = 0;
  float meanvPIN = 0;
  int nEvents = 0;

  // Procesamiento de cada fibra
  int ngraph = 0;
  for (int i=0; i<9; i++)
  {
    string fib = FIB[i];

    //Procesamiento de cada posicion
    for (int j=0; j<4; j++)
    {
      string pos = POS[j];
      string target =  arbolesDir + fib + pos + ".root";

      // Apertura del archivo
      file[j] = new TFile(target.c_str(),"read");
      TTree* info = (TTree*)file[j]->Get("info");

      // Punteros para leer elementos del archivo
      info->SetBranchAddress("aMPPC",&aMPPC);
      info->SetBranchAddress("vPIN",&vPIN);

      //Media y desviacion estandar (normalizados por valor de vPIN) para cada posicion
      nEvents = info->GetEntries();
      for ( int k = 0; k<nEvents; k++)
      {
        info->GetEntry(k);
        sum += aMPPC/vPIN;
        sum2 += (aMPPC/vPIN)*(aMPPC/vPIN);
        sumvPIN += vPIN;
      }

      meanvPIN = sumvPIN/nEvents;

      // Resultados
      MEAN[j] = (sum/nEvents)*meanvPIN;
      DESV[j] = TMath::Sqrt((sum2/nEvents)*meanvPIN*meanvPIN-(MEAN[j]*MEAN[j]));

      cout << "-----------------------------------------------"<<endl;
      cout << (fib+pos).c_str() <<endl;
      cout << "mean:" << MEAN[j] <<endl;
      cout << "desv:" << DESV[j] <<endl;
      cout << "-----------------------------------------------"<<endl;

      // Cierre de archivo
      file[j]->cd();
      file[j]->Close();
      sum = 0;
      sum2 = 0;
      sumvPIN = 0;

    }

    // Grafico por fibra
    tge[i] = new TGraph(4, POSX, MEAN);

    // Se configura el grafico en el primer elemento a graficar
    if (ngraph == 0)
    {
      tge[i]->SetTitle("Amplitud media v/s Posicion");
      tge[i]->GetXaxis()->SetTitle("Posicion [mm]");
      tge[i]->GetYaxis()->SetTitle("Amplitud Media [v]");
      tge[i]->SetLineColor((i+1));
      tge[i]->GetHistogram()->SetMaximum(12.);
      tge[i]->GetHistogram()->SetMinimum(0.);
      tge[i]->Draw("AL*");
      ngraph = 1;
    }
    else
    {
      tge[i]->SetLineColor((i+1));
      tge[i]->Draw("L*");
    }

    // Se agrega cada curva a la simbologia del grafico
    legend->AddEntry(tge[i],fib.c_str(),"lp");


  }

  // Se dibuja la leyenda
  legend->Draw();

  // Se guarda el canvas final en formato pdf
  grafico->SaveAs("../../../figures/mediaVSposicion.pdf");



  return;
}
