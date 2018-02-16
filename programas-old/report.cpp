#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include <iostream>
#include "TMath.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TSpectrum.h"

using namespace std;

//TCanvas *canvas = new TCanvas();

void rutina(string fibra, string poss);

void report(void)
{
  const char *FIB[] = {"S1up","S2up","S3up","M1up","M2up","F1up","F2up","E1up","E2up",};
  const char *POS[] = {"-05.1mm","-33.9mm","-62.7mm","-91.5mm"};
  for (int i = 0; i<9 ; i++)
  {
    string fib = FIB[i];
    for (int j = 0; j<4 ; j++)
    {
      string pos = POS[j];
      rutina(fib, pos);
    }
  }
}


void rutina(string fibra, string poss)
{
  string tuplasDir = "../../../tuplas/";
  string figuresDir = "../../../figures/";
  string target =  tuplasDir + fibra + poss + ".root";
  TFile* file = new TFile(target.c_str(),"read");
	TTree* info = (TTree*)file->Get("info");
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
  string figureName = figuresDir + fibra + poss + ".pdf";
  cout << "saving figure as :" << figureName.c_str() << endl;
  canvas->SaveAs(figureName.c_str());
}
