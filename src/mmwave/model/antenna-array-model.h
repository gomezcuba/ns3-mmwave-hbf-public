/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
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
*
*   Author: Marco Mezzavilla < mezzavilla@nyu.edu>
*                Sourjya Dutta <sdutta@nyu.edu>
*                Russell Ford <russell.ford@nyu.edu>
*                Menglei Zhang <menglei@nyu.edu>
*/

#ifndef ANTENNA_ARRAY_MODEL_H_
#define ANTENNA_ARRAY_MODEL_H_
#include <ns3/antenna-model.h>
#include <complex>
#include <ns3/net-device.h>
#include <map>
#include <ns3/nstime.h>
#include <ns3/node.h>
#include <ns3/mobility-model.h>

namespace ns3 {

namespace mmwave {

typedef std::vector< std::complex<double> > complexVector_t;

class AntennaArrayModel : public AntennaModel
{
public:
  AntennaArrayModel ();
  virtual ~AntennaArrayModel ();
  static TypeId GetTypeId ();
  virtual double GetGainDb (Angles a);

  // to store dummy info
  void SetBeamformingVectorPanel (complexVector_t antennaWeights, Ptr<NetDevice> device = 0);
  void SetBeamformingVectorWithDelay (complexVector_t antennaWeights, Ptr<NetDevice> device = 0);

  void SetBeamformingVectorPanelDevices (Ptr<NetDevice> thisDevice = 0, Ptr<NetDevice> otherDevice = 0);
  void ChangeBeamformingVectorPanel (Ptr<NetDevice> device);
  void ChangeBeamformingVectorPanelMultiLayer (Ptr<NetDevice> device, uint8_t layerInd);
  
  complexVector_t GetBeamformingVectorPanel ();
  complexVector_t GetBeamformingVectorPanel (Ptr<NetDevice> device);
  complexVector_t GetBeamformingVectorPanelMultiLayer (uint8_t layerInd);

  void ClearBeamformingVectorList ();
  
  void ChangeToOmniTx ();
  bool IsOmniTx ();
  double GetRadiationPattern (double vangle, double hangle = 0);
  Vector GetAntennaLocation (uint16_t index, uint16_t* antennaNum);
  void SetSector (uint8_t sector, uint16_t *antennaNum, double elevation = 90);

  void SetPlanesNumber (double planesNumber);
  double GetPlanesId (void);
  void SetDeviceType (bool isUe);
  void SetTotNoArrayElements (double arrayElements);
  double GetOffset ();

  void SetCurrNumLayers (uint8_t currNumLayers);
  uint8_t GetCurrNumLayers ();

  Ptr<NetDevice> GetCurrentDevice ();
  Ptr<NetDevice> GetCurrentDeviceMultilayer (uint8_t layerInd);
  Time GetLastUpdate (Ptr<NetDevice> device);

private:
  bool m_omniTx;
  // double m_minAngle;
  // double m_maxAngle;
//  complexVector_t m_beamformingVector;
  std::map<uint8_t, complexVector_t> m_beamformingVectorList;

//  int m_currentPanelId;
  std::map<uint8_t, int> m_currentPanelIdList;

  // std::map<Ptr<NetDevice>, complexVector_t> m_beamformingVectorMap;
  std::map<Ptr<NetDevice>, std::pair<complexVector_t,int> > m_beamformingVectorPanelMap;

  double m_disV;       //antenna spacing in the vertical direction in terms of wave length.
  double m_disH;       //antenna spacing in the horizontal direction in terms of wave length.

  double m_noPlane;
  bool m_isUe;
  double m_totNoArrayElements;
  double m_hpbw;
  double m_gMax;

//  Ptr<NetDevice> m_currentDev;
  std::map<uint8_t, Ptr<NetDevice> > m_currentDevList;

  std::map<Ptr<NetDevice>, Time> m_lastUpdateMap;
  std::map<Ptr<NetDevice>, Time> m_lastUpdatePairMap;

  bool m_isotropicElement;

  uint8_t m_currNumLayers;
};

} /* namespace mmwave */

} /* namespace ns3 */

#endif /* SRC_ANTENNA_MODEL_ANTENNA_ARRAY_MODEL_H_ */
