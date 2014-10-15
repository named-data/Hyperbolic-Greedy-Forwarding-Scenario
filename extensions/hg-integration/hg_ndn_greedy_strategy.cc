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
        typedef fib::FaceMetricContainer::type::index<fib::i_metric>::type FacesByMetric;
        FacesByMetric &faces = pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ();
        FacesByMetric::iterator faceIterator;
	FacesByMetric::iterator bestFaceIterator;
	double cur_distance = -1;
	double distance = -1;
	
	// 1. get destination node info from prefix parsing (i.e. first component)
	name::Component name_component = interest->GetName()[0];
	int dest_node_id = atoi(name_component.toBlob().c_str());

	// get a pointer to the hg_graph
	const hg_graph_t * g = faces.begin()->GetFace()->GetNode()->GetObject<Hg_ndn_node_info>()->get_graph_ptr();
	
	hg_coordinate_t dest_coordinate;
	dest_coordinate.r = (*g)[dest_node_id].r;
	dest_coordinate.theta = (*g)[dest_node_id].theta;

	Ptr<Node> face_node_ptr;
	Ptr<Hg_ndn_node_info> face_node_info;
      

	for(faceIterator = faces.begin(); faceIterator != faces.end(); faceIterator++) {	  
	  // compute distance between neighbor and destination
	  face_node_ptr = faceIterator->GetFace()->GetNode();
	  face_node_info = face_node_ptr->GetObject<Hg_ndn_node_info>();
	  // compute hyperbolic distance
	  cur_distance = hg_hyperbolic_distance(face_node_info->get_graph_ptr(),
						face_node_info->get_coordinate(),
						dest_coordinate);
	  if(distance == -1 or cur_distance < distance) {
	    bestFaceIterator = faceIterator;
	    distance = cur_distance;
	  }
	  else {
	    if(distance == cur_distance) {
	      // randomly decide to change interface
	      if(hg_rand_01_wrapper() < 0.5) {
		bestFaceIterator = faceIterator;
	      }
	    }
	  }
	}

        int propagatedCount = 0;
	// bestFaceIterator is the selected face for propagation
        if (bestFaceIterator != faces.end ()) {
          if (TrySendOutInterest (inFace, bestFaceIterator->GetFace(), interest, pitEntry)) {
            propagatedCount++;        
	    // DEBUG std::cout << interest->GetName() << "\t";
	    // DEBUG std::cout << bestFaceIterator->GetFace()->GetNode()->GetId() << "\t";
	    // DEBUG std::cout << distance << endl;
	  }
	  else{
	    // nothing to do
	    // DEBUG std::cout << bestFaceIterator->GetFace()->GetNode()->GetId() << "\t";
	    // DEBUG std::cout << distance << "\t" << "FAIL" << endl;
	  }
        }
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
        for (pit::Entry::out_container::iterator face = pitEntry->GetOutgoing ().begin ();
             face != pitEntry->GetOutgoing ().end ();
             face ++) {
	  m_counter --;
        }

        BaseStrategy::WillSatisfyPendingInterest (inFace, pitEntry);
      }
      
    } // end fw namespace
  } // end of ndn namespace
} // end of ns3 namespace
