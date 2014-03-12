# This cleans up the main code

class ConfigDefs:

    # Setup the channels like enum
    allCHs  = 0
    ee      = 1
    mm      = 2
    em      = 3
   
    # WH Grid
    SSWH = [ 'SSWH8TeV_130.0_0.0',  
             'SSWH8TeV_140.0_10.0', 
             'SSWH8TeV_150.0_0.0',  
             'SSWH8TeV_152.5_22.5', 
             'SSWH8TeV_162.5_12.5', 
             'SSWH8TeV_165.0_35.0', 
             'SSWH8TeV_175.0_0.0',  
             'SSWH8TeV_175.0_25.0', 
             'SSWH8TeV_177.5_47.5', 
             'SSWH8TeV_187.5_12.5', 
             'SSWH8TeV_187.5_37.5', 
             'SSWH8TeV_190.0_60.0', 
             'SSWH8TeV_200.0_0.0',  
             'SSWH8TeV_200.0_25.0', 
             'SSWH8TeV_200.0_50.0', 
             'SSWH8TeV_202.5_72.5', 
             'SSWH8TeV_212.5_12.5', 
             'SSWH8TeV_212.5_37.5', 
             'SSWH8TeV_212.5_62.5', 
             'SSWH8TeV_225.0_0.0',  
             'SSWH8TeV_225.0_25.0', 
             'SSWH8TeV_225.0_50.0', 
             'SSWH8TeV_237.5_12.5', 
             'SSWH8TeV_237.5_37.5', 
             'SSWH8TeV_250.0_0.0',  
             'SSWH8TeV_250.0_25.0' ]

    def getGrid(self,name):
        if 'SSWH' == name: return self.SSWH

        empty = []
        return empty 
