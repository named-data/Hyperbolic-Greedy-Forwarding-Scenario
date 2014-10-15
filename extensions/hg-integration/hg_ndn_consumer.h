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


#ifndef _HG_NDN_CONSUMER_H
#define _HG_NDN_CONSUMER_H


// ns3 libraries
#include "ns3/core-module.h"
#include "ns3/ndnSIM/apps/ndn-consumer-batches.h"

namespace ns3 {
  namespace ndn {

    class HGConsumer: public ConsumerBatches {
      
    public:
      static TypeId GetTypeId ();        
      static std::string GetLogName ();          
      HGConsumer();
      
      virtual void OnData (Ptr<const Data> contentObject);
      virtual void OnTimeout (uint32_t sequenceNumber);
    
      static int GetReceivedDataCount() { return received_data; }
      static int GetTimeoutDataCount() { return timeout_data; }

    protected:
      static int received_data;
      static int timeout_data;
    };

  } // end of ndn namespace
} // end of ns3 namespace

#endif /* _HG_NDN_CONSUMER_H */
