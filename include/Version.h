#ifndef VERSION_H_
#define VERSION_H_

#include <string>
namespace mlib{

using namespace std;
#define GIT_TAG "v0.1-2-g540bb51-dirty" 
#define GIT_DATE "Wed Feb 8 23:23:08 2017" 
#define GIT_COMMIT_SUBJECT "modify the version tag" 

	static const string mlibVersionTag= GIT_TAG;
	static const string mlibVersionDate= GIT_DATE;
	static const string mlibVersionCommitSubject= GIT_COMMIT_SUBJECT;



}

#endif
