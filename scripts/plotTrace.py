#!/usr/bin/python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm

plt.close('all')

traceFile = "test.trace"

traceData = pd.read_csv('test.trace',sep='\t',header=(0))
traceData2 = pd.read_csv('testIA.trace',sep='\t',header=(0))

[count,BLER]=np.histogram(traceData.TBler,bins=50,density=False)
[count2,BLER2]=np.histogram(traceData2.TBler,bins=50,density=False)

CDF=np.cumsum(np.concatenate([[0],count]) / traceData.TBler.size)
CDF2=np.cumsum(np.concatenate([[0],count2]) / traceData2.TBler.size)

plt.figure(1)
plt.plot(BLER,CDF,label="normal")
plt.plot(BLER2,CDF2,label="IA")
plt.legend()
plt.xlabel('BLER')
plt.ylabel('C.D.F')
plt.axis([0,1,0,1])

[count,SINRdB]=np.histogram(traceData['SINR(dB)'],bins=50,density=False)
[count2,SINRdB2]=np.histogram(traceData2['SINR(dB)'],bins=50,density=False)

CDF=np.cumsum(np.concatenate([[0],count]) / traceData.TBler.size)
CDF2=np.cumsum(np.concatenate([[0],count2]) / traceData2.TBler.size)

plt.figure(2)
plt.plot(SINRdB,CDF,label="normal")
plt.plot(SINRdB2,CDF2,label="IA")
plt.legend()
plt.xlabel('SINRdB')
plt.ylabel('C.D.F')
plt.axis([np.min(SINRdB),np.max(SINRdB),0,1])
