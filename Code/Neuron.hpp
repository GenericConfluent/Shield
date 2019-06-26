#pragma once
#include <vector>

namespace gfd{
  struct Weight{
    double weight;
    double deltaWeight;
  };

  class Neuron;
  typedef std::vector<Neuron> Layer;

  class Neuron{// Add the activation function and the function to calculate the output gradients
    public:
      Neuron(unsigned numWeights, unsigned index);
      void setActivation(double val);
      double getActivation() const;
      void feedForward(const Layer& previousLayer);
      void backPropagation(Layer& previousLayer, double currentGradientCalc);
      const double learningRate = 0.5;
    private:
      static double f_GenerateRandomWeight(void);
      static double f_Activation(double x);
      double _activation;
      unsigned _index;
      std::vector<Weight> _outputWeights;
      
  };
  
}
