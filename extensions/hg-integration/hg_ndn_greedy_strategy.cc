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


#include "ns3/ndn-fib.h"
#include "ns3/ndn-fib-entry.h"
#include "ns3/ndn-pit-entry.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-l3-protocol.h"


#include "hg_ndn_greedy_strategy.h"


namespace ns3 {
  namespace ndn {
    namespace fw {
      
      NS_OBJECT_ENSURE_REGISTERED(Hg_ndn_greedy_strategy);
      
      LogComponent Hg_ndn_greedy_strategy::g_log = LogComponent (Hg_ndn_greedy_strategy::GetLogName ().c_str ());

      std::string Hg_ndn_greedy_strategy::GetLogName () {
        return "ndn.fw.Hg_ndn_greedy_strategy";
      }
      
      TypeId Hg_ndn_greedy_strategy::GetTypeId (void) {
        static TypeId tid = TypeId ("ns3::ndn::fw::Hg_ndn_greedy_strategy")
          .SetGroupName ("Ndn")
          .SetParent <BaseStrategy> ()
          .AddConstructor <Hg_ndn_greedy_strategy> ()
          ;
        return tid;
      }

      Hg_ndn_greedy_strategy::Hg_ndn_greedy_strategy () : m_counter (0) {
        // nothing to do for constructor
      }

      bool Hg_ndn_greedy_strategy::DoPropagateInterest (Ptr<Face> inFace, 
							Ptr<const Interest> interest,
							Ptr<pit::Entry> pitEntry) {
      
	double cur_distance = -1;
	double distance = -1;
	
	Ptr<L3Protocol> l3p = GetObject<L3Protocol>();
	size_t nfaces = l3p->GetNFaces();
	Ptr<Face> face = l3p->GetFace(0);
	Ptr<Channel> ch;
	Ptr<NetDevice> nd;
	
	// 1. get destination node info from prefix parsing (i.e. first component)
	name::Component name_component = interest->GetName()[0];
	int dest_node_id = atoi(name_component.toBlob().c_str());

	// 2. get a pointer to the hg_graph (any node is fine, they all have the same pointer)
	const hg_graph_t * g = face->GetNode()->GetObject<Hg_ndn_node_info>()->get_graph_ptr();
	
	// 3. retrieve the destination coordinate
	hg_coordinate_t dest_coordinate;
	dest_coordinate.r = (*g)[dest_node_id].r;
	dest_coordinate.theta = (*g)[dest_node_id].theta;


	typedef fib::FaceMetricContainer::type::index<fib::i_metric>::type FacesByMetric;
        FacesByMetric &faces = pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ();
        FacesByMetric::iterator faceIterator;
	FacesByMetric::iterator bestFaceIterator;

	size_t i;
	int best_node = -1;
	Ptr<Node> other_node_ptr;
	Ptr<Hg_ndn_node_info> other_node_info;
	bestFaceIterator = faces.end();

	int r = 0;


	cout << "---------------------------------- START" << endl;
	// Print current node (i.e. the node the inFace belongs to)
	cout << "On node: " << inFace->GetNode()->GetId() << endl;

	// if I try to get the channel, then I get segfault
	if(inFace->GetNode()->GetId() == dest_node_id)
	  return false;

	// loop through all the faces
	for(faceIterator = faces.begin(); faceIterator != faces.end(); faceIterator++) {	  
	  // retrieve the two endpoints of the channel
	  // and select the other "node"
	  nd = DynamicCast<NetDeviceFace>(faceIterator->GetFace())->GetNetDevice();
	  ch = nd->GetChannel();
	  other_node_ptr = ch->GetDevice(0)->GetNode();
	  if(inFace->GetNode()->GetId() == other_node_ptr->GetId())
	    {
	      other_node_ptr = ch->GetDevice(1)->GetNode();		
	    }
	  other_node_info = other_node_ptr->GetObject<Hg_ndn_node_info>();
	  // compute hyperbolic distance between the selected neighbor
	  // and the coordinate of the destination
	  cur_distance = hg_hyperbolic_distance(other_node_info->get_graph_ptr(),
						other_node_info->get_coordinate(),
						dest_coordinate);
	  // output some debug information
	  cout << r << ") " << other_node_ptr->GetId() <<  "\t" << cur_distance << endl;	    
	  r++;
	  // if it is the first neighbor or if the distance
	  // is lower than the previous choice, then select 
	  // the new candidate
	  if(distance == -1 or cur_distance < distance) {
	    bestFaceIterator = faceIterator;
	    best_node = other_node_ptr->GetId();
	    distance = cur_distance;	    
	  }
	  else {
	    // if there is another node at the same distance
	    // then decide to switch randomly to another one
	    if(distance == cur_distance) {
	      // randomly decide to change interface
	      if(hg_rand_01_wrapper() < 0.5) {
		bestFaceIterator = faceIterator;
		best_node = other_node_ptr->GetId();
	      }
	    }
	  }	  
	}

	// Best node according to the  hyperbolic greedy forwarding approach
	cout << "CHOSEN: " << best_node <<  "\t" << distance << endl;	    

        int propagatedCount = 0;
	// bestFaceIterator is the selected face for propagation
        // if a face has been selected 
	if (bestFaceIterator != faces.end()) {
	  // then try to send out the interest
          if (TrySendOutInterest (inFace, bestFaceIterator->GetFace(), interest, pitEntry)) {
            propagatedCount++;        
	    std::cout << interest->GetName() << "\t";
	    std::cout << best_node << "\t";
	    std::cout << distance << endl;
	  }
	  else{
	    // otherwise signal the failure
	    std::cout << "Didn't work" << endl;
	    // nothing to do
	    std::cout << interest->GetName()  << "\t";
	    std::cout << distance << "\t" << "FAIL" << endl;
	  }
        }

	cout << "---------------------------------- END" << endl;

        return propagatedCount > 0;

      }


      void Hg_ndn_greedy_strategy::DidSendOutInterest (Ptr<Face> inFace, Ptr<Face> outFace,
                                                 Ptr<const Interest> interest,
                                                 Ptr<pit::Entry> pitEntry) {
        m_counter ++;
      }

      void Hg_ndn_greedy_strategy::WillEraseTimedOutPendingInterest (Ptr<pit::Entry> pitEntry) {
        for (pit::Entry::out_container::iterator face = pitEntry->GetOutgoing ().begin ();
             face != pitEntry->GetOutgoing ().end ();
             face ++) {
          m_counter --;
        }

        BaseStrategy::WillEraseTimedOutPendingInterest (pitEntry);
      }


      void Hg_ndn_greedy_strategy::WillSatisfyPendingInterest (Ptr<Face> inFace,
							 Ptr<pit::Entry> pitEntry) {
        for (pit::Entry::out_container::iterator face = pitEntry->GetOutgoing ().begin();
             face != pitEntry->GetOutgoing ().end ();
             face ++) {
	  m_counter --;
        }

        BaseStrategy::WillSatisfyPendingInterest (inFace, pitEntry);
      }
      
    } // end fw namespace
  } // end of ndn namespace
} // end of ns3 namespace
