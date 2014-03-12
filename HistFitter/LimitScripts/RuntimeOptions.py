# Pass the user input to HF

class RuntimeOptions:

    def __init__(self,

                 doToys,        # Turn on running of toys
                 gridForToys,   # Specify grid to use for toys
                 nToys,         # Specify how many toys to run

                 doExclusion,   # do Exclusion fit
                 doDiscovery,   # do Discovry fit
                 doValidation,  # do Validaiton

                 blindSR,       # Keep blind the SR
                 blindCR,       # Keep blind the CR
                 blindVR,       # Keep blind the VR

                 sigUncert,     # Specify type of sig uncert.  Ex: "NoSys"
                 sigGrid,       # Specify grid Ex: "ModeC"
                 signalRegion,  # Specify SR Ex: "SR4a"
                 leptonChannel, # Specify the two lepton channel

                 bkgFile,       # input background file      
                 sigFile,       # input signal file
                 anaName,       # analysis name where results saved under

                 inputLumi,     # input luminosity for tree
                 outputLumi,    # output luminosity to scale to
                 lumiUnits      # units
                 ):

        # Initialize
        self.doToys        = doToys
        self.gridForToys   = gridForToys
        self.nToys         = nToys
        self.doExclusion   = doExclusion
        self.doDiscovery   = doDiscovery
        self.doValidation  = doValidation
        self.blindSR       = blindSR
        self.blindCR       = blindCR
        self.blindVR       = blindVR
        self.sigUncert     = sigUncert
        self.sigGrid       = sigGrid
        self.signalRegion  = signalRegion
        self.leptonChannel = leptonChannel
        self.bkgFile       = bkgFile
        self.sigFile       = sigFile
        self.anaName       = anaName
        self.inputLumi     = inputLumi
        self.outputLumi    = outputLumi
        self.lumiUnits     = lumiUnits

    # end initialization of class

    # Clear
    def clear(self):
        self.doToys        = False
        self.doExclusion   = False
        self.doDiscovery   = False
        self.doValidation  = False
        self.blindSR       = False
        self.blindCR       = False
        self.blindVR       = False
        self.sigUncert     = ""
        self.sigGrid       = ""
        self.signalRegion  = ""
        self.leptonChannel = -1
        self.bkgFile       = ""
        self.sigFile       = ""
        self.anaName       = ""
        self.inputLumi     = 0.
        self.outputLumi    = 0.
        self.lumiUnits     = ""

    #end clear
