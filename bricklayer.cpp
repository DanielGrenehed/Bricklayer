
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

/*

  reads the ini and compiles the executable/Library

  */
int build(RWIni ini, std::string PATH) {
  std::cout << "INI is readable!" << std::endl;

  std::string cppfiles/* = ini.getValue("build", "cpp-files")*/;
  if(!ini.getValue_as_String("build", "cpp-files", cppfiles)) {
    std::cout << "Unable to retrieve cpp-filenames! (cpp-files=)" << std::endl;
    ini.setValue("build", "cpp-files", "");
    return -1;
  }
  std::cout << "Cpp-Files: " << cppfiles << std::endl;

  std::vector<std::string> cpp_files = split(cppfiles, char(' '));
  //std::vector<std::string> files = split(cppfiles, char(' '));

  std::string libfile;
  if (!ini.getValue_as_String("build", "lib-name", libfile)) {
    std::cout << "Unable to retrieve lib-name! (lib-name=)" << std::endl;
    ini.setValue("build", "lib-name", "");
    return -1;
  }

  std::cout << "Lib-File: " << libfile << std::endl;

  std::string command;
  if (!ini.getValue_as_String("build", "command", command)) {
    std::cout << "Unable to find command! (command=)" << std::endl;
    command = "g++ -dynamiclib";
    ini.setValue("build", "command", command);
  }

  std::cout << "command: " << command << std::endl;

  bool L_arg = true;
  std::string L;
  if (!ini.getValue_as_String("build", "L", L)) {
    std::cout << "No Library path set! (L=)" << std::endl;
    //ini.setValue("build", "L", "");
    L_arg = false;
  }

  std::string s_libs;
  if (L_arg) if(!ini.getValue_as_String("build", "libs", s_libs)) {
    std::cout << "No libs set! (libs=)" << std::endl;
    ini.setValue("build", "libs", "");
  }
  std::vector<std::string> libs = split(s_libs, char(' '));

  bool F_args = true;
  std::string s_frameworks;
  if (!ini.getValue_as_String("build", "frameworks", s_frameworks)) {
    std::cout << "No Frameworks set! (libs=)" << std::endl;
    ini.setValue("build", "frameworks", "");
    F_args = false;
  }

  std::string flags;
  if (!ini.getValue_as_String("build", "flags", flags)) {
    std::cout << "No extra flags set!" << std::endl;
  }

  std::vector<std::string> frameworks = split(s_frameworks, char(' '));


  // create command
  std::cout << "Creating command" << std::endl;

  std::string f_command = command + " -o "+PATH+libfile +" ";

  for (int i = 0; i < cpp_files.size(); i++) {
    f_command += PATH+cpp_files[i]+ " ";
  }

  if (L_arg) {
    f_command += "-L "+L +" ";
    for (int i = 0; i < libs.size(); i++) {
      f_command +=  "-l"+libs[i]+" ";
    }
  }

  if (F_args) {
    for (int i = 0; i < frameworks.size(); i++)  {
      f_command += "-framework "+frameworks[i]+" ";
    }
  }

  f_command += flags;
  std::cout << "Building product with command: " << f_command << std::endl;
  #ifdef __APPLE__
  //do stuff
  system(f_command.c_str());
  #endif
  /*
  File* compiler = popen(f_command, "r");
  char c;
  while ((c = std::fgetc(compiler) != EOF)) {
    std::cout << c << '\n';
  }
  pclose(compiler);*/
  std::cout << "Finished!" << std::endl;
  return 0;
}

/*



*/
int main(int argc, char const *argv[]) {

  RWIni ini;

  bool ini_arg = false;
  for (int i = 0; i < argc; i++) {
    std::cout << "[" << i << "]: " << argv[i] << std::endl;
    if (!ini_arg) if (ini.init(argv[i])) ini_arg = true;
  }
  std::string PATH = argv[0];
  PATH = PATH.substr(0, PATH.find_last_of("/"))+ "/";

  if (ini_arg) {
    ini.init(PATH+ini.getIniFilename());
  } else {
    ini.init(PATH + "build.ini");
  }

  std::cout << "INI: " << ini.getIniFilename() << std::endl;

  if (!ini.iniExists()) {
    std::cout << "Unable to build without ini-file" << std::endl;
    return -1;
  }


  return build(ini, PATH);
}
