#!/usr/bin/python

#!/usr/bin/python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm

plt.close('all')

logFileName = "TCP-SBF-PAD4-UNRL.log"

file = open(logFileName)


listaSINR = []
listaTBLER = []
for ln in file:
    if ln.find("dB TBLER")>=0:#checkeamos se a linea ten un texto caracteristico do print que nos interesa
    
        posIniSINR=ln.find("SINR ")+5#onde comeza o numero
        posFinSINR=ln.find(" dB")#onde comeza o numero
        SINR = float(ln[posIniSINR:posFinSINR])
        listaSINR.append(SINR)
        
        posIniTBLER=ln.find("TBLER ")+6#onde comeza o numero
        posFinTBLER=ln.find(" corrupted")#onde comeza o numero
        TBLER = float(ln[posIniTBLER:posFinTBLER])
        listaTBLER.append(TBLER)
    #elif ... : # poderiamos coller datos de varios print de formato coñecido no mesmo for
    #else: # non facemos nada con lineas de formato descoñecido
    
SINR = np.array(listaSINR)
TBLER = np.array(listaTBLER)

#alternativamente datos = pd.DataFrame(np.array([listaSINR,listaTBLER]).T,columns=("SINR","TBLER"))

[count,BLER]=np.histogram(TBLER,bins=50,density=False)

CDF=np.cumsum(np.concatenate([[0],count]) / TBLER.size)

plt.figure(1)
plt.plot(BLER,CDF)
plt.xlabel('BLER')
plt.ylabel('C.D.F')
plt.axis([0,1,0,1])

[count,SINRdB]=np.histogram(SINR,bins=50,density=False)

CDF=np.cumsum(np.concatenate([[0],count]) / SINR.size)

plt.figure(2)
plt.plot(SINRdB,CDF)
plt.xlabel('SINRdB')
plt.ylabel('C.D.F')
plt.axis([np.min(SINRdB),np.max(SINRdB),0,1])
