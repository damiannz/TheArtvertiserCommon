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
,md5File(folder + uid + ".bmp.md5")
//be careful when adding more files (update set also)
,roi(4)
{
	if(roiFile.exists()){
		roi.resize(4);
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
		roiFile.open(roiFile.path());
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
	md5File.open(folder + uid + ".bmp.md5");

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

ofFile Artvert::getMD5File() const{
	return md5File;
}

ofFile Artvert::getLocationFile() const{
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

ofFile Artvert::getCompressedImage() const{
	return compressedImage;
}

ofFile Artvert::getModel() const{
	return model;
}

ofFile Artvert::getROIFile() const{
	return roiFile;
}

ofFile Artvert::getDetectorData() const{
	return detectorData;
}

ofFile Artvert::getTrackerData() const{
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

bool Artvert::checkIntegrity(){
	if( !md5File.exists() || !compressedImage.exists() || !roiFile.exists() || !locationData.exists() ){
		return false;
	}else if(isReady() && (!detectorData.exists() || !trackerData.exists()) ){
		return false;
	}else{
		string storedmd5 = getStoredMD5();
		string testmd5 = generateMD5();

		return storedmd5 == testmd5;
	}
}

string Artvert::getStoredMD5(){
	string md5;
	string path = md5File.path();
	md5File.close();
	md5File.open(path);
	md5File >> md5;
	return md5;
}

string Artvert::generateMD5(){
	string testmd5;
	ofxMD5 md5;
	vector<char> filesSum;
	ofBuffer files;

	compressedImage.open(compressedImage.path());
	compressedImage >> files;
	compressedImage.open(compressedImage.path());
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	roiFile.open(roiFile.path());
	roiFile >> files;
	roiFile.open(roiFile.path());
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	locationData.open(locationData.path());
	locationData >> files;
	locationData.open(locationData.path());
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	if(!isReady()){
		testmd5 = md5.getMD5(filesSum);
		return testmd5;
	}

	detectorData.open(detectorData.path());
	detectorData >> files;
	detectorData.open(detectorData.path());
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	trackerData.open(trackerData.path());
	trackerData >> files;
	trackerData.open(trackerData.path());
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	testmd5 = md5.getMD5(filesSum);
	return testmd5;
}

void Artvert::remove(){
	compressedImage.remove();
	roiFile.remove();
	locationData.remove();
	detectorData.remove();
	trackerData.remove();
	model.remove();
	md5File.remove();
	//TODO: remove data from xml
}

void Artvert::removeAnalisys(){
	detectorData.remove();
	trackerData.remove();
	md5File.remove();
	ofFile md5(md5File.path(),ofFile::WriteOnly);
	md5 << generateMD5();
	md5.close();

}
