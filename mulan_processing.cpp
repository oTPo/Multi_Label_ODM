/*
��Mulan���ݼ�תMLC_SVM�������ݸ�ʽ��
	1.��ȡXML�ļ���ͳ�Ʊ�ǩ��Ŀ
	2.��ȡARFF�ļ�ͷ�ļ���Ϣ��ͳ�Ʊ�ǩ��Ŀ+������Ŀ֮�ͣ������õ�������Ŀ
	3.��ȡARFF�ļ�������Ϣ�����ж�ȡ������ΪMLC_SVM��������ݸ�ʽ��д���µ������ļ�

	3-���� �����������
		3.1 �ַ����и�
		3.2 ��ʽ�����ϲ��������ݵ������ַ���
		3.3 ����ϡ���ǩ�ַ���
		3.4 �����ַ�����д�����ļ� 

		3.2-���� ����������Ҫ���й�һ��(min-max��һ����
			3.2.1 �ָ��ַ���
			3.2.2 �ַ���ת������
			3.2.3 ���������ֵ������Сֵ
*/

#include "data_processing.h"

#define DATA_VALUE_MAX 10000000
#define DATA_VALUE_MIN -10000000

int countLabelsFromXml(string filePath) {
	ifstream infile;
	string fileLine;
	int labelsCount = 0;

	infile.open(filePath);
	if (!infile.is_open()) cout << filePath << " open failed" << endl;

	while (getline(infile, fileLine)) {
		if (fileLine.substr(1,6).compare("label ") == 0) labelsCount++;
	}

	infile.close();

	return labelsCount;
}

void getMinMaxFromArff(string arffFilePath, int labelsCount, vector<float> &colValMins, vector<float> &colValMaxs) {
	ifstream infile;
	string fileLine;
	int attributesCount = 0;
	int featuresCount;
	vector<string> attributeValues;
	vector<float> featureValues;
	int i;

	infile.open(arffFilePath);
	if (!infile.is_open()) cout << arffFilePath << " open failed" << endl;

	//count attributes
	while (getline(infile, fileLine)) {
		if (fileLine.compare("@data") == 0) break;
		else {
			if (!fileLine.empty() && fileLine.substr(1, 9).compare("attribute") == 0) attributesCount++;
		}
	}

	//calculate the number of features
	featuresCount = attributesCount - labelsCount;

	//initialize the min,max vectors
	colValMins.resize(featuresCount);
	colValMaxs.resize(featuresCount);
	for (int i = 0; i < featuresCount; i++) {
		colValMins[i] = DATA_VALUE_MAX;
		colValMaxs[i] = DATA_VALUE_MIN;
	}

	//read the data line and get min and max of each column
	while (getline(infile, fileLine)) {
		//split the data line
		attributeValues.clear();
		SplitString(fileLine, attributeValues, ",");

		featureValues.resize(featuresCount);      
		for (i = 0; i < featuresCount; i++) {
			//transfer the string into float 
			featureValues[i] = stof(attributeValues[i]);

			//update the min,max
			if (featureValues[i] < colValMins[i]) colValMins[i] = featureValues[i];
			if (featureValues[i] > colValMaxs[i]) colValMaxs[i] = featureValues[i];
		}		
	}

	infile.close();
}

void transferDataFromArff(string arffFilePath, int labelsCount, string newDataFilePath) {
	ifstream infile;
	ofstream outfile;
	string fileLine;
	vector<string> attributeValues;
	string formativeFeatures="";
	string formativeLabels="";
	vector<float> colValMins, colValMaxs;
	vector<float> featureVals;
	char newFeatureValStr[100];
	string newFeatureStr;
	bool hasLabels;
	int attributesCount = 0;
	int featuresCount;
	int exampleCount=1;
	int i;

	infile.open(arffFilePath);
	if (!infile.is_open()) cout << arffFilePath << " open failed" << endl;
	outfile.open(newDataFilePath);
	if (!outfile.is_open()) cout << newDataFilePath << " create failed" << endl;

	//count attributes
	while (getline(infile, fileLine)) {
		if (fileLine.compare("@data") == 0) break;
		else {
			if (!fileLine.empty()&&fileLine.substr(1, 9).compare("attribute") == 0) attributesCount++;
		}
	}

	//calculate the number of features
	featuresCount = attributesCount - labelsCount;

	//get normalized parameters
	getMinMaxFromArff(arffFilePath, labelsCount, colValMins, colValMaxs);

	//read the data line and transfer
	while (getline(infile, fileLine)) {
		//split the data line
		attributeValues.clear();
		SplitString(fileLine, attributeValues, ",");

		//format the feature string and create entire new one 
		formativeFeatures = "";
		featureVals.resize(featuresCount);
		for (i = 0; i < featuresCount; i++) {
			featureVals[i] = stof(attributeValues[i]);
			featureVals[i] = (featureVals[i] - colValMins[i]) / (colValMaxs[i] - colValMins[i]);
			snprintf(newFeatureValStr, sizeof(newFeatureValStr), "%d:%.6f ", i + 1, featureVals[i]);
			newFeatureStr = newFeatureValStr;
			formativeFeatures += newFeatureStr;
		}
		formativeFeatures.pop_back();

		//create sparse label string
		formativeLabels = "";
		for (i = 0; i < labelsCount; i++) {
			if (attributeValues[i + featuresCount] == "1") formativeLabels += to_string(i + 1) + ",";
		}
		hasLabels = true;
		if (formativeLabels.empty()) hasLabels = false;

		//write to the new data file if has labels
		if (hasLabels) {
			formativeLabels.pop_back();
			outfile << "example" << exampleCount << " " << formativeLabels << " " << formativeFeatures << endl;
			exampleCount++;
		}	
	}

	infile.close();
	outfile.close();
}

int main() {
	string xmlFilePath = "./Datasets/Mulan/birds.xml";
	string arffFilePath = "./Datasets/Mulan/birds_train.arff";
	string newDataFilePath = "./Datasets/Mulan/birds_xml_train.txt";
	int labelsCount = countLabelsFromXml(xmlFilePath);

	transferDataFromArff(arffFilePath, labelsCount, newDataFilePath);
	
	system("pause");
}