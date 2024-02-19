#include <math.h>
#include <assert.h>
#include <cstdlib> 
#include <ctime> 
#include <iostream>
#include "Neuron.hpp"

using gfd::Layer;

namespace gfd{

  Neuron::Neuron(unsigned numWeights, unsigned index) : _index(index){
    for(unsigned w = 0; w < numWeights; w++){
      _outputWeights.push_back(Weight());
      _outputWeights.back().weight = f_GenerateRandomWeight();
    }
  }

  void Neuron::setActivation(double val){
    _activation = val;
  }

  double Neuron::getActivation() const{
    return _activation;
  }

  void Neuron::feedForward(const Layer& previousLayer){
    double feedForwardVal = 0.0;
    for(int n = 0; n < previousLayer.size(); n++){
      feedForwardVal += previousLayer[n].getActivation() * previousLayer[n]._outputWeights[_index].weight;
    }
    _activation = f_Activation(feedForwardVal);
  }

  void Neuron::backPropagation(Layer &previousLayer, double currentGradientCalc){
    double z;
    for(unsigned pln = 0; pln < previousLayer.size(); pln++){
          z = previousLayer[pln].getActivation();// How much the output changes in relation to the other neuron connected to the weight
          previousLayer[pln]._outputWeights[_index].weight += (currentGradientCalc * z) * Neuron::learningRate;
    }
  }

  double Neuron::f_GenerateRandomWeight(){
    srand(time(0));
    return rand() / double(RAND_MAX);
  }

  double Neuron::f_Activation(double x){
    return tanh(x);
  }

}
