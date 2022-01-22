#ifndef REGRESSIONMODEL_HPP
#define REGRESSIONMODEL_HPP

#include "RapidLib/regression.h"

#include <memory>

class RegressionModel{

	private:
		regression m_staticRegression;
		std::vector<trainingExample> m_vTrainingSet;

	public:
		enum ParamType {
			INPUT,
			OUTPUT
		};

		struct DataInfo{
			std::string name;
			double value;
			double normVal;
			double minVal;
			double maxVal;
			ParamType paramType;
		};

		RegressionModel();
		void randomiseData(std::vector<std::unique_ptr<DataInfo>> &dataVec);
		void normaliseData(std::vector<std::unique_ptr<DataInfo>> &dataVec);
		void collectData(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec);
		bool trainModel();
		void run(std::vector<std::unique_ptr<DataInfo>> &inputDataVec, std::vector<std::unique_ptr<DataInfo>> &outputDataVec);
		void saveModel(std::string mySavedModel);
		bool loadModel(std::string mySavedModel);

};
#endif
