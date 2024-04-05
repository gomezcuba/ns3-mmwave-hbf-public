# !/bin/sh

for NROUND in {0..49}
do

	for NITER in {0..1}
	do
		let "NRUN = $NROUND*2+$NITER"
		
################################################################################
# 1-layer CBF with our padding scheduler

		echo "Codebook BF 1-layer padding-scheduler noHARQ noSIC PART "$NRUN
		#{ time sleep 2 ; } 2>SICF-ABF-1LAY-UNRL.run$NRUN.time &
		{ time ./waf --run "mmwave-hbf --RngRun=$NRUN --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --sic=false --harq=false --nLayers=1 --useTCP=false --interPacketInterval=2000  --ns3::MmWavePhyRxTrace::OutputFilename=SICF-ABF-1LAY-UNRL.run$NRUN.trace" 2> SICF-ABF-1LAY-UNRL.run$NRUN.log 1>SICF-ABF-1LAY-UNRL.run$NRUN.out ; } 2>SICF-ABF-1LAY-UNRL.run$NRUN.time &

################################################################################
# 4-layer HBF MIMO SDMA with our padding scheduler

		echo "Codebook BF 4-layer padding-scheduler noHARQ noSIC PART "$NRUN
		#{ time sleep 2 ; } 2>SICF-ABF-1LAY-UNRL.run$NRUN.time &
		{ time ./waf --run "mmwave-hbf --RngRun=$NRUN --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --sic=false --harq=false --nLayers=4 --useTCP=false --interPacketInterval=2000 --ns3::MmWavePhyRxTrace::OutputFilename=SICF-ABF-PAD4-UNRL.run$NRUN.trace" 2> SICF-ABF-PAD4-UNRL.run$NRUN.log 1>SICF-ABF-PAD4-UNRL.run$NRUN.out ; } 2>SICF-ABF-PAD4-UNRL.run$NRUN.time &
		
		echo "Codebook BF 4-layer padding-scheduler noHARQ SIC PART "$NRUN
		#{ time sleep 2 ; } 2>SICF-ABF-1LAY-UNRL.run$NRUN.time &
		{ time ./waf --run "mmwave-hbf --RngRun=$NRUN --sched=ns3::MmWavePaddedHbfMacScheduler --bfmod=ns3::MmWaveFFTCodebookBeamforming --sic=true --harq=false --nLayers=4 --useTCP=false --interPacketInterval=2000 --ns3::MmWavePhyRxTrace::OutputFilename=SICT-ABF-PAD4-UNRL.run$NRUN.trace" 2> SICT-ABF-PAD4-UNRL.run$NRUN.log 1>SICT-ABF-PAD4-UNRL.run$NRUN.out ; } 2>SICT-ABF-PAD4-UNRL.run$NRUN.time &
		
	done
	wait
done
