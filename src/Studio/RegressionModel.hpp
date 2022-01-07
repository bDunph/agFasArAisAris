#ifndef REGRESSIONMODEL_HPP
#define REGRESSIONMODEL_HPP

#include "RapidLib/regression.h"

#include <memory>

struct DataInfo; // forward declaration

class RegressionModel{

	private:
		std::vector<std::unique_ptr<double>> inputData;
		std::vector<std::unique_ptr<double>> outputData;

		regression staticRegression;
		trainingExample trainingEx;
		std::vector<trainingExample> trainingSet;

	public:
		RegressionModel();
		void sortData(const std::vector<std::unique_ptr<DataInfo>> &dataVec);
};
#endif
