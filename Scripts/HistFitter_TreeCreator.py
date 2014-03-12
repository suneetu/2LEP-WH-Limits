# Import Necessary Stuff
from ROOT import *
import os,sys

# Read Input
if len(sys.argv) != 3:
  print "\n\nThis tool expects exactly 2 input arguments: GRID SYSTEMATICS, quitting...\n\n"
  sys.exit(0)
else:
  grid         =sys.argv[1]
  systematicsMC=[sys.argv[2]]
  if grid != "SSBG8TeV" and grid != "SSWH8TeV":
    print "\n\nUnknown GRID type, options are SSBG8TeV and SSWH8TeV, quitting... \n\n"
    sys.exit(0)

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# USER SHOULDN'T MESS WITH ANYTHING ABOVE
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# Define Input and Output Directories
# Can read user input if neeed
path        = '/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140310/Raw/'
output_file = '/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140310/Processed/'+grid+'.root'

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# USER SHOULDN'T MESS WITH ANYTHING BELOW
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# Toggle to initiate ROOT file creation
doList=False

# Internal stuff
systematicsSignal           = systematicsMC
systematicsData             = systematicsMC

processesData               = {}
processesMC                 = {}
processesSignal             = {}

# Data and BG for WH Analysis
if 'SSBG8TeV' in grid:

    processesData['Data'] = [
             'id_periodA.physics_Egamma.PhysCont',
             'id_periodB.physics_Egamma.PhysCont',
             'id_periodC.physics_Egamma.PhysCont',
             'id_periodD.physics_Egamma.PhysCont',
             'id_periodE.physics_Egamma.PhysCont',
             'id_periodG.physics_Egamma.PhysCont',
             'id_periodH.physics_Egamma.PhysCont',
             'id_periodI.physics_Egamma.PhysCont',
             'id_periodJ.physics_Egamma.PhysCont',
             'id_periodL.physics_Egamma.PhysCont',
             'id_periodA.physics_Muons.PhysCont',
             'id_periodB.physics_Muons.PhysCont',
             'id_periodC.physics_Muons.PhysCont',
             'id_periodD.physics_Muons.PhysCont',
             'id_periodE.physics_Muons.PhysCont',
             'id_periodG.physics_Muons.PhysCont',
             'id_periodH.physics_Muons.PhysCont',
             'id_periodI.physics_Muons.PhysCont',
             'id_periodJ.physics_Muons.PhysCont',
             'id_periodL.physics_Muons.PhysCont']

    processesData['Fakes'] = [
             'id_fake.periodA.physics_Egamma.PhysCont',
             'id_fake.periodB.physics_Egamma.PhysCont',
             'id_fake.periodC.physics_Egamma.PhysCont',
             'id_fake.periodD.physics_Egamma.PhysCont',
             'id_fake.periodE.physics_Egamma.PhysCont',
             'id_fake.periodG.physics_Egamma.PhysCont',
             'id_fake.periodH.physics_Egamma.PhysCont',
             'id_fake.periodI.physics_Egamma.PhysCont',
             'id_fake.periodJ.physics_Egamma.PhysCont',
             'id_fake.periodL.physics_Egamma.PhysCont',
             'id_fake.periodA.physics_Muons.PhysCont',
             'id_fake.periodB.physics_Muons.PhysCont',
             'id_fake.periodC.physics_Muons.PhysCont',
             'id_fake.periodD.physics_Muons.PhysCont',
             'id_fake.periodE.physics_Muons.PhysCont',
             'id_fake.periodG.physics_Muons.PhysCont',
             'id_fake.periodH.physics_Muons.PhysCont',
             'id_fake.periodI.physics_Muons.PhysCont',
             'id_fake.periodJ.physics_Muons.PhysCont',
             'id_fake.periodL.physics_Muons.PhysCont']

    processesMC['Higgs'] = [
             'id_160155',
             'id_160205',
             'id_160255',
             'id_160305',
             'id_160505',
             'id_160555',
             'id_160655',
             'id_160705',
             'id_160755',
             'id_160805',
             'id_161005',
             'id_161055',
             'id_161105',
             'id_161155',
             'id_161305',
             'id_161555',
             'id_161566',
             'id_161577',
             'id_161675',
             'id_161686',
             'id_161697',
             'id_161708',
             'id_161719',
             'id_161730',
             'id_161805',
             'id_167418',
             'id_169072']
    
    processesMC['Top']  = [ 
             'id_110001',
             'id_108346',
             'id_119353',
             'id_174830',
             'id_174831',
             'id_119355',
             'id_174832',
             'id_174833',
             'id_179991',
             'id_179992',
             'id_119583',
             'id_169704',
             'id_169705',
             'id_169706',
             'id_158344']
 
    processesMC['WW']    = [
             'id_177997',
             'id_183734',
             'id_183736',
             'id_183738',
             'id_169471',
             'id_169472',
             'id_169473',
             'id_169474',
             'id_169475',
             'id_169476',
             'id_169477',
             'id_169478',
             'id_169479',
             'id_126988',
             'id_126989',
             'id_167006']
    
    processesMC['ZV']    = [ 
             'id_179974',
             'id_179975',
             'id_183585',
             'id_183587',
             'id_183589',
             'id_183591',
             'id_183735',
             'id_183737',
             'id_183739',
             'id_167007',
             'id_177999',
             'id_183586',
             'id_183588',
             'id_183590',
             'id_126894',
             'id_179396',
             'id_167008']
    
    processesMC['Zjets']  = [
			'id_178354',
			'id_178355',
			'id_178356',
			'id_178357',
			'id_178358',
			'id_178359',
			'id_178360',
			'id_178361',
			'id_178362',
			'id_178363',
			'id_178364',
			'id_178365',
			'id_178366',
			'id_178367',
			'id_178368',
			'id_178369',
			'id_178370',
			'id_178371',
			'id_178372',
			'id_178373',
			'id_178374',
			'id_178375',
			'id_178376',
			'id_178377',
			'id_178378',
			'id_178379',
			'id_178380',
			'id_178381',
			'id_178382',
			'id_178383',
			'id_178384',
			'id_178385',
			'id_178386',
			'id_178387',
			'id_178388',
			'id_178389',
			'id_178390',
			'id_178391',
			'id_178392',
			'id_178393',
			'id_178394',
			'id_178395',
			'id_178396',
			'id_178397',
			'id_178398',
			'id_178399',
			'id_178400',
			'id_178401',
			'id_178402',
			'id_178403',
			'id_178404',
			'id_178405',
			'id_178406',
			'id_178407',
			'id_117650',
			'id_117651',
			'id_117652',
			'id_117653',
			'id_117654',
			'id_117655',
			'id_110805',
			'id_110806',
			'id_110807',
			'id_110808',
			'id_110817',
			'id_110818',
			'id_110819',
			'id_110820',
			'id_117660',
			'id_117661',
			'id_117662',
			'id_117663',
			'id_117664',
			'id_117665',
			'id_110809',
			'id_110810',
			'id_110811',
			'id_110812',
			'id_110821',
			'id_110822',
			'id_110823',
			'id_110824',
			'id_117670',
			'id_117671',
			'id_117672',
			'id_117673',
			'id_117674',
			'id_117675',
			'id_110813',
			'id_110814',
			'id_110815',
			'id_110816',
			'id_110825',
			'id_110826',
			'id_110827',
			'id_110828']

