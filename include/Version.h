#ifndef VERSION_H_
#define VERSION_H_

using namespace std;
namespace mlib{

#define GIT_SHA1 "@GIT_SHA1@"
#define GIT_DATE "@GIT_DATE@"
#define GIT_COMMIT_SUBJECT "@GIT_COMMIT_SUBJECT@"

struct mlib_version{
	static const string mGitSha1=GIT_SHA1;
	static const string mGitDate=GIT_DATE;
	static const string mGitSubject=GIT_COMMIT_SUBJECT;
};


}

#endif
