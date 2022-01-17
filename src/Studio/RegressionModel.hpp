#ifndef REGRESSIONMODEL_HPP
#define REGRESSIONMODEL_HPP

#include "RapidLib/regression.h"

#include <memory>

class RegressionModel{

	private:
		regression m_staticRegression;
		//trainingExample m_trainingEx;
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
		//void randomiseOutputData(std::vector<std::unique_ptr<DataInfo>> &dataVec);
		void randomiseData(std::vector<DataInfo> &dataVec, std::vector<std::unique_ptr<DataInfo>> &ptrVals);
		//void collectData(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec);
		void collectData(std::vector<DataInfo> &inputDataVec, std::vector<DataInfo> &outputDataVec);
		bool trainModel();
		void run(std::vector<DataInfo> &inputDataVec, std::vector<DataInfo> &outputDataVec);
};
#endif
