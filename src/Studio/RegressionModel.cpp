#include "RegressionModel.hpp"

#include <iostream>
#include <utility>
#include <random>

RegressionModel::RegressionModel(){
	std::cout << "Hello from Regression Model class!" << std::endl;
}

//void RegressionModel::setOutputData(const std::vector<std::unique_ptr<DataInfo>> &dataVec){
//void RegressionModel::setOutputData(std::vector<DataInfo> &dataVec){
//	
//	for(int i = 0; i < dataVec.size(); i++){
//		m_vOutputData.push_back(std::move(dataVec[i]->value));
//		std::cout << "Output vector element " << i << " is " << m_vOutputData[i] << std::endl;
//	}
//}

//void RegressionModel::randomiseOutputData(std::vector<std::unique_ptr<DataInfo>> &dataVec){
void RegressionModel::randomiseOutputData(std::vector<DataInfo> &dataVec){
	
	std::cout << "I WILL RANDOMISE THE OUTPUT DATA" << std::endl;

	//random device
	std::random_device rd;

	//random audio params
	for(int i = 0; i < dataVec.size(); i++)
	{
		//std::uniform_real_distribution<double> distribution(datavec[i]->minval, datavec[i]->maxval);
		std::uniform_real_distribution<double> distribution(dataVec[i].minVal, dataVec[i].maxVal);
		std::default_random_engine generator(rd());
		//dataVec[i]->value = distribution(generator);
		dataVec[i].value = distribution(generator);
	}
}

//void RegressionModel::collectData(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec){
void RegressionModel::collectData(std::vector<DataInfo> &inputDataVec, std::vector<DataInfo> &outputDataVec){
	
	std::cout << "I WILL COLLECT DATA FOR THE TRAINING SET" << std::endl;

	for(int i = 0; i < inputDataVec.size(); i++){

		//double tempVal = (*inputDataVec[i]).value;
		//m_trainingEx.input.push_back(tempVal);
		m_trainingEx.input.push_back(inputDataVec[i].value);
	}

	for(int i = 0; i < outputDataVec.size(); i++)
	{
		//double tempVal = (*outputDataVec[i]).value;
		//m_trainingEx.output.push_back(tempVal);
		m_trainingEx.output.push_back(outputDataVec[i].value);
	}

	std::cout << "TRAINING EX IN VEC VALS: " << m_trainingEx.input[0] << "		" << m_trainingEx.input[1] << std::endl;
	std::cout << "TRAINING EX OUT VEC VALS: " << m_trainingEx.output[0] << "	" << m_trainingEx.output[1] << std::endl;
	m_vTrainingSet.push_back(m_trainingEx);
}

bool RegressionModel::trainModel(){
	
	bool trained = false;
	if(m_vTrainingSet.size() > 0){
		std::cout << "TRAINING SET SIZE : " << m_vTrainingSet.size() << std::endl;
		trained = m_staticRegression.train(m_vTrainingSet);

	} else {
		std::cout << "ERROR: NO DATA IN TRAINING SET (RegressionModel.cpp)" << std::endl;
	}
	return trained;
}





