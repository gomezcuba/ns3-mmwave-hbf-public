/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *   Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   Author: Marco Miozzo <marco.miozzo@cttc.es>
 *           Nicola Baldo  <nbaldo@cttc.es>
 *
 *   Modified by: Marco Mezzavilla < mezzavilla@nyu.edu>
 *                Sourjya Dutta <sdutta@nyu.edu>
 *                Russell Ford <russell.ford@nyu.edu>
 *                Menglei Zhang <menglei@nyu.edu>
 *
 *   Modified by: Junseok Kim <jskim14@mwnl.snu.ac.kr>
 *
 */


#include "ns3/mmwave-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/mmwave-point-to-point-epc-helper.h"

#include <math.h>

using namespace ns3;
using namespace mmwave;

// traces for TCP
static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RttChange (Ptr<OutputStreamWrapper> stream, Time oldRtt, Time newRtt)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldRtt.GetSeconds () << "\t" << newRtt.GetSeconds () << std::endl;
}

// server side traces
static void
Traces(uint32_t serverId, uint32_t ueId, std::string pathVersion, std::string finalPart)
{
	// Se crea un objeto helper para trazas ASCII
  AsciiTraceHelper asciiTraceHelper;

  //Se construye la ruta para cwnd
  std::ostringstream pathCW;
  pathCW<<"/NodeList/"<< serverId <<"/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow";

  std::ostringstream fileCW;
  fileCW << pathVersion << "tcp-cwnd-change"  << ueId+1 << "_" << finalPart;

  std::ostringstream pathRTT;
  pathRTT << "/NodeList/"<< serverId <<"/$ns3::TcpL4Protocol/SocketList/0/RTT";

  std::ostringstream fileRTT;
  fileRTT << pathVersion << "tcp-rtt"  << ueId+1 << "_" << finalPart;

  Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream (fileCW.str ().c_str ());
  *stream1->GetStream () << "Time" << "\t" << "oldCwnd" << "\t" << "newCwnd" << std::endl;
  Config::ConnectWithoutContext (pathCW.str ().c_str (), MakeBoundCallback(&CwndChange, stream1));

  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream (fileRTT.str ().c_str ());
  *stream2->GetStream () << "Time" << "\t" << "oldRtt" << "\t" << "newRtt" << std::endl;
  Config::ConnectWithoutContext (pathRTT.str ().c_str (), MakeBoundCallback(&RttChange, stream2));
}

// client side trace
static void Rx (Ptr<OutputStreamWrapper> stream, Ptr<const Packet> packet, const Address &from)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << packet->GetSize()<< std::endl;
}

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */
NS_LOG_COMPONENT_DEFINE ("EpcFirstExample");
	int
