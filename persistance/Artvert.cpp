/*
 * Artvert.cpp
 *
 *  Created on: 19/04/2011
 *      Author: arturo
 */

#include "Artvert.h"
#include "PersistanceEngine.h"


Artvert::Artvert()
:folder(ofFilePath::addTrailingSlash(DEFAULT_ARTVERT_FOLDER))
{

}

Artvert::Artvert(string uid,string folder)
:uid(uid)
,folder(ofFilePath::addTrailingSlash(folder))
,compressedImage(folder + uid+".jpg")
,model(folder + uid+".bmp")
,roiFile(folder + uid+".bmp.roi")
,detectorData(folder + uid+".bmp.detector_data")
,trackerData(folder + uid+".bmp.tracker_data")
,locationData(folder + uid+".bmp.location")
,roi(4)
{
	if(roiFile.exists()){
		roi.resize(4);
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
		roiFile.seekg(0,ios_base::beg);
	}

	ofxXmlSettings & xml = PersistanceEngine::artverts();
	int numAlias = xml.getNumTags("artvert");

	for(int i=0; i<numAlias; i++){
		if(uid==xml.getAttribute("artvert","uid","",i)){
			setAliasUID(xml.getAttribute("artvert","alias","",i));
			break;
		}
	}
}

string Artvert::getUID() const{
	return uid;
}

void Artvert::setUID(const string & _uid){
	uid = _uid;
	compressedImage.open(folder + uid+".jpg");
	model.open(folder + uid+".bmp");
	roiFile.open(folder + uid+".bmp.roi");
	if(roiFile.exists()){
		roi.resize(4);
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
		roiFile.seekg(0,ios_base::beg);
	}
	detectorData.open(folder + uid+".bmp.detector_data");
	trackerData.open(folder + uid+".bmp.tracker_data");
	locationData.open(folder + uid+".bmp.location");

	ofxXmlSettings & xml = PersistanceEngine::artverts();
	int numAlias = xml.getNumTags("artvert");

	for(int i=0; i<numAlias; i++){
		if(uid==xml.getAttribute("artvert","uid","",i)){
			setAliasUID(xml.getAttribute("artvert","alias","",i));
			break;
		}
	}
}

string Artvert::getAliasUID() const{
	return aliasUID;
}

void Artvert::setAliasUID(const string & uid){
	aliasUID = uid;
}

Artvert Artvert::getAlias(){
	return Artvert(aliasUID,folder);
}

bool Artvert::hasAlias() const{
	return aliasUID!="";
}

bool Artvert::isReady(){
	bool ret = uid!="" && model.exists() && roiFile.exists() && detectorData.exists() && trackerData.exists();
	if(ret){
		return true;
	}else if(hasAlias()){
		return getAlias().isReady();
	}else{
		return false;
	}
}

ofFile & Artvert::getCompressedImage(){
	return compressedImage;
}

ofFile & Artvert::getModel(){
	return model;
}

ofFile & Artvert::getROIFile(){
	return roiFile;
}

ofFile & Artvert::getDetectorData(){
	return detectorData;
}

ofFile & Artvert::getTrackerData(){
	return trackerData;
}

ofFile & Artvert::getLocationFile(){
	return locationData;
}

const ofFile & Artvert::getLocationFile() const{
	return locationData;
}

ofxLocation Artvert::getLocation(){
	ofxLocation location = {0,0,0,0,0};
	if(locationData.exists()){
		locationData >> location;
		locationData.seekg(0,ios_base::beg);
	}else{
		ofLog(OF_LOG_ERROR,"Artvert: error trying to open location file " + locationData.getAbsolutePath());
	}

	return location;
}

const ofFile & Artvert::getCompressedImage() const{
	return compressedImage;
}

const ofFile & Artvert::getModel() const{
	return model;
}

const ofFile & Artvert::getROIFile() const{
	return roiFile;
}

const ofFile & Artvert::getDetectorData() const{
	return detectorData;
}

const ofFile & Artvert::getTrackerData() const{
	return trackerData;
}

vector<ofPoint> Artvert::getROI(){
	if(!roi.empty()) return roi;

	if(roiFile.exists()){
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
		roiFile.seekg(0,ios_base::beg);
		return roi;
	}

	return vector<ofPoint>(4);

}

vector<ofFile> Artvert::getArtverts(){
	ofDirectory default_artverts_dir("artverts/");
	default_artverts_dir.allowExt("jpg");
	default_artverts_dir.allowExt("png");
	default_artverts_dir.allowExt("bmp");
	default_artverts_dir.listDir();
	vector<ofFile> artverts = default_artverts_dir.getFiles();

	ofDirectory artverts_dir("artverts/" + getUID());
	artverts_dir.allowExt("jpg");
	artverts_dir.allowExt("png");
	artverts_dir.listDir();
	vector<ofFile> own_artverts = artverts_dir.getFiles();
	artverts.insert(artverts.end(),own_artverts.begin(),own_artverts.end());

	return artverts;
}

void Artvert::save(){
	if(aliasUID=="") return;
	int artvert = PersistanceEngine::artverts().addTag("artvert");
	PersistanceEngine::artverts().addAttribute("artvert","uid",uid,artvert);
	PersistanceEngine::artverts().addAttribute("artvert","alias",aliasUID,artvert);
}

vector<Artvert> Artvert::listAll(string folder){


	cout << "listing: " <<  folder << endl;
	ofDirectory dir;
	dir.allowExt("jpg");
	int numArtverts = dir.listDir(folder);
	cout << "numArtverts: " << numArtverts << endl;
	vector<Artvert> artverts(numArtverts,Artvert("",folder));
	for(int i=0;i<numArtverts;i++){
		string uid = dir.getFile(i,ofFile::Reference).getBaseName();
		artverts[i].setUID(uid);
	}

	ofxXmlSettings & xml = PersistanceEngine::artverts();
	int numAlias = xml.getNumTags("artvert");

	for(int i=0; i<numAlias; i++){
		for(int j=0;j<numArtverts;j++){
			if(artverts[j].getUID()==xml.getAttribute("artvert","uid","",i)){
				artverts[j].setAliasUID(xml.getAttribute("artvert","alias","",i));
				break;
			}
		}
	}

	return artverts;
}

bool Artvert::operator==(const Artvert & artvert) const{
	return getUID()==artvert.getUID();
}

bool Artvert::operator!=(const Artvert & artvert) const{
	return getUID()!=artvert.getUID();
}

bool Artvert::operator<(const Artvert & artvert) const{
	return getUID()<artvert.getUID();
}

bool Artvert::operator<=(const Artvert & artvert) const{
	return getUID()<=artvert.getUID();
}

bool Artvert::operator>(const Artvert & artvert) const{
	return getUID()>artvert.getUID();
}

bool Artvert::operator>=(const Artvert & artvert) const{
	return getUID()>=artvert.getUID();
}
