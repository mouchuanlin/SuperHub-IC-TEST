#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=Initial.c emc_library.c EE_library.c Systen_Library.c Module_Library.c SMS_library.c Setup_mode.c Module_LB_Telit.c Module_LB_Gemalto.c OTA.c Main.c queue.c state.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Initial.p1 ${OBJECTDIR}/emc_library.p1 ${OBJECTDIR}/EE_library.p1 ${OBJECTDIR}/Systen_Library.p1 ${OBJECTDIR}/Module_Library.p1 ${OBJECTDIR}/SMS_library.p1 ${OBJECTDIR}/Setup_mode.p1 ${OBJECTDIR}/Module_LB_Telit.p1 ${OBJECTDIR}/Module_LB_Gemalto.p1 ${OBJECTDIR}/OTA.p1 ${OBJECTDIR}/Main.p1 ${OBJECTDIR}/queue.p1 ${OBJECTDIR}/state.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/Initial.p1.d ${OBJECTDIR}/emc_library.p1.d ${OBJECTDIR}/EE_library.p1.d ${OBJECTDIR}/Systen_Library.p1.d ${OBJECTDIR}/Module_Library.p1.d ${OBJECTDIR}/SMS_library.p1.d ${OBJECTDIR}/Setup_mode.p1.d ${OBJECTDIR}/Module_LB_Telit.p1.d ${OBJECTDIR}/Module_LB_Gemalto.p1.d ${OBJECTDIR}/OTA.p1.d ${OBJECTDIR}/Main.p1.d ${OBJECTDIR}/queue.p1.d ${OBJECTDIR}/state.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/Initial.p1 ${OBJECTDIR}/emc_library.p1 ${OBJECTDIR}/EE_library.p1 ${OBJECTDIR}/Systen_Library.p1 ${OBJECTDIR}/Module_Library.p1 ${OBJECTDIR}/SMS_library.p1 ${OBJECTDIR}/Setup_mode.p1 ${OBJECTDIR}/Module_LB_Telit.p1 ${OBJECTDIR}/Module_LB_Gemalto.p1 ${OBJECTDIR}/OTA.p1 ${OBJECTDIR}/Main.p1 ${OBJECTDIR}/queue.p1 ${OBJECTDIR}/state.p1

