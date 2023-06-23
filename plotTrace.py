#!/usr/bin/python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm

plt.close('all')

traceFile = "test.trace"

traceData = pd.read_csv('test.trace',sep='\t',header=(0))

[count,BLER]=np.histogram(traceData.TBler,bins=50,density=False)

CDF=np.cumsum(np.concatenate([[0],count]) / traceData.TBler.size)

plt.figure(1)
plt.plot(BLER,CDF)
plt.xlabel('BLER')
plt.ylabel('C.D.F')
plt.axis([0,1,0,1])

[count,SINRdB]=np.histogram(traceData['SINR(dB)'],bins=50,density=False)

CDF=np.cumsum(np.concatenate([[0],count]) / traceData.TBler.size)

plt.figure(2)
plt.plot(SINRdB,CDF)
plt.xlabel('SINRdB')
plt.ylabel('C.D.F')
plt.axis([np.min(SINRdB),np.max(SINRdB),0,1])
