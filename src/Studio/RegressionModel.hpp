#ifndef REGRESSIONMODEL_HPP
#define REGRESSIONMODEL_HPP

#include "RapidLib/regression.h"

#include <memory>

class RegressionModel{

	private:
		std::vector<double> m_vInputData;
		std::vector<double> m_vOutputData;

		regression m_regStaticRegression;
		trainingExample m_texTrainingEx;
		std::vector<trainingExample> m_vTrainingSet;

	public:
		enum ParamType {
			INPUT,
			OUTPUT
		};

		struct DataInfo{
			std::string name;
			double value;
			double minVal;
			double maxVal;
			ParamType paramType;
		};

		RegressionModel();
		void setOutputData(const std::vector<std::unique_ptr<DataInfo>> &dataVec);
		void randomiseOutputData(std::vector<std::unique_ptr<DataInfo>> &dataVec);
		void collectData(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec);
		void displayDataSet();
};
#endif
