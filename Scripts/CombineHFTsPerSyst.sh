# Script to merge HFTs
inputDIR="/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140310/Raw"

systematics=("NOM" "EL_FR_DOWN" "EL_FR_UP" "MU_FR_DOWN" "MU_FR_UP" "EL_RE_DOWN" "EL_RE_UP" "MU_RE_DOWN" "MU_RE_UP" "EES_Z_DN" "EES_Z_UP" "EER_DN" "EER_UP")
fancynames=("CENTRAL" "ELFRDOWN" "ELFRUP"  "MUFRDOWN"    "MUFRUP"   "ELREDOWN"   "ELREUP"   "MUREDOWN"   "MUREUP"  "EESZDOWN"  "EESZUP"  "EERDOWN" "EERUP") 
for j in "${!systematics[@]}"
do
  hadd ${inputDIR}/${fancynames[$j]}.root ${inputDIR}/${systematics[$j]}_*.root 
done


systematics2=("EES_LOW_DN" "EES_LOW_UP" "EES_MAT_DN" "EES_MAT_UP" "EES_PS_DN" "EES_PS_UP" "ID_DN" "ID_UP"  "JES_DN" "JES_UP" "MS_DN" "MS_UP")
fancynames2=("EESLOWDOWN"   "EESLOWUP"  "EESMATDOWN"  "EESMATUP"  "EESPSDOWN" "EESPSUP"  "IDDOWN"  "IDUP"  "JESDOWN" "JESUP" "MSDOWN" "MSUP" ) 
for j in "${!systematics2[@]}"
do
  hadd ${inputDIR}/${fancynames2[$j]}.root ${inputDIR}/${systematics2[$j]}_*.root 
done


systematics3=("RESOST"  "SCALEST_DN"  "SCALEST_UP")
fancynames3=("RESOST"   "SCALESTDOWN" "SCALESTUP") 
for j in "${!systematics3[@]}"
do
  hadd ${inputDIR}/${fancynames3[$j]}.root ${inputDIR}/${systematics3[$j]}_*.root 
done


# EER_DN_177997.root             EER_DN_177997.root
# EER_UP_177997.root             EER_UP_177997.root
# EES_LOW_DN_177997.root         EES_LOW_DN_177997.root
# EES_LOW_UP_177997.root         EES_LOW_UP_177997.root
# EES_MAT_DN_177997.root         EES_MAT_DN_177997.root
# EES_MAT_UP_177997.root         EES_MAT_UP_177997.root
# EES_PS_DN_177997.root          EES_PS_DN_177997.root
# EES_PS_UP_177997.root          EES_PS_UP_177997.root
# EES_Z_DN_177997.root           EES_Z_DN_177997.root
# EES_Z_UP_177997.root           EES_Z_UP_177997.root
# ID_DN_177997.root              ID_DN_177997.root
# ID_UP_177997.root              ID_UP_177997.root
# JER_177997.root                JER_177997.root
# JES_DN_177997.root             JES_DN_177997.root
# JES_UP_177997.root             JES_UP_177997.root
# MS_DN_177997.root              MS_DN_177997.root
# MS_UP_177997.root              MS_UP_177997.root
# NOM_177997.root                NOM_177997.root
# RESOST_177997.root             RESOST_177997.root
# SCALEST_DN_177997.root         SCALEST_DN_177997.root
# SCALEST_UP_177997.root         SCALEST_UP_177997.root
