/*
 * The ndn-sim hyperbolic graphs module
 *
 * Chiara Orsini, CAIDA, UC San Diego
 * chiara@caida.org
 *
 * Copyright (C) 2014 The Regents of the University of California.
 *
 * This file is part of the ndn-sim hyperbolic graphs module.
 *
 * The ndn-sim hyperbolic graphs module is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The ndn-sim hyperbolic graphs module is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the ndn-sim hyperbolic graphs module.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "hg_ndn_consumer.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"

#include "ns3/ndn-app-face.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"

NS_LOG_COMPONENT_DEFINE ("ndn.HGConsumer");

namespace ns3 {
  namespace ndn {

    NS_OBJECT_ENSURE_REGISTERED(HGConsumer);

    int HGConsumer::received_data = 0;
    int HGConsumer::timeout_data = 0;
    
    std::string HGConsumer::GetLogName () {
      return "ndn.HGConsumer";
    }
      
    TypeId HGConsumer::GetTypeId (void) {
      static TypeId tid = TypeId ("ns3::ndn::HGConsumer")
	.SetGroupName ("Ndn")
	.SetParent <ConsumerBatches> ()
	.AddConstructor <HGConsumer>()
	;
      return tid;
    }

    HGConsumer::HGConsumer () {
      // nothing to do for constructor

    }
    
    void HGConsumer::OnData (Ptr<const Data> contentObject) {      
      Consumer::OnData(contentObject);
      std::cout  << "RECEIVED" << std::endl;
      NS_LOG_DEBUG ("Received Data packet");
      received_data++;
    }

    void HGConsumer::OnTimeout (uint32_t sequenceNumber) {
      Consumer::OnTimeout(sequenceNumber);
      timeout_data++;
    }

  } // end of ndn namespace
} // end of ns3 namespace