# Signal for WH Analysis
if 'SSWH8TeV' in grid:
    processesSignal['lookup_SSWH8TeV_177501'] = ['id_177501']
    processesSignal['lookup_SSWH8TeV_177502'] = ['id_177502']
    processesSignal['lookup_SSWH8TeV_177503'] = ['id_177503']
    processesSignal['lookup_SSWH8TeV_177504'] = ['id_177504']
    processesSignal['lookup_SSWH8TeV_177505'] = ['id_177505']
    processesSignal['lookup_SSWH8TeV_177506'] = ['id_177506']
    processesSignal['lookup_SSWH8TeV_177507'] = ['id_177507']
    processesSignal['lookup_SSWH8TeV_177508'] = ['id_177508']
    processesSignal['lookup_SSWH8TeV_177509'] = ['id_177509']
    processesSignal['lookup_SSWH8TeV_177510'] = ['id_177510']
    processesSignal['lookup_SSWH8TeV_177511'] = ['id_177511']
    processesSignal['lookup_SSWH8TeV_177512'] = ['id_177512']
    processesSignal['lookup_SSWH8TeV_177513'] = ['id_177513']
    processesSignal['lookup_SSWH8TeV_177514'] = ['id_177514']
    processesSignal['lookup_SSWH8TeV_177515'] = ['id_177515']
    processesSignal['lookup_SSWH8TeV_177516'] = ['id_177516']
    processesSignal['lookup_SSWH8TeV_177517'] = ['id_177517']
    processesSignal['lookup_SSWH8TeV_177518'] = ['id_177518']
    processesSignal['lookup_SSWH8TeV_177519'] = ['id_177519']
    processesSignal['lookup_SSWH8TeV_177520'] = ['id_177520']
    processesSignal['lookup_SSWH8TeV_177521'] = ['id_177521']
    processesSignal['lookup_SSWH8TeV_177522'] = ['id_177522']
    processesSignal['lookup_SSWH8TeV_177523'] = ['id_177523']
    processesSignal['lookup_SSWH8TeV_177524'] = ['id_177524']
    processesSignal['lookup_SSWH8TeV_177525'] = ['id_177525']
    processesSignal['lookup_SSWH8TeV_177526'] = ['id_177526']

