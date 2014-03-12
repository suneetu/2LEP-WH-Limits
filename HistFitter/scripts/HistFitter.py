#!/usr/bin/env python
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

from ROOT import gROOT,gSystem,gDirectory,RooAbsData,RooRandom,RooWorkspace
gSystem.Load("libSusyFitter.so")
from ROOT import ConfigMgr
gROOT.Reset()
import os
import argparse

from logger import Logger
log = Logger('HistFitter')

#def enum(typename, field_names):
#    """Create a new enumeration type"""
#
#    if isinstance(field_names, str):
#        field_names = field_names.replace(',', ' ').split()
#    d = dict((reversed(nv) for nv in enumerate(field_names)), __slots__ = ())
#    return type(typename, (object,), d)()


def GenerateFitAndPlotCPP(fc, anaName, drawBeforeFit, drawAfterFit, drawCorrelationMatrix, drawSeparateComponents, drawLogLikelihood, minos, minosPars):
    from ROOT import Util
    
    log.debug('GenerateFitAndPlotCPP: anaName %s ' % anaName)
    log.debug("GenerateFitAndPlotCPP: drawBeforeFit %s " % drawBeforeFit) 
    log.debug("GenerateFitAndPlotCPP: drawAfterFit %s " % drawAfterFit) 
    log.debug("GenerateFitAndPlotCPP: drawCorrelationMatrix %s " % drawCorrelationMatrix) 
    log.debug("GenerateFitAndPlotCPP: drawSeparateComponents %s " % drawSeparateComponents)
    log.debug("GenerateFitAndPlotCPP: drawLogLikelihood %s " % drawLogLikelihood)
    log.debug("GenerateFitAndPlotCPP: minos %s " % minos)
    log.debug("GenerateFitAndPlotCPP: minosPars %s " % minosPars)

    Util.GenerateFitAndPlot(fc.name, anaName, drawBeforeFit, drawAfterFit, drawCorrelationMatrix,
                            drawSeparateComponents, drawLogLikelihood, minos, minosPars)