main (int argc, char *argv[])
{
	  //LogComponentEnable ("LteUeRrc", LOG_LEVEL_ALL);
	  //LogComponentEnable ("LteEnbRrc", LOG_LEVEL_ALL);
	  //LogComponentEnable("MmWavePointToPointEpcHelper",LOG_LEVEL_ALL);
	  //LogComponentEnable("EpcUeNas",LOG_LEVEL_ALL);
	  //LogComponentEnable ("MmWaveSpectrumPhy", LOG_LEVEL_DEBUG);
	  //LogComponentEnable ("MmWaveUePhy", LOG_LEVEL_INFO);
	  //LogComponentEnable ("MmWaveEnbPhy", LOG_LEVEL_INFO);
	  //LogComponentEnable ("MmWaveEnbPhy", LOG_LEVEL_DEBUG);
	  //LogComponentEnable ("MmWaveBeamformingModel", LOG_LEVEL_DEBUG);
	  //LogComponentEnable ("MmWaveUeMac", LOG_LEVEL_LOGIC);
	  //LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
	  //LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
	  //LogComponentEnable ("ThreeGppSpectrumPropagationLossModel", LOG_LEVEL_DEBUG);
	  //LogComponentEnable ("ThreeGppChannel", LOG_LEVEL_DEBUG);
	  //LogComponentEnable("PropagationLossModel",LOG_LEVEL_ALL);
	  //    LogComponentEnable ("MmwaveHbfSpectrumChannel", LOG_LEVEL_INFO);
	  LogComponentEnable ("MmWavePaddedHbfMacScheduler", LOG_LEVEL_INFO);
	  // LogComponentEnable ("MmWaveAsyncHbfMacScheduler", LOG_LEVEL_INFO);
	  //  LogComponentEnable ("MmWaveFlexTtiMacScheduler", LOG_LEVEL_INFO);
	  //LogComponentEnable ("MmWaveInterAvoidHbfMacScheduler", LOG_LEVEL_LOGIC);
	  LogComponentEnable ("MmWavePhy", LOG_LEVEL_INFO);
	  //  LogComponentEnable ("MmWaveEnbMac", LOG_LEVEL_INFO);
	  //  LogComponentEnable ("MmWaveUeMac", LOG_LEVEL_INFO);

	uint16_t numEnb = 1;
	uint16_t numUe = 7;
	uint16_t numEnbLayers = 4;
	double startTime = 0.02;
        double simTime = 1.2;
	double packetSize = 1460; // packet size in byte
	double interPacketInterval = 5000; // 500 microseconds
	bool harqEnabled = false;
	bool sicEnabled = false; //Successive Interference Cancelation
	bool rlcAmEnabled = false;
	//bool useIdealRrc = true;
	bool fixedTti = false;
        bool tcpApp = false;
//	bool smallScale = false;
//	double speed = 3;

	std::string schedulerType =
//	    "ns3::MmWaveFlexTtiMacScheduler"
//	    "ns3::MmWaveAsyncHbfMacScheduler"
	    "ns3::MmWavePaddedHbfMacScheduler"
//	    "ns3::MmWaveInterAvoidHbfMacScheduler" //(Este es el mío)
	    ;
	std::string beamformerType =
//	    "ns3::MmWaveDftBeamforming"
	    "ns3::MmWaveFFTCodebookBeamforming"
//            "ns3::MmWaveMMSEBeamforming"
//	    "ns3::MmWaveMMSESpectrumBeamforming"
	    ;

	// Command line arguments
	CommandLine cmd;
	cmd.AddValue ("numEnb", "Number of eNBs", numEnb);
	cmd.AddValue ("numUe", "Number of UEs per eNB", numUe);
	cmd.AddValue ("simTime", "Total duration of the simulation [s])", simTime);
	cmd.AddValue ("interPacketInterval", "Inter-packet interval [us])", interPacketInterval);
	cmd.AddValue ("harq", "Enable Hybrid ARQ", harqEnabled);
	cmd.AddValue ("sic", "Enable SIC", sicEnabled);
	cmd.AddValue ("rlcAm", "Enable RLC-AM", rlcAmEnabled);
	cmd.AddValue ("fixedTti", "Fixed TTI scheduler option", fixedTti);
        cmd.AddValue ("sched", "The type of scheduler algorithm", schedulerType);
        cmd.AddValue ("bfmod", "The type of beamformer algorithm", beamformerType);
        cmd.AddValue ("nLayers", "The number of HBF layers per eNB", numEnbLayers);
        cmd.AddValue ("useTCP", "Use TCP BulkSendApplication instead of UDPClient", tcpApp);
	//cmd.AddValue ("useIdealRrc", "whether to use ideal RRC layer or not", useIdealRrc);
	cmd.Parse (argc, argv);

	//Primer log que aparece en pantalla
        NS_LOG_UNCOND("Scheduler: " << schedulerType << " Beamformer: " << beamformerType << " HARQ: " << harqEnabled);
        NS_LOG_UNCOND("La simulacion utiliza SIC: " << sicEnabled);



	Config::SetDefault ("ns3::MmWaveHelper::RlcAmEnabled", BooleanValue (rlcAmEnabled));
	Config::SetDefault ("ns3::MmWaveHelper::HarqEnabled", BooleanValue (harqEnabled));
	Config::SetDefault ("ns3::MmWaveHelper::sicEnabled", BooleanValue (sicEnabled));
//	Config::SetDefault ("ns3::MmWaveHelper::UseIdealRrc", BooleanValue (useIdealRrc));
//	Config::SetDefault ("ns3::MmWaveFlexTtiMacScheduler::HarqEnabled", BooleanValue (harqEnabled));
//	Config::SetDefault ("ns3::MmWaveFlexTtiMacScheduler::CqiTimerThreshold", UintegerValue (1000));
//	Config::SetDefault ("ns3::MmWaveFlexTtiMaxWeightMacScheduler::HarqEnabled", BooleanValue (harqEnabled));
//	Config::SetDefault ("ns3::MmWaveFlexTtiMaxWeightMacScheduler::FixedTti", BooleanValue (fixedTti));
//	Config::SetDefault ("ns3::MmWaveFlexTtiMaxWeightMacScheduler::SymPerSlot", UintegerValue (6));

  // each chunk corresponds to a NR RB, each NR RB is composed of 12 subcarriers
  Config::SetDefault ("ns3::MmWavePhyMacCommon::SubcarriersPerChunk", UintegerValue (12));
  
  // for u=2, the SCS is 60 kHz 
  Config::SetDefault ("ns3::MmWavePhyMacCommon::ChunkWidth", DoubleValue (12 * 60.0e3)); 
  
  // the maximum number of NR RBs is 275, which corresponds to a bw of 198 MHz 
  Config::SetDefault ("ns3::MmWavePhyMacCommon::ChunkPerRB", UintegerValue (275)); 
  
  // NOTE: NYU subframe corresponds to NR slot
  // NR has 14 OFDM symbols per slot
	Config::SetDefault ("ns3::MmWavePhyMacCommon::SymbolsPerSubframe", UintegerValue (14));
  
  // reserve one resource element every 2 RB for PTRS transmission
  Config::SetDefault ("ns3::MmWavePhyMacCommon::NumRefSCPerSym", UintegerValue (275 / 2));
	
  // with u=2 there are 4 slots per subframe, hence the slot durantion is 1 ms / 4
  Config::SetDefault ("ns3::MmWavePhyMacCommon::SubframePeriod", DoubleValue (250.0)); 
	Config::SetDefault ("ns3::MmWavePhyMacCommon::SymbolPeriod", DoubleValue (250.0 / 14));
  
	Config::SetDefault ("ns3::MmWavePhyMacCommon::TbDecodeLatency", UintegerValue (200.0));
	Config::SetDefault ("ns3::MmWavePhyMacCommon::NumEnbLayers", UintegerValue (numEnbLayers));
	//Config::SetDefault ("ns3::MmWaveBeamforming::LongTermUpdatePeriod", TimeValue (MilliSeconds (100.0)));
	Config::SetDefault ("ns3::LteEnbRrc::SystemInformationPeriodicity", TimeValue (MilliSeconds (5.0)));
//	Config::SetDefault ("ns3::MmWavePropagationLossModel::ChannelStates", StringValue ("n"));
	Config::SetDefault ("ns3::LteRlcAm::ReportBufferStatusTimer", TimeValue (MicroSeconds (100.0)));
  Config::SetDefault ("ns3::LteRlcUmLowLat::ReportBufferStatusTimer", TimeValue (MicroSeconds (100.0)));
  Config::SetDefault ("ns3::LteRlcUm::ReportBufferStatusTimer", TimeValue (MicroSeconds (100.0)));
  
  Config::SetDefault ("ns3::LteRlcUmLowLat::ReorderingTimeExpires", TimeValue (MilliSeconds (10.0)));
  Config::SetDefault ("ns3::LteRlcUm::ReorderingTimer", TimeValue (MilliSeconds (10.0)));
	Config::SetDefault ("ns3::LteRlcAm::ReorderingTimer", TimeValue (MilliSeconds (10.0)));
  
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (10 * 1024 * 1024));
  Config::SetDefault ("ns3::LteRlcUmLowLat::MaxTxBufferSize", UintegerValue (10 * 1024 * 1024));
  Config::SetDefault ("ns3::LteRlcAm::MaxTxBufferSize", UintegerValue (10 * 1024 * 1024));
  
	//Config::SetDefault ("ns3::MmWaveBeamforming::SmallScaleFading", BooleanValue (smallScale));
	//Config::SetDefault ("ns3::MmWaveBeamforming::FixSpeed", BooleanValue (true));
	//Config::SetDefault ("ns3::MmWaveBeamforming::UeSpeed", DoubleValue (speed));
	// configs for TCP
	if(tcpApp)
	{
		// enable RLC AM
		// Config::SetDefault ("ns3::MmWaveHelper::RlcAmEnabled", BooleanValue (true));
		// Config::SetDefault ("ns3::LteRlcAm::PollRetransmitTimer", TimeValue(MilliSeconds(2.0)));
		// Config::SetDefault ("ns3::LteRlcAm::ReorderingTimer", TimeValue(MilliSeconds(1.0)));
		// Config::SetDefault ("ns3::LteRlcAm::StatusProhibitTimer", TimeValue(MilliSeconds(1.0)));
		// Config::SetDefault ("ns3::LteRlcAm::ReportBufferStatusTimer", TimeValue(MilliSeconds(2.0)));
		// // the following may need to be adjusted
		// Config::SetDefault ("ns3::LteRlcAm::MaxTxBufferSize", UintegerValue (10 * 1024 * 1024));

		Config::SetDefault ("ns3::TcpSocketBase::MinRto", TimeValue (MilliSeconds (200)));
		Config::SetDefault ("ns3::Ipv4L3Protocol::FragmentExpirationTimeout", TimeValue (Seconds (0.2)));
		Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (packetSize-40));
		Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (1));
		Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (131072*400));
		Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (131072*400));
	}

    Config::SetDefault (schedulerType+"::HarqEnabled", BooleanValue (harqEnabled));
    //Config::SetDefault (schedulerType+"::sicEnabled", BooleanValue (sicEnabled));
    Config::SetDefault (schedulerType+"::CqiTimerThreshold", UintegerValue (100));

	Ptr<MmWaveHelper> mmwaveHelper = CreateObject<MmWaveHelper> ();
        mmwaveHelper->SetSchedulerType (schedulerType);
        mmwaveHelper->SetBeamformerType (beamformerType);

	mmwaveHelper->SetPathlossModelType ("ns3::ThreeGppUmaPropagationLossModel");
	mmwaveHelper->SetChannelConditionModelType ("ns3::ThreeGppUmaChannelConditionModel");
	mmwaveHelper->SetChannelModelType ("ns3::ThreeGppSpectrumPropagationLossModel");
	mmwaveHelper->SetChannelModelAttribute("Scenario",StringValue( "UMa" ));//TODO make this configurable in command line

	Ptr<MmWavePointToPointEpcHelper>  epcHelper = CreateObject<MmWavePointToPointEpcHelper> ();
	mmwaveHelper->SetEpcHelper (epcHelper);
	mmwaveHelper->SetHarqEnabled (harqEnabled);
	mmwaveHelper->Set_SIC (sicEnabled);

	Config::SetDefault ("ns3::MmWaveSpectrumPhy::SicEnabled", BooleanValue (sicEnabled));

	ConfigStore inputConfig;
	inputConfig.ConfigureDefaults ();

	// parse again so you can override default values from the command line
	cmd.Parse (argc, argv);

	// Create multiple RemoteHost
	Ptr<Node> pgw = epcHelper->GetPgwNode ();
	NodeContainer remoteHostContainer;
	remoteHostContainer.Create (numUe);
	InternetStackHelper internet;
	internet.Install (remoteHostContainer);
	std::vector<Ipv4Address> remoteHostAddresses;
	Ipv4StaticRoutingHelper ipv4RoutingHelper;

	// Create the Internet
	for (uint16_t hostId = 0; hostId < remoteHostContainer.GetN(); ++hostId)
	{
		Ipv4InterfaceContainer internetIpIfaces;
		
		auto remoteHost = remoteHostContainer.Get(hostId);

		PointToPointHelper p2ph;
		p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
		p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
		p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
		NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
		Ipv4AddressHelper ipv4h;
		std::ostringstream subnet;
		subnet << hostId << ".1.0.0";
		ipv4h.SetBase (subnet.str ().c_str (), "255.255.0.0");    
		internetIpIfaces = ipv4h.Assign (internetDevices);
		// interface 0 is localhost, 1 is the p2p device
		Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);
		remoteHostAddresses.push_back(remoteHostAddr);
		NS_LOG_UNCOND(remoteHostAddr); //segundo print
		Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
		remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
	}
	NodeContainer ueNodes;
	NodeContainer enbNodes;
	enbNodes.Create (numEnb);
	ueNodes.Create (numUe);

  //Install Mobility Model
	Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
	enbPositionAlloc->Add (Vector (0.0, 0.0, 25.0));
	MobilityHelper enbmobility;
	enbmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	enbmobility.SetPositionAllocator (enbPositionAlloc);
	enbmobility.Install (enbNodes);

	MobilityHelper uemobility;
	Ptr<UniformDiscPositionAllocator> uePositionAlloc = CreateObject<UniformDiscPositionAllocator> ();
  uePositionAlloc->SetRho (100.0);
  uePositionAlloc->SetZ (1.6);
	uemobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	uemobility.SetPositionAllocator (uePositionAlloc);
	uemobility.Install (ueNodes);

	// Install mmWave Devices to the nodes
	NetDeviceContainer enbmmWaveDevs = mmwaveHelper->InstallEnbDevice (enbNodes);
	NetDeviceContainer uemmWaveDevs = mmwaveHelper->InstallUeDevice (ueNodes);

	// Install the IP stack on the UEs
	internet.Install (ueNodes);
	Ipv4InterfaceContainer ueIpIface;
	ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (uemmWaveDevs));
	// Assign IP address to UEs, and install applications
	for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
	{
		Ptr<Node> ueNode = ueNodes.Get (u);
		// Set the default gateway for the UE
		Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
		ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
	}

	mmwaveHelper->AttachToClosestEnb (uemmWaveDevs, enbmmWaveDevs);

	// Install and start applications on UEs and remote host
	uint16_t dlPort = 1234;
	uint16_t ulPort = 2000;
	ApplicationContainer clientApps;
	ApplicationContainer serverApps;
	for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
	  {
	    if ( tcpApp )
	      {
	        NS_LOG_UNCOND("Building TCP apps for UE "<< (int) u << " with infinite app Buffer");
                ulPort++;
	        PacketSinkHelper dlPacketSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
	        PacketSinkHelper ulPacketSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
	        serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get (u)));
	        serverApps.Add (ulPacketSinkHelper.Install (remoteHostContainer.Get (u)));
	        BulkSendHelper dlClient ("ns3::TcpSocketFactory", InetSocketAddress ( ueIpIface.GetAddress (u), dlPort) );
	        dlClient.SetAttribute ("MaxBytes", UintegerValue ( 0 ));//this means send an infinite data stream
	        BulkSendHelper ulClient ("ns3::TcpSocketFactory", InetSocketAddress (remoteHostAddresses.at(u), ulPort ) );
	        ulClient.SetAttribute ("MaxBytes", UintegerValue ( 0 ));//this means send an infinite data stream
	        clientApps.Add (dlClient.Install (remoteHostContainer.Get(u)));
	        clientApps.Add (ulClient.Install (ueNodes.Get(u)));

	        // with a single remote host it is not possible to easily distinguish traces
			uint32_t serverId = remoteHostContainer.Get(u)->GetId();
			uint32_t ueId = ueNodes.Get(u)->GetId();

			NS_LOG_UNCOND("ueId " << ueId << " serverId " << serverId);

			std::ostringstream fileName;
			fileName << "tcp-rx-data"  << ueId+1 << ".txt";

			AsciiTraceHelper asciiTraceHelper;
			// connect RX trace
			Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (fileName.str ().c_str ());
			*stream->GetStream () << "Time" << "\t" << "PacketSize" << std::endl;
			serverApps.Get(2 * u)->TraceConnectWithoutContext("Rx", MakeBoundCallback (&Rx, stream));

			// connect the other traces (need to connect after the socket is created, see below)
			Simulator::Schedule (Seconds (startTime + 0.001 * u), &Traces, serverId, ueId, 
			    "", ".txt");

	      }
	    else
	      {
	    	//Aparece esto porque va por UDP para 7 UEs
	        NS_LOG_UNCOND("Building UDP apps for UE "<<(int) u<<" with packet period " << interPacketInterval << " us ");
	        ulPort++;
	        PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
	        PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
	        serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get (u)));
	        serverApps.Add (ulPacketSinkHelper.Install (remoteHostContainer.Get(u)));
	        UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
	        dlClient.SetAttribute ("Interval", TimeValue (MicroSeconds (interPacketInterval)));
	        dlClient.SetAttribute ("PacketSize", UintegerValue (packetSize));
	        dlClient.SetAttribute ("MaxPackets", UintegerValue (1+std::ceil(simTime*1000000/interPacketInterval)));//make sure app does not end prematurely
	        UdpClientHelper ulClient (remoteHostAddresses.at(u), ulPort);
	        ulClient.SetAttribute ("Interval", TimeValue (MicroSeconds (interPacketInterval)));
	        ulClient.SetAttribute ("PacketSize", UintegerValue (packetSize));
	        ulClient.SetAttribute ("MaxPackets", UintegerValue (1+std::ceil(simTime*1000000/interPacketInterval)));//make sure app does not end prematurely
	        clientApps.Add (dlClient.Install (remoteHostContainer.Get(u)));
	        clientApps.Add (ulClient.Install (ueNodes.Get(u)));
	      }
	  }
	serverApps.Start (Seconds (startTime));
	clientApps.Start (Seconds (startTime));
	mmwaveHelper->EnableTraces ();
	//Uncomment to enable PCAP tracing
	//p2ph.EnablePcapAll ("mmwave-epc-simple");

	Simulator::Stop (Seconds (simTime));
	Simulator::Run ();

	//Throughput
	for (uint32_t u=0; u < ueNodes.GetN(); u++)
	{
		Ptr<PacketSink> sink = serverApps.Get (2*(u+1)-2)->GetObject<PacketSink> ();
		//double nrThroughput = sink->GetTotalRx () * 8.0 / (1000000.0*(simTime - 0.01));
		NS_LOG_UNCOND ("The number of DL received bytes for UE " << u+1 << ": " << sink->GetTotalRx ());
		//NS_LOG_UNCOND ("UE(" << ueIpIface.GetAddress(0) <<") NR throughput: " << nrThroughput << " Mbps");
		sink = serverApps.Get (2*(u+1)-1)->GetObject<PacketSink> ();
//		//double nrThroughput = sink->GetTotalRx () * 8.0 / (1000000.0*(simTime - 0.01));
		NS_LOG_UNCOND ("The number of UL received bytes for UE " << u+1 << ": " << sink->GetTotalRx ());
//		//NS_LOG_UNCOND ("UE(" << ueIpIface.GetAddress(0) <<") NR throughput: " << nrThroughput << " Mbps");
	}

	Simulator::Destroy ();
	return 0;

}
