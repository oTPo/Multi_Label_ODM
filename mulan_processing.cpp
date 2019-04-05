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
*/

#include "data_processing.h"

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

void transferDataFromArff(string arffFilePath, int labelsCount, string newDataFilePath) {
	ifstream infile;
	ofstream outfile;
	string fileLine;
	vector<string> attributeValues;
	string formativeFeatures="";
	string formativeLabels="";
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

	//read the data line and transfer
	while (getline(infile, fileLine)) {
		//split the data line
		attributeValues.clear();
		SplitString(fileLine, attributeValues, ",");

		//format the feature string and create entire new one 
		formativeFeatures = "";
		for (i = 0; i < featuresCount; i++) {
			formativeFeatures += to_string(i + 1) + ":" + attributeValues[i]+" ";
		}
		formativeFeatures.pop_back();

		//create sparse label string
		formativeLabels = "";
		for (i = 0; i < labelsCount; i++) {
			if (attributeValues[i + featuresCount] == "1") formativeLabels += to_string(i + 1) + ",";
		}
		formativeLabels.pop_back();

		//write to the new data file
		outfile << "example" << exampleCount << " " << formativeLabels << " " << formativeFeatures << endl;
		exampleCount++;
	}

	infile.close();
	outfile.close();
}

int main() {
	string xmlFilePath = "./Datasets/Mulan/Corel5k.xml";
	string arffFilePath = "./Datasets/Mulan/Corel5k-train.arff";
	string newDataFilePath = "./Datasets/Mulan/Corel5k-train.txt";
	int labelsCount = countLabelsFromXml(xmlFilePath);

	transferDataFromArff(arffFilePath, labelsCount, newDataFilePath);
	
	system("pause");
}