if __name__ == "__main__":
    from configManager import configMgr

    configMgr.readFromTree = False
    configMgr.executeHistFactory = False
    runInterpreter = False
    runFit = False
    printLimits = False
    doHypoTests = False
    doUL = True           # default is exclusion. goes together with doHypoTests
    drawBeforeFit = False
    drawAfterFit = False
    drawCorrelationMatrix = False
    drawSeparateComponents = False
    drawLogLikelihood = False
    pickedSRs = []
    runToys = False
    runMinos = False
    minosPars = ""
    doCodeProfiling = False
    
    FitType = configMgr.FitType #enum('FitType','Discovery , Exclusion , Background')
    myFitType=FitType.Background
    doValidation = False
    
    print "\n * * * Welcome to HistFitter * * *\n"

    parser = argparse.ArgumentParser()
    parser.add_argument("configFile", nargs="+", help="configuration file to execute")
    parser.add_argument("-L", "--log-level", help="set log level", choices=["VERBOSE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL", "ALWAYS"])
    parser.add_argument("-F", "--fit-type", help="type of fit to run", choices=["bkg", "disc", "excl"])
    parser.add_argument("-t", "--create-histograms", help="re-create histograms from TTrees", action="store_true", default=configMgr.readFromTree)
    parser.add_argument("-w", "--create-workspace", help="re-create workspace from histograms", action="store_true", default=configMgr.executeHistFactory)
    parser.add_argument("-x", "--use-XML", help="write XML files by hand and call hist2workspace on them, instead of directly writing workspaces", action="store_true", default=configMgr.writeXML)
    parser.add_argument("-f", "--fit", help="fit the workspace", action="store_true", default=configMgr.executeHistFactory)
    parser.add_argument("--fitname", dest="fitname", help="workspace name for fit (not specified takes 1st available fitConfig)", default="")
    parser.add_argument("-m", "--minos", help="run minos for asymmetric error calculation, optionally give parameter names for which minos should be run, space separated. For all params, use ALL", metavar="PARAM")
    parser.add_argument("-n", "--num_toys", type=int, help="set the number of toys, <=0 means to use real data", default=configMgr.nTOYs)
    parser.add_argument("-s", "--seed", type=int, help="set the random seed for toy generation", default=configMgr.toySeed)
    parser.add_argument("-a", "--use-asimov", help="use Asimov dataset for fitting and plotting", action="store_true", default=configMgr.useAsimovSet)
    parser.add_argument("-i", "--interactive", help="remain in interactive mode after running", action="store_true", default=runInterpreter)
    parser.add_argument("-l", "--limit-plot", help="make limit plot of workspace", action="store_true", default=printLimits)
    parser.add_argument("-p", "--hypotest", help="run exclusion hypothesis test", action="store_true", default=doHypoTests)
    parser.add_argument("-z", "--discovery-hypotest", help="run discovery hypothesis test", action="store_true", default=not doUL)
    parser.add_argument("-g", "--grid_points", help="grid points to process (comma-seperated)")
    parser.add_argument("-r", "--regions", help="signal regions to process (comma-seperated)", default="all")
    
    # note that we cannot make -d and -D the same due to http://bugs.python.org/issue9338
    # if we do so, specifying -d without argument would, if -d is the last option, eat the configFile as draw option
    # i.e. "HistFitter -f -d configFile.py" would fail, "HistFitter -d -f configFile.py" would work 
    # (a workaround using "-f -d -- configFile.py" exists but it would confuse users)
    # --GJ 14/11/2012 
    parser.add_argument("-d", action="store_true", help="draw before/after plots")
    parser.add_argument("-D", "--draw", help="specify plots to draw, comma separated; choose from "+str(["allPlots", "before","after", "corrMatrix", "sepComponents", "likelihood"]))
    
    parser.add_argument("-b", "--background", help="when doing hypotest, set background levels to values, form of bkgParName,value")
    parser.add_argument("-0", "--no-empty", help="do not draw empty bins when drawing", action="store_true")
    parser.add_argument("-T", "--run-toys", help="run toys (default with mu)", action="store_true")
    parser.add_argument("-V", "--validation", help="include validation regions", action="store_true")
    parser.add_argument("-c", "--cmd", help="python commands to process (semi-colon-seperated)")
    parser.add_argument("-u", "--userArg", help="arbitrary user argument(s)", default="")
    parser.add_argument("-A", "--use-archive-histfile", help="use backup histogram cache file", action="store_true")
    parser.add_argument("-P", "--run-profiling", help="Run a python profiler during main HistFitter execution", action="store_true")

    HistFitterArgs = parser.parse_args()
   
    if HistFitterArgs.fit_type == "bkg":
        myFitType = FitType.Background
    elif HistFitterArgs.fit_type == "excl":
        myFitType = FitType.Exclusion
    elif HistFitterArgs.fit_type == "disc":
        myFitType = FitType.Discovery

    configMgr.myFitType = myFitType
 
    if HistFitterArgs.validation:
        doValidation = True

    if HistFitterArgs.use_archive_histfile:
        configMgr.useHistBackupCacheFile = True

    if HistFitterArgs.run_profiling:
        doCodeProfiling = True

    if HistFitterArgs.create_histograms:
        configMgr.readFromTree = True

    if HistFitterArgs.create_workspace:
        configMgr.executeHistFactory = True

    if HistFitterArgs.use_XML:
        configMgr.writeXML = True

    if HistFitterArgs.fit:
        runFit = True

    configMgr.userArg=HistFitterArgs.userArg
    configMgr.nTOYs = HistFitterArgs.num_toys

    if HistFitterArgs.interactive:
        runInterpreter = True

    if HistFitterArgs.log_level:
        log.setLevel(HistFitterArgs.log_level, True) #do not add a default to HistFitterArgs.log_level or we will always lock it

    if HistFitterArgs.limit_plot:
        printLimits = True

    if HistFitterArgs.hypotest:
        doHypoTests = True

    if HistFitterArgs.discovery_hypotest:
        doHypoTests = True
        doUL = False

    if HistFitterArgs.d:
        drawBeforeFit = True
        drawAfterFit = True
    
    if HistFitterArgs.draw:
        drawArgs = HistFitterArgs.draw.split(",")
        if len(drawArgs) == 1 and drawArgs[0] == "allPlots":
            drawBeforeFit = True
            drawAfterFit = True
            drawCorrelationMatrix = True
            drawSeparateComponents = True
            drawLogLikelihood = True
        elif len(drawArgs)>0:
            for drawArg in drawArgs:
                if drawArg == "before":
                    drawBeforeFit = True
                elif drawArg == "after":
                    drawAfterFit = True
                elif drawArg == "corrMatrix":
                    drawCorrelationMatrix = True
                elif drawArg == "sepComponents":
                    drawSeparateComponents = True
                elif drawArg == "likelihood":
                    drawLogLikelihood = True
                else:
                    log.fatal("Wrong draw argument: %s\n  Possible draw arguments are 'allPlots' or comma separated 'before after corrMatrix sepComponents likelihood'" % drawArg) 

    if HistFitterArgs.no_empty:
        configMgr.removeEmptyBins = True

    if HistFitterArgs.seed != 0: #0 is default because type is int
        configMgr.toySeedSet = True
        configMgr.toySeed = HistFitterArgs.seed

    if HistFitterArgs.use_asimov:
        configMgr.useAsimovSet = True

    if HistFitterArgs.grid_points and HistFitterArgs.grid_points != "":
        sigSamples = HistFitterArgs.grid_points.split(",")
        log.info("Grid points specified: %s" % sigSamples)

    if HistFitterArgs.regions and HistFitterArgs.regions != "" and HistFitterArgs.regions != "all":
        pickedSRs = HistFitterArgs.regions.split(",")
    else:
        pickedSRs = [] #MB: used by 0-lepton fit

    if len(pickedSRs) > 0: 
        log.info("Selected signal regions: %s" % pickedSRs) 

    if HistFitterArgs.run_toys:
        runToys = True

    if HistFitterArgs.background:
        bkgArgs = HistFitterArgs.background.split(',')
        if len(bkgArgs) == 2:
            configMgr.SetBkgParName(bkgArgs[0])
            configMgr.SetBkgCorrVal(float(bkgArgs[1]))
            configMgr.SetBkgChlName("")
        elif len(bkgArgs) >= 3 and len(bkgArgs) % 3 == 0:
            for iChan in xrange(len(bkgArgs) / 3):
                iCx = iChan * 3
                configMgr.AddBkgChlName(bkgArgs[iCx])
                configMgr.AddBkgParName(bkgArgs[iCx+1])
                configMgr.AddBkgCorrVal(float(bkgArgs[iCx+2]))
                continue

    if HistFitterArgs.minos:
        runMinos = True
        
        minosArgs = HistFitterArgs.minos.split(",")
        for idx, arg in enumerate(minosArgs):
            if arg.lower() == "all":
                minosArgs[idx] = "all"

        minosPars = ",".join(minosArgs)

    if HistFitterArgs.cmd:
        log.info("Python commands executed: %s" % HistFitterArgs.cmd)
        exec(HistFitterArgs.cmd) ## python execute

    gROOT.SetBatch(not runInterpreter)

    #mandatory user-defined configuration
    execfile(HistFitterArgs.configFile[0]) #[0] since any extra arguments (sys.argv[-1], etc.) are caught here

    #standard execution from now on.
    configMgr.initialize()

    # initialize: set the toy seed
    RooRandom.randomGenerator().SetSeed(configMgr.toySeed)

    #runs Trees->histos and/or histos->workspace according to specifications
    if configMgr.readFromTree or configMgr.executeHistFactory:
        if doCodeProfiling:
            import cProfile
            cProfile.run('configMgr.executeAll()')
        else:
            configMgr.executeAll()

    if runFit:
        idx = 0
        if len(configMgr.fitConfigs) > 0:
           
            #print args
            if HistFitterArgs.fitname != "": # user specified a fit name
                fitFound = False
                for (i, config) in enumerate(configMgr.fitConfigs):
                    if configMgr.fitConfigs[i].name == HistFitterArgs.fitname:
                        idx = i
                        fitFound = True
                        log.info("Found fitConfig with name %s at index %d" % (HistFitterArgs.fitname, idx))
                        break
                        
                if not fitFound:
                    log.fatal("Unable to find fitConfig with name %s, bailing out" % HistFitterArgs.fitname)

            log.info("Running on fitConfig %s" % configMgr.fitConfigs[idx].name)
            r = GenerateFitAndPlotCPP(configMgr.fitConfigs[idx], configMgr.analysisName, drawBeforeFit, drawAfterFit, drawCorrelationMatrix, drawSeparateComponents, drawLogLikelihood, runMinos, minosPars)
            pass
        log.info(" GenerateFitAndPlotCPP(configMgr.fitConfigs[%d], configMgr.analysisName, drawBeforeFit, drawAfterFit, drawCorrelationMatrix, drawSeparateComponents, drawLogLikelihood)" % idx)
        log.info("   where drawBeforeFit, drawAfterFit, drawCorrelationMatrix, drawSeparateComponents, drawLogLikelihood are booleans")
        pass

    if printLimits:
        for fc in configMgr.fitConfigs:
            if len(fc.validationChannels) > 0:
                raise(Exception, "Validation regions should be turned off for setting an upper limit!")
            pass
        configMgr.cppMgr.doUpperLimitAll()
        pass

    if doHypoTests:
        for fc in configMgr.fitConfigs:
            if len(fc.validationChannels) > 0 and not (fc.signalSample is None or 'Bkg' in fc.signalSample):
                raise(Exception,"Validation regions should be turned off for doing hypothesis test!")
            pass
        configMgr.cppMgr.doHypoTestAll('results/',doUL)
        pass

    if runToys and configMgr.nTOYs > 0 and doHypoTests == False and printLimits == False and runFit == False:
        configMgr.cppMgr.runToysAll()
        pass

    if runInterpreter:
        from code import InteractiveConsole
        from ROOT import Util
        cons = InteractiveConsole(locals())
        cons.interact("Continuing interactive session... press Ctrl+d to exit")
        pass

    log.info("Leaving HistFitter... Bye!")