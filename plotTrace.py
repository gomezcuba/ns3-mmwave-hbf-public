#!/usr/bin/python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm

plt.close('all')

traceData_one = pd.read_csv('./SICF-ABF-1LAY-UNRL.trace',sep='\t',header=(0))
traceData_nosic = pd.read_csv('./SICF-ABF-PAD4-UNRL.trace',sep='\t',header=(0))
traceData_sic = pd.read_csv('./SICT-ABF-PAD4-UNRL.trace',sep='\t',header=(0))
nMaxLayer_nosic = np.max(traceData_nosic.RxLayer)
nMaxLayer_sic = np.max(traceData_sic.RxLayer)
nMaxLayer = np.maximum( nMaxLayer_nosic, nMaxLayer_sic)

def plotCDFdecorate(data,N,text,linfmt='-',clr='b'):
    [count,vals]=np.histogram(data,bins=N,density=False)
    CDF=np.cumsum(np.concatenate([[0],count]) / np.sum(count))
    plt.plot(vals,CDF,linfmt,color=clr,label=text)

plt.figure(1)
plotCDFdecorate(traceData_one[(traceData_one.iloc[:,0]=='UL')].TBler,50,"Single Layer",'--o',clr='k')
for l in range(nMaxLayer_nosic+1):
    plotCDFdecorate(traceData_nosic[(traceData_nosic.iloc[:,0]=='UL') & (traceData_nosic.RxLayer==l)].TBler,50,"TIN $\ell=%d$"%(l),':x',clr=cm.rainbow_r(l/(nMaxLayer+1)))
    
for l in range(nMaxLayer_sic+1):
    plotCDFdecorate(traceData_sic[(traceData_sic.iloc[:,0]=='UL') & (traceData_sic.RxLayer==l)].TBler,50,"SIC $\ell=%d$"%(l),'-',clr=cm.rainbow_r(l/(nMaxLayer+1)))
plt.legend()
plt.xlabel('BLER')
plt.ylabel('C.D.F')
plt.axis([0,1,0,1])
plt.savefig('./cdfBLERSIC.eps')

plt.figure(2)
plotCDFdecorate(traceData_one[(traceData_one.iloc[:,0]=='UL')]['SINR(dB)'],50,"Single Layer",'--o',clr='k')
for l in range(nMaxLayer_nosic+1):
    plotCDFdecorate(traceData_nosic[(traceData_nosic.iloc[:,0]=='UL') & (traceData_nosic.RxLayer==l)]['SINR(dB)'],50,"TIN $\ell=%d$"%(l),':x',clr=cm.rainbow_r(l/(nMaxLayer+1)))
    
for l in range(nMaxLayer_sic+1):
    plotCDFdecorate(traceData_sic[(traceData_sic.iloc[:,0]=='UL') & (traceData_sic.RxLayer==l)]['SINR(dB)'],50,"SIC $\ell=%d$"%(l),'-',clr=cm.rainbow_r(l/(nMaxLayer+1)))
plt.legend()
plt.xlabel('SINR (dB)')
plt.ylabel('C.D.F')
plt.axis([-10,50,0,1])
plt.savefig('./cdfSINRSIC.eps')
