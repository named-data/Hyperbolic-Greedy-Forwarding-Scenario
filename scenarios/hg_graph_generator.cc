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


int main (int argc, char **argv) {
  
  const int n = 10000; // number of nodes
  const double k_bar = 10; // average degree
  const double exp_gamma = 100000; // expected gamma (10 = infinite gamma)
  const double t = 0; // temperature (100 = infinite temperature)
  const double zeta = 1; // space curvature
  const int seed = 1; // seed

  // create an hyperbolic graph
  ns3::ndn::Hg_ndn_graph hng(n, k_bar, exp_gamma, t, zeta, seed);

  // get ndn nodes
  ns3::NodeContainer nodes = hng.get_nodes();

  // customize nodes - info @ http://ndnsim.net/helpers.html

  // Install NDN stack on all nodes
  ns3::ndn::StackHelper ndnHelper;
  ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::Hg_ndn_greedy_strategy");
  ndnHelper.Install(nodes);      

  // Install ndnGlobalRoutingHelper on all nodes
  ns3::ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.Install(nodes);

  int consumer_id = 2;
  int producer_id = 1;
  stringstream content;
  content << "/" << producer_id << "/prefix";

  // Consumer
  // ns3::ndn::AppHelper consumerHelper ("ns3::ndn::HGConsumerBatches");
  ns3::ndn::AppHelper consumerHelper ("ns3::ndn::HGConsumer");
  // Consumer will request /X/prefix/0, /X/prefix/1, ...
  consumerHelper.SetPrefix(content.str());
  consumerHelper.SetAttribute ("Batches", ns3::StringValue("2s 1"));
  consumerHelper.Install(nodes.Get(consumer_id)); 

  // Create Producer
  ns3::ndn::AppHelper producerHelper("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /X/prefix
  producerHelper.SetPrefix(content.str());
  producerHelper.SetAttribute ("PayloadSize", ns3::StringValue("1024"));
  producerHelper.Install(nodes.Get(producer_id));

  // enable producer to announce prefix
  ndnGlobalRoutingHelper.AddOrigins(content.str(), nodes.Get(producer_id));

  // calculate routes
  ndnGlobalRoutingHelper.CalculateRoutes();

  ns3::Simulator::Stop(ns3::Seconds (20.0));
  ns3::Simulator::Run();
  ns3::Simulator::Destroy();

  cout << "Producer: " << producer_id << endl;
  cout << "Consumer: " << consumer_id << endl;
  cout <<"Successful attempts: ";
  // cout << ns3::ndn::HGConsumerBatches::GetReceivedDataCount() << endl;
  cout <<"Unsuccessful attempts: ";
  // cout << ns3::ndn::HGConsumerBatches::GetTimeoutDataCount() << endl;

  return 0;
}

