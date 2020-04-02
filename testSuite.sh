#!/bin/sh

echo "Geometric BF SISO noHARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWaveFlexTtiMacScheduler --bfmod=ns3::MmWaveDftBeamforming --harq=false" 2> logGSISO.txt
echo "Geometric BF SISO HARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWaveFlexTtiMacScheduler --bfmod=ns3::MmWaveDftBeamforming --harq=true" 2> logGSISOh.txt

echo "Codebook BF SISO noHARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWaveFlexTtiMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --harq=false" 2> logSISO.txt
echo "Codebook BF SISO HARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWaveFlexTtiMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --harq=true" 2> logSISOh.txt

echo "Geometric BF SISO noHARQ paddedsched"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveDftBeamforming --harq=false --nLayers=1 " 2> logGSISO2.txt
echo "Geometric BF SISO HARQ  paddedsched"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveDftBeamforming --harq=true --nLayers=1 " 2> logGSISO2h.txt
echo "Codebook BF SISO noHARQ paddedsched"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --harq=false --nLayers=1 " 2> logSISO2.txt
echo "Codebook BF SISO HARQ paddedsched"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --harq=true --nLayers=1 " 2> logSISO2h.txt

echo "Geometric BF MIMO noHARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveDftBeamforming --harq=false" 2> logGBF.txt
echo "Geometric BF MIMO HARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveDftBeamforming --harq=true" 2> logGBFh.txt

echo "Codebook BF MIMO noHARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --harq=false" 2> logABF.txt
echo "Codebook BF MIMO HARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --harq=true" 2> logABFh.txt

echo "Single-subcarrier MMSE BF MIMO noHARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveMMSEBeamforming --harq=false" 2> logHBF.txt
echo "Single-subcarrier MMSE BF MIMO HARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveMMSEBeamforming --harq=true" 2> logHBFh.txt

echo "Full-spectrum MMSE BF MIMO noHARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveMMSESpectrumBeamforming --harq=false" 2> logSBF.txt
echo "Full-spectrum MMSE BF MIMO HARQ"
time ./waf --run "mmwave-hbf --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveMMSESpectrumBeamforming --harq=true" 2> logSBFh.txt
