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

#include "hg_ndn_node_info.h"


namespace ns3 {
  namespace ndn {

    NS_OBJECT_ENSURE_REGISTERED(Hg_ndn_node_info);

    TypeId Hg_ndn_node_info::GetTypeId ( ) {
      static TypeId tid = TypeId ("ns3::ndn::Hg_ndn_node_info")
	.SetGroupName("ndn")
	.SetParent<Object>()
	.AddConstructor<Hg_ndn_node_info> ()   
	;
      return tid;
    }
    
    Hg_ndn_node_info::Hg_ndn_node_info() :
      graph(NULL) {
      coordinate.r = 0;
      coordinate.theta = 0;
      connected_component_id = 0;
    }

    Hg_ndn_node_info::Hg_ndn_node_info(const hg_graph_t * g, const hg_coordinate_t & c,  const int & id) :
      graph(g) {
      this->coordinate.r = c.r;
      this->coordinate.theta = c.theta;  
      this->connected_component_id = id;
    }

    Hg_ndn_node_info::Hg_ndn_node_info(const Hg_ndn_node_info & other) :
      graph(other.graph) {
      this->coordinate = other.coordinate;
      this->connected_component_id = other.connected_component_id;
    }

    Hg_ndn_node_info::~Hg_ndn_node_info() {
      graph = NULL;
    }


    const hg_graph_t * Hg_ndn_node_info::get_graph_ptr() const {
      return this->graph ;
    }

    const hg_coordinate_t Hg_ndn_node_info::get_coordinate() const {
      return this->coordinate;
    }

    const int Hg_ndn_node_info::get_connected_component_id() const {
      return this->connected_component_id;
    }

  } // end of ndn namespace
} // end of ns3 namespace
