#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/IBackEnd.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/File_Ads.o \
	${OBJECTDIR}/File_Queries.o \
	${OBJECTDIR}/Interfaces.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/usr/lib/libmysqlpp.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ca_2

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ca_2: /usr/lib/libmysqlpp.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ca_2: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ca_2 ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/IBackEnd.o: IBackEnd.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include/mysql++ -I/usr/include/mysql -I../../opt/boost_1_52_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/IBackEnd.o IBackEnd.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include/mysql++ -I/usr/include/mysql -I../../opt/boost_1_52_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/File_Ads.o: File_Ads.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include/mysql++ -I/usr/include/mysql -I../../opt/boost_1_52_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/File_Ads.o File_Ads.cpp

${OBJECTDIR}/File_Queries.o: File_Queries.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include/mysql++ -I/usr/include/mysql -I../../opt/boost_1_52_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/File_Queries.o File_Queries.cpp

${OBJECTDIR}/Interfaces.o: Interfaces.cc 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include/mysql++ -I/usr/include/mysql -I../../opt/boost_1_52_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Interfaces.o Interfaces.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ca_2

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
