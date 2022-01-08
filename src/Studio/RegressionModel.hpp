#ifndef REGRESSIONMODEL_HPP
#define REGRESSIONMODEL_HPP

#include "RapidLib/regression.h"

#include <memory>

class RegressionModel{

	private:
		regression m_staticRegression;
		trainingExample m_trainingEx;
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
		//void setOutputData(const std::vector<std::unique_ptr<DataInfo>> &dataVec);
		void setOutputData(std::vector<DataInfo> &dataVec);
		//void randomiseOutputData(std::vector<std::unique_ptr<DataInfo>> &dataVec);
		void randomiseOutputData(std::vector<DataInfo> &dataVec);
		//void collectData(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec);
		void collectData(std::vector<DataInfo> &inputDataVec, std::vector<DataInfo> &outputDataVec);
		bool trainModel();
};
#endif
