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

#include "hg_ndn_graph.h"
#include <boost/graph/connected_components.hpp>



namespace ns3 {
  namespace ndn {
    
    NS_OBJECT_ENSURE_REGISTERED(Hg_ndn_graph);


    // temporary function
    static hg_coordinate_t get_coordinate_from_hg_graph(const hg_graph_t * graph, uint32_t id){
      assert(graph !=NULL);
      hg_coordinate_t c;
      c.r = (*graph)[id].r;
      c.theta = (*graph)[id].theta;
      return c;
    }

    void Hg_ndn_graph::init_graph(const hg_graph_t * graph) {
      assert(graph != NULL);
      // compute connected components
      std::vector<int> component((*graph)[boost::graph_bundle].expected_n);
      int num_components = connected_components(*graph, &(component[0]));
      // create n nodes
      this->nodes.Create((*graph)[boost::graph_bundle].expected_n); 
      Ptr<Node> node_ptr;
      uint32_t nNodes = this->nodes.GetN();
      uint32_t i; // iterator
      for (i = 0; i < nNodes; ++i) {
	node_ptr = this->nodes.Get(i);
	// aggregation
	// https://www.nsnam.org/docs/release/3.13/manual/html/object-model.html#aggregation
	// "aggregate" coordinates and pointer to hg_graph_t
	Ptr<Hg_ndn_node_info> hni = CreateObject<Hg_ndn_node_info>(graph, 
								   get_coordinate_from_hg_graph(graph, i),
								   component[i]);
	node_ptr->AggregateObject(hni);
      }
      // Connect NDN nodes according to the graph 
      // PointToPointHelper p2p;
      int source_node, destination_node;
      hg_graph_t::edge_iterator edgeIt, edgeEnd;
      tie(edgeIt, edgeEnd) = edges(*graph);
      for (; edgeIt != edgeEnd; ++edgeIt) { 
	source_node = source(*edgeIt, *graph);
	destination_node = target(*edgeIt, *graph); 
	// std::cout << "T:\t" << source_node << "\t" << destination_node << endl;
	this->p2p.Install(this->nodes.Get(source_node), this->nodes.Get(destination_node));
      }
      // reset seed before any other computation
      hg_init_random_generator((*graph)[boost::graph_bundle].seed);
    }


    TypeId Hg_ndn_graph::GetTypeId ( ) {
      static TypeId tid = TypeId ("ns3::ndn::Hg_ndn_graph")
	.SetGroupName("ndn")
	.SetParent<Object>()
	.AddConstructor<Hg_ndn_graph> ()   
	;
      return tid;
    }

    Hg_ndn_graph::Hg_ndn_graph() {
      /* default graph:
       * 1000 nodes
       * gamma = 2
       * temperature = 0
       * zeta = 1
       * seed = 1
       */
      this->graph = hg_graph_generator(1000, 10, 2, 0, 1, 1);
      init_graph(this->graph);
    }

    Hg_ndn_graph::Hg_ndn_graph(const string graph_filename) {
      this->graph = hg_read_graph(graph_filename);
      init_graph(this->graph);
    }

    Hg_ndn_graph::Hg_ndn_graph(const int n, const double k_bar, const double exp_gamma,
			       const double t, const double zeta, const int seed) {
      // build graph according to hg-lib
      this->graph = hg_graph_generator(n, k_bar, exp_gamma, t, zeta, seed);
      init_graph(this->graph);
    }


    Hg_ndn_graph::Hg_ndn_graph(const Hg_ndn_graph & other) {
      assert(other.graph !=NULL);
      this->graph = new hg_graph_t((*other.graph)[boost::graph_bundle].expected_n);
      // copy graph properties
      (*this->graph)[boost::graph_bundle].type = (*other.graph)[boost::graph_bundle].type;
      (*this->graph)[boost::graph_bundle].expected_n = (*other.graph)[boost::graph_bundle].expected_n;
      (*this->graph)[boost::graph_bundle].temperature = (*other.graph)[boost::graph_bundle].temperature;
      (*this->graph)[boost::graph_bundle].expected_gamma = (*other.graph)[boost::graph_bundle].expected_gamma;
      (*this->graph)[boost::graph_bundle].expected_degree = (*other.graph)[boost::graph_bundle].expected_degree;
      (*this->graph)[boost::graph_bundle].zeta_eta = (*other.graph)[boost::graph_bundle].zeta_eta;
      (*this->graph)[boost::graph_bundle].seed = (*other.graph)[boost::graph_bundle].seed;
      // copying nodes coordinates
      int id; // node identifier [0,n-1]
      for(id = 0; id < (*other.graph)[boost::graph_bundle].expected_n; id++) {
	(*this->graph)[id].r = (*other.graph)[id].r;
	(*this->graph)[id].theta = (*other.graph)[id].theta;
      }
      int source_node, destination_node;
      hg_graph_t::edge_iterator edgeIt, edgeEnd;
      tie(edgeIt, edgeEnd) = edges(*other.graph);
      for (; edgeIt != edgeEnd; ++edgeIt) { 
	source_node = source(*edgeIt, *other.graph);
	destination_node = target(*edgeIt, *other.graph); 
	add_edge(source_node, destination_node, *this->graph);
      }
      init_graph(this->graph);
    }
    
    Hg_ndn_graph::~Hg_ndn_graph() {
      delete this->graph; 
      this->graph = NULL;
    }

    void Hg_ndn_graph::print_coordinates() const {
      Ptr<Node> node_ptr;
      Ptr<Hg_ndn_node_info> hnc;
      uint32_t i; // iterator
      uint32_t nNodes = this->nodes.GetN ();
      hg_coordinate_t c;
      for (i = 0; i < nNodes; ++i) {
	node_ptr = this->nodes.Get(i);
	hnc = node_ptr->GetObject<Hg_ndn_node_info>();
	c = hnc->get_coordinate();
	cout << i << "\t" << c.r << "\t" << c.theta << endl;
      }
    }

    NodeContainer Hg_ndn_graph::get_nodes() const {
      return nodes;
    }

    int Hg_ndn_graph::get_random_node_id() const {
      return floor(hg_rand_01_wrapper() *((*this->graph)[boost::graph_bundle].expected_n - 1.0));
}
    
    bool Hg_ndn_graph::same_connected_component(int i, int j) const {
      if( i < (*this->graph)[boost::graph_bundle].expected_n and
	  j < (*this->graph)[boost::graph_bundle].expected_n ) {
	Ptr<Node> i_ptr = this->nodes.Get(i);
	Ptr<Node> j_ptr = this->nodes.Get(j);
	if(i_ptr->GetObject<Hg_ndn_node_info>()->get_connected_component_id() ==
	   j_ptr->GetObject<Hg_ndn_node_info>()->get_connected_component_id()){
	  return true;
	}
      }
      return false;      
    }

    vector<int> Hg_ndn_graph::get_node_neighbors(int node) const {
      vector<int> neighbors_ids;
      hg_graph_t::adjacency_iterator neighbourIt, neighbourEnd;

      tie(neighbourIt, neighbourEnd) = adjacent_vertices(node, *graph); 
      int k = 0;
      for (; neighbourIt != neighbourEnd; ++neighbourIt){ 
	neighbors_ids.push_back(*neighbourIt);
      }
      
      return neighbors_ids;
    }


  } // end of ndn namespace
} // end of ns3 namespace
