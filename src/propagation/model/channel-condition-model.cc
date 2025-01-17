/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 SIGNET Lab, Department of Information Engineering,
 * University of Padova
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "channel-condition-model.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/mobility-model.h"
#include <cmath>
#include "ns3/node.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ChannelConditionModel");

// ------------------------------------------------------------------------- //
NS_OBJECT_ENSURE_REGISTERED (ChannelCondition);

TypeId
ChannelCondition::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ChannelCondition")
    .SetParent<Object> ()
    .SetGroupName ("Propagation")
  ;
  return tid;
}

ChannelCondition::ChannelCondition ()
{
}

ChannelCondition::~ChannelCondition ()
{
}

ChannelCondition::LosConditionValue
ChannelCondition::GetLosCondition ()
{
  return m_losCondition;
}

void
ChannelCondition::SetLosCondition (LosConditionValue cond)
{
  m_losCondition = cond;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (ChannelConditionModel);

TypeId
ChannelConditionModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ChannelConditionModel")
    .SetParent<Object> ()
    .SetGroupName ("Propagation")
  ;
  return tid;
}

ChannelConditionModel::ChannelConditionModel ()
{
}

ChannelConditionModel::~ChannelConditionModel ()
{
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (ThreeGppChannelConditionModel);

TypeId
ThreeGppChannelConditionModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ThreeGppChannelConditionModel")
    .SetParent<ChannelConditionModel> ()
    .SetGroupName ("Propagation")
    .AddAttribute ("UpdatePeriod", "Update the shadow fading after this time.",
                   TimeValue (MilliSeconds (100.0)),
                   MakeTimeAccessor (&ThreeGppChannelConditionModel::m_updatePeriod),
                   MakeTimeChecker ())
  ;
  return tid;
}

ThreeGppChannelConditionModel::ThreeGppChannelConditionModel ()
  : ChannelConditionModel ()
{
  m_uniformVar = CreateObject<UniformRandomVariable> ();
  m_uniformVar->SetAttribute ("Min", DoubleValue (0));
  m_uniformVar->SetAttribute ("Max", DoubleValue (1));
}

ThreeGppChannelConditionModel::~ThreeGppChannelConditionModel ()
{
}

Ptr<ChannelCondition>
ThreeGppChannelConditionModel::GetChannelCondition (Ptr<const MobilityModel> a,
                                                    Ptr<const MobilityModel> b)
{
  Ptr<ChannelCondition> cond;

  // get the key for this channel
  uint32_t key = GetKey (a, b);

  bool notFound = false; // indicates if the channel condition is not present in the map
  bool update = false; // indicates if the channel condition has to be updated

  // look for the channel condition in m_channelConditionMap
  if (m_channelConditionMap.find (key) != m_channelConditionMap.end ())
    {
      NS_LOG_DEBUG ("found the channel condition in the map");
      cond = m_channelConditionMap.at (key).m_condition;

      // check if it has to be updated
      if (m_updatePeriod.GetNanoSeconds () != 0.0 && Simulator::Now () - m_channelConditionMap.at (key).m_generatedTime > m_updatePeriod)
        {
          NS_LOG_DEBUG ("it has to be updated");
          update = true;
        }
    }
  else
    {
      NS_LOG_DEBUG ("channel condition not found");
      notFound = true;
    }

  // if the channel condition was not found or if it has to be updated
  // generate a new channel condition
  if (notFound || update)
    {
      // compute the LOS probability (see 3GPP TR 38.901, Sec. 7.4.2)
      double pLos = ComputePlos (a, b);

      // draw a random value
      double pRef = m_uniformVar->GetValue ();

      // get the channel condition
      cond = CreateObject<ChannelCondition> ();
      if (pRef <= pLos)
        {
          // LOS
          cond->SetLosCondition (ChannelCondition::LosConditionValue::LOS);
        }
      else
        {
          // NLOS
          cond->SetLosCondition (ChannelCondition::LosConditionValue::NLOS);
        }

      // store the channel condition in m_channelConditionMap
      Item mapItem;
      mapItem.m_condition = cond;
      mapItem.m_generatedTime = Simulator::Now ();
      m_channelConditionMap [key] = mapItem;
    }

  return cond;
}

int64_t
ThreeGppChannelConditionModel::AssignStreams (int64_t stream)
{
  m_uniformVar->SetStream (stream);
  return 1;
}

double
ThreeGppChannelConditionModel::Calculate2dDistance (Vector a, Vector b)
{
  double x = a.x - b.x;
  double y = a.y - b.y;
  double distance2D = sqrt (x * x + y * y);

  return distance2D;
}

