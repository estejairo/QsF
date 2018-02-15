#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include <iostream>
#include "TMath.h"
#include "TGraphErrors.h"
#include "TCanvas.h"

using namespace std;

//TCanvas *canvas = new TCanvas();

void report(void)
{
  const char *arr[] = {"-05.1mm","-33.9mm","-62.7mm","-91.5mm"};
}


void rutina(string fibra, string poss)
{
  string tuplasDir = "../../../tuplas/";
  string figuresDir = "../../../figures/";
  string target =  tuplasDir + fibra + poss + ".root";
  string figureName = figuresDir + fibra + poss + ".pdf";
  TFile* file = new TFile(target.c_str(),"read");
	TTree* info = (TTree*)file->Get("info");
  TCanvas *canvas = new TCanvas();
  info->Draw("aMPPC>>haMPPC(100,0,12)");
  cout << "saving figure as :" << figureName.c_str() << endl;
  canvas->SaveAs(figureName.c_str());
}
