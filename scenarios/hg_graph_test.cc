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
#include "hg-integration/hg_ndn_utils.h"

using namespace std;


void usage (string exe_name){
  cout << "NAME: " << endl;
  cout << "\t" << "hg_graph_test" << " - create or load a graph into a ndnSIM scenario " << endl;
  cout << "\t" << "\t" << "\t" << "that simulates a network embedded" << endl;
  cout << "\t" << "\t" << "\t" << "into a hyperbolic space." << endl;
  cout << endl;
  cout << "SYNOPSIS: " << endl;
  cout << "\t" << "hg_graph_test" << " [options] [args] " << endl;
  cout << endl;
  cout << "DESCRIPTION:" << endl;
  cout << "\t" << "Build a graph embedded into an hyperbolic space (or load the topology and" << endl;
  cout << "\t" << "its geometry from a file) according to the specifications described in" << endl;
  cout << "\t" << "http://dx.doi.org/10.1103/PhysRevE.82.036106." << endl;
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
  cout << "\t" << "-h" << "\t" << "print help menu" << endl; 
  cout << "\t" << "-v" << "\t" << "verbose (print information about the graph)" << endl;
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

  bool verbose = false;  // verbose is false by default

  char *cvalue = NULL;
  int index;
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "cf:n:k:g:t:z:s:hv")) != -1) {
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
  if(!c_set and !f_set)
    {
      cerr << "Choose c or f option" << endl;
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
  
  if(c_set) 
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

  if(verbose)
    {
      cout << "Created topology with " << nNodes << " nodes." << endl;
    }

  // customize nodes - info @ http://ndnsim.net/helpers.html

  // Install NDN stack on all nodes
  ns3::ndn::StackHelper ndnHelper;
  ndnHelper.Install(nodes);      

  // Install ndnGlobalRoutingHelper on all nodes
  ns3::ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.Install(nodes);  
     

  // CONFIGURE YOUR SIMULATION HERE

  
  // 20 seconds simulation
  double total_time = 20.0;

  // Run Simulator
  ns3::Simulator::Stop(ns3::Seconds(total_time));
  
  if(verbose)
    {
      cout << "Simution started." << endl;
    }
  ns3::Simulator::Run();

  if(verbose)
    {
      cout << "Simution ended." << endl;
    }
  ns3::Simulator::Destroy();

  // deallocate memory for hyperbolic graph network
  delete hng;

  return 0;
}

