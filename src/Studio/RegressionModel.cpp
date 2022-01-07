#include "RegressionModel.hpp"

#include <iostream>
#include <utility>
#include <random>

RegressionModel::RegressionModel(){
	std::cout << "Hello from Regression Model class!" << std::endl;
}

void RegressionModel::setOutputData(const std::vector<std::unique_ptr<DataInfo>> &dataVec){
	
	for(int i = 0; i < dataVec.size(); i++){
		m_vOutputData.push_back(std::move(dataVec[i]->value));
		std::cout << "Output vector element " << i << " is " << m_vOutputData[i] << std::endl;
	}
}

void RegressionModel::randomiseOutputData(std::vector<std::unique_ptr<DataInfo>> &dataVec){
	
	std::cout << "I WILL RANDOMISE THE OUTPUT DATA" << std::endl;

	//random device
	std::random_device rd;

	//random audio params
	for(int i = 0; i < dataVec.size(); i++)
	{
		std::uniform_real_distribution<double> distribution(dataVec[i]->minVal, dataVec[i]->maxVal);
		std::default_random_engine generator(rd());
		dataVec[i]->value = distribution(generator);
	}
}

void RegressionModel::collectData(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec){
	
	std::cout << "I WILL COLLECT DATA FOR THE TRAINING SET" << std::endl;

	//shader values provide input to neural network
	for(int i = 0; i < inputDataVec.size(); i++){

		m_vInputData.push_back(inputDataVec[i]->value);
	}

	//neural network outputs to audio engine 
	for(int i = 0; i < outputDataVec.size(); i++)
	{
		m_vOutputData.push_back(outputDataVec[i]->value);
	}

	m_texTrainingEx.input = m_vInputData;
	m_texTrainingEx.output = m_vOutputData;
	m_vTrainingSet.push_back(m_texTrainingEx);

	m_vInputData.clear();
	m_vOutputData.clear();
}

void RegressionModel::displayDataSet(){

	for(int i = 0; i < m_vTrainingSet.size(); i++){
		std::cout << "Trainin Set Contents: /n/n" << "Element " << i << ": /n" << "Input Vec : " << m_vTrainingSet[i].input[i] << "/n" << "Output Vec : " << m_vTrainingSet[i].output[i] << ", " << m_vTrainingSet[i].output[i] << std::endl; 
	}

}



