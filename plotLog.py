#!/usr/bin/python

#!/usr/bin/python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm

plt.close('all')

def diggestLogFile(logFileName):
    listaSINR = []
    listaTBLER = []
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

phyStats_one,recBytes_one = diggestLogFile('./SICF-ABF-1LAY-UNRL.log')
phyStats_nosic,recBytes_nosic = diggestLogFile('./SICF-ABF-PAD4-UNRL.log')
phyStats_sic,recBytes_sic = diggestLogFile('./SICT-ABF-PAD4-UNRL.log')
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

data = pd.read_csv ('UlPdcpStats.txt', delimiter = " ", index_col=False, names = ['mode', 'time', 'txid','rxid','drbid', 'size', 'delay'], engine='python', header=0)

grouped=data[data['mode']=='Rx'].groupby('rxid').delay.mean()

plt.figure(4)
plt.bar(grouped.index,grouped*1e-6)