#####################################################################
# Group various BG component
#####################################################################
def group(systematics,processes):
    f_out = TFile(output_file,'UPDATE')
    for systematic in systematics:
        f_in = TFile(path+systematic+'.root','READ')
        for process, samples in processes.iteritems():
            name_tree=''
            #check if process starts with lookup, if so, then lookup the x and y coordinates
            if process.split('_')[0] == 'lookup':
                #print process
                if "DGemt" in process or 'DLiSlep' in process:
                    x,y=getCoordfromMCID(process.split('_')[3],str(process.split('_')[1])+"_"+str(process.split('_')[2]))
                    name_tree=process.split('_')[1]+'_'+str(x)+'_'+str(y)+'_'+systematic
                else:
                    x,y=getCoordfromMCID(process.split('_')[2],process.split('_')[1])
                    name_tree=process.split('_')[1]+'_'+str(x)+'_'+str(y)+'_'+systematic
            else:
                name_tree=process+'_'+systematic
            list_sample = TList()
            for sample in samples:
                if (f_in.Get(sample)):
                    list_sample.Add(f_in.Get(sample))
                else:
                    print '    '+sample+' for '+name_tree+' ('+process+'_'+systematic+') is a null pointer, does the TTree exist? Skipping...'
            #print systematic,process,samples,list_sample.At(0)
            f_out.cd()
            if list_sample.At(0):
                tree=list_sample.At(0).MergeTrees(list_sample)      #somewhat strange implementation: t.MergeTrees(TList l) creates a new tree in the memory
                if tree:
                    tree.Write(name_tree)
                    print '    Tree '+name_tree+' ('+process+'_'+systematic+') written with '+str(tree.GetEntries())+' events.'
                else:
                    print '    Tree '+name_tree+' ('+process+'_'+systematic+') not written due to 0 events.'
                del tree
            else:
                print '    '+name_tree+' ('+process+'_'+systematic+') is a null pointer, does any TTree in the list exist? Skipping...'
            del list_sample
        f_in.Close()
    f_out.Close()

#####################################################################
# Get Signal Coordinates
#####################################################################
def getCoordfromMCID(mcid,grid):
	fname = {}
	entrynr = {}
	# set file with data
	fname["SSWH8TeV"  ]="data/modeA_WH_notauhad_MC1eqMN2_DiagonalMatrix.txt"
	# set position of x,y in line when .split() is done
	entrynr["SSWH8TeV"]=[1,2]
	# for now assuming mcid position to be 0
	# in case this no longer holds, introduce another dict

	try:
		fname[grid]
	except:
		print "Could not identify your grid ("+str(grid)+") Possibilities are:"
		print fname.keys()
		return (-1,-1)
	index=0
	with open(fname[grid]) as dataf:
		for line in dataf:
			if line.split()[index]==mcid:
				print mcid
				#print "found",mcid,"coord:",line.split()[entrynr[grid][0]],line.split()[entrynr[grid][1]]
				return (line.split()[entrynr[grid][0]],line.split()[entrynr[grid][1]])
		print "Could not find mcid",mcid
	return (-1,-1)


#####################################################################
# Run
#####################################################################
if doList or (not doList and 'CENTRAL' in sys.argv[2]):
	f_out = TFile(output_file,'RECREATE')
	f_out.Close()
print 'Grouping samples to processes and assigning systematics for MC.'
group(systematicsMC,processesMC)
print 'Grouping samples to processes and assigning systematics for Signal.'
group(systematicsSignal,processesSignal)
print 'Grouping samples for Data.'
group(systematicsData,processesData)

print 'Created '+output_file
print 'Bye.'
