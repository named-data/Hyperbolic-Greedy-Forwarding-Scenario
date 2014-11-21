/*
 * The HG-graphs package
 *
 * Chiara Orsini, CAIDA, UC San Diego
 * chiara@caida.org
 *
 * Copyright (C) 2014 The Regents of the University of California.
 *
 * This file is part of the HG-graphs package.
 *
 * The HG-graphs package is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The HG-graphs package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with theHG-graphs package.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

// standard libraries
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include <sstream>


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
#include "ns3/ipv4-global-routing-helper.h"

#include "hg-integration/hg_ndn_graph.h"
#include "hg-integration/hg_ndn_consumer.h"

using namespace std;


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



void usage (string exe_name){
  cout << "NAME: " << endl;
  cout << "\t" << "hg_greedy_test" << " - test the performances of the greedy forwarding strategy" << endl;
  cout << "\t" << "\t" << "\t" << " in a ndnSIM scenario that simulates a network embedded" << endl;
  cout << "\t" << "\t" << "\t" << " into a hyperbolic space." << endl;
  cout << endl;
  cout << "SYNOPSIS: " << endl;
  cout << "\t" << "hg_greedy_test" << " [options] [args] " << endl;
  cout << endl;
  cout << "DESCRIPTION:" << endl;
  cout << "\t" << "Build a graph embedded into an hyperbolic space (or load the topology and" << endl;
  cout << "\t" << "its geometry from a file) according to the specifications described in" << endl;
  cout << "\t" << "http://dx.doi.org/10.1103/PhysRevE.82.036106 and it computes the greedy" << endl;
  cout << "\t" << "routing success ratio simulating random nodes sending interest packets" << endl;
  cout << "\t" << "to other (random) nodes over the network." << endl;
  cout << "\t" << "The program outputs the success ratio at the end of the simulation." << endl;
  cout << endl;
  cout << endl;
  cout << "OPTIONS:"<< endl;
  cout << "\t" << "-c" << "\t" << "build the hyperbolic graph according to the following options:" << endl;
  cout << "\t" << "\t" << "-n" << "\t" << "graph size (number of nodes)" << endl;
  cout << "\t" << "\t" << "\t" << "default value is 1000" << endl;
  cout << "\t" << "\t" << "-k" << "\t" << "expected average degree" << endl;
  cout << "\t" << "\t" << "\t" << "default value is 10" << endl;
  cout << "\t" << "\t" << "-g" << "\t" << "expected power-law exponent gamma" << endl;
  cout << "\t" << "\t" << "\t" << "default value is 2 (infinite gamma = ";
  cout << HG_INF_GAMMA << ")" << endl;
  cout << "\t" << "\t" << "-t" << "\t" << "temperature" << endl;
  cout << "\t" << "\t" << "\t" << "default value is 0 (infinite temperature = ";
  cout << HG_INF_TEMPERATURE << ")" << endl;
  cout << "\t" << "\t" << "-z" << "\t" << "square root of curvature zeta=sqrt(-K)" << endl;
  cout << "\t" << "\t" << "\t" << "or ratio eta=zeta/T in Soft Configuration Model" << endl;
  cout << "\t" << "\t" << "\t" << "default value is 1" << endl;
  cout << "\t" << "\t" << "-s" << "\t" << "random seed" << endl;
  cout << "\t" << "\t" << "\t" << "default value is 1" << endl;
  cout << endl;
  cout << "\t" << "-f" << "\t" << "graph file name, load graph from (.hg) file" << endl; 
  cout << endl;
  cout << "\t" << "-a" << "\t" << "number of attempts" << endl;
  cout << "\t" << "\t" << "default value is 10 x number of nodes" << endl;
  cout << "\t" << "-h" << "\t" << "print help menu" << endl; 
  cout << "\t" << "-v" << "\t" << "verbose (print information about the random producers,consumers" << endl;
  cout << "\t" << "\t" << "that have been selected for testing" << endl; 
  cout << endl;
  return;
}

int main (int argc, char **argv) {


  int n = 1000;         // number of nodes in the graph
  double k_bar = 10;    // expected average degree
  double exp_gamma = 2; // expected gamma or gamma out
  double t = 0;         // temperature
  bool zeta_eta_provided = false; 
  double zeta_eta = 1;  // parameter associated with curvature
  int seed = 1;         // random seed

  bool build_graph = true; // by default it builds a random graph
  // if false, then it loads a graph from file

  bool f_set = false; // tells whether the f option has been set
  bool c_set = false; // tells whether the c option has been set

  string graph_filename = ""; // name of the file to load

  int num_attempts = 0;

  bool verbose = false;  // verbose is false by default

  char *cvalue = NULL;
  int index;
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "cf:n:k:g:t:z:s:a:hv")) != -1) {
    switch (c) {
    case 'c':
      c_set = true;
      break;
    case 'f':
      graph_filename = optarg;
      f_set = true;
      break;
    case 'n':
      n = atoi(optarg);
      break;
    case 'k':
      k_bar = atof(optarg);
      break;
    case 'g':
      exp_gamma = atof(optarg);
      break;
    case 't':
      t = atof(optarg);
      break;
    case 'z':
      zeta_eta = atof(optarg);
      zeta_eta_provided = true;
      break;
    case 's':
      seed = atoi(optarg);
      break;
    case 'a':
      num_attempts = atoi(optarg);
      break;
    case 'v':
      verbose = true;
      break;
    case 'h':
      usage(argv[0]);
      return 0;
    case '?':
      cout << "Unknown option provided or missing argument" << endl << endl;
      usage(argv[0]);
      return 0;
    default:
      cout << "Unknown option provided" << endl << endl;
      usage(argv[0]);     
      return 1;
    }
  }
  
  if(c_set and f_set)
    {
      cerr << "c and f options are mutually exclusive, set only one of them" << endl;
      usage(argv[0]);     
      return 1;      
    }

  // hyperbolic ndn topology
  ns3::ndn::Hg_ndn_graph * hng = NULL;

  if(f_set)
    {
      // loading graph from file
      if(verbose)
	{
	  cout << endl;
	  cout << "Loading graph topology from: " << graph_filename <<  "..." << endl;
	}
      hng = new ns3::ndn::Hg_ndn_graph(graph_filename);
    }
  else // c_set or default, build the graph
    {
      //Limit cases that we do not take into account
      if(n < 3)
	{
	  cerr << "Number of nodes must be n>=3. \n\t  Quitting." << endl;
	  return 1;
	}
      if(k_bar < 1 or k_bar > n-1)
	{
	  cerr << "Avg degree must be greater than 0 and less than n-1. \n\t  Quitting." << endl;
	  return 1;
	}
      if(t<0)
	{
	  cerr << "Temperature must be positive (t >= 0). \n\t  Quitting." << endl;
	  return 1;
	}  
      if(exp_gamma<2)
	{
	  cerr << "Gamma must be greater or equal 2 (Gamma >= 2). \n\t  Quitting." << endl;
	  return 1;
	}
      // Warnings 
      if(zeta_eta_provided and exp_gamma>=HG_INF_GAMMA) 
	{
	  cerr << "zeta or eta make sense only at finite values of gamma." << endl;
	  cerr << "\t  The provided value of zeta (or eta) will be ignored." << endl;
	  zeta_eta = 1;
	}
      if(seed < 1)
	{
	  cerr << "Seed has to be greater than 0. \n\t  Assuming seed = 1." << endl;
	  seed = 1;
	}

      // start building the graph
      if(verbose)
	{
	  cout << endl;
	  cout << "Building graph..." << endl;
	}
      hng = new ns3::ndn::Hg_ndn_graph(n, k_bar, exp_gamma, t, zeta_eta, seed);
    }

  // get ndn nodes
  ns3::NodeContainer nodes = hng->get_nodes();
  uint32_t nNodes = nodes.GetN();
  if(num_attempts == 0)
    {
      num_attempts = 10 * nNodes;
    }
  if(verbose)
    {
      cout << endl;
      cout << "-> created topology with " << nNodes << " nodes." << endl;
      cout << "-> attempting " << num_attempts << " interest requests" << endl;
    }

  // customize nodes - info @ http://ndnsim.net/helpers.html

  // Install NDN stack on all nodes
  ns3::ndn::StackHelper ndnHelper;
  ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::Hg_ndn_greedy_strategy");
  ndnHelper.Install(nodes);      

  // Install ndnGlobalRoutingHelper on all nodes
  ns3::ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.Install(nodes);  
     
  int effective_attempts = 0;
  int a;
  int producer_node;
  int consumer_node;
  stringstream content_prefix;
  stringstream request_time;

  // Create Producers Vector
  vector<ns3::ndn::AppHelper> producers;
  // Create Consumers Vector
  vector<ns3::ndn::AppHelper> consumers;

  for(a = 0; a < num_attempts; a++) {

    // extract a random producer and a random consumer
    producer_node = hng->get_random_node_id();
    consumer_node = hng->get_random_node_id();
        
    if(producer_node == consumer_node or
       !hng->same_connected_component(producer_node,consumer_node)) {
      continue;
    }
    // Print consumer - producer
    if(verbose)
      {
	cout << endl;
	cout << effective_attempts << ") Consumer - Producer:" << endl;
	cout << "\t" << consumer_node << " - " << producer_node << endl;
      }
    effective_attempts++;
    // preparing content name
    content_prefix.str("");
    content_prefix << "/" << producer_node << "/prefix_" << effective_attempts;
    if(verbose)
      {
	cout << "\t" << "Interest name: " << content_prefix.str() << endl;
      }


    // Producer will reply to all requests starting with /producer/prefix
    ns3::ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetPrefix (content_prefix.str().c_str());
    producerHelper.SetAttribute ("PayloadSize", ns3::StringValue("1024"));
    producerHelper.Install(nodes.Get(producer_node)); 
    producers.push_back(producerHelper);

    
    ns3::ndn::AppHelper consumerHelper ("ns3::ndn::HGConsumer");
    // Consumer will request /producer/prefix/0, /producer/prefix/1, ...
    consumerHelper.SetPrefix(content_prefix.str().c_str());    

    request_time.str("");
    request_time << effective_attempts + 1.0 << "s 1";
    if(verbose)
      {
	cout << "\t" << "Request scheduled at time: " << effective_attempts + 1.0 << endl;
      }
    // ask for 1 data transfer every second
    consumerHelper.SetAttribute ("Batches", ns3::StringValue(request_time.str().c_str()));
    consumerHelper.Install(nodes.Get(consumer_node));
    consumers.push_back(consumerHelper);
    
    ndnGlobalRoutingHelper.AddOrigins(content_prefix.str().c_str(), nodes.Get(producer_node));
    install_route_on_every_node(hng,nodes, producer_node, content_prefix.str());
  }

  ndnGlobalRoutingHelper.CalculateRoutes(false);

  double total_time = effective_attempts + 20.0;
  // Run Simulator for 5 seconds
  ns3::Simulator::Stop(ns3::Seconds(total_time));
  ns3::Simulator::Run();
  ns3::Simulator::Destroy();

  if(effective_attempts == 0)
    {
      cout << "SR" << "\t" << "-1" << endl;
      if(verbose)
	{
	  cout << "No valid attempt found." << endl;
	}
    }
  else
    {
      cout << "SR" << "\t";
      cout << (double) ns3::ndn::HGConsumer::GetReceivedDataCount() / (double) effective_attempts << endl;
      if(verbose)
	{
	  cout << "Received data: " << ns3::ndn::HGConsumer::GetReceivedDataCount() << endl;
	  cout << "Timed-out interests: " << ns3::ndn::HGConsumer::GetTimeoutDataCount() << endl;
	  cout << "Effective attempts: " << effective_attempts << endl;
	}
    }

  delete hng;

  return 0;
}

