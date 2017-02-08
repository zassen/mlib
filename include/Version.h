#ifndef VERSION_H_
#define VERSION_H_

#include <string>
namespace mlib{

using namespace std;
#define GIT_TAG "v0.1-3-gd70c109-dirty" 
#define GIT_DATE "Wed Feb 8 23:31:16 2017" 
#define GIT_COMMIT_SUBJECT "correct the version variable tag" 

	static const string mlibVersionTag= GIT_TAG;
	static const string mlibVersionDate= GIT_DATE;
	static const string mlibVersionCommitSubject= GIT_COMMIT_SUBJECT;



}

#endif
