# Testing!!!

#Channels=( ee mm em )
Channels=( ee mm em )
SRs=( SR1jet SR23jets ) # SR1jet SR23jets
for chan in "${Channels[@]}"
do
  for sr in "${SRs[@]}"
  do
    HistFitter.py -twf LimitScripts/LimitConfig.py ${chan} ${sr} SSWH NoSys
    YieldsTable.py -b -c ${chan}${sr} -s Fakes,ZV,WW,Top,Zjets,Higgs,DiscoveryMode_SIG -w results/${sr}_${chan}_SSWH_NoSys/TopLvlXML_combined_NormalMeasurement_model_afterFit.root -o Tables/discTable_${chan}_${sr}.tex
    SysTable.py    -% -c ${chan}${sr} -s Fakes,ZV,WW,Top,Zjets,Higgs,DiscoveryMode_SIG -w results/${sr}_${chan}_SSWH_NoSys/TopLvlXML_combined_NormalMeasurement_model_afterFit.root -o Tables/discSysTable_${chan}_${sr}.tex;
  done
done

