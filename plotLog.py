#!/usr/bin/python

#!/usr/bin/python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm

plt.close('all')

Nparts=16

def digestTimeFile(name):
    file = open(name)
    count=0
    for ln in file:
        if ln.find("user")>=0:#checkeamos se a linea ten un texto caracteristico do print que nos interesa
            minu,seco,mili = [int(x.strip('s')) for x in ln.split()[1].replace('m',',').split(',')]
            count+=60*minu+seco+1e-3*mili
    return(count)

def diggestLogFile(logFileName):
    file = open(logFileName)
    recBytes = pd.DataFrame(columns=['rnti','recBytesDL','recBytesUL']).set_index(['rnti'])
    phyStatsList = []
    for ln in file:
        if ln.find("dB TBLER")>=0:#checkeamos se a linea ten un texto caracteristico do print que nos interesa
            _,_,rnti,_,_,size,_,mcs,_,bitmap,_,rv,_,SINRdB,_,_,TBLER,_,corrupted,_,_,_,_,_,_,mcs_ideal,_=ln.split()
            phyStatsList.append([int(rnti),int(size),int(mcs),int(bitmap),int(rv),float(SINRdB),float(TBLER),int(corrupted[:-1]),int(mcs_ideal)])
        if ln.find('The number of DL received bytes for UE ')>=0:
            rnti=int(ln[ln.find('UE')+3:ln.find(':',ln.find('UE')+3)])
            pkts=int(ln[ln.find(':')+2:])
            if rnti in recBytes.index:
                recBytes.loc[rnti].recBytesDL+=pkts#should never reach this point
            else:
                recBytes.loc[rnti]=[pkts,0]
        if ln.find('The number of UL received bytes for UE ')>=0:
            rnti=int(ln[ln.find('UE')+3:ln.find(':',ln.find('UE')+3)])
            pkts=int(ln[ln.find(':')+2:])
            if rnti in recBytes.index:
                recBytes.loc[rnti].recBytesUL+=pkts#should never reach this point
            else:
                recBytes.loc[rnti]=[0,pkts]
    file.close()
    phyStats = pd.DataFrame(data=phyStatsList,columns=['rnti','size','mcs','bitmap','rv','SINRdB','TBLER','corrupted','mcs_ideal'])
    return (phyStats,recBytes)
    
def plotCDFdecorate(data,N,text,linfmt='-',clr='b'):
    [count,vals]=np.histogram(data,bins=N,density=False)
    CDF=np.cumsum(np.concatenate([[0],count]) / np.sum(count))
    plt.plot(vals,CDF,linfmt,color=clr,label=text)
#alternativamente datos = pd.DataFrame(np.array([listaSINR,listaTBLER]).T,columns=("SINR","TBLER"))

# phyStats_one,recBytes_one = diggestLogFile('./SICF-ABF-1LAY-UNRL.log')
# phyStats_nosic,recBytes_nosic = diggestLogFile('./SICF-ABF-PAD4-UNRL.log')
# phyStats_sic,recBytes_sic = diggestLogFile('./SICT-ABF-PAD4-UNRL.log')
def getDFparts(template,Nparts,parsef):
    l=[]
    for npart in range(Nparts):
        results_tuple = parsef(template.format(npart))
        l.append(results_tuple)
    tuple_of_lists = zip(*l)
    iterable_of_df = map( lambda l: pd.concat(l,keys=range(Nparts)) , tuple_of_lists)
    return( tuple(iterable_of_df ) )

phyStats_one,recBytes_one_all = getDFparts('./SICF-ABF-1LAY-UNRL.run{}.log',Nparts,diggestLogFile)    
recBytes_one = recBytes_one_all.groupby("rnti").mean()
phyStats_nosic,recBytes_nosic_all = getDFparts('./SICF-ABF-PAD4-UNRL.run{}.log',Nparts,diggestLogFile)    
recBytes_nosic = recBytes_nosic_all.groupby("rnti").mean()
phyStats_sic,recBytes_sic_all = getDFparts('./SICT-ABF-PAD4-UNRL.run{}.log',Nparts,diggestLogFile)    
recBytes_sic = recBytes_sic_all.groupby("rnti").mean()

