
/*
#ifdef __APPLE__
#include <system>
#else
#include <stdio.h>
#endif
*/
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "rwini.hpp"


template<typename Out>
/*

 splits the given string by delimiter and sets the result to all the strings

 */
void split(std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) { // split by delim
        *(result++) = item;
    }
}

/*

 splits the given string by all instances of the delimiter and returns a vector of all strings without empty strings

 */
std::vector<std::string> split(std::string& str, char delim) {
    std::vector<std::string> elems;
    split(str, delim, std::back_inserter(elems)); // do actual split

    for (int i = elems.size()-1; i >= 0; i-=1) {// delete empty elements
        if (elems[i].compare("") == 0) {
            elems.erase(elems.begin()+i);
        }
    }

    return elems; // return elements
}

bool getCppFiles(RWIni ini, std::string &res) {
  if (!ini.getValue_as_String("build", "cpp-files", res)) {
    std::cout << "Unable to retrieve cpp-filenames! (cpp-files=)" << std::endl;
    ini.setValue("build", "cpp-files", "");
    return false;
  }
  return true;
}


bool getLibFile(RWIni ini, std::string &res) {
  if (!ini.getValue_as_String("build", "lib-name", res)) {
    std::cout << "Unable to retrieve lib-name! (lib-name=)" << std::endl;
    ini.setValue("build", "lib-name", "");
    return false;
  }
  return true;
}

bool getCommand(RWIni ini, std::string &res) {
  if (!ini.getValue_as_String("build", "command", res)) {
    std::cout << "Unable to find command! (command=)" << std::endl;
    command = "g++ -dynamiclib";
    ini.setValue("build", "command", res);
    return false;
  }
  return true;
}

bool getLPS(RWIni ini, std::string &res) {
  if (!ini.getValue_as_String("build", "L", res)) {
    std::cout << "No Library path set! (L=)" << std::endl;
    ini.setValue("build", "L", "");
    return false;
  }
  return true;
}

bool getLibs(RWIni ini, std::string &res) {
  if (L_arg) if(!ini.getValue_as_String("build", "libs", res)) {
    std::cout << "No libs set! (libs=)" << std::endl;
    ini.setValue("build", "libs", "");
    return false;
  }
  return true;
}

bool isFWSet(RWIni ini, std::string & res) {
  if (!ini.getValue_as_String("build", "frameworks", res)) {
    std::cout << "No Frameworks set! (libs=)" << std::endl;
    ini.setValue("build", "frameworks", "");
    return false;
  }
  return true;
}

bool getCppLine(RWIni ini, std::string path, std::string &line) {
  std::string cppfiles;
  if (!getCppFiles(ini, cppfiles)) return false;
  std::cout << "Cpp-Files: " << cppfiles << std::endl;
  std::vector<std::string> cpp_files = split(cppfiles, char(' '));
  for (int i = 0; i < cpp_files.size(); i++) {
    line += path+cpp_files[i]+ " ";
  }
  return true;
}

bool getLibLine(RWIni, std::string &line) {
  std:string L;
  bool L_arg = getLPS(ini, L);
  if (L_arg) {
    std::string s_libs;
    getLibs(ini, s_libs);
    std::vector<std::string> libs = split(s_libs, char(' '));
    line += "-L "+L +" ";
    for (int i = 0; i < libs.size(); i++) {
      line +=  "-l"+libs[i]+" ";
    }
    return true;
  }
  return false;
}

bool getFrameworkLine(RWIni, std::string &line) {
  std::string s_frameworks;
  bool F_args = isFWSet(ini, s_frameworks);
  std::vector<std::string> frameworks = split(s_frameworks, char(' '));
  if (F_args) {
    for (int i = 0; i < frameworks.size(); i++)  {
      line += "-framework "+frameworks[i]+" ";
    }
    return true;
  }
  return false;
}

bool getFlagLine(RWIni ini, std::string &line) {
  std::string flags;
  if (!ini.getValue_as_String("build", "flags", flags)) {
    std::cout << "No extra flags set!" << std::endl;
    return false;
  }
  line += flags;
  return true;
}

/*

  reads the ini and compiles the executable/Library

  */
int build(RWIni ini, std::string PATH) {
  std::string cppline;
  if (!getCppLine(ini, PATH, cppline)) return -1;
  std::string libfile;
  if (!getLibFile(ini, libfile)) return -1;
  std::string command;
  getCommand(ini, command);
  std::string f_command = command + " -o "+PATH+libfile +" ";
  f_command += cppline;
  std::string libline;
  if (getLibLine(ini, libline)) f_command += libline;
  std::string frameworkline;
  if (getFrameworkLine(ini, frameworkline)) f_command += frameworkline;
  std::string flagline;
  if (getFlagLine(ini, flagline)) f_command += flagline;
  std::cout << "Building Command: " << f_command << std::endl;
  #ifdef __APPLE__
  system(f_command.c_str());
  #endif
  return 0;
}

/*
*/
int main(int argc, char const *argv[]) {
  RWIni ini;
  // check for a .ini in the main arguments.
  bool ini_arg = false;
  for (int i = 0; i < argc; i++) {
    std::cout << "[" << i << "]: " << argv[i] << std::endl;
    if (!ini_arg) if (ini.init(argv[i])) ini_arg = true;
  }

  std::string PATH = argv[0]; //path to executable
  PATH = PATH.substr(0, PATH.find_last_of("/"))+ "/"; // relative search path to executable

  if (ini_arg) {// if found ini
    ini.init(PATH+ini.getIniFilename());
  } else { // else set ini-file to /relative/path/to/executable/build.ini
    ini.init(PATH + "build.ini");
  }

  if (!ini.iniExists()) { // check if file exists
    std::cout << "Unable to build without ini-file" << std::endl;
    return -1;
  } else {
    std::cout << "INI: " << ini.getIniFilename() << std::endl; // print what file is used
  }

  return build(ini, PATH); // parse commands!
}
