// vim: ts=4:sw=4
#include "TString.h"
class TTree;
class TH2F;
class TH2D;
class TH2;

namespace DrawUtil 
{
    TH2D* triwsmooth( TTree* tree, const char* varstr, const char* name=0, const char* title=0, const char* cutstr="1", TH2D* inputHist=0);
    TH2F* makesignificancehistos(  TTree* tree, int mode, TString id1="m0",TString id2="m12", int   nbinsX=21,int nbinsY=17, float minX=20,float maxX=860, float minY=92.5, float maxY=347.5);

    TH2F* linearsmooth(const TH2& hist, const char* name=0, const char* title=0);
}