# plt.figure(1)
# plotCDFdecorate(phyStats_one.TBLER,50,"Single Layer",'--o',clr='k')
# plt.xlabel('BLER')
# plt.ylabel('C.D.F')
# plt.axis([0,1,0,1])

# plt.figure(2)
# plotCDFdecorate(phyStats_one.SINRdB,50,"Single Layer",'--o',clr='k')
# plt.xlabel('SINRdB')
# plt.ylabel('C.D.F')
# plt.axis([np.min(SINRdB),np.max(SINRdB),0,1])

plt.figure(3)
plt.bar(recBytes_one.index,1500/2e-3 *8*1e-6,width=.9,label='Offered Traffic',color=(0,0,0,0),edgecolor=(0,0,0,1),linestyle='dotted')
Nbar=3
barwidth=0.9/Nbar
# for nu in range(Nu):
nb=0
offset=(nb-(Nbar-1)/2)*barwidth
plt.bar(recBytes_one.index+offset,recBytes_one.recBytesUL/1.15 *8*1e-6,width=barwidth,label='Single Layer')
nb=1
offset=(nb-(Nbar-1)/2)*barwidth
plt.bar(recBytes_nosic.index+offset,recBytes_nosic.recBytesUL/1.15 *8*1e-6,width=barwidth,label='4 Layer TIN')
nb=2
offset=(nb-(Nbar-1)/2)*barwidth
plt.bar(recBytes_sic.index+offset,recBytes_sic.recBytesUL/1.15 *8*1e-6,width=barwidth,label='4 Layer SIC')
plt.xticks(ticks=recBytes_one.index,labels=['U%d'%x for x in recBytes_one.index])
plt.ylabel('User Achievable rate (Mbps)')
plt.legend()
plt.savefig('./rate_vs_user.eps')

# data = pd.read_csv ('UlPdcpStats.txt', delimiter = " ", index_col=False, names = ['mode', 'time', 'txid','rxid','drbid', 'size', 'delay'], engine='python', header=0)

# grouped=data[data['mode']=='Rx'].groupby('rxid').delay.mean()

# plt.figure(4)
# plt.bar(grouped.index,grouped*1e-6)

# getDFparts('./SICF-ABF-1LAY-UNRL.run{}.time',6,lambda s: pd.DataFrame([digestTimeFile(s)]) )

def getArrayparts(template,Nparts,parsef):
    l=[]
    for npart in range(Nparts):
        results_tuple = parsef(template.format(npart))
        l.append(results_tuple)
    return( np.array(l) )

runtimes_one=getArrayparts('./SICF-ABF-1LAY-UNRL.run{}.time',Nparts,digestTimeFile)
runtimes_nosic=getArrayparts('./SICF-ABF-PAD4-UNRL.run{}.time',Nparts,digestTimeFile)
runtimes_sic=getArrayparts('./SICT-ABF-PAD4-UNRL.run{}.time',Nparts,digestTimeFile)

runtimes=np.vstack((runtimes_one,runtimes_nosic,runtimes_sic))
plt.figure(4)
plt.bar(range(3),np.mean(runtimes,axis=1))
# plt.errorbar(range(3),np.mean(runtimes,axis=1),np.std(runtimes,axis=1),None,'sk',capsize=5,label='Mean and STD')
# plt.scatter(np.tile(range(3),(Nparts,1)),runtimes.T,marker='x',color='b',label='Scatter cloud')
# plt.axis([-.3,2.3,1900,2400])
# plt.legend()
plt.xticks(range(3),['Single Layer','4 Layer TIN','4 Layer SIC'])
plt.ylabel("Mean run time (s)")
plt.savefig('./runtime_vs_alg.eps')
print(''.join(['{:.0f}m{:.2f}s & '.format(d//60,np.mod(d,60)) for d in np.mean(runtimes,axis=1)]))