uint32_t
ThreeGppChannelConditionModel::GetKey (Ptr<const MobilityModel> a, Ptr<const MobilityModel> b)
{
  // use the nodes ids to obtain an unique key for the channel between a and b
  // sort the nodes ids so that the key is reciprocal
  uint32_t x1 = std::min (a->GetObject<Node> ()->GetId (), b->GetObject<Node> ()->GetId ());
  uint32_t x2 = std::max (a->GetObject<Node> ()->GetId (), b->GetObject<Node> ()->GetId ());

  // use the cantor function to obtain the key
  uint32_t key = (((x1 + x2) * (x1 + x2 + 1)) / 2) + x2;

  return key;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (ThreeGppRmaChannelConditionModel);

TypeId
ThreeGppRmaChannelConditionModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ThreeGppRmaChannelConditionModel")
    .SetParent<ThreeGppChannelConditionModel> ()
    .SetGroupName ("Propagation")
    .AddConstructor<ThreeGppRmaChannelConditionModel> ()
  ;
  return tid;
}

ThreeGppRmaChannelConditionModel::ThreeGppRmaChannelConditionModel ()
  : ThreeGppChannelConditionModel ()
{
}

ThreeGppRmaChannelConditionModel::~ThreeGppRmaChannelConditionModel ()
{
}

double
ThreeGppRmaChannelConditionModel::ComputePlos (Ptr<const MobilityModel> a,
                                               Ptr<const MobilityModel> b)
{
  // compute the 2D distance between a and b
  double distance2D = Calculate2dDistance (a->GetPosition (), b->GetPosition ());

  // NOTE: no indication is given about the heights of the BS and the UT used
  // to derive the LOS probability

  // compute the LOS probability (see 3GPP TR 38.901, Sec. 7.4.2)
  double pLos = 0.0;
  if (distance2D <= 10.0)
    {
      pLos = 1.0;
    }
  else
    {
      pLos = exp (-(distance2D - 10.0) / 1000.0);
    }

  return pLos;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (ThreeGppUmaChannelConditionModel);

TypeId
ThreeGppUmaChannelConditionModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ThreeGppUmaChannelConditionModel")
    .SetParent<ThreeGppChannelConditionModel> ()
    .SetGroupName ("Propagation")
    .AddConstructor<ThreeGppUmaChannelConditionModel> ()
  ;
  return tid;
}

ThreeGppUmaChannelConditionModel::ThreeGppUmaChannelConditionModel ()
  : ThreeGppChannelConditionModel ()
{
}

ThreeGppUmaChannelConditionModel::~ThreeGppUmaChannelConditionModel ()
{
}

double
ThreeGppUmaChannelConditionModel::ComputePlos (Ptr<const MobilityModel> a,
                                               Ptr<const MobilityModel> b)
{
  // compute the 2D distance between a and b
  double distance2D = Calculate2dDistance (a->GetPosition (), b->GetPosition ());

  // retrieve h_UT, it should be smaller than 23 m
  double h_UT = std::min (a->GetPosition ().z, b->GetPosition ().z);
  if (h_UT > 23.0)
    {
      NS_LOG_WARN ("The height of the UT should be smaller than 23 m (see TR 38.901, Table 7.4.2-1)");
    }

  // retrieve h_BS, it should be equal to 25 m
  double h_BS = std::max (a->GetPosition ().z, b->GetPosition ().z);
  if (h_BS != 25.0)
    {
      NS_LOG_WARN ("The LOS probability was derived assuming BS antenna heights of 25 m (see TR 38.901, Table 7.4.2-1)");
    }

  // compute the LOS probability (see 3GPP TR 38.901, Sec. 7.4.2)
  double pLos = 0.0;
  if (distance2D <= 18.0)
    {
      pLos = 1.0;
    }
  else
    {
      // compute C'(h_UT)
      double c = 0.0;
      if (h_UT <= 13.0)
        {
          c = 0;
        }
      else
        {
          c = pow ((h_UT - 13.0) / 10.0, 1.5);
        }

      pLos = (18.0 / distance2D + exp (-distance2D / 63.0) * (1.0 - 18.0 / distance2D)) * (1.0 + c * 5.0 / 4.0 * pow (distance2D / 100.0, 3.0) * exp (-distance2D / 150.0));
    }

  return pLos;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (ThreeGppUmiStreetCanyonChannelConditionModel);

TypeId
ThreeGppUmiStreetCanyonChannelConditionModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ThreeGppUmiStreetCanyonChannelConditionModel")
    .SetParent<ThreeGppChannelConditionModel> ()
    .SetGroupName ("Propagation")
    .AddConstructor<ThreeGppUmiStreetCanyonChannelConditionModel> ()
  ;
  return tid;
}

ThreeGppUmiStreetCanyonChannelConditionModel::ThreeGppUmiStreetCanyonChannelConditionModel ()
  : ThreeGppChannelConditionModel ()
{
}

ThreeGppUmiStreetCanyonChannelConditionModel::~ThreeGppUmiStreetCanyonChannelConditionModel ()
{
}

