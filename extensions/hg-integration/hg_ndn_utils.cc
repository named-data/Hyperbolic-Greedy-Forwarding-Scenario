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

#include "hg_ndn_utils.h"

void hg_install_route(ns3::ndn::Hg_ndn_graph *ndn_graph,
		      ns3::NodeContainer nodes, 
		      int producer,
		      string content_prefix,
		      ns3::ndn::GlobalRoutingHelper grh)
{
  grh.AddOrigins(content_prefix.c_str(), nodes.Get(producer));
    std::string prefix = content_prefix; // some prefix
  uint16_t metric = 0;                  // some routing metric
  uint32_t nNodes = nodes.GetN();
  uint32_t i;    
  ns3::Ptr<ns3::Node> node;
  ns3::Ptr<ns3::Node> node_neigh;
  vector<int> neighbors;
  vector<int> :: const_iterator j;
  ns3::Ptr<ns3::ndn::L3Protocol> protocol;
  ns3::Ptr<ns3::ndn::Face> face;
  const hg_graph_t * g = nodes.Get(0)->GetObject<ns3::ndn::Hg_ndn_node_info>()->get_graph_ptr();

  // each node is a possible route toward the destination
  for (i = 0; i < nNodes; ++i) 
    {
      if(i == producer)
      	continue;

      node = nodes.Get(i);
      neighbors = ndn_graph->get_node_neighbors(i);
      for (j = neighbors.begin(); j != neighbors.end(); j++)
	{
	  // the idea is to use the metric as a way to
	  // tell to which node is connected every face
	  metric = *j;
	  // adding a new route on node "node": each of its neighbors
	  // is a good candidate for the prefix
	  ns3::ndn::StackHelper::AddRoute(node, prefix, nodes.Get(*j), metric);        
	}
    } 
  return;

}



void install_route_on_every_node(ns3::ndn::Hg_ndn_graph *ndn_graph,
				 ns3::NodeContainer nodes, 
				 int producer,
				 string content_prefix) 
{

  std::string prefix = content_prefix; // some prefix
  uint16_t metric = 0;                  // some routing metric
  uint32_t nNodes = nodes.GetN();
  uint32_t i;    
  ns3::Ptr<ns3::Node> node;
  ns3::Ptr<ns3::Node> node_neigh;
  vector<int> neighbors;
  vector<int> :: const_iterator j;
  ns3::Ptr<ns3::ndn::L3Protocol> protocol;
  ns3::Ptr<ns3::ndn::Face> face;
  const hg_graph_t * g = nodes.Get(0)->GetObject<ns3::ndn::Hg_ndn_node_info>()->get_graph_ptr();

  // each node is a possible route toward the destination
  for (i = 0; i < nNodes; ++i) 
    {
      if(i == producer)
      	continue;

      node = nodes.Get(i);
      neighbors = ndn_graph->get_node_neighbors(i);
      for (j = neighbors.begin(); j != neighbors.end(); j++)
	{
	  // the idea is to use the metric as a way to
	  // tell to which node is connected every face
	  metric = *j;
	  // adding a new route on node "node": each of its neighbors
	  // is a good candidate for the prefix
	  ns3::ndn::StackHelper::AddRoute(node, prefix, nodes.Get(*j), metric);        
	}
    } 
  return;
}

