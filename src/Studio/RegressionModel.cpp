#include "RegressionModel.hpp"

#include <iostream>
#include <utility>
#include <random>

RegressionModel::RegressionModel(){
	std::cout << "Hello from Regression Model class!" << std::endl;
}

//void RegressionModel::randomiseOutputData(std::vector<std::unique_ptr<DataInfo>> &dataVec){
void RegressionModel::randomiseData(std::vector<DataInfo> &dataVec, std::vector<std::unique_ptr<DataInfo>> &ptrVals){
	
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

	for(int i = 0; i < ptrVals.size(); i++)
	{
		//std::uniform_real_distribution<double> distribution(datavec[i]->minval, datavec[i]->maxval);
		std::uniform_real_distribution<double> distribution(ptrVals[i]->minVal, ptrVals[i]->maxVal);
		std::default_random_engine generator(rd());
		//dataVec[i]->value = distribution(generator);
		double randVal = distribution(generator);
		ptrVals[i]->value = randVal;
		//***************HHHHHHHHHHHEEEEEEEEERRRRRRRRRRRREEEEEEEEEEEEEE*****************
	}
}

//void RegressionModel::collectData(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec){
void RegressionModel::collectData(std::vector<DataInfo> &inputDataVec, std::vector<DataInfo> &outputDataVec){
	
	std::cout << "I WILL COLLECT DATA FOR THE TRAINING SET" << std::endl;

	trainingExample trainingEx;

	std::cout << "InputDataVec Size = " << inputDataVec.size() << std:: endl;
	std::cout << "OutputDataVec Size = " << outputDataVec.size() << std:: endl;

	for(int i = 0; i < inputDataVec.size(); i++){

		trainingEx.input.push_back(inputDataVec[i].value);
	}

	for(int i = 0; i < outputDataVec.size(); i++)
	{
		trainingEx.output.push_back(outputDataVec[i].value);
	}

	std::cout << "TRAINING EX IN VEC VALS: " << trainingEx.input[0] << std::endl;
	std::cout << "TRAINING EX OUT VEC VALS: " << trainingEx.output[0] << std::endl;
	
	m_vTrainingSet.push_back(trainingEx);
	trainingEx.input.clear();
	trainingEx.output.clear();
}

bool RegressionModel::trainModel(){

	trainingExample tEx;
	double in1 = 0.3f;
	double out1 = 0.7f;
	tEx.input.push_back(in1);
	tEx.output.push_back(out1);

	trainingExample tEx1;
	tEx1.input.push_back(in1);
	tEx1.output.push_back(out1);

	std::vector<trainingExample> tSetEx;
	tSetEx.push_back(tEx);
	tSetEx.push_back(tEx1);
	
	bool trained = false;
	if(m_vTrainingSet.size() > 0){
//	if(tSetEx.size() > 0){
		std::cout << "TRAINING SET SIZE : " << m_vTrainingSet.size() << std::endl;
		//std::cout << "TRAINING SET SIZE : " << tSetEx.size() << std::endl;
		trained = m_staticRegression.train(m_vTrainingSet);
		m_vTrainingSet.clear();
		//trained = m_staticRegression.train(tSetEx);

	} else {
		std::cout << "ERROR: NO DATA IN TRAINING SET (RegressionModel.cpp)" << std::endl;
	}
	return trained;
}

void RegressionModel::run(std::vector<DataInfo> &inputDataVec, std::vector<DataInfo> &outputDataVec){

	std::vector<double> inputs;
	std::vector<double> outputs;

	for(int i = 0; i < inputDataVec.size(); i++){
		inputs.push_back(inputDataVec[i].value);
	}

	outputs = m_staticRegression.run(inputs);

	for(int i = 0; i < outputDataVec.size(); i++){
		outputDataVec[i].value = outputs[i];
	}

	inputs.clear();
	outputs.clear();
}



