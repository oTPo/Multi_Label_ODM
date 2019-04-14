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
