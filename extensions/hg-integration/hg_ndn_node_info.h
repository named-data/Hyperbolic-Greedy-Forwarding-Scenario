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


#ifndef _HG_NDN_NODE_INFO_H
#define _HG_NDN_NODE_INFO_H


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


#include "../hg-lib/hg_graphs_lib.h"


namespace ns3 {
  namespace ndn {
    
    // Object to attach to every node of 
    // hg_ndn_graph

    class Hg_ndn_node_info : public Object {
      // node hyperbolic coordinate
      hg_coordinate_t coordinate;
      int connected_component_id;
      // pointer to the graph
      const hg_graph_t * graph;      
    public:
      static TypeId GetTypeId ();
      // fake constructor
      Hg_ndn_node_info();
      // real constructor
      Hg_ndn_node_info(const hg_graph_t * g, const hg_coordinate_t & c, const int & id);
      // copy constructor
      Hg_ndn_node_info(const Hg_ndn_node_info & other);
      // destructor
      ~Hg_ndn_node_info();
      // get coordinate
      const hg_coordinate_t get_coordinate() const;
      // get pointer to graph
      const hg_graph_t * get_graph_ptr() const;
      // get connected component id
      const int get_connected_component_id() const;      
    };



  }
}

#endif /* _HG_NDN_NODE_INFO_H */
