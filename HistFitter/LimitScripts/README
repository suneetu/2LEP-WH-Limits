To run the Wh Limits:

1) Copy the entire /gdata/atlas/amete/WHLimits2014 directory
2) Go into "HistFitter/" and source "setup_UCIT3.sh" to set up and configure 
   ==> If running for the first time go into "src/" and "make"
3) Inside "HistFitter/" first clean-up the output folders that hold previous results
   ==> rm -rf "data/*" "resuts/*" "Tables/*" "batchLog/*" "batchlog/*"
4) Run the batch script "./LimitScripts/runBatchLimit.sh" 
5) Once the jobs finish, go to "PlotScripts/"
6) Clean current results via "rm -rf limitResults/ FirstTest_20140221/*"
7) Convert HistFitter output to Histgorams to be plotted via "./makeHists.sh"
8) Assuming 7) succeeds, run "allPlots.sh" to make the final plot,
   which will appear under "FirstTest_20140221/"