double
ThreeGppUmiStreetCanyonChannelConditionModel::ComputePlos (Ptr<const MobilityModel> a,
                                                           Ptr<const MobilityModel> b)
{
  // compute the 2D distance between a and b
  double distance2D = Calculate2dDistance (a->GetPosition (), b->GetPosition ());

  // NOTE: no idication is given about the UT height used to derive the
  // LOS probability

  // h_BS should be equal to 10 m. We check if at least one of the two
  // nodes has height equal to 10 m
  if (a->GetPosition ().z != 10.0 && b->GetPosition ().z)
    {
      NS_LOG_WARN ("The LOS probability was derived assuming BS antenna heights of 10 m (see TR 38.901, Table 7.4.2-1)");
    }

  // compute the LOS probability (see 3GPP TR 38.901, Sec. 7.4.2)
  double pLos = 0.0;
  if (distance2D <= 18.0)
    {
      pLos = 1.0;
    }
  else
    {
      pLos = 18.0 / distance2D + exp (-distance2D / 36.0) * (1.0 - 18.0 / distance2D);
    }

  return pLos;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (ThreeGppIndoorMixedOfficeChannelConditionModel);

TypeId
ThreeGppIndoorMixedOfficeChannelConditionModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ThreeGppIndoorMixedOfficeChannelConditionModel")
    .SetParent<ThreeGppChannelConditionModel> ()
    .SetGroupName ("Propagation")
    .AddConstructor<ThreeGppIndoorMixedOfficeChannelConditionModel> ()
  ;
  return tid;
}

ThreeGppIndoorMixedOfficeChannelConditionModel::ThreeGppIndoorMixedOfficeChannelConditionModel ()
  : ThreeGppChannelConditionModel ()
{
}

ThreeGppIndoorMixedOfficeChannelConditionModel::~ThreeGppIndoorMixedOfficeChannelConditionModel ()
{
}

double
ThreeGppIndoorMixedOfficeChannelConditionModel::ComputePlos (Ptr<const MobilityModel> a,
                                                             Ptr<const MobilityModel> b)
{
  // compute the 2D distance between a and b
  double distance2D = Calculate2dDistance (a->GetPosition (), b->GetPosition ());

  // NOTE: no idication is given about the UT height used to derive the
  // LOS probability

  // retrieve h_BS, it should be equal to 3 m
  double h_BS = std::max (a->GetPosition ().z, b->GetPosition ().z);
  if (h_BS != 3.0)
    {
      NS_LOG_WARN ("The LOS probability was derived assuming BS antenna heights of 3 m (see TR 38.901, Table 7.4.2-1)");
    }

  // compute the LOS probability (see 3GPP TR 38.901, Sec. 7.4.2)
  double pLos = 0.0;
  if (distance2D <= 1.2)
    {
      pLos = 1.0;
    }
  else if (distance2D > 1.2 && distance2D < 6.5)
    {
      pLos = exp (-(distance2D - 1.2) / 4.7);
    }
  else
    {
      pLos = exp (-(distance2D - 6.5) / 32.6) * 0.32;
    }

  return pLos;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (ThreeGppIndoorOpenOfficeChannelConditionModel);

TypeId
ThreeGppIndoorOpenOfficeChannelConditionModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ThreeGppIndoorOpenOfficeChannelConditionModel")
    .SetParent<ThreeGppChannelConditionModel> ()
    .SetGroupName ("Propagation")
    .AddConstructor<ThreeGppIndoorOpenOfficeChannelConditionModel> ()
  ;
  return tid;
}

ThreeGppIndoorOpenOfficeChannelConditionModel::ThreeGppIndoorOpenOfficeChannelConditionModel ()
  : ThreeGppChannelConditionModel ()
{
}

ThreeGppIndoorOpenOfficeChannelConditionModel::~ThreeGppIndoorOpenOfficeChannelConditionModel ()
{
}

double
ThreeGppIndoorOpenOfficeChannelConditionModel::ComputePlos (Ptr<const MobilityModel> a,
                                                            Ptr<const MobilityModel> b)
{
  // compute the 2D distance between a and b
  double distance2D = Calculate2dDistance (a->GetPosition (), b->GetPosition ());

  // NOTE: no idication is given about the UT height used to derive the
  // LOS probability

  // retrieve h_BS, it should be equal to 3 m
  double h_BS = std::max (a->GetPosition ().z, b->GetPosition ().z);
  if (h_BS != 3.0)
    {
      NS_LOG_WARN ("The LOS probability was derived assuming BS antenna heights of 3 m (see TR 38.901, Table 7.4.2-1)");
    }

  // compute the LOS probability (see 3GPP TR 38.901, Sec. 7.4.2)
  double pLos = 0.0;
  if (distance2D <= 5.0)
    {
      pLos = 1.0;
    }
  else if (distance2D > 5.0 && distance2D <= 49.0)
    {
      pLos = exp (-(distance2D - 5.0) / 70.8);
    }
  else
    {
      pLos = exp (-(distance2D - 49.0) / 211.7) * 0.54;
    }

  return pLos;
}

} // end namespace ns3
