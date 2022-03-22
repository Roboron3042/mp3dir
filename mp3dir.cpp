#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <ftw.h>
#include <algorithm>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <errno.h>
  
using namespace std;

/*
 * Get extension of a given file
 */
const char *get_filename_ext(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if( !dot || dot == filename) return "";
	return dot + 1;
}
/*
 * Get album tag from a music file
 */
std::string get_album(const char *fpath){

	TagLib::FileRef f(fpath);

	if( !f.isNull() && f.tag() ) {
		TagLib::Tag *tag = f.tag();
		return tag->album().to8Bit(1);
	} else {
		return "";
	}
}
/*
 * Remove invalid symbols for directory name
 */
void sanitize(std::string &album) {
	album.erase(std::remove(album.begin(), album.end(), '/'), album.end());
	album.erase(std::remove(album.begin(), album.end(), ':'), album.end());
	album.erase(std::remove(album.begin(), album.end(), '.'), album.end());
	album.erase(std::remove(album.begin(), album.end(), '"'), album.end());
}

static int move_file(const char *fpath, const struct stat *sb,
                    int tflag, struct FTW *ftwbuf)
{
	std::string album, new_file;
	if ( tflag == FTW_D ) return 0; // Can directories have tags? It's a rethoric question. Please do not answer.
	if (strcmp(get_filename_ext(fpath + ftwbuf->base),"mp3") &&
		strcmp(get_filename_ext(fpath + ftwbuf->base),"flac") ) {
		return 0; // Don't even try if it is not a music file
	}
	album = get_album(fpath);
	sanitize(album);
	mkdir(album.c_str());
	new_file = album + "/" + fpath;
	printf("Old file: %s\n", fpath);
	printf("New file: %s\n", new_file.c_str());
	if ( rename(fpath, new_file.c_str()) ){
		printf("An error ocurred while moving file. Error code: %d\n", errno);
	}
	return 0; // To tell nftw() to continue
}

int main(int argc , char *argv[])
{
	
	if (nftw(".", move_file, 20, FTW_SKIP_SUBTREE) == -1) {
		perror("nftw");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}
