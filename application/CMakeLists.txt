#add cpp to WRAPPER variable
file(GLOB_RECURSE APP ./ *.cpp *.cc)

#Compile WRAPPER to lib
add_library(app ${APP})