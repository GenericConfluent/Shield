#include "Network.hpp"
#include "ResourcePath.hpp"
#include <assert.h>
#include <fstream>
#include <cmath>

namespace gfd{
    Network::Network(std::vector<unsigned> topology){
      for(unsigned l = 0; l < topology.size(); l++){
        _network.push_back(Layer());
        unsigned numOutputs = l == topology.size() - 1 ? 0 : topology[l+1];
        
        int created = 0;
        for(unsigned n = 0; n < topology[l]; n++){
          _network.back().push_back(Neuron(numOutputs, n));
          
          created += 1;
        }
      }
    }
    
    void Network::loadNetwork(std::string path){
        std::ifstream file;
    }

    void Network::feedForward(const std::vector<double> inputs){
      assert(inputs.size() == _network[0].size());

      for(unsigned n = 0; n < inputs.size(); n++){
        _network[0][n].setActivation(inputs[n]);
      }

      for(unsigned l = 1; l < _network.size(); l++){
        for(unsigned n = 0; n < _network[l].size(); n++){
          _network[l][n].feedForward(_network[l-1]);
        }
      }

      std::vector<double> networkOutputs;
      for(unsigned n = 0; n < _network[_network.size() - 1].size(); n++){
        networkOutputs.push_back(_network[_network.size() - 1][n].getActivation());
      }
      output = networkOutputs;
    }

    void Network::f_backPropagation(const std::vector<double> targets){
      assert(targets.size() == _network[_network.size()-1].size());
      double x, y;
      for(unsigned l = _network.size()-1; l > 0; l--){
        for(unsigned n = 0; n < _network[l].size(); n++){
          x = -(targets[n] - _network[l][n].getActivation());
          y = _network[l][n].getActivation()*(-_network[l][n].getActivation()); //Logistics function derivative
          _network[l][n].backPropagation(_network[l-1], (double)(x*y));
        }
      }
    }

    void Network::train(const std::vector<double> inputs, const std::vector<double> targets) {
      feedForward(inputs);
      f_backPropagation(targets);
    }

    void Network::printNetwork(){
      for(int i = 0; i < _network.size(); i++){
        
        for(int j = 0; j < _network[i].size(); j++){
          
        }
      }
    }

}