# Source Files
SOURCEFILES=Initial.c emc_library.c EE_library.c Systen_Library.c Module_Library.c SMS_library.c Setup_mode.c Module_LB_Telit.c Module_LB_Gemalto.c OTA.c Main.c queue.c state.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F26K22
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Initial.p1: Initial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Initial.p1.d 
	@${RM} ${OBJECTDIR}/Initial.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Initial.p1 Initial.c 
	@-${MV} ${OBJECTDIR}/Initial.d ${OBJECTDIR}/Initial.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Initial.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/emc_library.p1: emc_library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/emc_library.p1.d 
	@${RM} ${OBJECTDIR}/emc_library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/emc_library.p1 emc_library.c 
	@-${MV} ${OBJECTDIR}/emc_library.d ${OBJECTDIR}/emc_library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/emc_library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/EE_library.p1: EE_library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/EE_library.p1.d 
	@${RM} ${OBJECTDIR}/EE_library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/EE_library.p1 EE_library.c 
	@-${MV} ${OBJECTDIR}/EE_library.d ${OBJECTDIR}/EE_library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/EE_library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Systen_Library.p1: Systen_Library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Systen_Library.p1.d 
	@${RM} ${OBJECTDIR}/Systen_Library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Systen_Library.p1 Systen_Library.c 
	@-${MV} ${OBJECTDIR}/Systen_Library.d ${OBJECTDIR}/Systen_Library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Systen_Library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Module_Library.p1: Module_Library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Module_Library.p1.d 
	@${RM} ${OBJECTDIR}/Module_Library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Module_Library.p1 Module_Library.c 
	@-${MV} ${OBJECTDIR}/Module_Library.d ${OBJECTDIR}/Module_Library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Module_Library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/SMS_library.p1: SMS_library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SMS_library.p1.d 
	@${RM} ${OBJECTDIR}/SMS_library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/SMS_library.p1 SMS_library.c 
	@-${MV} ${OBJECTDIR}/SMS_library.d ${OBJECTDIR}/SMS_library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/SMS_library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Setup_mode.p1: Setup_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Setup_mode.p1.d 
	@${RM} ${OBJECTDIR}/Setup_mode.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Setup_mode.p1 Setup_mode.c 
	@-${MV} ${OBJECTDIR}/Setup_mode.d ${OBJECTDIR}/Setup_mode.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Setup_mode.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Module_LB_Telit.p1: Module_LB_Telit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Module_LB_Telit.p1.d 
	@${RM} ${OBJECTDIR}/Module_LB_Telit.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Module_LB_Telit.p1 Module_LB_Telit.c 
	@-${MV} ${OBJECTDIR}/Module_LB_Telit.d ${OBJECTDIR}/Module_LB_Telit.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Module_LB_Telit.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Module_LB_Gemalto.p1: Module_LB_Gemalto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Module_LB_Gemalto.p1.d 
	@${RM} ${OBJECTDIR}/Module_LB_Gemalto.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Module_LB_Gemalto.p1 Module_LB_Gemalto.c 
	@-${MV} ${OBJECTDIR}/Module_LB_Gemalto.d ${OBJECTDIR}/Module_LB_Gemalto.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Module_LB_Gemalto.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/OTA.p1: OTA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/OTA.p1.d 
	@${RM} ${OBJECTDIR}/OTA.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/OTA.p1 OTA.c 
	@-${MV} ${OBJECTDIR}/OTA.d ${OBJECTDIR}/OTA.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/OTA.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Main.p1: Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Main.p1.d 
	@${RM} ${OBJECTDIR}/Main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Main.p1 Main.c 
	@-${MV} ${OBJECTDIR}/Main.d ${OBJECTDIR}/Main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/queue.p1: queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/queue.p1.d 
	@${RM} ${OBJECTDIR}/queue.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/queue.p1 queue.c 
	@-${MV} ${OBJECTDIR}/queue.d ${OBJECTDIR}/queue.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/queue.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/state.p1: state.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/state.p1.d 
	@${RM} ${OBJECTDIR}/state.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/state.p1 state.c 
	@-${MV} ${OBJECTDIR}/state.d ${OBJECTDIR}/state.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/state.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/Initial.p1: Initial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Initial.p1.d 
	@${RM} ${OBJECTDIR}/Initial.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Initial.p1 Initial.c 
	@-${MV} ${OBJECTDIR}/Initial.d ${OBJECTDIR}/Initial.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Initial.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/emc_library.p1: emc_library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/emc_library.p1.d 
	@${RM} ${OBJECTDIR}/emc_library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/emc_library.p1 emc_library.c 
	@-${MV} ${OBJECTDIR}/emc_library.d ${OBJECTDIR}/emc_library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/emc_library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/EE_library.p1: EE_library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/EE_library.p1.d 
	@${RM} ${OBJECTDIR}/EE_library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/EE_library.p1 EE_library.c 
	@-${MV} ${OBJECTDIR}/EE_library.d ${OBJECTDIR}/EE_library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/EE_library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Systen_Library.p1: Systen_Library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Systen_Library.p1.d 
	@${RM} ${OBJECTDIR}/Systen_Library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Systen_Library.p1 Systen_Library.c 
	@-${MV} ${OBJECTDIR}/Systen_Library.d ${OBJECTDIR}/Systen_Library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Systen_Library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Module_Library.p1: Module_Library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Module_Library.p1.d 
	@${RM} ${OBJECTDIR}/Module_Library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Module_Library.p1 Module_Library.c 
	@-${MV} ${OBJECTDIR}/Module_Library.d ${OBJECTDIR}/Module_Library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Module_Library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/SMS_library.p1: SMS_library.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SMS_library.p1.d 
	@${RM} ${OBJECTDIR}/SMS_library.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/SMS_library.p1 SMS_library.c 
	@-${MV} ${OBJECTDIR}/SMS_library.d ${OBJECTDIR}/SMS_library.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/SMS_library.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Setup_mode.p1: Setup_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Setup_mode.p1.d 
	@${RM} ${OBJECTDIR}/Setup_mode.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Setup_mode.p1 Setup_mode.c 
	@-${MV} ${OBJECTDIR}/Setup_mode.d ${OBJECTDIR}/Setup_mode.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Setup_mode.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Module_LB_Telit.p1: Module_LB_Telit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Module_LB_Telit.p1.d 
	@${RM} ${OBJECTDIR}/Module_LB_Telit.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Module_LB_Telit.p1 Module_LB_Telit.c 
	@-${MV} ${OBJECTDIR}/Module_LB_Telit.d ${OBJECTDIR}/Module_LB_Telit.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Module_LB_Telit.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Module_LB_Gemalto.p1: Module_LB_Gemalto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Module_LB_Gemalto.p1.d 
	@${RM} ${OBJECTDIR}/Module_LB_Gemalto.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Module_LB_Gemalto.p1 Module_LB_Gemalto.c 
	@-${MV} ${OBJECTDIR}/Module_LB_Gemalto.d ${OBJECTDIR}/Module_LB_Gemalto.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Module_LB_Gemalto.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/OTA.p1: OTA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/OTA.p1.d 
	@${RM} ${OBJECTDIR}/OTA.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/OTA.p1 OTA.c 
	@-${MV} ${OBJECTDIR}/OTA.d ${OBJECTDIR}/OTA.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/OTA.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Main.p1: Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Main.p1.d 
	@${RM} ${OBJECTDIR}/Main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/Main.p1 Main.c 
	@-${MV} ${OBJECTDIR}/Main.d ${OBJECTDIR}/Main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/Main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/queue.p1: queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/queue.p1.d 
	@${RM} ${OBJECTDIR}/queue.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/queue.p1 queue.c 
	@-${MV} ${OBJECTDIR}/queue.d ${OBJECTDIR}/queue.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/queue.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/state.p1: state.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/state.p1.d 
	@${RM} ${OBJECTDIR}/state.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits $(COMPARISON_BUILD)  -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/state.p1 state.c 
	@-${MV} ${OBJECTDIR}/state.d ${OBJECTDIR}/state.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/state.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -Wl,-Map=dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.map  -D__DEBUG=1  -DXPRJ_default=$(CND_CONF)  -Wl,--defsym=__MPLAB_BUILD=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto        $(COMPARISON_BUILD) -Wl,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -o dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -Wl,-Map=dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.map  -DXPRJ_default=$(CND_CONF)  -Wl,--defsym=__MPLAB_BUILD=1  -fshort-double -fshort-float -memi=wordwrite -O1 -fasmfile -maddrqual=ignore -xassembler-with-cpp -Wa,-a -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mno-keep-startup -mno-download -mdefault-config-bits -std=c90 -gdwarf-3 -mstack=compiled:auto:auto:auto     $(COMPARISON_BUILD) -Wl,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -o dist/${CND_CONF}/${IMAGE_TYPE}/SuperHub-IC-TEST.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
