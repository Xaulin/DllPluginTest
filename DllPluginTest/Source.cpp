#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS
#define NORMAL_FILE_ATTRIBUTE FILE_ATTRIBUTE_DIRECTORY | \
	FILE_ATTRIBUTE_HIDDEN

class PluginsManager{
public:
	class Plugin{
	public:
		friend class PluginsManager;
		void* operator[](const char* func);

	private:
		HMODULE m;
	};
	class Iter{
	public:
		Iter(PluginsManager* pm, size_t pos);
		bool operator!=(Iter& other);
		Plugin& operator*();
		const Iter& operator++();

	private:
		PluginsManager* pm;
		size_t pos;
	};
	class Exception{ public: const char* msg; };

	PluginsManager();
	PluginsManager(char* path);
	~PluginsManager();

	Plugin& operator[](size_t index);

	void registerFromPath(char* path);
	void unregister(size_t index);
	void unregister();
	size_t getSize();

	Iter begin();
	Iter end();

private:
	class{
	public:
		Plugin* p;
		size_t s;
	}pa;
};

PluginsManager::PluginsManager() : pa({ 0 }){}
PluginsManager::PluginsManager(char* path) : pa({ 0 }){ registerFromPath(path); }
PluginsManager::~PluginsManager(){ unregister(); }

PluginsManager::Plugin& PluginsManager::operator[](size_t index) {
	if (index < pa.s) return pa.p[index];
	else throw Exception{ "Index out of array" };
}

void PluginsManager::registerFromPath(char* path){
	int pathLen = strlen(path);
	char buf[MAX_PATH];
	memcpy(buf, path, pathLen + 1);
	WIN32_FIND_DATAA wfd;
	HANDLE fndf = FindFirstFileA(buf, &wfd);

	if (fndf != INVALID_HANDLE_VALUE){
		do if (!(wfd.dwFileAttributes & (NORMAL_FILE_ATTRIBUTE)) &&
			strcmp(strchr(wfd.cFileName, '.') + 1, "dll") == 0){
			memcpy(&buf[pathLen - 1], wfd.cFileName, strlen(wfd.cFileName) + 1);
			if (HMODULE dll = LoadLibraryA(buf)){
				pa.p = (Plugin*)realloc(pa.p, sizeof(Plugin)*(pa.s + 1));
				pa.p[pa.s++].m = dll;
			}
		}while (FindNextFileA(fndf, &wfd));
	}

	FindClose(fndf);
}

void PluginsManager::unregister(size_t index){
	FreeLibrary(pa.p[index].m);
	Plugin* plugins = (Plugin*)malloc(sizeof(Plugin)*(pa.s - 1));
	for (size_t i = 0, j = 0; i < pa.s; ++i)
		if (i != index) plugins[j++] = pa.p[i];
	free(pa.p);
	pa.p = plugins;
}

void PluginsManager::unregister(){
	for (size_t i = 0; i < pa.s; ++i)
		FreeLibrary(pa.p[i].m);
}

size_t PluginsManager::getSize(){ return pa.s; }

PluginsManager::Iter PluginsManager::begin(){
	return PluginsManager::Iter(this, 0);
}

PluginsManager::Iter PluginsManager::end(){
	return PluginsManager::Iter(this, pa.s);
}

PluginsManager::Iter::Iter(PluginsManager* pm, size_t pos){
	this->pm = pm;
	this->pos = pos;
}

bool PluginsManager::Iter::operator!=(Iter& other){
	return pos != other.pos;
}

PluginsManager::Plugin& PluginsManager::Iter::operator*(){
	return (*pm)[pos];
}

const PluginsManager::Iter& PluginsManager::Iter::operator++(){
	++pos;
	return *this;
}

void* PluginsManager::Plugin::operator[](const char* func){
	return GetProcAddress(m, func);
}

int main(){
	try{
		PluginsManager pa("D:/newProjects/maj/DllPluginTest/Debug/*");
		for (PluginsManager::Plugin& p : pa)
			if (void(*f)() = (void(*)()) p["func"]) f();
	}
	catch (PluginsManager::Exception& e){
		printf("Eception: %s", e);
	}
}