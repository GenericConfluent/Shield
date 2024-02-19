#pragma once
#include "Neuron.hpp"
#include <vector>
#include <iostream>


namespace gfd{
  class Network{
    public:
      Network(std::vector<unsigned> topology);
      
      void loadNetwork(std::string path);
      void feedForward(const std::vector<double> inputs);
      void train(const std::vector<double> inputs, std::vector<double> targets);
      void printNetwork();

      std::vector<double> output;
    private:
      void f_backPropagation(std::vector<double> targets);
      std::vector<Layer> _network;
  };
}
