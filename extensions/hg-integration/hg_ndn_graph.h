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


#ifndef _HG_NDN_GRAPH_H
#define _HG_NDN_GRAPH_H


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
#include "hg_ndn_node_info.h"


namespace ns3 {
  namespace ndn {

    class Hg_ndn_graph : public Object {
    private:
      // pointer to the graph
      hg_graph_t * graph;
      // set of nodes
      NodeContainer nodes;
      void init_graph(const hg_graph_t * graph);
    public: 
      static TypeId GetTypeId (void);
      // default constructor (gamma 2, temperature 0 i.e., hyperbolic rgg)
      Hg_ndn_graph();
      // load graph from file
      Hg_ndn_graph(const string graph_filename);
      // graph constructor
      Hg_ndn_graph(const int n, const double k_bar, const double exp_gamma,
		   const double t, const double zeta, const int seed);
      // copy constructor
      Hg_ndn_graph(const Hg_ndn_graph & other);
      // graph destructor
      ~Hg_ndn_graph();
       // print coordinates
      void print_coordinates() const ;
      // get the nodes
      NodeContainer get_nodes() const;
      // get random node id
      int get_random_node_id() const;
      // check if two nodes belong to the same connected component 
      bool same_connected_component(int i, int j) const;
    };

  }
}

#endif /* _HG_NDN_GRAPH_H */
