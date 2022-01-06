#ifndef REGRESSIONMODEL_HPP
#define REGRESSIONMODEL_HPP

#include "RapidLib/regression.h"

class RegressionModel{

	private:
		std::vector<double> inputData;
		std::vector<double> outputData;

		regression staticRegression;
		trainingExample trainingEx;
		std::vector<trainingExample> trainingSet;

	public:
		RegressionModel();
};
#endif
