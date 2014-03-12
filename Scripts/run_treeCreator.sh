remDIR="/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140310/Raw/"

# make sure CENTRAL is the first
sys_cat=("CENTRAL" "ELFRDOWN" "ELFRUP" "MUFRDOWN" "MUFRUP" "ELREDOWN" "ELREUP" "MUREDOWN" "MUREUP" "EESZDOWN"  "EESZUP"  "EERDOWN" "EERUP" "EESLOWDOWN"   "EESLOWUP"  "EESMATDOWN"  "EESMATUP"  "EESPSDOWN" "EESPSUP"  "IDDOWN"  "IDUP"  "JESDOWN" "JESUP" "MSDOWN" "MSUP" "RESOST"   "SCALESTDOWN" "SCALESTUP")

for j in "${!sys_cat[@]}"
do
  rm -f ${remDIR}${sys_cat[$j]}.root
done

source CombineHFTsPerSyst.sh

for j in "${!sys_cat[@]}"
do
  python HistFitter_TreeCreator.py SSBG8TeV ${sys_cat[$j]}
  python HistFitter_TreeCreator.py SSWH8TeV ${sys_cat[$j]}
done

:<<'*/'
fancynames=("CENTRAL" "ELFRDOWN" "ELFRUP"  "MUFRDOWN"    "MUFRUP"   "ELREDOWN"   "ELREUP"   "MUREDOWN"   "MUREUP"  "EESZDOWN"  "EESZUP"  "EERDOWN" "EERUP") 
fancynames2=("EESLOWDOWN"   "EESLOWUP"  "EESMATDOWN"  "EESMATUP"  "EESPSDOWN" "EESPSUP"  "IDDOWN"  "IDUP"  "JESDOWN" "JESUP" "MSDOWN" "MSUP" ) 
fancynames3=("RESOST"   "SCALESTDOWN" "SCALESTUP") 
*/

