#include "RegressionModel.hpp"

#include <iostream>
#include <utility>
#include <random>
#include <algorithm>

RegressionModel::RegressionModel(){
	std::cout << "Hello from Regression Model class!" << std::endl;
}

void RegressionModel::randomiseData(std::vector<std::unique_ptr<DataInfo>> &dataVec){
	
	std::random_device rd;

	for(int i = 0; i < dataVec.size(); i++)
	{
		std::uniform_real_distribution<double> distribution(dataVec[i]->minVal, dataVec[i]->maxVal);
		std::default_random_engine generator(rd());
		dataVec[i]->value = distribution(generator);
	}
}

void RegressionModel::normaliseData(std::vector<std::unique_ptr<DataInfo>> &dataVec){

	for(int i = 0; i < dataVec.size(); i++){
		dataVec[i]->normVal = std::max(0.0, std::min(1.0, (dataVec[i]->value - dataVec[i]->minVal) / (dataVec[i]->maxVal - dataVec[i]->minVal)));
	}

}

void RegressionModel::remapData(std::vector<std::unique_ptr<DataInfo>> &dataVec){

	for(int i = 0; i < dataVec.size(); i++){
		dataVec[i]->value = dataVec[i]->minVal + (dataVec[i]->normVal * (dataVec[i]->maxVal - dataVec[i]->minVal));
	}

}

void RegressionModel::collectData(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec){

	trainingExample trainingEx;

	for(int i = 0; i < inputDataVec.size(); i++){
		double inVal = inputDataVec[i]->normVal;
		trainingEx.input.push_back(inVal);
	}

	for(int i = 0; i < outputDataVec.size(); i++)
	{
		double outVal = outputDataVec[i]->normVal;
		trainingEx.output.push_back(outVal);
	}

	m_vTrainingSet.push_back(trainingEx);
	trainingEx.input.clear();
	trainingEx.output.clear();
}

bool RegressionModel::trainModel(){

	bool trained = false;
	if(m_vTrainingSet.size() > 0){
		trained = m_staticRegression.train(m_vTrainingSet);
		m_vTrainingSet.clear();

	} else {
		std::cout << "ERROR: NO DATA IN TRAINING SET (RegressionModel.cpp)" << std::endl;
	}
	return trained;
}

void RegressionModel::run(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec){

	std::vector<double> inputs;
	std::vector<double> outputs;

	for(int i = 0; i < inputDataVec.size(); i++){
		double inVal = inputDataVec[i]->normVal;
		inputs.push_back(inVal);
	}

	outputs = m_staticRegression.run(inputs);

	for(int i = 0; i < outputDataVec.size(); i++){
		outputDataVec[i]->normVal = outputs[i];
	}

	inputs.clear();
	outputs.clear();
}

void RegressionModel::saveModel(std::string mySavedModel){

	m_staticRegression.writeJSON(mySavedModel);
	std::cout << "Saving Training Data" << std::endl;
}

bool RegressionModel::loadModel(std::string mySavedModel){

	bool modelTrained = false;

	m_staticRegression.reset();
	m_staticRegression.readJSON(mySavedModel);	
	modelTrained = true;
	std::cout << "Loading Data and Training Model" << std::endl;
	return modelTrained;
}
