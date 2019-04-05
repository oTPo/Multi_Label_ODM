/*
【Mulan数据集转MLC_SVM所需数据格式】
	1.读取XML文件，统计标签数目
	2.读取ARFF文件头文件信息，统计标签数目+特征数目之和，进而得到特征数目
	3.读取ARFF文件数据信息，按行读取并重组为MLC_SVM所需的数据格式，写入新的数据文件

	3-补充 如何重组数据
		3.1 字符串切割
		3.2 格式化并合并单条数据的特征字符串
		3.3 生成稀疏标签字符串
		3.4 重组字符串并写入新文件 
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