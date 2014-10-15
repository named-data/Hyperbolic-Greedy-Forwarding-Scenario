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


#ifndef _HG_NDN_GREEDY_STRATEGY_H
#define _HG_NDN_GREEDY_STRATEGY_H


// ns3 libraries
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ndnSIM-module.h"

#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/global-route-manager.h"
#include "ns3/netanim-module.h"
#include "ns3/assert.h"
#include "ns3/object.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "ns3/ndn-forwarding-strategy.h"
#include "ns3/ndn-l3-protocol.h"


#include "hg_ndn_graph.h"


namespace ns3 {
  namespace ndn {
    namespace fw {


      typedef ForwardingStrategy BaseStrategy;

      class Hg_ndn_greedy_strategy: public BaseStrategy {

      public:
        static TypeId GetTypeId ();        
        static std::string GetLogName ();          
        Hg_ndn_greedy_strategy();
        
        virtual void DidSendOutInterest (Ptr<Face> inFace, Ptr<Face> outFace,
                                         Ptr<const Interest> interest,
                                         Ptr<pit::Entry> pitEntry);       
        virtual void WillEraseTimedOutPendingInterest (Ptr<pit::Entry> pitEntry);        
        virtual void WillSatisfyPendingInterest (Ptr<Face> inFace,
                                                 Ptr<pit::Entry> pitEntry);

      protected:
        virtual bool  DoPropagateInterest (Ptr<Face> incomingFace,
                                           Ptr<const Interest> interest,
                                           Ptr<pit::Entry> pitEntry);       
        static LogComponent g_log;        

      private:
        uint32_t m_counter;

      };


    } // end fw namespace
  } // end of ndn namespace
} // end of ns3 namespace

#endif /* _HG_NDN_GREEDY_STRATEGY_H */
