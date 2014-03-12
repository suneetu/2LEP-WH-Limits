
////////////////////////////////////////////////
// The output from running HistFitter.py with //
// -l option produces root files containing   //
// the expected upper limit.  It is easy to   //
// make the necessary files from these, so    //
// basically we will hadd them dynamically    //
// based on the file names                    //
////////////////////////////////////////////////


void makeUpperLimitFiles(char* SR, char* channel, char* grid, char* sys, char* LR, int nFiles=0)
{

  TString append = "_Output_upperlimit.root";
  TString base   = TString(Form("%s_%s_%s%s_%s",SR,channel,grid,LR,sys));
  
  TString command = "hadd -f limitResults/"+ base + "/" + base + append;
  for(int i=0; i<nFiles; ++i)
    command += " ../results/" + TString(Form("%s_%s_%s%i%s_%s",SR,channel,grid,i,LR,sys)) + append;
  
  cout<<"Executing: "<<command<<endl;
  
  system(command.Data());

}
