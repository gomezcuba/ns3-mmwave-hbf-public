# !/bin/sh

################################################################################
# 1-layer CBF with our padding scheduler

echo "Codebook BF 1-layer padding-scheduler noHARQ noSIC PART "$NPART
time ./waf --run "mmwave-hbf --RngRun=$NPART --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --sic=false --harq=false --nLayers=1 --useTCP=false --interPacketInterval=2000  --ns3::MmWavePhyRxTrace::OutputFilename=SICF-ABF-1LAY-UNRL.trace" 2> SICF-ABF-1LAY-UNRL.log

################################################################################
# 4-layer HBF MIMO SDMA with our padding scheduler TIN

echo "Codebook BF 4-layer padding-scheduler noHARQ noSIC PART "$NPART
	time ./waf --run "mmwave-hbf --RngRun=$NPART --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --sic=false --harq=false --nLayers=4 --useTCP=false --interPacketInterval=2000 --ns3::MmWavePhyRxTrace::OutputFilename=SICF-ABF-PAD4-UNRL.trace" 2> SICF-ABF-PAD4-UNRL.log 

# 4-layer HBF MIMO SDMA with our padding scheduler SIC
echo "Codebook BF 4-layer padding-scheduler noHARQ SIC PART "$NPART
time ./waf --run "mmwave-hbf --RngRun=$NPART --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --sic=true --harq=false --nLayers=4 --useTCP=false --interPacketInterval=2000 --ns3::MmWavePhyRxTrace::OutputFilename=SICT-ABF-PAD4-UNRL.trace" 2> SICT-ABF-PAD4-UNRL.log
